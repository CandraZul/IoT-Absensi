#include <SPI.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "ui.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> 

const char* ssid = "enumatechz";
const char* password = "3numaTechn0l0gy";

//Your Domain name with URL path or IP address with path
String serverName = "http://192.168.1.6:8080/api/users";
String apiCheckin = "http://192.168.1.6:8080/api/attendance";

// Defines the T_CS Touchscreen PIN.
#define T_CS_PIN  13 //--> T_CS

// Defines the screen resolution.
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// Defines the Touchscreen calibration result values.
// Replace with your own calibration values.
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

#define LED_PIN 27

// LVGL draw into this buffer, 1/10 screen size usually works well.
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint8_t *draw_buf;

uint32_t lastTick = 0;

// Declaring the "XPT2046_Touchscreen" object as "touchscreen".
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

struct EventData {
  int id;
  String type;
};

//________________________________________________________________________________ 
// If logging is enabled, it will inform the user about what is happening in the library.
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();

    // Validasi untuk memastikan bahwa sentuhan valid
    float alpha_x, beta_x, alpha_y, beta_y, delta_x, delta_y;
    alpha_x = 0.000;
    beta_x = 0.089;
    delta_x = -26.650;
    alpha_y = 0.066;
    beta_y = -0.001;
    delta_y = -14.171;

    // Hitung nilai x dan y setelah kalibrasi
    int calibrated_x = alpha_y * p.x + beta_y * p.y + delta_y;
    calibrated_x = min(SCREEN_WIDTH - 1, max(0, calibrated_x));

    int calibrated_y = alpha_x * p.x + beta_x * p.y + delta_x;
    calibrated_y = min(SCREEN_HEIGHT - 1, max(0, calibrated_y));

    int pressure = p.z; // Tekanan sentuhan

    // Periksa apakah tekanan cukup besar untuk dianggap sebagai sentuhan valid
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = calibrated_x;
    data->point.y = calibrated_y;

    Serial.print("X = ");
    Serial.print(calibrated_x);
    Serial.print(" | Y = ");
    Serial.print(calibrated_y);
    Serial.print(" | Pressure = ");
    Serial.println(pressure);
    
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}


// Event handler for the "Register" button.
static void button_register_event_handler(lv_event_t * e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    Serial.println("pindah ke reg");
    // get_table_data();
    String status = "Input Member ID";
    lv_label_set_text(objects.label_input_member, status.c_str());
    lv_scr_load(objects.reg_screen);  // Go to Register screen.
  }
}

// Event handler for the "Attendance" button.
static void button_attendance_event_handler(lv_event_t * e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    Serial.println("pindah ke attendance");
    lv_scr_load(objects.attendance_screen);  // Go to Attendance screen.
  }
}

static void button_home_event_handler(lv_event_t * e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    Serial.println("kembali ke home");
    lv_scr_load(objects.main);  // Go to Register screen.
  }
}

static void button_verification_event_handler(lv_event_t * e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    Serial.println("kembali ke home");
    lv_scr_load(objects.verification_screen);  // Go to Register screen.
  }
}

// static void add_finger_screen_event_handler(const String& name) {
//   // Menggunakan *name untuk mengakses nilai yang ditunjuk oleh pointer
//   String labelText = name; // Gabungkan nama dengan string
//   lv_label_set_text(objects.label_finger_name, labelText.c_str()); // Gunakan c_str() untuk mengonversi String ke const char*

//   lv_scr_load(objects.add_finger_screen); // Pindah ke layar yang ditentukan
// }

// static void table_event_handler(lv_event_t *e) {
//   lv_obj_t * obj = (lv_obj_t *)lv_event_get_current_target_obj(e);
//   uint32_t col;
//   uint32_t row;
//   lv_table_get_selected_cell(obj, &row, &col);
//   Serial.print("Baris yang dipilih: ");
//   Serial.println(row);
//   Serial.print("Kolom yang dipilih: ");
//   Serial.println(col);
//   if(row > 0){
//     const char* cell_value = lv_table_get_cell_value(obj, row, 1);
//     // Mengonversi const char* menjadi String
//     if (cell_value != nullptr && strlen(cell_value) > 0) {  // Periksa apakah nilai tidak null dan tidak kosong
//       String cell_value_str = String(cell_value);  // Mengonversi const char* menjadi String
//       add_finger_screen_event_handler(cell_value_str);  // Panggil fungsi handler dengan nilai sel
//     } else {
//       Serial.println("Nilai sel kosong.");
//     }
//   }
// }

