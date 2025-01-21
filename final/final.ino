#include <SPI.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "ui.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Fingerprint.h>

const char *ssid = "enumatechz";
const char *password = "3numaTechn0l0gy";

//Your Domain name with URL path or IP address with path
String serverName = "https://absen.enumatechnology.com/api/users";
String apiCheckin = "https://absen.enumatechnology.com/api/attendance";
String authToken = "jgk0advefk90gj4ngin4290";

// Defines the T_CS Touchscreen PIN.
#define T_CS_PIN 13  //--> T_CS

// Defines the screen resolution.
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Defines the Touchscreen calibration result values.
// Replace with your own calibration values.
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

// LVGL draw into this buffer, 1/10 screen size usually works well.
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

HardwareSerial mySerial(2);  // UART2 untuk ESP32
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t *draw_buf;

uint32_t lastTick = 0;

// Declaring the "XPT2046_Touchscreen" object as "touchscreen".
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

int finger_id;

//________________________________________________________________________________
// If logging is enabled, it will inform the user about what is happening in the library.
void log_print(lv_log_level_t level, const char *buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data) {
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

    int pressure = p.z;  // Tekanan sentuhan

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
static void button_register_event_handler(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    Serial.println("pindah ke reg");
    // get_table_data();
    String status = "Input Member ID";
    lv_label_set_text(objects.label_input_member, status.c_str());
    lv_scr_load(objects.reg_screen);  // Go to Register screen.
  }
}

// Event handler for the "Attendance" button.
static void button_attendance_event_handler(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    Serial.println("pindah ke attendance");
    lv_scr_load(objects.attendance_screen);  // Go to Attendance screen.
  }
}

static void button_home_event_handler(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    Serial.println("kembali ke home");
    lv_scr_load(objects.main);  // Go to Register screen.
  }
}

static void button_verification_event_handler(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    Serial.println("kembali ke home");
    lv_scr_load(objects.verification_screen);  // Go to Register screen.
  }
}


static void get_table_data(String id) {
  // Get data from the server
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Build server path
    String serverPath = serverName + "/" + id;

    // Connect to the server
    http.begin(serverPath.c_str());
    http.addHeader("Authorization", authToken);

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
        finger_id = doc["users"]["id_fingerprint"].as<int>();
        if (name != "null") {
          // Insert ID and Name into the table
          lv_table_set_cell_value(objects.table_user, 1, 0, id.c_str());
          lv_table_set_cell_value(objects.table_user, 1, 1, name.c_str());
          lv_table_set_cell_value(objects.table_user, 1, 2, category.c_str());


          lv_scr_load(objects.add_finger_screen);
        } else {
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

void httpTask(void *param) {
  // Data yang diterima dari parameter task
  uint8_t id = *(uint8_t *)param;
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = apiCheckin;

    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", authToken);

    StaticJsonDocument<1024> doc;
    doc["user_id"] = id;

    String requestBody;
    serializeJson(doc, requestBody);

    // Kirim HTTP POST request
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
      if (!error) {
        String status = responseDoc["status"].as<String>();
        if (status == "success") {
          String name = responseDoc["user"]["name"].as<String>();
          lv_label_set_text(objects.label_name_popup, name.c_str());
          String type = responseDoc["data"]["status"].as<String>();
          if (type == "departed") {
            showPopupSuccess("Goodbye");
          } else {
            showPopupSuccess("Welcome");
          }
        } else {
          String errorMsg = responseDoc["message"].as<String>();
          showPopupError(errorMsg);
        }
      } else {
        Serial.print("JSON Deserialization failed: ");
        Serial.println(error.c_str());
        showPopupError("Invalid server response");
      }
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      showPopupError("Problem with server");
    }

    http.end(); // Akhiri koneksi HTTP
  } else {
    showPopupError("No WiFi connection");
  }

  // Hapus task setelah selesai
  vTaskDelete(NULL);
}


static void checkin_event_handler(uint8_t id) {
  // Salin ID ke memori dinamis agar tetap tersedia untuk task
  uint8_t *idParam = (uint8_t *)malloc(sizeof(uint8_t));
  if (idParam == NULL) {
    showPopupError("Memory allocation failed");
    return;
  }
  *idParam = id;

  // Buat task untuk menjalankan HTTP POST
  xTaskCreate(
    httpTask,         // Fungsi task
    "HTTP Task",      // Nama task
    4096,             // Ukuran stack task
    idParam,          // Parameter task (user ID)
    1,                // Prioritas task
    NULL              // Handle task (tidak diperlukan)
  );

  // Tetap perbarui timer LVGL di fungsi utama
  lv_tick_inc(millis() - lastTick);
  lastTick = millis();
  lv_timer_handler();
}


void hidePopupCallback(lv_timer_t *timer) {
  lv_obj_add_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN);  // Sembunyikan panel
  lv_timer_del(timer);                                            // Hapus timer setelah selesai
}

void showPopupSuccess(String status) {
  lv_label_set_text(objects.label_status, status.c_str());
  lv_obj_clear_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN);     // Tampilkan panel
  lv_timer_t *timer = lv_timer_create(hidePopupCallback, 4000, NULL);  // Buat timer untuk menyembunyikan panel
}

