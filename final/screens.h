#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *attendance_screen;
    lv_obj_t *main;
    lv_obj_t *reg_screen;
    lv_obj_t *add_finger_screen;
    lv_obj_t *verification_screen;
    lv_obj_t *delete_screen;
    lv_obj_t *label_name;
    lv_obj_t *obj0;
    lv_obj_t *button_back_2;
    // lv_obj_t *button_checkin;
    lv_obj_t *popup_attendance;
    lv_obj_t *pic_success;
    lv_obj_t *label_name_popup;
    lv_obj_t *label_status;
    lv_obj_t *pic_error;
    lv_obj_t *button_register;
    lv_obj_t *button_attendance;
    lv_obj_t *obj1;
    lv_obj_t *label_input_member;
    lv_obj_t *label_input_member_1;
    lv_obj_t *button_back_1;
    lv_obj_t *input_id;
    lv_obj_t *keyboard_id;
    lv_obj_t *button_delete;
    lv_obj_t *label_name_1;
    lv_obj_t *obj2;
    lv_obj_t *button_back_3;
    lv_obj_t *label_register_2;
    lv_obj_t *table_user;
    lv_obj_t *label_name_2;
    lv_obj_t *obj3;
    lv_obj_t *button_back_4;
    lv_obj_t *input_password;
    lv_obj_t *keyboard_password;
    lv_obj_t *popup_verification;
    lv_obj_t *label_status_1;
    lv_obj_t *pic_error_1;
    lv_obj_t *label_status_finger;
    lv_obj_t *label_status_finger_2;
    lv_obj_t *popup_register;
    lv_obj_t *pic_success_register;
    lv_obj_t *label_name_popup_register;
    lv_obj_t *pic_error_register;
    lv_obj_t *label_status_register;
    lv_obj_t *obj4;
    lv_obj_t *button_back_5;
    lv_obj_t *input_id_1;
    lv_obj_t *keyboard_id_1;
    lv_obj_t *label_name_3;
    lv_obj_t *obj5;
    lv_obj_t *button_reset;

} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_ATTENDANCE_SCREEN = 1,
    SCREEN_ID_MAIN = 2,
    SCREEN_ID_REG_SCREEN = 3,
    SCREEN_ID_ADD_FINGER_SCREEN = 4,
    SCREEN_ID_VERIFICATION_SCREEN = 5,
    SCREEN_ID_DELETE_SCREEN = 6,
    SCREEN_ID_VERIFICATION_SCREEN_1 = 7,
};

void create_screen_attendance_screen();
void tick_screen_attendance_screen();

void create_screen_main();
void tick_screen_main();

void create_screen_reg_screen();
void tick_screen_reg_screen();

void create_screen_add_finger_screen();
void tick_screen_add_finger_screen();

void create_screen_verification_screen();
void tick_screen_verification_screen();

void create_screen_delete_screen();
void tick_screen_delete_screen();

void create_screen_verification_screen_1();
void tick_screen_verification_screen_1();

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/