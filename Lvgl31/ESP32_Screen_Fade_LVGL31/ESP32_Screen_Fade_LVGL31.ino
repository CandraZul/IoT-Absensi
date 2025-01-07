/*#include <Ticker.h>*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <SimpleFTPServer.h>/* https://github.com/xreef/SimpleFTPServer */
#include "SPIFFS.h"
#include "FS.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
/* #define LVGL_TICK_PERIOD 20*/
/* display buffer https://docs.lvgl.io/latest/en/html/porting/display.html */
/*#define LVGL_TICK_PERIOD 20
  Ticker tick; // timer for interrupt handler
  SemaphoreHandle_t xSemaphore = NULL;*/
static lv_style_t style_kb;
static lv_disp_buf_t disp_buf;
static uint8_t BMPHeader[128];
int kbCall = 0;
int8_t NumeroNetwork = 0;
int Connessione = 1;
int Brightness = 255;/* overwrite by Preferences */
int WifiLimit = -75;/* overwrite by Preferences */
String ssid = "";/* overwrite by Preferences */
String wifipw = "";/* overwrite by Preferences */
/* USER and PASSWORD FTP is set by default to "Esp32" */
String FtpOnOff = "ON"; /* overwrite by Preferences */
FtpServer ftpSrv;
char TT[512];
String T = "";
int BytesWrites = 0;
String command;
int TipoFormato = 1; /* 1=with BMP Header16 bit .BMP      2=without BMP header 16bit .RAW*/
int incomingByte = 0;
File pFileOut;
uint16_t SelectedRow;
uint16_t SelectedCol;
/* Styles */
static lv_style_t styleLabelBTN;
/* LVGL objects */
static lv_obj_t * kb;
static lv_obj_t * kb_ta;
static lv_obj_t * Screen1;
static lv_obj_t * Sfondo1;
static lv_obj_t * LabelToast;
static lv_obj_t * RollerSSID;
static lv_obj_t * LabelWIFI;
static lv_obj_t * LabelPREF;
static lv_obj_t * LabelSSID;
static lv_obj_t * LabelPASSW;
static lv_obj_t * LabelSlider;
static lv_obj_t * pageTable;
static lv_obj_t * tablePrefs;

const int screenWidth = 480;
const int  screenHeight = 320;
static lv_color_t buf[screenWidth * screenHeight / 10];
/*static void lv_tick_handler(void) {
  lv_tick_inc(LVGL_TICK_PERIOD);
  }*/
/* ====================     Display flushing      Display flushing     Display flushing     Display flushing   ========================*/
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint16_t c;
  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full; tft.writeColor(c, 1); color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}
/* =====================   READ TOUCHPAD ========================= */
bool my_input_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data) {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY, 600);
  if (!touched) {
    return false;
  }
  if (touchX > screenWidth || touchY > screenHeight) {
  } else {
    data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    data->point.x = touchX;
    data->point.y = touchY;
  }
  return false; /*Return `false` because we are not buffering and no more data to read*/
}

