#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_attendance_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.attendance_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // label_name
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_name = obj;
            lv_obj_set_pos(obj, 160, 181);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 320, 240);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 320, 38);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff11ffe3), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 72, 8);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Attendance Page");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // button_back_2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_back_2 = obj;
            lv_obj_set_pos(obj, 0, 2);
            lv_obj_set_size(obj, 50, 34);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff32121), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "X");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_status_finger = obj;
            lv_obj_set_pos(obj, 68, 93);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Put your finger\non the sensor");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        // {
        //     // button_checkin
        //     lv_obj_t *obj = lv_btn_create(parent_obj);
        //     objects.button_checkin = obj;
        //     lv_obj_set_pos(obj, 110, 156);
        //     lv_obj_set_size(obj, 100, 50);
        //     {
        //         lv_obj_t *parent_obj = obj;
        //         {
        //             lv_obj_t *obj = lv_label_create(parent_obj);
        //             lv_obj_set_pos(obj, 0, 0);
        //             lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        //             lv_label_set_text(obj, "absen bang");
        //             lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        //         }
        //     }
        // }
        {
            // popup_attendance
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.popup_attendance = obj;
            lv_obj_set_pos(obj, 35, 79);
            lv_obj_set_size(obj, 251, 136);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // pic_success
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.pic_success = obj;
                    lv_obj_set_pos(obj, 80, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_icon_success);
                }
                {
                    // label_name_popup
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_name_popup = obj;
                    lv_obj_set_pos(obj, 0, 84);
                    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Name");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // label_status
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_status = obj;
                    lv_obj_set_pos(obj, 0, 57);
                    lv_obj_set_size(obj, LV_PCT(100), 22);
                    lv_label_set_text(obj, "Welcome");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // pic_error
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.pic_error = obj;
                    lv_obj_set_pos(obj, 80, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_icon_fail);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                }
            }
        }
    }
}

void tick_screen_attendance_screen() {
}

void create_screen_main() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // button_register
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_register = obj;
            lv_obj_set_pos(obj, 87, 28);
            lv_obj_set_size(obj, 147, 53);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Register");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // button_attendance
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_attendance = obj;
            lv_obj_set_pos(obj, 87, 160);
            lv_obj_set_size(obj, 147, 51);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, -1, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Attendance");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            // button_delete
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_delete = obj;
            lv_obj_set_pos(obj, 88, 94);
            lv_obj_set_size(obj, 147, 53);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Delete");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
    }
}

void tick_screen_main() {
}

void create_screen_reg_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.reg_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj1 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 320, 36);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff11ffe3), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 89, 8);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Register Page");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // button_back_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_back_1 = obj;
            lv_obj_set_pos(obj, 0, 1);
            lv_obj_set_size(obj, 50, 34);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff32121), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "X");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_input_member = obj;
            lv_obj_set_pos(obj, 0, 42);
            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Input Member ID");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }

        {
            // input_id
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.input_id = obj;
            lv_obj_set_pos(obj, 41, 65);
            lv_obj_set_size(obj, 236, 38);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
        }
        {
            // keyboard_id
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard_id = obj;
            lv_obj_set_pos(obj, 10, 111);
            lv_obj_set_size(obj, 300, 120);

                        static const char * kb_map[] = {
                "1", "2", "3", "\n",
                "4", "5", "6", "\n",
                "7", "8", "9", "\n",
                LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_OK, 
                NULL
            };

            /* Set the relative width of the buttons and other controls */
            static const lv_buttonmatrix_ctrl_t kb_ctrl[] = {
                4, 4, 4,      // Row 1: "1", "2", "3", Backspace
                4, 4, 4,      // Row 2: "4", "5", "6", New Line
                4, 4, 4,      // Row 3: "7", "8", "9", New Line
                4, 4, 4,      // Row 4: "0", ".", Close, OK
            };

            lv_keyboard_set_map(obj, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_1);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    lv_keyboard_set_textarea(objects.keyboard_id, objects.input_id);
}

void tick_screen_reg_screen() {
}

void create_screen_add_finger_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.add_finger_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // label_name_1
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_name_1 = obj;
            lv_obj_set_pos(obj, 160, 181);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj2 = obj;
            lv_obj_set_pos(obj, 1, 0);
            lv_obj_set_size(obj, 319, 34);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff11ffe3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // button_back_3
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_back_3 = obj;
            lv_obj_set_pos(obj, 1, 0);
            lv_obj_set_size(obj, 50, 34);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff32121), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "X");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_status_finger_2 = obj;
            lv_obj_set_pos(obj, 69, 154);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Put your finger\non the sensor");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // label_register_2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_register_2 = obj;
            lv_obj_set_pos(obj, 118, 5);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Register");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // table_user
            lv_obj_t *obj = lv_table_create(parent_obj);
            objects.table_user = obj;
            lv_obj_set_pos(obj, 0, 34);
            lv_obj_set_size(obj, 320, 100);
        }
        {
            // popup_register
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.popup_register = obj;
            lv_obj_set_pos(obj, 35, 79);
            lv_obj_set_size(obj, 251, 136);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // pic_success
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.pic_success_register = obj;
                    lv_obj_set_pos(obj, 80, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_icon_success);
                }
                {
                    // label_name_popup
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_name_popup_register = obj;
                    lv_obj_set_pos(obj, 0, 84);
                    lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "Name");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // label_status
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_status_register = obj;
                    lv_obj_set_pos(obj, 0, 57);
                    lv_obj_set_size(obj, LV_PCT(100), 22);
                    lv_label_set_text(obj, "Welcome");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // pic_error
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.pic_error_register = obj;
                    lv_obj_set_pos(obj, 80, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_icon_fail);
                    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                }
            }
        }
    }
}

