// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.4.0
// Project name: HID Interface
// 在这个界面要自定义图标可以修改ICON_CNT数量，添加图片、选择hid_id、
#include "../ui.h"
static const uint8_t ICON_CNT = 8;
static const char *TAG = "SCREEN2";
static lv_group_t* group;
static lv_obj_t* pointer;
static int icon_index = 0;
static lv_timer_t * pointer_task;
static UI_HID_INFO *ui_icon_hid;
static lv_obj_t* arc_bat;
static lv_obj_t* ble_img;
static void ui_icon_hid_init();
static void setl_label_info(uint8_t index);
static uint8_t screen_init_flag = 0;
static struct
{
    lv_style_t unfocused_0;//在焦点旁边 半透明
    lv_style_t unfocused_1;//远离焦点 低透明度
    lv_style_t unfocused_2;//远离焦点 低透明度 
    lv_style_t focus;
    lv_style_t font16;
    lv_style_t font20;
    lv_style_t ble_img;
}style;
static struct
{
    lv_obj_t* name;
    lv_obj_t* left;
    lv_obj_t* mid;
    lv_obj_t* right;
}ui_label;
static const foc_knob_param_t hid_foc_knob_param_lst[] = {
    [0] = { 0, 0, 5 * PI / 180, 1, 1, 1.1, ""},    //surface dial            
    [1] = { 0, 0, 1 * PI / 180, 1, 1, 1.1, ""},    //鼠标左右         
    [2] = { 0, 0, 1 * PI / 180, 1, 1, 1.1, ""},    //鼠标上下         
    [3] = { 0, 0, 2 * PI / 180, 1, 1, 1.1, ""},    //滚轮上下         
    [4] = { 0, 0, 2 * PI / 180, 1, 1, 1.1, ""},    //滚轮左右         
    [5] = { 0, 0, 30 * PI / 180, 1, 1, 1, ""},    //快捷键         
    [6] = { 50, 25, 5 * PI / 180, 1, 1, 1.1, ""},    //音量         
    [7] = { 0, 0, 2 * PI / 180, 1, 1, 1.1, ""},    //方向键左右
};
static const foc_knob_param_t press_foc_knob_param = { 0, 0, 10 * PI / 180, 2, 1, 1.1, ""}; //按下旋转的力度
void set_screen2_index()
{
    nvs_set_u8_data(NVS_SYS_HID_INDEX,icon_index);
}
static void ui_icon_info_init()
{
    uint8_t id;
    icon_index = sys_config.hid_sys_index;
    ui_icon_hid = heap_caps_malloc(ICON_CNT*sizeof(UI_HID_INFO),MALLOC_CAP_SPIRAM);
    memset(ui_icon_hid, 0x00, ICON_CNT*sizeof(UI_HID_INFO));//全部清零
    //surface dial
    id = 0;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_DIAL;
    ui_icon_hid[id].dial_sta[DIAL_STA_RELEASE].hid_data[0] = DIAL_RELEASE;
    ui_icon_hid[id].dial_sta[DIAL_STA_PRESS].hid_data[0] = DIAL_PRESS;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[0] = DIAL_R;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[0] = DIAL_L;

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_dial_png;
    ui_icon_hid[id].icon.name = "旋钮";
    ui_icon_hid[id].icon.left_info = "-";
    ui_icon_hid[id].icon.mid_info = "Surface Dial";
    ui_icon_hid[id].icon.right_info = "+";
    //鼠标左右，按下鼠标左键
    id = 1;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_MOUSE;
    ui_icon_hid[id].dial_sta[DIAL_STA_RELEASE].hid_data[0] = 0;
    ui_icon_hid[id].dial_sta[DIAL_STA_PRESS].hid_data[0] = MOUSE_BUTTON_LEFT;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[1] = 10;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[1] = -10;

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_left_right_png;
    ui_icon_hid[id].icon.name = "鼠标左右";
    ui_icon_hid[id].icon.left_info = "-10X";
    ui_icon_hid[id].icon.mid_info = "Left";
    ui_icon_hid[id].icon.right_info = "+10X";
    //鼠标上下，按下鼠标右键
    id = 2;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_MOUSE;
    ui_icon_hid[id].dial_sta[DIAL_STA_PRESS].hid_data[0] = MOUSE_BUTTON_RIGHT;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[2] = 10;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[2] = -10;

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_up_dowm_png;
    ui_icon_hid[id].icon.name = "鼠标上下";
    ui_icon_hid[id].icon.left_info = "-10Y";
    ui_icon_hid[id].icon.mid_info = "Right";
    ui_icon_hid[id].icon.right_info = "+10Y";
    //鼠标滚轮上下移动，按下鼠标中键
    id = 3;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_MOUSE;
    ui_icon_hid[id].dial_sta[DIAL_STA_PRESS].hid_data[0] = 0;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[3] = -1;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[3] = 1;

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_wheel_png;
    ui_icon_hid[id].icon.name = "上下滚轮";
    ui_icon_hid[id].icon.left_info = "-1";
    ui_icon_hid[id].icon.mid_info = "Middle";
    ui_icon_hid[id].icon.right_info = "+1";

    //鼠标滚轮水平移动，按下鼠标中键
    id = 4;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_MOUSE;
    ui_icon_hid[id].dial_sta[DIAL_STA_PRESS].hid_data[0] = 0;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[4] = 1;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[4] = -1;
    ui_icon_hid[id].img_angle = 900; //翻转90.0度

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_wheel_png;
    ui_icon_hid[id].icon.name = "左右滚轮";
    ui_icon_hid[id].icon.left_info = "-1";
    ui_icon_hid[id].icon.mid_info = "Middle";
    ui_icon_hid[id].icon.right_info = "+1";
    //键盘ctrl+c/ctrl+v，按下ctrl+s
    id = 5;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_KEYBOARD;
    ui_icon_hid[id].dial_sta[DIAL_STA_CLICK].hid_data[0] = KEYBOARD_MODIFIER_LEFTCTRL;
    ui_icon_hid[id].dial_sta[DIAL_STA_CLICK].hid_data[1] = HID_KEY_Z;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[0] = KEYBOARD_MODIFIER_LEFTCTRL;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[1] = HID_KEY_V;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[0] = KEYBOARD_MODIFIER_LEFTCTRL;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[1] = HID_KEY_C;

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_copy_png;
    ui_icon_hid[id].icon.name = "快捷键";
    ui_icon_hid[id].icon.left_info = "Ctrl+C";
    ui_icon_hid[id].icon.mid_info = "Ctrl+Z";
    ui_icon_hid[id].icon.right_info = "Ctrl+V";
    //媒体音量大小，按下静音
    id = 6;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_MEDIA;
    ui_icon_hid[id].dial_sta[DIAL_STA_CLICK].hid_data[0] = HID_USAGE_CONSUMER_MUTE&0xff;
    ui_icon_hid[id].dial_sta[DIAL_STA_CLICK].hid_data[1] = HID_USAGE_CONSUMER_MUTE>>8;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[0] = HID_USAGE_CONSUMER_VOLUME_INCREMENT&0xff;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[1] = HID_USAGE_CONSUMER_VOLUME_INCREMENT>>8;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[0] = HID_USAGE_CONSUMER_VOLUME_DECREMENT&0xff;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[1] = HID_USAGE_CONSUMER_VOLUME_DECREMENT>>8;

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_volume_png;
    ui_icon_hid[id].icon.name = "音量";
    ui_icon_hid[id].icon.left_info = "0";
    ui_icon_hid[id].icon.mid_info = "Mute";
    ui_icon_hid[id].icon.right_info = "100";
    //方向键左右，按下空格
    id = 7;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_KEYBOARD;
    ui_icon_hid[id].dial_sta[DIAL_STA_CLICK].hid_data[1] = HID_KEY_SPACE;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[1] = HID_KEY_ARROW_RIGHT;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[1] = HID_KEY_ARROW_LEFT;

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_key_left_right_png;
    ui_icon_hid[id].icon.name = "方向键";
    ui_icon_hid[id].icon.left_info = "Left";
    ui_icon_hid[id].icon.mid_info = "Space";
    ui_icon_hid[id].icon.right_info = "Right";
}
static void hid_send(uint8_t state)
{
        uint8_t hid_id = ui_icon_hid[icon_index].hid_id;
        uint8_t keycode[6];
        memcpy(keycode, &ui_icon_hid[icon_index].dial_sta[state], 6);
        ui_send_hid_command(hid_id,keycode,state);
}
void ui_Screen2_hid_event(uint8_t state)
{
    if(screen_init_flag == 0)
        return;
    if(screen_init_flag == 1)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        screen_init_flag = 2;
    }
    static uint8_t hid_dial_press_flag = 0;//用于是否在dial界面按下按键，判断按下旋转时切换icon发送按下旋转事件
    switch (state)
    {
    case DIAL_STA_RELEASE:
        hid_send(state);
        foc_knob_set_param(ui_icon_hid[icon_index].param_list);
        hid_dial_press_flag = 0;
        break;
    case DIAL_STA_PRESS:
        hid_send(state);
        if(icon_index == 0)
        {
            hid_dial_press_flag = 1;
        }
        foc_knob_set_param(press_foc_knob_param);
        break;
    case DIAL_STA_R:
        hid_send(state);
        break;
    case DIAL_STA_L:
        hid_send(state);
        break;
    case DIAL_STA_CLICK:
        hid_send(state);
        break;
    case DIAL_STA_DOUBLE_CLICK:
        set_screen2_index();
        _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen1_screen_init);
        break;
    case DIAL_STA_P_R:
        if(hid_dial_press_flag == 0&& icon_index == 0);
        else
            hid_send(state);
        hid_dial_press_flag = 0;
        enc_num++;
        break;
    case DIAL_STA_P_L:
        if(hid_dial_press_flag == 0&& icon_index == 0);
        else
            hid_send(state);
        hid_dial_press_flag = 0;
        enc_num--;
        break;
    default:
        break;
    }
}
static lv_obj_t* container_create(lv_obj_t* screen)
{
    lv_obj_t* ui_Container = lv_obj_create(screen);
    lv_obj_remove_style_all(ui_Container);
    lv_obj_set_width(ui_Container, 220);
    lv_obj_set_height(ui_Container, 115);
    lv_obj_set_x(ui_Container, 0);
    lv_obj_set_y(ui_Container, 74);
    lv_obj_set_align(ui_Container, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_Container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_Container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_clear_flag(ui_Container, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_Container, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui_Container, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui_Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui_Container, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui_Container, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_Container, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui_Container, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_Container, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui_Container, LV_BORDER_SIDE_TOP, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    return ui_Container;
}
static void Style_Init()
{
    lv_style_init(&style.focus);
    lv_style_set_radius(&style.focus, 15);
    lv_style_set_bg_color(&style.focus, lv_color_hex(0x000000));
    lv_style_set_bg_opa(&style.focus, 50);
    lv_style_set_bg_grad_color(&style.focus, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_main_stop(&style.focus, 150);
    lv_style_set_bg_grad_stop(&style.focus, 255);
    lv_style_set_bg_grad_dir(&style.focus, LV_GRAD_DIR_VER);

    lv_style_init(&style.unfocused_0);
    lv_style_set_opa(&style.unfocused_0, 150);
    // lv_style_set_transform_zoom(&style.unfocused_0,220);
    lv_style_set_translate_x(&style.unfocused_0,0);
    lv_style_set_translate_y(&style.unfocused_0, 15);

    lv_style_init(&style.unfocused_1);
    lv_style_set_opa(&style.unfocused_1, 100);
    // lv_style_set_transform_zoom(&style.unfocused_1, 150);
    lv_style_set_translate_x(&style.unfocused_1, 15);
    lv_style_set_translate_y(&style.unfocused_1, 25);

    lv_style_init(&style.unfocused_2);
    lv_style_set_opa(&style.unfocused_2, 100);
    // lv_style_set_transform_zoom(&style.unfocused_2, 150);
    lv_style_set_translate_x(&style.unfocused_2, -15);
    lv_style_set_translate_y(&style.unfocused_2, 25);

    static const lv_style_prop_t style_prop[] =
    {
        LV_STYLE_TRANSLATE_X,
        // LV_STYLE_TRANSLATE_Y,
        // LV_STYLE_TRANSFORM_ZOOM,
        LV_STYLE_PROP_INV
    };

    static lv_style_transition_dsc_t trans;
    lv_style_transition_dsc_init(
        &trans,
        style_prop,
        lv_anim_path_bounce,
        200,
        0,
        NULL
    );
    lv_style_set_transition(&style.focus, &trans);
    lv_style_set_transition(&style.unfocused_0, &trans);
    lv_style_set_transition(&style.unfocused_1, &trans);
    lv_style_set_transition(&style.unfocused_2, &trans);

    lv_style_init(&style.font20);
    lv_style_set_text_font(&style.font20, &ui_font_SmileySansOblique20);
    lv_style_init(&style.font16);
    lv_style_set_text_font(&style.font16, &ui_font_SmileySansOblique16);

    lv_style_init(&style.ble_img);
    lv_style_set_img_recolor(&style.ble_img,lv_color_hex(0x0097FF));
    lv_style_set_img_recolor_opa(&style.ble_img,255);

}
static lv_obj_t* icon_create(lv_obj_t* container, const void* img_src ,uint16_t img_angle)
{
    lv_obj_t* ui_Image = lv_img_create(container);

    lv_img_set_src(ui_Image, img_src);
    lv_obj_remove_style_all(ui_Image);
    lv_obj_clear_flag(ui_Image, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(ui_Image, &style.unfocused_0, LV_STATE_USER_1);
    lv_obj_add_style(ui_Image, &style.unfocused_1, LV_STATE_USER_2);
    lv_obj_add_style(ui_Image, &style.unfocused_2, LV_STATE_USER_3);
    lv_obj_add_style(ui_Image, &style.focus, LV_STATE_FOCUSED);
    lv_img_set_angle(ui_Image, img_angle);
    return ui_Image;
}
static void onFocus(lv_group_t* g)
{
    lvgl_port_lock(0);
    lv_obj_t* icon = lv_group_get_focused(g);
    uint32_t current_btn_index = lv_obj_get_index(icon);
    //printf("current_btn_index:%ld\n", current_btn_index);
    lv_obj_t* container = lv_obj_get_parent(icon);
    uint32_t icon_num = lv_obj_get_child_cnt(container);
    uint32_t mid_btn_index = (icon_num - 1) / 2;
    if (current_btn_index > mid_btn_index)
    {
        icon_index++;
        if (icon_index > icon_num - 1)
            icon_index = 0;
        lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index - 1), LV_ANIM_OFF);
        lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index), LV_ANIM_ON);
        lv_obj_move_to_index(lv_obj_get_child(container, 0), -1);
    }
    else if (current_btn_index < mid_btn_index)
    {
        icon_index--;
        if (icon_index < 0)
        {
            icon_index = icon_num - 1;
        }
        lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index + 1), LV_ANIM_OFF);
        lv_obj_scroll_to_view(lv_obj_get_child(container, mid_btn_index), LV_ANIM_ON);
        lv_obj_move_to_index(lv_obj_get_child(container, -1), 0);
    }
    uint8_t cnt = lv_obj_get_child_cnt(container);
    for (uint8_t i = 0; i < cnt; i++)
    {
        lv_obj_t* obt = lv_obj_get_child(container, i);
        lv_obj_clear_state(obt, LV_STATE_USER_1| LV_STATE_USER_2| LV_STATE_USER_3);
        //远离中心两个图标以上的图标隐藏
        if (i + 2 < mid_btn_index)
        {
            lv_obj_add_flag(obt, LV_OBJ_FLAG_HIDDEN);
        }
        else if(i > mid_btn_index + 2)
        {
            lv_obj_add_flag(obt, LV_OBJ_FLAG_HIDDEN);
        }
        else
            lv_obj_clear_flag(obt, LV_OBJ_FLAG_HIDDEN);
    }
    //设置中间焦点左右两边图标样式，实现越远越小，越远越透明的效果,多个if以防icon数量不到5个
    if(cnt>1)
    {
        lv_img_set_zoom(lv_obj_get_child(container, mid_btn_index), 256);                       /// ZOOM
        lv_obj_add_state(lv_obj_get_child(container, mid_btn_index - 1), LV_STATE_USER_1);       /// States
        lv_img_set_zoom(lv_obj_get_child(container, mid_btn_index - 1), 220);                       /// ZOOM
    }
    if(cnt>2)
    {
        lv_obj_add_state(lv_obj_get_child(container, mid_btn_index + 1), LV_STATE_USER_1);       /// States
        lv_img_set_zoom(lv_obj_get_child(container, mid_btn_index + 1), 220);                       /// ZOOM
    }
    if(cnt>3)
    {
        lv_obj_add_state(lv_obj_get_child(container, mid_btn_index - 2), LV_STATE_USER_2);       /// States
        lv_img_set_zoom(lv_obj_get_child(container, mid_btn_index - 2), 150);                       /// ZOOM
    }
    if(cnt>4)
    {
        lv_obj_add_state(lv_obj_get_child(container, mid_btn_index + 2), LV_STATE_USER_3);       /// States
        lv_img_set_zoom(lv_obj_get_child(container, mid_btn_index + 2), 150);                       /// ZOOM
    }

    setl_label_info(icon_index);

    lvgl_port_unlock();
}
static void group_init(lv_obj_t* container,int8_t id)
{
    icon_index = 0;
    group = lv_group_create();
    uint8_t cnt = lv_obj_get_child_cnt(container);
    //printf("%ld\n", cnt);
    for (uint8_t i = 0; i < cnt; i++)
    {
    	lv_group_add_obj(group, lv_obj_get_child(container,i));
    }
    lv_group_set_focus_cb(group, onFocus);
    uint32_t mid_btn_index = (lv_obj_get_child_cnt(container) - 1) / 2; 
    for (uint8_t i = 0; i < id; i++)
    {
       lv_group_focus_obj(lv_obj_get_child(container, -1));
    }
    lv_group_focus_obj(lv_obj_get_child(container, mid_btn_index));

}
static void setl_label_info(uint8_t index)
{
    lv_label_set_text(ui_label.name, ui_icon_hid[index].icon.name);
    lv_label_set_text(ui_label.left, ui_icon_hid[index].icon.left_info);
    lv_label_set_text(ui_label.mid, ui_icon_hid[index].icon.mid_info);
    lv_label_set_text(ui_label.right, ui_icon_hid[index].icon.right_info);
}
static void Create(lv_obj_t* root)
{
    ui_label.name = lv_label_create(root);
    lv_obj_set_width(ui_label.name, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_label.name, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_label.name, 0);
    lv_obj_set_y(ui_label.name, 93);
    lv_obj_set_align(ui_label.name, LV_ALIGN_CENTER);
    lv_obj_add_style(ui_label.name, &style.font20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_label.left = lv_label_create(root);
    lv_obj_set_width(ui_label.left, LV_SIZE_CONTENT);   /// 73
    lv_obj_set_height(ui_label.left, LV_SIZE_CONTENT);    /// 40
    lv_obj_set_x(ui_label.left, -67);
    lv_obj_set_y(ui_label.left, 0);
    lv_obj_set_align(ui_label.left, LV_ALIGN_CENTER);
    lv_obj_add_style(ui_label.left, &style.font16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_label.mid = lv_label_create(root);
    lv_obj_set_width(ui_label.mid, LV_SIZE_CONTENT);   /// 73
    lv_obj_set_height(ui_label.mid, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_label.mid, LV_ALIGN_CENTER);
    lv_obj_add_style(ui_label.mid, &style.font16, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_label.right = lv_label_create(root);
    lv_obj_set_width(ui_label.right, LV_SIZE_CONTENT);   /// 74
    lv_obj_set_height(ui_label.right, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_label.right, 67);
    lv_obj_set_y(ui_label.right, 0);
    lv_obj_set_align(ui_label.right, LV_ALIGN_CENTER);
    lv_obj_add_style(ui_label.right, &style.font16, LV_PART_MAIN | LV_STATE_DEFAULT);

    pointer = lv_img_create(root);
    lv_img_set_src(pointer, &ui_img_pointer_png);
    lv_obj_set_align(pointer, LV_ALIGN_TOP_MID);
    lv_obj_set_y(pointer, 10);
    lv_img_set_pivot(pointer,24,110);
    lv_obj_add_flag(pointer, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(pointer, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    arc_bat = lv_arc_create(root);
    lv_obj_set_width(arc_bat, 240);
    lv_obj_set_height(arc_bat, 240);
    lv_obj_set_x(arc_bat, 3);
    lv_obj_set_y(arc_bat, -3);
    lv_obj_set_align(arc_bat, LV_ALIGN_CENTER);
    lv_arc_set_value(arc_bat, 50);
    lv_arc_set_bg_angles(arc_bat, 108, 169);
    lv_obj_set_style_arc_color(arc_bat, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_bat, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_bat, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(arc_bat, true, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(arc_bat, lv_color_hex(0xB0E14A), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_bat, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_bat, 6, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(arc_bat, true, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(arc_bat, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(arc_bat, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    ble_img = lv_img_create(root);
    lv_img_set_src(ble_img, &ui_img_ble_png);
    lv_obj_set_width(ble_img, LV_SIZE_CONTENT);   /// 48
    lv_obj_set_height(ble_img, LV_SIZE_CONTENT);    /// 48
    lv_obj_set_x(ble_img, -85);
    lv_obj_set_y(ble_img, 35);
    lv_obj_set_align(ble_img, LV_ALIGN_CENTER);
    lv_obj_add_flag(ble_img, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ble_img, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_zoom(ble_img, 125);
    // lv_obj_set_style_img_recolor(ble_img, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_img_recolor_opa(ble_img, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(ble_img, &style.ble_img, LV_STATE_USER_1);
}
static void task_pointer_cb()
{
    lv_img_set_angle(pointer, get_fknob_shaft_angle());
    uint8_t value = bat_val_get();
	lv_arc_set_value(arc_bat, value);
    if(s_ble_hid_param.is_connected)
    {
        lv_obj_add_state(ble_img,LV_STATE_USER_1);
    }
    else
    {
        lv_obj_clear_state(ble_img,LV_STATE_USER_1);
    }
}
static void scr_Screen2_unloaded_cb(lv_event_t * e)
{
    lv_obj_t ** var = lv_event_get_user_data(e);
    lv_timer_del(pointer_task);
    lv_group_del(group);
    lv_obj_del(*var);
    (*var) = NULL;
}
static void scr_Screen2_loaded_cb(lv_event_t * e)
{
    usb_device_init();
    ui_state.index = UI_HID_INTERFACE;
    lv_indev_set_group(encoder_indev, group);
    foc_knob_set_param(ui_icon_hid[icon_index].param_list);
}
void ui_Screen2_screen_init(void)
{
    ui_Screen2 = lv_obj_create(NULL);
    ui_icon_info_init();
    lv_obj_add_event_cb(ui_Screen2, scr_Screen2_loaded_cb, LV_EVENT_SCREEN_LOADED, &ui_Screen2);
    // lv_obj_add_event_cb(ui_Screen2, scr_Screen2_unloaded_cb, LV_EVENT_SCREEN_UNLOADED, &ui_Screen2);
    lv_obj_clear_flag(ui_Screen2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_img_src(ui_Screen2, &ui_img_bg1_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    Style_Init();
    Create(ui_Screen2);
    setl_label_info(icon_index);
    lv_obj_t* container = container_create(ui_Screen2);
    for (uint8_t i = 0; i < ICON_CNT; i++)
    {
        icon_create(container, ui_icon_hid[i].icon.img_src,ui_icon_hid[i].img_angle);
    }
    uint32_t mid_btn_index = (lv_obj_get_child_cnt(container) - 1) / 2;
    for (uint32_t i = 0; i < mid_btn_index; i++) 
    {
        lv_obj_move_to_index(lv_obj_get_child(container, -1), 0);
    }
    group_init(container,icon_index);
    pointer_task = lv_timer_create(task_pointer_cb, 10, 0);
	lv_timer_set_repeat_count(pointer_task,-1);

    screen_init_flag = 1;
}
