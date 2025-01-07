#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *reg_screen;
    lv_obj_t *attendance_screen;
    lv_obj_t *button_register;
    lv_obj_t *button_attendance;
    lv_obj_t *table;
    lv_obj_t *obj0;
    lv_obj_t *button_back_1;
    lv_obj_t *label_name;
    lv_obj_t *obj1;
    lv_obj_t *button_back_2;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_REG_SCREEN = 2,
    SCREEN_ID_ATTENDANCE_SCREEN = 3,
};

void create_screen_main();
void tick_screen_main();

void create_screen_reg_screen();
void tick_screen_reg_screen();

void create_screen_attendance_screen();
void tick_screen_attendance_screen();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/