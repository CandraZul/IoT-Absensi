#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "ui.h"
//---------------------------------------- 

// Defines the T_CS Touchscreen PIN.
#define T_CS_PIN  13 //--> T_CS

// Defines the screen resolution.
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

//---------------------------------------- Defines the Touchscreen calibration result value.
// Replace with the calibration results on your TFT LCD Touchscreen.
#define touchscreen_Min_X 0
#define touchscreen_Max_X 0
#define touchscreen_Min_Y 0
#define touchscreen_Max_Y 0
//---------------------------------------- 

#define LED_PIN 27

// LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes.
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint8_t *draw_buf;

// Variables for x, y and z values ​​on the touchscreen.
uint16_t x, y, z;

// Used to track the tick timer.
uint32_t lastTick = 0;

int Count_Val = 0;

// Declaring the "XPT2046_Touchscreen" object as "touchscreen" and its settings.
XPT2046_Touchscreen touchscreen(T_CS_PIN);



//________________________________________________________________________________ log_print()
// If logging is enabled, it will inform the user about what is happening in the library.
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}


void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if (touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    
    //---------------------------------------- 
    // Calibrate Touchscreen points with map function to the correct width and height.
    x = map(p.x, touchscreen_Max_X, touchscreen_Min_X, 1, SCREEN_HEIGHT);
    y = map(p.y, touchscreen_Max_Y, touchscreen_Min_Y, 1, SCREEN_WIDTH);
    
    // If the touchscreen on the TFT LCD is upside down, try the code line below.
    // If there is an error on the touchscreen, edit and try other settings in the code section below.
    // Because my TFT LCD Touchscreen device may have different settings from your TFT LCD device.
    //x = map(p.x, touchscreen_Min_X, touchscreen_Max_X, 1, SCREEN_HEIGHT);
    //y = map(p.y, touchscreen_Min_Y, touchscreen_Max_Y, 1, SCREEN_WIDTH);
    //---------------------------------------- 
    
    z = p.z;

    // Set the coordinates.
    data->point.x = x;
    data->point.y = y;

    data->state = LV_INDEV_STATE_PRESSED;

    // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor.
    /*Serial.print("X = ");
    Serial.print(x);
    Serial.print(" | Y = ");
    Serial.print(y);
    Serial.print(" | Pressure = ");
    Serial.print(z);
    Serial.println();*/
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}


//________________________________________________________________________________ 
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println();
  delay(3000);

  Serial.println("ESP32 + TFT LCD Touchscreen ILI9341 + LVGL + EEZ Studio");
  Serial.println();
  delay(500);

  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println(LVGL_Arduino);
  delay(500);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  //---------------------------------------- LVGL setup.
  Serial.println();
  Serial.println("Start LVGL Setup.");
  delay(500);
  

  // Start the SPI for the touchscreen and init the touchscreen.
  touchscreen.begin();

  // Set the Touchscreen rotation in landscape mode.
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 0. "touchscreen.setRotation(0);"
  touchscreen.setRotation(0);

  // Start LVGL.
  lv_init();
  
  // Register print function for debugging.
  lv_log_register_print_cb(log_print);

  // Create a display object.
  lv_display_t * disp;
  
  // Initialize the TFT display using the TFT_eSPI library.
  draw_buf = new uint8_t[DRAW_BUF_SIZE];
  disp = lv_tft_espi_create(SCREEN_HEIGHT, SCREEN_WIDTH, draw_buf, DRAW_BUF_SIZE);
  // If the display on the TFT LCD has problems, try the line of code below:
  //disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, DRAW_BUF_SIZE);
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);
    
  // Initialize an LVGL input device object (Touchscreen).
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  
  // Set the callback function to read Touchscreen input.
  lv_indev_set_read_cb(indev, touchscreen_read);

  Serial.println();
  Serial.println("LVGL Setup Completed.");
  delay(500);
  //---------------------------------------- 
  // Integrate EEZ Studio GUI.
  ui_init();
}
//________________________________________________________________________________ 
  
//________________________________________________________________________________ 
void loop() {
  // put your main code here, to run repeatedly:
  lv_tick_inc(millis() - lastTick); //--> Update the tick timer. Tick is new for LVGL 9.
  lastTick = millis();
  lv_timer_handler(); //--> Update the UI.
  delay(5);
}