void hidePopupErrorCallback(lv_timer_t *timer) {
  lv_obj_add_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN);  // Sembunyikan panel
  lv_obj_add_flag(objects.pic_error, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(objects.pic_success, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(objects.label_name_popup, LV_OBJ_FLAG_HIDDEN);
  lv_timer_del(timer);  // Hapus timer setelah selesai
}

void showPopupError(String message) {
  lv_obj_clear_flag(objects.pic_error, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.pic_success, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(objects.label_name_popup, message.c_str());
  String status = "Error";
  lv_label_set_text(objects.label_status, status.c_str());
  lv_obj_clear_flag(objects.popup_attendance, LV_OBJ_FLAG_HIDDEN);          // Tampilkan panel
  lv_timer_t *timer = lv_timer_create(hidePopupErrorCallback, 4000, NULL);  // Buat timer untuk menyembunyikan panel
}

void hidePopupVerification(lv_timer_t *timer) {
  lv_obj_add_flag(objects.popup_verification, LV_OBJ_FLAG_HIDDEN);
  lv_timer_del(timer);
}

void hidePopupRegister(lv_timer_t *timer) {
  lv_obj_add_flag(objects.popup_register, LV_OBJ_FLAG_HIDDEN);  // Sembunyikan panel
  lv_timer_del(timer);                                          // Hapus timer setelah selesai
}

void showPopupRegister(String info, bool success) {
  if (success) {
    String status = "Success";
    lv_label_set_text(objects.label_status_register, status.c_str());
    lv_label_set_text(objects.label_name_popup_register, info.c_str());
    lv_obj_clear_flag(objects.popup_register, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.pic_error_register, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.pic_success_register, LV_OBJ_FLAG_HIDDEN);
    lv_timer_t *timer = lv_timer_create(hidePopupRegister, 3000, NULL);
  } else {
    String title = "Failed";
    lv_label_set_text(objects.label_status_register, title.c_str());
    lv_label_set_text(objects.label_name_popup_register, info.c_str());
    lv_obj_clear_flag(objects.popup_register, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.pic_error_register, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.pic_success_register, LV_OBJ_FLAG_HIDDEN);
    lv_timer_t *timer = lv_timer_create(hidePopupRegister, 3000, NULL);
  }
}

static void password_event_handler(lv_event_t *e) {
  int btn_index = lv_keyboard_get_selected_btn(objects.keyboard_password);
  String password = lv_textarea_get_text(objects.input_password);
  Serial.println(btn_index);
  Serial.println(password);
  if (btn_index == 11) {
    if (password == "123456") {
      String status = "Input Member ID";
      lv_label_set_text(objects.label_input_member, status.c_str());
      lv_scr_load(objects.reg_screen);
    } else {
      Serial.print("pasword salah");
      lv_obj_clear_flag(objects.popup_verification, LV_OBJ_FLAG_HIDDEN);
      lv_timer_t *timer = lv_timer_create(hidePopupVerification, 1000, NULL);
    }
    lv_textarea_set_text(objects.input_password, "");
  }
}

static void get_user_event_handler(lv_event_t *e) {
  int btn_index = lv_keyboard_get_selected_btn(objects.keyboard_id);
  if (btn_index == 11) {
    String id = lv_textarea_get_text(objects.input_id);
    get_table_data(id);
    lv_textarea_set_text(objects.input_id, "");
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    return p;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Image converted");
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence ");
    Serial.println(finger.confidence);
    checkin_event_handler(finger.fingerID);
  } else {
    String status = "Not found ID";
    Serial.println("No match found");
    showPopupError(status);
  }
  return p;
}

uint8_t getFingerprintEnroll(int id) {
  uint8_t p = finger.getImage();
  
  lv_tick_inc(millis() - lastTick);    // Update the tick timer.
  lastTick = millis();
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
      lv_tick_inc(millis() - lastTick);  // Update the tick timer.
      lastTick = millis();
      lv_timer_handler();
      return p;
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  lv_tick_inc(millis() - lastTick);  // Update the tick timer.
  lastTick = millis();
  lv_timer_handler();
  showPopupRegister("Remove finger then place the same finger again", true);

  bool loop = true;
  while(loop){
    lv_timer_handler();
    if (millis() - lastTick >= 2000) {
      loop = false; 
    }
  }

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");

  unsigned long startTime = millis(); 
  while (p != FINGERPRINT_OK) {
    lv_tick_inc(millis() - lastTick);  // Update the tick timer.
    lastTick = millis();
    lv_timer_handler();

    p = finger.getImage();

    // Periksa waktu yang telah berlalu
    if (millis() - startTime > 6000) {
      Serial.println("Timeout reached: 6 seconds");
      showPopupRegister("Operation timed out. Try again.", false);
      break;  // Keluar dari loop setelah 6 detik
    }

    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
    delay(50);
  }
  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      showPopupRegister("Fingerprint too messy", false);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    showPopupRegister("Stored", true);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}

void handleFingerprintEvent() {
  if (lv_scr_act() == objects.attendance_screen) {
    getFingerprintID();
  } else if (lv_scr_act() == objects.add_finger_screen) {
    getFingerprintEnroll(finger_id);
  }
}

//________________________________________________________________________________
void setup() {
  Serial.begin(115200);
  mySerial.begin(57600, SERIAL_8N1, 22, 27);  // RX=22, TX=27 (sesuaikan dengan pin Anda)

  Serial.println("\nAdafruit Fingerprint sensor test");

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

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
  lv_display_t *disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, DRAW_BUF_SIZE);
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);

  // Initialize LVGL input device (Touchscreen)
  lv_indev_t *indev = lv_indev_create();
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
  lv_obj_add_flag(objects.popup_register, LV_OBJ_FLAG_HIDDEN);

  // EventData* checkin = new EventData;
  // checkin->id = 1;

  // lv_obj_add_event_cb(objects.button_checkin, checkin_event_handler, LV_EVENT_CLICKED, checkin);

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
  lv_tick_inc(millis() - lastTick);  // Update the tick timer.
  lastTick = millis();
  lv_timer_handler();  // Update the UI.

  if (lv_scr_act() == objects.attendance_screen) {
    getFingerprintID();
  } else if (lv_scr_act() == objects.add_finger_screen) {
    getFingerprintEnroll(finger_id);
  }

  delay(50);
}