void tick_screen_add_finger_screen() {
}

void create_screen_verification_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.verification_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            // label_name_2
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_name_2 = obj;
            lv_obj_set_pos(obj, 160, 181);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_30, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 320, 240);
            lv_obj_set_style_pad_left(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(obj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj3 = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 320, 38);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff11ffe3), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 0, 8);
            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Verification");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // button_back_4
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_back_4 = obj;
            lv_obj_set_pos(obj, 0, 2);
            lv_obj_set_size(obj, 50, 34);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff32121), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "X");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 81, 46);
            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Input the Password");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // input_password
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.input_password = obj;
            lv_obj_set_pos(obj, 43, 69);
            lv_obj_set_size(obj, 236, 38);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, true);
            lv_textarea_set_password_mode(obj, true);
        }
        {
            // keyboard_password
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard_password = obj;
            lv_obj_set_pos(obj, 12, 115);
            lv_obj_set_size(obj, 300, 120);
                        static const char * kb_map[] = {
                "1", "2", "3", "\n",
                "4", "5", "6", "\n",
                "7", "8", "9", "\n",
                LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_OK, 
                NULL
            };

            /* Set the relative width of the buttons and other controls */
            static const lv_buttonmatrix_ctrl_t kb_ctrl[] = {
                4, 4, 4,      // Row 1: "1", "2", "3", Backspace
                4, 4, 4,      // Row 2: "4", "5", "6", New Line
                4, 4, 4,      // Row 3: "7", "8", "9", New Line
                4, 4, 4,      // Row 4: "0", ".", Close, OK
            };

            lv_keyboard_set_map(obj, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_1);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // popup_verification
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.popup_verification = obj;
            lv_obj_set_pos(obj, 36, 55);
            lv_obj_set_size(obj, 251, 136);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // label_status_1
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    objects.label_status_1 = obj;
                    lv_obj_set_pos(obj, 0, 76);
                    lv_obj_set_size(obj, LV_PCT(100), 22);
                    lv_label_set_text(obj, "Wrong Password");
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
                {
                    // pic_error_1
                    lv_obj_t *obj = lv_img_create(parent_obj);
                    objects.pic_error_1 = obj;
                    lv_obj_set_pos(obj, 80, -7);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_img_set_src(obj, &img_icon_fail);
                }
            }
        }
    }
    lv_keyboard_set_textarea(objects.keyboard_password, objects.input_password);
}

void tick_screen_verification_screen() {
}

void create_screen_delete_screen() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.delete_screen = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 320, 240);
    {
        lv_obj_t *parent_obj = obj;
        {
            lv_obj_t *obj = lv_obj_create(parent_obj);
            objects.obj4 = obj;
            lv_obj_set_pos(obj, 0, -1);
            lv_obj_set_size(obj, 320, 36);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xff11ffe3), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            lv_obj_set_pos(obj, 0, 6);
            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Delete Page");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // button_back_5
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.button_back_5 = obj;
            lv_obj_set_pos(obj, 0, 1);
            lv_obj_set_size(obj, 50, 34);
            lv_obj_set_style_radius(obj, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(obj, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_color(obj, lv_color_hex(0xfff32121), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_label_set_text(obj, "X");
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
        }
        {
            lv_obj_t *obj = lv_label_create(parent_obj);
            objects.label_input_member_1 = obj;
            lv_obj_set_pos(obj, 0, 42);
            lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
            lv_label_set_text(obj, "Input Fingerprint ID");
            lv_obj_set_style_text_font(obj, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        {
            // input_id_1
            lv_obj_t *obj = lv_textarea_create(parent_obj);
            objects.input_id_1 = obj;
            lv_obj_set_pos(obj, 41, 65);
            lv_obj_set_size(obj, 236, 38);
            lv_textarea_set_max_length(obj, 128);
            lv_textarea_set_one_line(obj, false);
            lv_textarea_set_password_mode(obj, false);
        }
        {
            // keyboard_id
            lv_obj_t *obj = lv_keyboard_create(parent_obj);
            objects.keyboard_id_1 = obj;
            lv_obj_set_pos(obj, 10, 111);
            lv_obj_set_size(obj, 300, 120);

                        static const char * kb_map[] = {
                "1", "2", "3", "\n",
                "4", "5", "6", "\n",
                "7", "8", "9", "\n",
                LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_OK, 
                NULL
            };

            /* Set the relative width of the buttons and other controls */
            static const lv_buttonmatrix_ctrl_t kb_ctrl[] = {
                4, 4, 4,      // Row 1: "1", "2", "3", Backspace
                4, 4, 4,      // Row 2: "4", "5", "6", New Line
                4, 4, 4,      // Row 3: "7", "8", "9", New Line
                4, 4, 4,      // Row 4: "0", ".", Close, OK
            };

            lv_keyboard_set_map(obj, LV_KEYBOARD_MODE_USER_1, kb_map, kb_ctrl);
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_1);
            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
    lv_keyboard_set_textarea(objects.keyboard_id_1, objects.input_id_1);
}

void tick_screen_delete_screen() {
}


void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_attendance_screen();
    create_screen_main();
    create_screen_reg_screen();
    create_screen_add_finger_screen();
    create_screen_verification_screen();
    create_screen_delete_screen();
}

typedef void (*tick_screen_func_t)();

tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_attendance_screen,
    tick_screen_main,
    tick_screen_reg_screen,
    tick_screen_add_finger_screen,
    tick_screen_verification_screen,
    tick_screen_delete_screen,
};

void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