static void tablePrefs_eventhandler(lv_obj_t * tablePrefs, lv_event_t event) {/*     CHANGE PREFERENCES    */
  if (event == LV_EVENT_LONG_PRESSED) {
    lv_res_t result = lv_table_get_pressed_cell(tablePrefs, &SelectedRow, &SelectedCol);
    if (result == LV_RES_OK ) {/*if a valid cell/row is pressed*/
      T = lv_table_get_cell_value(tablePrefs, SelectedRow, SelectedCol);
      Serial.print(T); Serial.print("      row ="); Serial.print(SelectedRow); Serial.print("      Col ="); Serial.println(SelectedCol);
      if (SelectedRow == 0) {/* EXIT and close Prefs table */
        lv_obj_set_hidden(pageTable, true);
        lv_obj_set_style_local_shadow_color(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
        lv_obj_set_style_local_text_color(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
        lv_obj_set_style_local_text_opa(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 150);
      } else {
        if (SelectedCol > 0) {/*not consider column 0 */
          TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
          if (T == "ADD ROW") {
            int roww = lv_table_get_row_cnt(tablePrefs); /* get_row give already the max number from 1 to tot row */
            lv_table_set_row_cnt(tablePrefs, roww);
            T = String(roww);  TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
            lv_table_set_cell_value(tablePrefs, roww, 0, TT);/* number of row*/
            lv_table_set_cell_value(tablePrefs, roww, 1, "ADD ROW"); lv_table_set_cell_value(tablePrefs, roww, 2, "DEL ROW");
            for (int i = 1; i <= 2 ; i++) {
              lv_table_set_cell_value(tablePrefs, roww - 1, i, ""); /* erase row content but no initial number*/
            }
            lv_table_set_cell_value(tablePrefs, roww - 1, 1, "CMD"); /* by default we put Command string*/
            savePref( ); return;
          }
          if (T == "DEL ROW") {
            int roww = lv_table_get_row_cnt(tablePrefs); /* get_row give already the max number from 1 to tot row */
            if (roww <= 7) {
              lv_label_set_text(LabelToast, "Cannot delete 0 to 5 Row\nIs system variables"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10);
              lv_obj_set_hidden(LabelToast, false); lv_obj_move_foreground(LabelToast);
              return;
            } else {
              T = String(roww);  TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
              lv_table_set_cell_value(tablePrefs, roww - 2, 1, "ADD ROW"); lv_table_set_cell_value(tablePrefs, roww - 2, 2, "DEL ROW");
              for (int i = 0; i <= 2 ; i++) {
                lv_table_set_cell_value(tablePrefs, roww, i, ""); /* erase row content */
              }
              lv_table_set_row_cnt(tablePrefs, roww - 1);
              savePref( ); return;
            }
          } else {
            kbCall = 2;
            lv_textarea_set_text(kb_ta, TT);
            lv_obj_set_hidden(kb_ta, false);
            lv_obj_set_hidden(kb, false);
            lv_obj_move_foreground(kb_ta);lv_obj_move_foreground(kb);
          }
        }
      }
    }
  }
}

static void LabelToast_eventhandler(lv_obj_t * LabelToast, lv_event_t event) {/*     LABEL TOAST INFO EVENT    */
  if (event == LV_EVENT_PRESSED) {
    lv_obj_set_hidden(LabelToast, true);
  }
}
static void LabelWIFI_eventhandler(lv_obj_t * LabelWIFI, lv_event_t event) { /*     WIFI LABEL BUTTON EVENT    */
  if (event == LV_EVENT_PRESSED) {
    lv_obj_set_hidden(LabelToast, false);
    if ( Connessione == 1 && ssid == "") {
      lv_obj_set_hidden(LabelToast, false);
      lv_obj_set_hidden(LabelSSID, true);
      lv_obj_set_hidden(LabelPASSW, true);
      lv_obj_set_hidden(pageTable, true);
      lv_obj_set_style_local_shadow_color(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
      lv_obj_set_style_local_text_color(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
      lv_obj_set_style_local_text_opa(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 150);
      lv_label_set_text(LabelToast, ">>> SCAN FOR NETWORKS <<<\nwait pls"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
      lv_refr_now(NULL);
      delay(100);
      NumeroNetwork = WiFi.scanNetworks();
      if (NumeroNetwork == 0) {
        T = "No Networks found with "; T += String (WifiLimit); T += " Wifi Limit";
        TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
        lv_label_set_text(LabelToast, TT);  lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
        Serial.println("no networks found");
        /*ESP.restart(); RESET*/
      } else {
        T = String(NumeroNetwork); T += "   Networks found with "; T += String (WifiLimit); T += " Wifi Limit";
        TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
        lv_label_set_text(LabelToast, TT); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
        T = "";
        for (int i = 0; i < NumeroNetwork; ++i) {
          if (WiFi.RSSI(i) >= WifiLimit && WiFi.RSSI(i) != 0) { /* i limit networks dispayed on Roller only if is from 0 to WifiLimit */
            T += String(WiFi.SSID(i)); T += "   "; T += String( WiFi.RSSI(i));
            //            byte encryption = WiFi.encryptionType(i);
            //            Serial.println(encryption);
            if (WiFi.encryptionType(i) == 0) {
              T += "  OPEN\n";
            } else {
              T += "  Closed\n";
            }
          }
          TT[(T.length() + 1)];
          T.toCharArray(TT, T.length() + 1);
          /*Serial.print("TT Lenght = "); Serial.println((String)T.length());*/
          lv_roller_set_options(RollerSSID, TT, LV_ROLLER_MODE_NORMAL);
          lv_obj_align(RollerSSID, Screen1, LV_ALIGN_CENTER, 0, 0);
          lv_obj_set_hidden(RollerSSID, false);
        }
        /* ESP.restart();*/
      }
    } else {
      if ( Connessione == 2 ) {/* if is connected i disconnect*/
        lv_label_set_text(LabelToast, "Disconnected from wifi"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
        lv_obj_set_style_local_shadow_color(LabelWIFI, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
        lv_obj_set_style_local_text_color(LabelWIFI, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(252 * 256 * 256 + 252 * 256 + 252));
        lv_obj_set_style_local_text_opa(LabelWIFI, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 150);
        WiFi.disconnect(); Connessione = 1;
        //ftpSrv.disconnectClient();
      } else {
        lv_label_set_text(LabelToast, "Connecting to Wifi."); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
        lv_obj_set_hidden(LabelSSID, true);
        lv_obj_set_hidden(LabelPASSW, true);
        lv_refr_now(NULL);
        delay(100);
        wifiConn();
      }
    }
  }
}
static void LabelPREF_eventhandler(lv_obj_t * LabelPREF, lv_event_t event) { /*     PREF BUTTON EVENT    */
  if (event == LV_EVENT_SHORT_CLICKED) {
    lv_obj_set_hidden(LabelToast, true);
    lv_obj_set_hidden(pageTable, false);
    lv_obj_set_style_local_shadow_color(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(90 * 256 * 256 + 152 * 256 + 215));
    lv_obj_set_style_local_text_color(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(254 * 256 * 256 + 253 * 256 + 206));
    lv_obj_set_style_local_text_opa(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
  } else {
    if (event == LV_EVENT_LONG_PRESSED) {
      lv_obj_set_hidden(pageTable, true);/* Pref window  CLOSE*/
      lv_obj_set_style_local_shadow_color(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
      lv_obj_set_style_local_text_color(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
      lv_obj_set_style_local_text_opa(LabelPREF, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 150);
    }
  }
}
static void RollerSSID_eventhandler(lv_obj_t * RollerSSID, lv_event_t event) {
  if (event == LV_EVENT_LONG_PRESSED) {
    T = "";
    T = String(WiFi.SSID(lv_roller_get_selected(RollerSSID)));
    if (T != "") {
      TT[(T.length() + 1)];
      T.toCharArray(TT, T.length() + 1);
      lv_label_set_text(LabelSSID, TT);
      lv_obj_align(LabelSSID, Screen1, LV_ALIGN_CENTER, -138, -90); lv_label_refr_text(LabelSSID);
      ssid =  lv_label_get_text(LabelSSID);
      T = ssid;
      TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
      lv_table_set_cell_value(tablePrefs, 1, 2, TT);
      lv_obj_set_hidden(LabelSSID, false);
      lv_obj_set_hidden(LabelPASSW, false);
      lv_obj_set_hidden(RollerSSID, true);
      if (wifipw == "") {
        lv_label_set_text(LabelToast, "Select a Password now"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
        kbCall = 1;
        lv_textarea_set_text(kb_ta, "");
        lv_obj_set_hidden(kb_ta, false);
        lv_obj_set_hidden(kb, false);
        lv_obj_move_foreground(kb_ta);lv_obj_move_foreground(kb);
      } else {
        lv_label_ins_text(LabelToast, LV_LABEL_POS_LAST, "\npress WIFI button for connect"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10);
        lv_obj_move_foreground(LabelToast);
        lv_obj_set_hidden(LabelSSID, true);
        lv_obj_set_hidden(LabelPASSW, true);
      }
    } else {
      lv_label_set_text(LabelToast, "Wrong selection"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
    }
  }
}
/* -----------  KEYBOARD EVENT  --------------*/
static void keyboard_event_cb(lv_obj_t * kb, lv_event_t event) {
  lv_keyboard_def_event_cb(kb, event);
  if (event == LV_EVENT_APPLY) {
    lv_obj_set_hidden(kb_ta, true);
    lv_obj_set_hidden(kb, true);
    String msg = lv_textarea_get_text(kb_ta);
    //    Serial.print(msg); Serial.print("   CALL = "); Serial.println((String)kbCall);
    //    if (msg.length() > 0) {
    switch (kbCall) {
      case 1 : { /* ---------- set password -------- */
          wifipw = "";
          //          if (msg.length() > 0) {
          T = msg; TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
          lv_label_set_text(LabelPASSW, TT); lv_obj_align(LabelPASSW, Screen1, LV_ALIGN_CENTER, -138, -40); lv_label_refr_text(LabelPASSW);
          //wifipw = lv_label_get_text(LabelPASSW);
          wifipw = msg;
          //          }
          savePref( );
          lv_label_ins_text(LabelToast, LV_LABEL_POS_LAST, "\npress WIFI button for connect"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10);
          lv_obj_move_foreground(LabelToast);
          lv_textarea_set_text(kb_ta, "");
          break;
        }
      case 2 : { /* ---------- set table-------- */
          T = msg; TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
          lv_table_set_cell_value(tablePrefs, SelectedRow, SelectedCol, TT);
          savePref( );
          break;
        }
      default:
        break;
    }
    //    }
  } else if (event == LV_EVENT_CANCEL) {
    lv_obj_set_hidden(kb_ta, true);
    lv_obj_set_hidden(kb, true);
  }
}
#if USE_LV_LOG != 0
/* Serial debugging */
void my_print(lv_log_level_t level, const char * file, uint32_t line, const char * dsc) {
  Serial.printf("%s@%d->%s\r\n", file, line, dsc);
  Serial.flush();
}
#endif
/* ==========================    SETUP     SETUP      SETUP       SETUP         SETUP             SETUP  ====================== */
void setup() {
  //vTaskDelay(10);
  ledcSetup(0, 5000/*freq*/, 10 /*resolution*/);
  ledcAttachPin(32, 0);/* PWM on Pin 32 if connected */
  ledcWrite(0, 128); // Start @ initial Brightnes (0-255)
  Serial.begin(115200); /* prepare for possible serial debug */
  //Serial.begin(250000); /* prepare for possible serial debug */
  //Serial.flush();
  delay(100);
  //  WiFi.mode(WIFI_STA);
  //  WiFi.disconnect();
  //  delay(100);
  //NumeroNetwork = WiFi.scanNetworks();
  delay(1000);
  if (SPIFFS.begin()) {
    Serial.println(F("SPIFFS mounted correctly."));
  } else {
    Serial.println(F("!An error occurred during SPIFFS mounting"));
  }
  lv_init();
  tft.begin(); /* TFT init */
  tft.setRotation(3);
  uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
  tft.setTouch(calData);
  lv_disp_buf_init(&disp_buf, buf, NULL, screenWidth * screenHeight / 10);
  /*Initialize the DISPLAY*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);
  /*Initialize the INPUT DEVICE DRIVER*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);      /*Basic initialization*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;   /*Touch pad is a pointer-like device*/
  indev_drv.read_cb = my_input_read;
  /*Register the driver in LVGL and save the created input device object*/
  lv_indev_drv_register(&indev_drv);
  /*tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);
    xSemaphore = xSemaphoreCreateMutex(); //mutex to handle the processes*/
#if USE_LV_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif
  //lv_style_t styleLabelBTN;
  lv_style_set_bg_color(&styleLabelBTN,   LV_STATE_DEFAULT, lv_color_hex(45 * 256 * 256 + 160 * 256 + 183));
  lv_style_set_text_color(&styleLabelBTN,  LV_STATE_DEFAULT, lv_color_hex(252 * 256 * 256 + 252 * 256 + 252));
  lv_style_set_text_opa(&styleLabelBTN, LV_STATE_DEFAULT, 150);
  lv_style_set_text_font(&styleLabelBTN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_style_set_bg_opa(&styleLabelBTN, LV_STATE_DEFAULT, 255);
  lv_style_set_bg_grad_color(&styleLabelBTN, LV_STATE_DEFAULT, lv_color_hex(10 * 256 * 256 + 19 * 256 + 24));
  lv_style_set_bg_main_stop(&styleLabelBTN, LV_STATE_DEFAULT, 20);
  lv_style_set_bg_grad_stop(&styleLabelBTN, LV_STATE_DEFAULT, 128);
  lv_style_set_radius(&styleLabelBTN, LV_STATE_DEFAULT, 20);
  lv_style_set_bg_grad_dir(&styleLabelBTN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_style_set_border_color(&styleLabelBTN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_style_set_border_opa(&styleLabelBTN, LV_STATE_DEFAULT, 255);
  lv_style_set_border_width(&styleLabelBTN, LV_STATE_DEFAULT, 1);
  lv_style_set_border_side(&styleLabelBTN, LV_STATE_DEFAULT, LV_BORDER_SIDE_FULL);
  lv_style_set_shadow_color(&styleLabelBTN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_style_set_shadow_opa(&styleLabelBTN, LV_STATE_DEFAULT, 200);
  lv_style_set_shadow_width(&styleLabelBTN, LV_STATE_DEFAULT, 30);
  lv_style_set_shadow_spread(&styleLabelBTN, LV_STATE_DEFAULT, 5);
  lv_style_set_pad_left(&styleLabelBTN, LV_STATE_DEFAULT, 8);
  lv_style_set_pad_right(&styleLabelBTN, LV_STATE_DEFAULT, 8);
  lv_style_set_pad_top(&styleLabelBTN, LV_STATE_DEFAULT, 4);
  lv_style_set_pad_bottom(&styleLabelBTN, LV_STATE_DEFAULT, 4);
  lv_style_set_text_color(&styleLabelBTN, LV_STATE_PRESSED, lv_color_hex(254 * 256 * 256 + 253 * 256 + 206));
  lv_style_set_text_opa(&styleLabelBTN, LV_STATE_PRESSED, 255);
  lv_style_set_text_font(&styleLabelBTN, LV_STATE_PRESSED, LV_THEME_DEFAULT_FONT_SUBTITLE);
  lv_style_set_shadow_color(&styleLabelBTN, LV_STATE_PRESSED, lv_color_hex(90 * 256 * 256 + 152 * 256 + 215));
  lv_style_set_shadow_opa(&styleLabelBTN, LV_STATE_PRESSED, 255);
  lv_style_set_shadow_width(&styleLabelBTN, LV_STATE_PRESSED, 40);
  lv_style_set_shadow_spread(&styleLabelBTN, LV_STATE_PRESSED, 6);
  lv_style_set_text_color(&styleLabelBTN, LV_STATE_DISABLED, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_style_set_text_opa(&styleLabelBTN, LV_STATE_DISABLED, 255);

  Screen1 = lv_obj_create(NULL, NULL);/*-------------------------- SCREEN1 ----------------------------*/
  lv_disp_load_scr(Screen1);/*  <<<<<<<<<<  IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT ------------*/
  Sfondo1 = lv_obj_create(Screen1, NULL);/*-------------------------- SFONDO1 BACKGROUND----------------------------*/
  lv_obj_clean_style_list(Sfondo1, LV_OBJ_PART_MAIN);
  lv_obj_set_click(Sfondo1, false);
  lv_obj_set_hidden(Sfondo1, false);
  lv_obj_clear_state(Sfondo1, LV_STATE_DISABLED);
  lv_obj_set_size(Sfondo1, 480, 320);  // force: 0
  lv_obj_align(Sfondo1, Screen1, LV_ALIGN_CENTER, 0, 0); // force: 480
  lv_obj_set_drag(Sfondo1, false);
  lv_obj_set_style_local_bg_color(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(48 * 256 * 256 + 169 * 256 + 192));
  lv_obj_set_style_local_bg_opa(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_bg_grad_color(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(6 * 256 * 256 + 38 * 256 + 52));
  lv_obj_set_style_local_bg_main_stop(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_bg_grad_stop(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 320);
  lv_obj_set_style_local_radius(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 50);
  lv_obj_set_style_local_bg_grad_dir(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_border_color(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_obj_set_style_local_border_opa(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_border_width(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 2);
  lv_obj_set_style_local_border_side(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_FULL);
  lv_obj_set_style_local_shadow_color(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_obj_set_style_local_shadow_opa(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_shadow_width(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 60);
  lv_obj_set_style_local_shadow_spread(Sfondo1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 10);

  /*FONT_SMALL  montserrat_12      FONT_NORMAL montserrat_14    FONT_SUBTITLE montserrat_20   FONT_TITLE montserrat_24*/
  LabelToast = lv_label_create(Screen1, NULL);/*-------------------------- LABEL INFO TOAST ----------------------------*/
  lv_label_set_long_mode(LabelToast, LV_LABEL_LONG_EXPAND);
  lv_label_set_align(LabelToast, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(LabelToast, "Info Text  click for close");
  lv_obj_set_size(LabelToast, 180, 22);  // force: 0
  lv_obj_set_click(LabelToast, true);
  lv_obj_set_hidden(LabelToast, false);
  lv_obj_clear_state(LabelToast, LV_STATE_DISABLED);
  lv_obj_set_drag(LabelToast, false);
  lv_obj_add_style(LabelToast, LV_OBJ_PART_MAIN, &styleLabelBTN);
  lv_obj_set_style_local_text_color(LabelToast, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(255 * 256 * 256 + 255 * 256 + 255));
  lv_obj_set_style_local_text_opa(LabelToast, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_text_font(LabelToast, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SMALL);
  lv_obj_set_style_local_text_font(LabelToast, LV_LABEL_PART_MAIN, LV_STATE_PRESSED, LV_THEME_DEFAULT_FONT_SMALL);
  lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); // force: 260
  lv_obj_set_event_cb(LabelToast, LabelToast_eventhandler);/*Assign an event function*/

  /*FONT_SMALL  montserrat_12      FONT_NORMAL montserrat_14    FONT_SUBTITLE montserrat_20   FONT_TITLE montserrat_24*/
  LabelWIFI = lv_label_create(Screen1, NULL);/*-------------------------- LABEL WIFI ----------------------------*/
  lv_label_set_long_mode(LabelWIFI, LV_LABEL_LONG_CROP);
  lv_label_set_align(LabelWIFI, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(LabelWIFI, "WIFI\n");
  lv_obj_set_size(LabelWIFI, 88, 30);
  lv_obj_set_click(LabelWIFI, true);
  lv_obj_set_hidden(LabelWIFI, false);
  lv_obj_set_drag(LabelWIFI, false);
  lv_obj_add_style(LabelWIFI, LV_OBJ_PART_MAIN, &styleLabelBTN);
  lv_obj_align(LabelWIFI, Screen1, LV_ALIGN_CENTER, -138, 140); // force: 88
  lv_obj_set_event_cb(LabelWIFI, LabelWIFI_eventhandler);/*Assign an event function*/

  LabelPREF = lv_label_create(Screen1, NULL);/*-------------------------- LABEL PREF ----------------------------*/
  lv_label_set_long_mode(LabelPREF, LV_LABEL_LONG_CROP);
  lv_label_set_align(LabelPREF, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(LabelPREF, "PREF");
  lv_obj_set_size(LabelPREF, 88, 30);
  lv_obj_set_click(LabelPREF, true);
  lv_obj_set_hidden(LabelPREF, false);
  lv_obj_clear_state(LabelPREF, LV_STATE_DISABLED);
  lv_obj_set_drag(LabelPREF, false);
  lv_obj_add_style(LabelPREF, LV_OBJ_PART_MAIN, &styleLabelBTN);
  lv_obj_align(LabelPREF, Screen1, LV_ALIGN_CENTER, 138, 140); // force: 88
  lv_obj_set_event_cb(LabelPREF, LabelPREF_eventhandler);/*Assign an event function*/

  /*FONT_SMALL  montserrat_12      FONT_NORMAL montserrat_14    FONT_SUBTITLE montserrat_20   FONT_TITLE montserrat_24*/
  RollerSSID = lv_roller_create(Screen1, NULL);/*-------------------------- ROLLER SSID ----------------------------*/
  lv_roller_set_options(RollerSSID, "SSID", LV_ROLLER_MODE_NORMAL);
  lv_roller_set_anim_time(RollerSSID, 100);
  lv_roller_set_align(RollerSSID, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_click(RollerSSID, true);
  lv_obj_set_hidden(RollerSSID, true);
  lv_obj_clear_state(RollerSSID, LV_STATE_DISABLED);
  lv_obj_set_drag(RollerSSID, false);
  lv_obj_align(RollerSSID, Screen1, LV_ALIGN_CENTER, 0, 0);
  lv_roller_set_visible_row_count(RollerSSID, 3);
  lv_obj_set_style_local_text_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_text_opa(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 150);
  lv_obj_set_style_local_text_font(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SMALL);
  lv_obj_set_style_local_bg_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_hex(100 * 256 * 256 + 163 * 256 + 192));
  lv_obj_set_style_local_bg_opa(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_bg_grad_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_hex(9 * 256 * 256 + 22 * 256 + 29));
  lv_obj_set_style_local_bg_main_stop(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_bg_grad_stop(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 600);
  lv_obj_set_style_local_radius(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 16);
  lv_obj_set_style_local_bg_grad_dir(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_border_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_obj_set_style_local_border_opa(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_border_width(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 1);
  lv_obj_set_style_local_border_side(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, LV_BORDER_SIDE_FULL);
  lv_obj_set_style_local_shadow_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_obj_set_style_local_shadow_opa(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 200);
  lv_obj_set_style_local_shadow_width(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 30);
  lv_obj_set_style_local_shadow_spread(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_text_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_CHECKED, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_text_opa(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_CHECKED, 255);
  lv_obj_set_style_local_text_font(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_SMALL);
  lv_obj_set_style_local_text_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_PRESSED, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_text_opa(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_PRESSED, 255);
  lv_obj_set_style_local_text_font(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_PRESSED, LV_THEME_DEFAULT_FONT_SMALL);
  lv_obj_set_style_local_shadow_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_PRESSED, lv_color_hex(90 * 256 * 256 + 152 * 256 + 215));
  lv_obj_set_style_local_shadow_opa(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_PRESSED, 200);
  lv_obj_set_style_local_shadow_width(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_PRESSED, 40);
  lv_obj_set_style_local_shadow_spread(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_PRESSED, 8);
  lv_obj_set_style_local_text_color(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_text_opa(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_CHECKED | LV_STATE_PRESSED, 255);
  lv_obj_set_style_local_text_font(RollerSSID, LV_ROLLER_PART_BG, LV_STATE_CHECKED | LV_STATE_PRESSED, LV_THEME_DEFAULT_FONT_SMALL);
  lv_obj_set_style_local_text_color(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_text_opa(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_text_font(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, LV_THEME_DEFAULT_FONT_NORMAL);
  lv_obj_set_style_local_bg_color(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_hex(105 * 256 * 256 + 143 * 256 + 164));
  lv_obj_set_style_local_bg_opa(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_bg_grad_color(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_hex(23 * 256 * 256 + 46 * 256 + 59));
  lv_obj_set_style_local_bg_main_stop(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_bg_grad_stop(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, 128);
  lv_obj_set_style_local_radius(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, 8);
  lv_obj_set_style_local_bg_grad_dir(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_shadow_color(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_hex(3 * 256 * 256 + 17 * 256 + 24));
  lv_obj_set_style_local_shadow_opa(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_shadow_width(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, 60);
  lv_obj_set_style_local_shadow_spread(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, 8);
  lv_obj_set_style_local_text_color(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_CHECKED, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_text_opa(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_CHECKED, 255);
  lv_obj_set_style_local_text_color(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_PRESSED, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_text_opa(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_PRESSED, 255);
  lv_obj_set_style_local_text_font(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_PRESSED, LV_THEME_DEFAULT_FONT_NORMAL);
  lv_obj_set_style_local_text_color(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_CHECKED | LV_STATE_PRESSED, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_text_opa(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_CHECKED | LV_STATE_PRESSED, 255);
  lv_obj_set_style_local_text_font(RollerSSID, LV_ROLLER_PART_SELECTED, LV_STATE_CHECKED | LV_STATE_PRESSED, LV_THEME_DEFAULT_FONT_NORMAL);
  lv_roller_set_auto_fit(RollerSSID, true);
  lv_obj_set_event_cb(RollerSSID, RollerSSID_eventhandler);

  /*FONT_SMALL  montserrat_12      FONT_NORMAL montserrat_14    FONT_SUBTITLE montserrat_20   FONT_TITLE montserrat_24*/
  LabelSSID = lv_label_create(Screen1, NULL);/*-------------------------- LABEL SSID ----------------------------*/
  lv_label_set_long_mode(LabelSSID, LV_LABEL_LONG_EXPAND);
  lv_label_set_align(LabelSSID, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(LabelSSID, "SSID");
  lv_obj_set_size(LabelSSID, 58, 38);  // force: -138
  lv_obj_set_click(LabelSSID, true);
  lv_obj_set_hidden(LabelSSID, true);
  lv_obj_clear_state(LabelSSID, LV_STATE_DISABLED);
  lv_obj_set_drag(LabelSSID, false);
  lv_obj_add_style(LabelSSID, LV_OBJ_PART_MAIN, &styleLabelBTN);
  lv_obj_align(LabelSSID, Screen1, LV_ALIGN_CENTER, -138, -90); // force: 58

  /*FONT_SMALL  montserrat_12      FONT_NORMAL montserrat_14    FONT_SUBTITLE montserrat_20   FONT_TITLE montserrat_24*/
  LabelPASSW = lv_label_create(Screen1, NULL);/*-------------------------- LABEL PASSW ----------------------------*/
  lv_label_set_long_mode(LabelPASSW, LV_LABEL_LONG_EXPAND);
  lv_label_set_align(LabelPASSW, LV_LABEL_ALIGN_CENTER);
  lv_label_set_text(LabelPASSW, "PASSW");
  lv_obj_set_size(LabelPASSW, 81, 38);  // force: -138
  lv_obj_set_click(LabelPASSW, true);
  lv_obj_set_hidden(LabelPASSW, true);
  lv_obj_clear_state(LabelPASSW, LV_STATE_DISABLED);
  lv_obj_set_drag(LabelPASSW, false);
  lv_obj_add_style(LabelPASSW, LV_OBJ_PART_MAIN, &styleLabelBTN);
  lv_obj_align(LabelPASSW, Screen1, LV_ALIGN_CENTER, -138, -40); // force: 81

  lv_style_init(&style_kb);/*Create a STYLE for keyboard */
  lv_style_set_bg_color(&style_kb, LV_STATE_DEFAULT, lv_color_hex(45 * 256 * 256 + 160 * 256 + 183));
  lv_style_set_bg_grad_color(&style_kb, LV_STATE_DEFAULT, lv_color_hex(10 * 256 * 256 + 19 * 256 + 24));
  lv_style_set_bg_grad_stop(&style_kb, LV_STATE_DEFAULT, 128);
  lv_style_set_bg_grad_dir(&style_kb,  LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_style_set_text_color(&style_kb, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_style_set_radius(&style_kb, LV_STATE_DEFAULT, 8);
  // lv_style_set_bg_opa(&style_kb, LV_BTN_STATE_PRESSED, LV_OPA_50);
  lv_style_set_bg_color(&style_kb, LV_BTN_STATE_PRESSED, lv_color_hex(90 * 256 * 256 + 152 * 256 + 215));

  kb_ta  = lv_textarea_create(Screen1, NULL);/*Create a text area and keyboard */
  lv_obj_set_size(kb_ta, screenWidth / 2, 24);
  lv_textarea_set_one_line(kb_ta, true);
  lv_textarea_set_text_align(kb_ta, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(kb_ta, NULL, LV_ALIGN_CENTER, 0, -20);
  lv_textarea_set_text(kb_ta, "");
  kb = lv_keyboard_create(Screen1, NULL);
  lv_obj_add_style(kb, LV_KEYBOARD_PART_BG, &style_kb);
  lv_obj_add_style(kb, LV_KEYBOARD_PART_BTN, &style_kb);
  lv_obj_set_size(kb,  screenWidth, screenHeight / 2);
  lv_obj_set_pos(kb, 0, 160);
  lv_keyboard_set_cursor_manage(kb, true);
  lv_obj_set_event_cb(kb, keyboard_event_cb);
  lv_keyboard_set_textarea(kb, kb_ta);
  lv_obj_set_hidden(kb_ta, true);
  lv_obj_set_hidden(kb, true);

  pageTable = lv_page_create(Screen1 , NULL);/*--------------------create a PAGE container for TABLE PREFERENCES -----------------------*/
  lv_obj_set_size(pageTable, 400, 160);
  lv_obj_align(pageTable, Screen1, LV_ALIGN_IN_TOP_MID, 0, 40);
  lv_obj_set_hidden(pageTable, true);
  lv_obj_set_click(pageTable, true);
  lv_obj_clear_state(pageTable, LV_STATE_DISABLED);
  lv_obj_set_style_local_bg_color(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(37 * 256 * 256 + 130 * 256 + 148));
  lv_obj_set_style_local_bg_opa(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_bg_grad_color(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(9 * 256 * 256 + 33 * 256 + 38));
  lv_obj_set_style_local_bg_main_stop(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_bg_grad_stop(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 200);
  lv_obj_set_style_local_radius(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 16);
  lv_obj_set_style_local_bg_grad_dir(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_clip_corner(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, true);
  lv_obj_set_style_local_border_width(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 1);
  lv_obj_set_style_local_border_color(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_obj_set_style_local_shadow_color(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0 * 256 * 256 + 0 * 256 + 0));
  lv_obj_set_style_local_shadow_opa(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_shadow_width(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 30);
  lv_obj_set_style_local_shadow_spread(pageTable, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 2);
  lv_page_set_scrollbar_mode(pageTable, LV_SCROLLBAR_MODE_OFF);

  tablePrefs = lv_table_create(pageTable, NULL);/*-------------------- TABLE PREFERENCES -----------------------*/
  //lv_obj_clean_style_list(tablePrefs, LV_OBJ_PART_MAIN);
  lv_obj_set_click(tablePrefs, true);
  lv_obj_clear_state(tablePrefs, LV_STATE_DISABLED);
  lv_obj_set_drag(tablePrefs, true);
  lv_obj_set_drag_dir(tablePrefs, LV_DRAG_DIR_ONE) ;
  lv_obj_set_hidden(tablePrefs, false);
  //lv_obj_set_scrollbar_mode(tablePrefs, LV_SCROLLBAR_MODE_AUTO)
  lv_obj_set_style_local_text_color(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(213 * 256 * 256 + 250 * 256 + 254));
  lv_obj_set_style_local_bg_color(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(37 * 256 * 256 + 130 * 256 + 148));
  lv_obj_set_style_local_bg_opa(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
  lv_obj_set_style_local_bg_grad_color(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(9 * 256 * 256 + 33 * 256 + 38));
  lv_obj_set_style_local_bg_main_stop(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_bg_grad_stop(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 200);
  lv_obj_set_style_local_radius(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 16);
  lv_obj_set_style_local_bg_grad_dir(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
  lv_obj_set_style_local_clip_corner(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, true);
  lv_obj_set_style_local_border_width(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_left(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_right(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_top(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_bottom(tablePrefs, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_table_set_col_cnt(tablePrefs, 3);
  lv_table_set_row_cnt(tablePrefs, 4);
  lv_obj_set_size(tablePrefs, 300, ((lv_table_get_row_cnt(tablePrefs) + 1) * 32));
  lv_table_set_col_width(tablePrefs, 0, 30); lv_table_set_col_width(tablePrefs, 1, 108); lv_table_set_col_width(tablePrefs, 2, 260);
  /*Fill the first column  T = pFileOut.readStringUntil('\n') ; T.trim(); TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1); */
  for (int i = 0; i < lv_table_get_row_cnt(tablePrefs) - 1; i++) {
    T = String(i); TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
    lv_table_set_cell_value(tablePrefs, i, 0, TT);
    lv_table_set_cell_value(tablePrefs, 0, 1, ">> EXIT <<");
    lv_table_set_cell_value(tablePrefs, 1, 1, "SSID");
    lv_table_set_cell_value(tablePrefs, 2, 1, "Password");
    lv_table_set_cell_value(tablePrefs, 3, 1, "Brightness");
    /*Fill the second column*/
    lv_table_set_cell_value(tablePrefs, 0, 2, "");
    lv_table_set_cell_value(tablePrefs, 1, 2, "Ilva1");
    lv_table_set_cell_value(tablePrefs, 2, 2, "pass");
    lv_table_set_cell_value(tablePrefs, 3, 2, "128");
  }
  lv_table_set_cell_merge_right(tablePrefs, 0, 1, true);
  lv_table_set_cell_align(tablePrefs, 0, 1, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(tablePrefs, pageTable, LV_ALIGN_IN_TOP_MID, 0, -1);
  lv_obj_set_event_cb(tablePrefs, tablePrefs_eventhandler);

  ledcWrite(0, Brightness); // Start @ initial Brightnes
  listDir(SPIFFS, "/", 0);
  loadPref( );
}
/*   -------------------    END SETUP               END SETUP                     END SETUP                  END SETUP  ---------------------*/

void loop() {/*   ======================      START LOOP     START LOOP     START LOOP     START LOOP       =======================*/
  /* xSemaphoreTake(xSemaphore, portMAX_DELAY);
    while (1) {
    lv_task_handler();
    delay(5);
    }
    xSemaphoreGive(xSemaphore);
    delay(5);*/
  /*while (1) {*/
  lv_task_handler();
  if ( FtpOnOff == "ON")   ftpSrv.handleFTP();
  delay(1);

  if (Serial.available() > 0) {
    // Read the command byte
    command = Serial.readStringUntil('\n');
    command.trim();// elimina LF e CR
    // Serial.print("String received "); Serial.println(command);
    char commandChar = command.charAt(0);
    switch (commandChar) {
      case '1' : { /* ---------- SCREENSHOT -------- */
          long posizione = command.indexOf("/");
          if (posizione != -1) {
            TipoFormato = 1; /* BMP header by default*/
            String nome = command.substring(posizione); nome.toLowerCase();
            if (nome.endsWith(".raw")) {
              TipoFormato = 2;
            }
            int lung = nome.length();
            char BufSer [lung + 1];
            nome.toCharArray(BufSer, lung + 1);
            BytesWrites = 0;
            guiTakeScreenshot(SPIFFS, BufSer);
          } else {
            Serial.println("/ char missed");
          }
          break;
        }
      case '2' : { /* ---------- DIRECTORY -------- */
          listDir(SPIFFS, "/", 0);
          break;
        }
      case '3': {/* ---------- WRITE FILE BINARY TO HEX-------- */
          long posizione = command.indexOf("/");
          if (posizione != -1) {
            String nome = command.substring(posizione);
            int lung = nome.length();
            char BufSer [lung + 1];
            nome.toCharArray(BufSer, lung + 1);
            readFileH(SPIFFS, BufSer);
          } else {
            Serial.println("/ char missed");
          }
          break;
        }
      case '4' : {/* ---------- ERASE-------- */
          long posizione = command.indexOf("/");
          if (posizione != -1) {
            String nome = command.substring(posizione);
            int lung = nome.length();
            char BufSer [lung + 1];
            nome.toCharArray(BufSer, lung + 1);
            deleteFile(SPIFFS, BufSer);
          } else {
            Serial.println("/ char missed");
          }
          break;
        }
      case '5' : { /* ---------- FORMAT-------- */
          formatto();
          break;
        }
      case 't': {/* ---------- WRITE FILE TO SERIAL -------- */
          long posizione = command.indexOf("/");
          if (posizione != -1) {
            String nome = command.substring(posizione);
            int lung = nome.length();
            char BufSer [lung + 1];
            nome.toCharArray(BufSer, lung + 1);
            readFile(SPIFFS, BufSer);
          } else {
            Serial.println("/ char missed");
          }
          break;
        }
      default:
        Serial.println("Command not recognized");
        break;
    }
  }
}/*   ----------------------------------------------- END LOOP     END LOOP      END LOOP      END LOOP        END LOOP  -----------------------------------------------*/

/* -----------  SAVE PREFERENCES  --------------*/
static void savePref( ) {
  pFileOut = SPIFFS.open("/Pref.txt", "w"); /* 1. Open a stream for writing*/
  /*SPIFFS open function needs path and mode:  mode = "r", "w", "a", "r+", "w+", "a+" (text file) or "rb", "wb", "ab", "rb+", "wb+", "ab+" (binary)
    where r = read, w = write, a = append   + means file is open for update (read and write)   b means the file os open for binary operations*/
  if (pFileOut) {
    T = lv_table_get_row_cnt(tablePrefs) - 1 ; pFileOut.println(T); /* number of ROW */
    Serial.print("get_row_cnt = "); Serial.println(T);
    for (int i = 0; i < lv_table_get_row_cnt(tablePrefs)  ; i++) {
      T = lv_table_get_cell_value(tablePrefs, i, 0); pFileOut.println(T);
      T = lv_table_get_cell_value(tablePrefs, i, 1); pFileOut.println(T);
      T = lv_table_get_cell_value(tablePrefs, i, 2); pFileOut.println(T);
    }
    pFileOut.close(); // 7. Close file
    ssid = lv_table_get_cell_value(tablePrefs, 1, 2);  wifipw = lv_table_get_cell_value(tablePrefs, 2, 2);
    T = lv_table_get_cell_value(tablePrefs, 3, 2);   Brightness = T.toInt(); ledcWrite(0, Brightness);
    T = lv_table_get_cell_value(tablePrefs, 4, 2); WifiLimit = T.toInt();
    FtpOnOff = lv_table_get_cell_value(tablePrefs, 5, 2);
    lv_label_set_text(LabelToast, "PREFERENCES SAVED"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_set_hidden(LabelToast, false); lv_obj_move_foreground(LabelToast);
  } else {
    Serial.print("cannot write PREFERENCES");
    lv_label_set_text(LabelToast, "ERROR Cannot write PREFERENCES"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10);
    lv_obj_set_hidden(LabelToast, false); lv_obj_move_foreground(LabelToast);
  }
}
/* -----------  LOAD PREFERENCES  --------------*/
static void loadPref( ) {
  pFileOut = SPIFFS.open("/Pref.txt", "r"); /* 1. Open a stream for writing */
  /*SPIFFS open function needs path and mode:  mode = "r", "w", "a", "r+", "w+", "a+" (text file) or "rb", "wb", "ab", "rb+", "wb+", "ab+" (binary)
    where r = read, w = write, a = append   + means file is open for update (read and write)   b means the file os open for binary operations*/
  if (SPIFFS.exists("/Pref.txt")) {
    int roww = 0; int coll = 0;
    T = pFileOut.readStringUntil('\n') ; T.trim();
    int RowCount = T.toInt();
    lv_table_set_row_cnt(tablePrefs, RowCount);
    while (roww <= RowCount) {
      T = pFileOut.readStringUntil('\n') ; T.trim(); TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
      lv_table_set_cell_value(tablePrefs, roww, coll, TT); coll ++;
      if (coll == 3) {
        coll = 0; roww ++;
      }
    }
    pFileOut.close(); // 7. Close file
    ssid = lv_table_get_cell_value(tablePrefs, 1, 2);    wifipw = lv_table_get_cell_value(tablePrefs, 2, 2);
    T = lv_table_get_cell_value(tablePrefs, 3, 2); Brightness = T.toInt(); ledcWrite(0, Brightness);
    T = lv_table_get_cell_value(tablePrefs, 4, 2); WifiLimit = T.toInt();
    FtpOnOff = lv_table_get_cell_value(tablePrefs, 5, 2);
    lv_label_ins_text(LabelToast, LV_LABEL_POS_LAST, "\nPREFERENCES FOUND"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
  } else {
    Serial.print("PREFERENCES /Pref.txt NOT FOUND");
    lv_label_ins_text(LabelToast, LV_LABEL_POS_LAST, "\nPREFERENCES /Pref.txt NOT FOUND"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
    NumeroNetwork = WiFi.scanNetworks();
    if (NumeroNetwork == 0) {
      lv_obj_set_state(LabelWIFI, LV_STATE_DISABLED);
    }
  }
}
/* ----------------  WIFI and FTP CONNECTION ------------------ */
void wifiConn() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  ssid.trim(); wifipw.trim();
  char ssid_char[40]; char wifipw_char[40];
  ssid.toCharArray(ssid_char, ssid.length() + 1); wifipw.toCharArray(wifipw_char, wifipw.length() + 1);
  WiFi.begin(ssid_char, wifipw_char);
  int maxRepeat = 0;
  while (WiFi.status() != WL_CONNECTED && maxRepeat < 20) {
    delay(500);
    Serial.print("Connecting to WiFi..  "); Serial.println(ssid.c_str());
    maxRepeat++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    lv_label_set_text(LabelToast, "Connected to the WiFi network\n");
    /*T += "\nFREE SPACE:  "; T += String (availBytes); T += " bytes";*/
    T = ssid; T += "\nLocal IP = "; T += WiFi.localIP().toString().c_str();
    //    TT[(T.length() + 1)];  T.toCharArray(TT, T.length() + 1);
    //    lv_label_ins_text(LabelToast, LV_LABEL_POS_LAST, TT); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10);
    Serial.print("Connected to the WiFi network ");    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
    lv_obj_set_style_local_shadow_color(LabelWIFI, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(90 * 256 * 256 + 152 * 256 + 215));
    lv_obj_set_style_local_text_color(LabelWIFI, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(254 * 256 * 256 + 253 * 256 + 206));
    lv_obj_set_style_local_text_opa(LabelWIFI, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 255);
    T += "\nMac: "; T += String(WiFi.macAddress());
    T += "\nMax Free Heap: "; T += String(ESP.getMaxAllocHeap());
    T += "\nFTP User = Esp32"; T += "\nFTP Pass = Esp32"; T += "\nFTP Connection is set to "; T += FtpOnOff ;
    TT[(T.length() + 1)];  T.toCharArray(TT, T.length() + 1);
    lv_label_ins_text(LabelToast, LV_LABEL_POS_LAST, TT); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
    //lv_refr_now(NULL);
    Serial.println(FtpOnOff);
    if (FtpOnOff == "ON") ftpSrv.begin("Esp32", "Esp32"); 
    Connessione = 2;
  } else {
    Serial.println("Timeout");
    lv_label_set_text(LabelToast, "TIMEOUT"); lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
  }
}
/* ========================  SCREENSHOT  =========================*/

void guiTakeScreenshot(fs::FS & fs, const char * pFileName) {
  pFileOut = fs.open(pFileName, "w"); // 1. Open a stream for writing
  /*SPIFFS open function needs path and mode:  mode = "r", "w", "a", "r+", "w+", "a+" (text file) or "rb", "wb", "ab", "rb+", "wb+", "ab+" (binary)
    where r = read, w = write, a = append   + means file is open for update (read and write)   b means the file os open for binary operations*/
  if (pFileOut) {
    if (TipoFormato == 1 ) {
      gui_get_bitmap_header(BMPHeader, sizeof(BMPHeader));
      size_t len = pFileOut.write(BMPHeader, 122);
      /* Write a 122 bytes BMP fileheader, describing the pixelformat */
      Serial.println(F("Start write BMP Header"));
      if (len != 122) {
        Serial.println("Fail to write BMP header...abort");
        return;
      }
    }
    /*  Temporarily swap the flush_cb*/
    /* Get current callback */
    lv_disp_t * disp_drv = lv_disp_get_default();
    void (*flush_cb)(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
    flush_cb = disp_drv->driver.flush_cb; /* store callback */
    /* Set the screenshot callback */
    disp_drv->driver.flush_cb = gui_screenshot_to_file;
    /* 4. Refresh screen to call the screenshot callback */
    lv_obj_invalidate(lv_scr_act());
    /* 5. Captures the refreshes to the stream, also also update the tft */
    lv_refr_now(NULL);
    /* 6. Restore callback */
    disp_drv->driver.flush_cb = flush_cb; /* restore callback */
    Serial.print("Bitmap data flushed to "); Serial.println(pFileName);
    pFileOut.close(); // 7. Close file
    Serial.println("==========   END SCREENSHOT   =============");
    Serial.print("Writes Tot= "); Serial.print(BytesWrites); Serial.print("  + 122 bytes IF BMP header = "); Serial.println((BytesWrites + 122));
    Serial.print(screenWidth); Serial.print("x"); Serial.print(screenHeight); Serial.print(" * 2 = "); Serial.println((screenWidth * screenHeight * 2));
  } else {
    Serial.print("cannot write "); Serial.println(pFileName);
  }
}
///* Flush VDB bytes to a file  */
static void gui_screenshot_to_file(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p) {
  size_t len = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1); /* Number of pixels */
  len *= sizeof(lv_color_t);
  int aaa = 0;/* how many bytes is written actually*/
  while (aaa < len) {/* if the number of bytes written on SPIFFS is equal to the number of bytes of the buffer it exits the cycle */
    size_t res = pFileOut.write((uint8_t *)color_p + aaa , len - aaa);
    aaa = aaa + res;    BytesWrites += res;
    if (len != aaa) Serial.println("  retry write on SPIFFS");
    delay(0);
  }
  Serial.print("Flush... written = "); Serial.println((String)BytesWrites);
  my_disp_flush(disp, area, color_p);
}

static void gui_get_bitmap_header(uint8_t * buffer1, size_t bufsize) { /* ========== BMP HEADER format 4 =========*/
  //memset(buffer1, 0, bufsize);
  //lv_disp_t * disp = lv_disp_get_default();
  buffer1[0]        = 0x42; // B
  buffer1[1]        = 0x4D; // M
  buffer1[10 + 0] = 122;      // full header size
  buffer1[14 + 0] = 122 - 14; // dib header size
  buffer1[26 + 0] = 1;        // number of color planes
  buffer1[28 + 0] = 16;       // 16 or 24, bbp
  buffer1[30 + 0] = 3;        // compression, 0 = RGB / 3 = RGBA
  // file size
  guiSetBmpHeader(&buffer1[2], 122 + screenWidth * screenHeight * buffer1[28] / 8);
  // horizontal resolution
  guiSetBmpHeader(&buffer1[18], screenWidth);
  // vertical resolution
  guiSetBmpHeader(&buffer1[22], -screenHeight);
  // bitmap size
  guiSetBmpHeader(&buffer1[34], screenWidth * screenHeight * buffer1[28 + 0] / 8);
  // horizontal pixels per meter
  guiSetBmpHeader(&buffer1[38], 2836);
  // vertical pixels per meter
  guiSetBmpHeader(&buffer1[42], 2836);
  // R: 1111 1000 | 0000 0000
  guiSetBmpHeader(&buffer1[54], 0xF800); // Red bitmask
  // G: 0000 0111 | 1110 0000
  guiSetBmpHeader(&buffer1[58], 0x07E0); // Green bitmask
  // B: 0000 0000 | 0001 1111
  guiSetBmpHeader(&buffer1[62], 0x001F); // Blue bitmask
  // A: 0000 0000 | 0000 0000
  guiSetBmpHeader(&buffer1[66], 0x0000); // No Aplpha Mask
  // "Win
  buffer1[70 + 3] = 0x57;
  buffer1[70 + 2] = 0x69;
  buffer1[70 + 1] = 0x6E;
  buffer1[70 + 0] = 0x20;
}
static void guiSetBmpHeader(uint8_t * buffer_p, int32_t data) {
  *buffer_p++ = data & 0xFF;
  *buffer_p++ = (data >> 8) & 0xFF;
  *buffer_p++ = (data >> 16) & 0xFF;
  *buffer_p++ = (data >> 24) & 0xFF;
}
/* ----------------------------------  END SCREENSHOT  ----------------------------*/

/* ========================  SERIAL COMMANDS  =========================*/
void listDir(fs::FS & fs, const char * dirname, uint8_t levels) {/* LIST DIRECTORY */
  unsigned int totalBytes = SPIFFS.totalBytes();
  unsigned int usedBytes = SPIFFS.usedBytes();
  unsigned int availBytes = totalBytes - usedBytes;
  String  T = "\n===== SPIFFS SYSTEM INFO =====";
  T += "\nTotal space:   "; T += String(totalBytes); T += " bytes";
  T += "\nUsed space:  "; T += String( usedBytes); T += " bytes";
  T += "\nFREE SPACE:  "; T += String (availBytes); T += " bytes";
  TT[(T.length() + 1)];
  T.toCharArray(TT, T.length() + 1);
  lv_label_set_text(LabelToast, "Info Text Label   click for close"); lv_label_refr_text(LabelToast);
  // lv_label_set_text(LabelToast,  lv_label_get_text(LabelToast) & TT);
  lv_label_ins_text(LabelToast, LV_LABEL_POS_LAST, TT);
  lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10);
  lv_obj_set_hidden(LabelToast, false); lv_obj_move_foreground(LabelToast);
  Serial.println(T);
  //  Serial.println("===== FILE SYSTEM INFO =====");
  //  Serial.print("Total space:      ");  Serial.print(totalBytes);  Serial.println(" byte");
  //  Serial.print("Total space used: ");  Serial.print(usedBytes);  Serial.println(" byte");
  //  Serial.print("FREE SPACE  :      ");  Serial.print(availBytes);  Serial.println(" byte");
  Serial.println();
  T = "\n>>>> LISTING DIRECTORY <<<";
  Serial.printf(">>>>> LISTING DIRECTORY: %s\r\n", dirname);
  File root = fs.open(dirname);
  if (!root) {
    Serial.println("− failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" − not a directory");
    return;
  }
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      T += "\n>FILE: "; T += String( file.name()); T += " Size: "; T += String( file.size());
      Serial.print(">>>>>  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  file.close();
  TT[(T.length() + 1)]; T.toCharArray(TT, T.length() + 1);
  lv_label_ins_text(LabelToast, LV_LABEL_POS_LAST, TT);
  lv_obj_align(LabelToast, Screen1, LV_ALIGN_IN_TOP_MID, 0, 10); lv_obj_move_foreground(LabelToast);
  Comandi();
}
/* ----------- READ FILE BINARY TO HEX----------*/
void readFileH(fs::FS & fs, const char * path) {
  size_t len = 0;
  size_t i = 0;
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "rb");
  file.setTimeout(0);
  if (file && !file.isDirectory()) {
    len = file.size();
    size_t flen = len;
    Serial.println("- reading" );
    uint32_t sstart = millis();
    uint32_t eend = sstart;
    for (i = 0; i < len; i++) {
      incomingByte = file.read();
      if ( incomingByte < 16) {/* need put a 0 more or we get a 5 and not a 05 in hex result when byte is from 0 to 15*/
        Serial.print("0");
      }
      Serial.print(incomingByte, HEX);
    }
    eend = millis() - sstart;
    Serial.println(""); Serial.println("");
    file.close();
    Serial.printf("- %u bytes read in %u ms\r\n", flen, eend);
    Serial.println("----- FINISH -----");
  } else {
    Serial.println("- failed to open file for reading");
  }
}
void readFile(fs::FS & fs, const char * path) {/* ----------- READ FILE BINARY AND WRITE TO SERIAL----------*/
  //File file = fs.open(path, "rb");/* read binary mode */
  File file = fs.open(path);
  file.setTimeout(0);
  if (file && !file.isDirectory()) {
    size_t len = file.size();
    for (int i = 0; i < len; i++) {
      Serial.print((char)file.read());
      //      incomingByte = file.read();
      //      Serial.write(incomingByte);
    }
    file.close();
  } else {
    Serial.println("- failed to open file for reading");
  }
}
/* ----------- DELETE FILE  ----------*/
void deleteFile(fs::FS & fs, const char * path) {
  Serial.printf("Deleting file: %s\r\n", path);
  Serial.println(">>>" + (String) path + "<<<");
  if (fs.remove(path)) {
    Serial.println("− file deleted");
  } else {
    Serial.println("− delete failed");
  }
}
/* ----------- FORMAT  ----------*/
void formatto() {
  Serial.println("Format SPIFF");
  SPIFFS.format();
  SPIFFS.begin();
  Serial.println("SPIFF FORMATTED");
}
void Comandi() {
  Serial.println("");
  Serial.println("========== LIST OF COMMANDS ==========");
  Serial.println("1 /name   << Save Screenshot to SPIFF (example: 1 /shot.bmp or .raw without bmp header)");
  Serial.println("2         << Read Directory List of SPIFF");
  Serial.println("3 /name   << Read file Binary TO HEX   (example: 3 /Pippo/shot.bmp and put HEX data output to serial monitor)");
  Serial.println("4 /name   << Delete file  (example: 4 /shot.bmp' for delete file shot.bmp)");
  Serial.println("5         << Format SPIFF and erase all content");
  Serial.println("t /name   << Send file from SPIFFS to serial in binary mode for capture with Realterm(need close serial monitor)");
  Serial.println("======================================");
}/* ----------------------------------  END SERIAL COMMANDS  ----------------------------*/
//void printEvent(String Event, lv_event_t event) {
//  Serial.print(Event);
//  printf(" ");
//  switch (event) {
//    case LV_EVENT_PRESSED:
//      printf("Pressed\n");
//      break;
//    case LV_EVENT_SHORT_CLICKED:
//      printf("Short clicked\n");
//      break;
//    case LV_EVENT_CLICKED:
//      printf("Clicked\n");
//      break;
//    case LV_EVENT_LONG_PRESSED:
//      printf("Long press\n");
//      break;
//    case LV_EVENT_LONG_PRESSED_REPEAT:
//      printf("Long press repeat\n");
//      break;
//    case LV_EVENT_RELEASED:
//      printf("Released\n");
//      break;
//  }
//}
