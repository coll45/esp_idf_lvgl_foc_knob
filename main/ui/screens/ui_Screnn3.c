// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.4.0
// Project name: Customize HID Interface
// 在这个界面要自定义图标可以修改ICON_CNT数量，添加图片、选择hid_id、
#include "../ui.h"
static const uint8_t ICON_CNT = 1;
static const char *TAG = "SCREEN2";
static lv_group_t* group;
static lv_obj_t* pointer;
static int icon_index = 0;
static lv_timer_t * pointer_task;
static UI_HID_INFO *ui_icon_hid;

static void ui_icon_hid_init();
static void setl_label_info(uint8_t index);
static struct
{
    lv_style_t unfocused_0;//在焦点旁边 半透明
    lv_style_t unfocused_1;//远离焦点 低透明度
    lv_style_t unfocused_2;//远离焦点 低透明度 
    lv_style_t focus;
    lv_style_t font16;
    lv_style_t font20;
}style;
static struct
{
    lv_obj_t* name;
    lv_obj_t* left;
    lv_obj_t* mid;
    lv_obj_t* right;
}ui_label;
static const foc_knob_param_t hid_foc_knob_param_lst[] = {
    [0] = { 0, 0, 1 * PI / 180, 1, 1, 1.1, ""},    //第一个icon的力度          
};
static const foc_knob_param_t screen3_press_foc_knob_param = { 0, 0, 10 * PI / 180, 2, 1, 0.7, ""}; //按下旋转的力度
static void ui_icon_hid_init()
{
    uint8_t id;
    ui_icon_hid = heap_caps_malloc(ICON_CNT*sizeof(UI_HID_INFO),MALLOC_CAP_SPIRAM);
    memset(ui_icon_hid, 0x00, ICON_CNT*sizeof(UI_HID_INFO));//全部清零

    id = 0;
    ui_icon_hid[id].icon_id = id;
    ui_icon_hid[id].hid_id = HID_ITF_PROTOCOL_MOUSE;
    ui_icon_hid[id].dial_sta[DIAL_STA_RELEASE].hid_data[0] = 0;
    ui_icon_hid[id].dial_sta[DIAL_STA_PRESS].hid_data[0] = MOUSE_BUTTON_LEFT;
    ui_icon_hid[id].dial_sta[DIAL_STA_CLICK].hid_data[0] = MOUSE_BUTTON_LEFT;
    ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[1] = 10;
    ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[1] = -10;

    ui_icon_hid[id].param_list = hid_foc_knob_param_lst[id];

    ui_icon_hid[id].icon.img_src = &ui_img_dial_png;
    ui_icon_hid[id].icon.name = "旋钮";
    ui_icon_hid[id].icon.left_info = "-";
    ui_icon_hid[id].icon.mid_info = "Surface Dial";
    ui_icon_hid[id].icon.right_info = "+";
}
static void hid_send(uint8_t state)
{
    uint8_t hid_id = ui_icon_hid[icon_index].hid_id;
    uint8_t keycode[6];
    memcpy(keycode, &ui_icon_hid[icon_index].dial_sta[state], 6);
    ui_send_hid_command(hid_id,keycode,state);
}
void ui_Screen3_Custom_hid_event(uint8_t state)
{
    static bool mouse_click_press_flag = 0; //单击按下后长按鼠标左键，切换flag
    switch (state)
    {
    case DIAL_STA_RELEASE:
        hid_send(state);
        foc_knob_set_param(ui_icon_hid[icon_index].param_list);
        break;
    case DIAL_STA_PRESS:
        hid_send(state);
        foc_knob_set_param(screen3_press_foc_knob_param);
        break;
    case DIAL_STA_R:
        hid_send(state);
        break;
    case DIAL_STA_L:
        hid_send(state);
        break;
    case DIAL_STA_CLICK:
        if(icon_index == 0)
        {   
            uint8_t id = icon_index;
            if(mouse_click_press_flag)
            {
                mouse_click_press_flag = 0;
                ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[0] = 0;
                ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[0] = 0;
            }
            else
            {
                ui_icon_hid[id].dial_sta[DIAL_STA_R].hid_data[0] = ui_icon_hid[id].dial_sta[DIAL_STA_CLICK].hid_data[0];
                ui_icon_hid[id].dial_sta[DIAL_STA_L].hid_data[0] = ui_icon_hid[id].dial_sta[DIAL_STA_CLICK].hid_data[0];
                mouse_click_press_flag = 1;
            }
        }
        // hid_send(state);
        break;
    case DIAL_STA_DOUBLE_CLICK:
        _ui_screen_change(&ui_Screen1, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen1_screen_init);
        break;
    case DIAL_STA_P_R:
        mouse_click_press_flag = 0;
        hid_send(state);
        enc_num++;
        break;
    case DIAL_STA_P_L:
        mouse_click_press_flag = 0;
        hid_send(state);
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
    lv_group_focus_obj(lv_obj_get_child(container, mid_btn_index));
    for (uint8_t i = 0; i < id; i++)
    {
       lv_group_focus_obj(lv_obj_get_child(container, 1));
    }
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

    pointer = lv_img_create(ui_Screen2);
    lv_img_set_src(pointer, &ui_img_pointer_png);
    lv_obj_set_align(pointer, LV_ALIGN_TOP_MID);
    lv_obj_set_y(pointer, 10);
    lv_img_set_pivot(pointer,24,110);
    lv_obj_add_flag(pointer, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(pointer, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
}
static void task_pointer_cb()
{
    uint16_t num = 0;
    num = 360 - (uint16_t)(get_motor_shaft_angle() * 57.3) % 360;
    if(num > 360)
    {
        num = 0;
    }
    num = num*10;
    lv_img_set_angle(pointer, num);
}
static void scr_Screen3_unloaded_cb(lv_event_t * e)
{
    lv_obj_t ** var = lv_event_get_user_data(e);
    lv_timer_del(pointer_task);
    lv_group_del(group);
    lv_obj_del(*var);
    (*var) = NULL;
}
static void scr_Screen3_loaded_cb(lv_event_t * e)
{
    usb_device_init();
    ui_state.index = UI_HID_CUSTOM_INTERFACE;
    lv_indev_set_group(encoder_indev, group);
    foc_knob_set_param(ui_icon_hid[icon_index].param_list);
}
void ui_Screen3_screen_init(void)
{
    ui_Screen3 = lv_obj_create(NULL);
    ui_icon_hid_init();
    lv_obj_add_event_cb(ui_Screen3, scr_Screen3_loaded_cb, LV_EVENT_SCREEN_LOADED, &ui_Screen3);
    // lv_obj_add_event_cb(ui_Screen3, scr_Screen2_unloaded_cb, LV_EVENT_SCREEN_UNLOADED, &ui_Screen3);
    lv_obj_clear_flag(          ui_Screen3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_bg_img_src(ui_Screen3, &ui_img_bg1_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    Style_Init();
    Create(ui_Screen3);
    setl_label_info(icon_index);
    lv_obj_t* container = container_create(ui_Screen3);
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
}