static void get_table_data(String id){
  // Get data from the server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Build server path
    String serverPath = serverName + "/" + id;

    // Connect to the server
    http.begin(serverPath.c_str());

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      String payload = http.getString();
      Serial.println("Received Payload:");
      // Serial.println(payload);

      // Parse JSON payload
      StaticJsonDocument<1024> doc;  // Adjust buffer size as needed
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        // Extract members array
        String name = doc["users"]["name"].as<String>();
        String category = doc["users"]["category_name"].as<String>();
        if(name != "null"){
          // Insert ID and Name into the table
          lv_table_set_cell_value(objects.table_user, 1, 0, id.c_str());
          lv_table_set_cell_value(objects.table_user, 1, 1, name.c_str());
          lv_table_set_cell_value(objects.table_user, 1, 2, category.c_str());

          
          lv_scr_load(objects.add_finger_screen);
        }else{
          String status = "Member Not Found";
          lv_label_set_text(objects.label_input_member, status.c_str());
        }

      } else {
        Serial.print("JSON Deserialization failed: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}

static void checkin_event_handler(lv_event_t *e){
  EventData* checkin = (EventData*)lv_event_get_user_data(e);

  int id = checkin->id;
  String type = checkin->type;

  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String serverPath = apiCheckin;
    // Connect to the server
    http.begin(serverPath.c_str());
    // Set HTTP headers (for JSON)
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<1024> doc;
    doc["user_id"] = id;

    String requestBody;
    serializeJson(doc, requestBody);

    // Send HTTP POST request
    int httpResponseCode = http.POST(requestBody);

     if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      String payload = http.getString();
      Serial.println("Received Payload:");
      Serial.println(payload);

      // Parse JSON payload
      StaticJsonDocument<1024> responseDoc;
      DeserializationError error = deserializeJson(responseDoc, payload);
      String status = responseDoc["status"].as<String>();
      if (status == "success") {
        // Handle response (for example, extract data)
        String responseMessage = responseDoc["message"].as<String>();
        Serial.println("Response message: " + responseMessage);
        String name = responseDoc["user"]["name"].as<String>();
        lv_label_set_text(objects.label_name_popup, name.c_str());
        String type = responseDoc["data"]["status"].as<String>();
        if(type=="departed"){
          showPopupSuccess("Goodbye");
        }else{
          showPopupSuccess("Welcome");
        }
      } else {
        String status = responseDoc["message"];
        showPopupError(status);
        Serial.print("JSON Deserialization failed: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      String status = "Problem with server";
      showPopupError(status);
    }
  }
}

void hidePopupCallback(lv_timer_t *timer) {
    lv_obj_add_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN); // Sembunyikan panel
    lv_timer_del(timer); // Hapus timer setelah selesai
}

void showPopupSuccess(String status) {
    lv_label_set_text(objects.label_status, status.c_str());
    lv_obj_clear_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN); // Tampilkan panel
    lv_timer_t *timer = lv_timer_create(hidePopupCallback, 3000, NULL); // Buat timer untuk menyembunyikan panel
}

void hidePopupErrorCallback(lv_timer_t *timer) {
    lv_obj_add_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN); // Sembunyikan panel
    lv_obj_add_flag(objects.pic_error, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.pic_success, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.label_name_popup, LV_OBJ_FLAG_HIDDEN);
    lv_timer_del(timer); // Hapus timer setelah selesai
}

void showPopupError(String message) {
    lv_obj_clear_flag(objects.pic_error, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.pic_success, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(objects.label_name_popup, message.c_str());
    String status = "Error";
    lv_label_set_text(objects.label_status, status.c_str());
    lv_obj_clear_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN); // Tampilkan panel
    lv_timer_t *timer = lv_timer_create(hidePopupErrorCallback, 3000, NULL); // Buat timer untuk menyembunyikan panel
}

void hidePopupVerification(lv_timer_t *timer){
  lv_obj_add_flag(objects.popup_verification, LV_OBJ_FLAG_HIDDEN);
  lv_timer_del(timer);
}

static void password_event_handler(lv_event_t *e){
  int btn_index = lv_keyboard_get_selected_btn(objects.keyboard_password);
  String password = lv_textarea_get_text(objects.input_password);
  Serial.println(btn_index);
  Serial.println(password);
  if(btn_index == 11){
    if(password == "123456"){
      String status = "Input Member ID";
      lv_label_set_text(objects.label_input_member, status.c_str());
      lv_scr_load(objects.reg_screen);
    }else{
      Serial.print("pasword salah");
      lv_obj_clear_flag(objects.popup_verification, LV_OBJ_FLAG_HIDDEN);
      lv_timer_t *timer = lv_timer_create(hidePopupVerification, 1000, NULL);
    }
    lv_textarea_set_text(objects.input_password, ""); 
  }
}

static void get_user_event_handler(lv_event_t *e){
    int btn_index = lv_keyboard_get_selected_btn(objects.keyboard_id);
    if(btn_index == 11){
      String id = lv_textarea_get_text(objects.input_id);
      get_table_data(id);
      lv_textarea_set_text(objects.input_id, "");
    }
}


//________________________________________________________________________________ 
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  // delay(3000);
  
  Serial.println("ESP32 + TFT LCD Touchscreen ILI9341 + LVGL + EEZ Studio");
  delay(500);

  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println(LVGL_Arduino);
  delay(500);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize the touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 3: touchscreen.setRotation(3);
  touchscreen.setRotation(2);

  // Initialize LVGL
  lv_init();
  lv_log_register_print_cb(log_print);

  // Create display buffer
  draw_buf = new uint8_t[DRAW_BUF_SIZE];
  lv_display_t * disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, DRAW_BUF_SIZE);
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);

  // Initialize LVGL input device (Touchscreen)
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  Serial.println("LVGL Setup Completed.");
  delay(500);

  // Initialize EEZ Studio GUI
  ui_init();

  // Register button event handlers
  lv_obj_add_event_cb(objects.button_register, button_verification_event_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.button_attendance, button_attendance_event_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.button_back_1, button_home_event_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.button_back_2, button_home_event_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.button_back_3, button_register_event_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.button_back_4, button_home_event_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN);

  EventData* checkin = new EventData;
  checkin->id = 1;

  lv_obj_add_event_cb(objects.button_checkin, checkin_event_handler, LV_EVENT_CLICKED, checkin);

  lv_obj_add_event_cb(objects.keyboard_password, password_event_handler, LV_EVENT_CLICKED, NULL);
  lv_obj_add_event_cb(objects.keyboard_id, get_user_event_handler, LV_EVENT_CLICKED, NULL);

  lv_table_set_col_cnt(objects.table_user, 3);
  lv_table_set_col_width(objects.table_user, 0, 60); 
  lv_table_set_col_width(objects.table_user, 1, 140); 
  lv_table_set_col_width(objects.table_user, 2, 110); 

  lv_table_set_row_count(objects.table_user, 2);
  lv_obj_set_style_text_font(objects.table_user, &lv_font_montserrat_12, LV_PART_ITEMS | LV_STATE_DEFAULT);

  lv_table_set_cell_value(objects.table_user, 0, 0, "ID");
  lv_table_set_cell_value(objects.table_user, 0, 1, "Name");
  lv_table_set_cell_value(objects.table_user, 0, 2, "Category");
}

void loop() {
  lv_tick_inc(millis() - lastTick); // Update the tick timer.
  lastTick = millis();
  lv_timer_handler(); // Update the UI.
  delay(5);
}
