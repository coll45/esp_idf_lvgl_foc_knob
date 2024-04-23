// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.4.0
// Project name: dial

#include "../ui.h"
#define ITEM_HEIGHT_MIN   90
#define ITEM_PAD          10
static const char *TAG = "SCREEN1";
static lv_obj_t* ui_ArcScreen1;
static lv_group_t* group;
static int icon_index = 0;
static lv_timer_t * task1;
static struct
{
    lv_style_t def;//默认
    lv_style_t focus;
    lv_style_t icon_def;
    lv_style_t icon_focus;
    lv_style_t name;  
    lv_style_t info;
    lv_style_t data;
	lv_style_t font;
}style;
static const foc_knob_param_t screen1_foc_knob_param = { 0, 0, 30 * PI / 180, 1, 1, 0.6, ""};
static void onFocus(lv_group_t* g)
{
    lv_obj_t* icon = lv_group_get_focused(g);
    uint32_t current_btn_index = lv_obj_get_index(icon);
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
        lv_obj_clear_state(lv_obj_get_child(obt, 0), LV_STATE_FOCUSED);
        //远离中心一个图标以上的图标隐藏
        if (i + 1 < mid_btn_index)
        {
            lv_obj_add_flag(obt, LV_OBJ_FLAG_HIDDEN);
        }
        else if (i > mid_btn_index + 1)
        {
            lv_obj_add_flag(obt, LV_OBJ_FLAG_HIDDEN);
        }
        else
            lv_obj_clear_flag(obt, LV_OBJ_FLAG_HIDDEN);
        if (i == mid_btn_index)
        {
            lv_obj_add_state(lv_obj_get_child(obt, 0), LV_STATE_FOCUSED);       /// States
        }
    }
}
static void group_init(lv_obj_t* container, uint8_t id)
{
    icon_index = 0;
    group = lv_group_create();
    uint8_t cnt = lv_obj_get_child_cnt(container);
    for (uint8_t i = 0; i < cnt; i++)
    {
        lv_group_add_obj(group, lv_obj_get_child(container, i));
    }
    lv_group_set_focus_cb(group, onFocus);
    uint32_t mid_btn_index = (lv_obj_get_child_cnt(container) - 1) / 2;
    lv_group_focus_obj(lv_obj_get_child(container, mid_btn_index));
    for (uint8_t i = 0; i < id; i++)
    {
        lv_group_focus_obj(lv_obj_get_child(container, -3));
    }
}
static void Style_Init()
{
    lv_style_init(&style.def);
    lv_style_set_width(&style.def, 70); 

    lv_style_init(&style.focus);
    lv_style_set_width(&style.focus, 220);
    lv_style_set_pad_column(&style.focus, ITEM_PAD);

    static const lv_style_prop_t style_prop[] =
    {
        LV_STYLE_WIDTH,
        LV_STYLE_PROP_INV
    };

    static lv_style_transition_dsc_t trans;
    lv_style_transition_dsc_init(
        &trans,
        style_prop,
        lv_anim_path_overshoot,
        200,
        0,
        NULL
    );
    lv_style_set_transition(&style.focus, &trans);
    lv_style_set_transition(&style.def, &trans);

    lv_style_init(&style.icon_focus);
    lv_style_set_border_side(&style.icon_focus, LV_BORDER_SIDE_RIGHT);
    lv_style_set_border_width(&style.icon_focus, 2);
    lv_style_set_border_color(&style.icon_focus, lv_color_hex(0xff0000));

	lv_style_init(&style.font);
    lv_style_set_text_font(&style.font, &ui_font_SmileySansOblique16);
}
static void Item_Create(
	lv_obj_t* container,
	const char* name,
	const void* img_src,
	const char* infos
)
{
    lv_obj_t* box = lv_obj_create(container);
    lv_obj_remove_style_all(box);
    lv_obj_set_align(box, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(box, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(box, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(box, &style.def, 0);
    lv_obj_add_style(box, &style.focus, LV_STATE_FOCUSED);

    lv_obj_t* icon = lv_obj_create(box);
    lv_obj_remove_style_all(icon);
    lv_obj_set_width(icon, 70);
    lv_obj_set_align(icon, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(icon, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(icon, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_pad_row(icon,5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_style(icon, &style.icon_focus, LV_STATE_FOCUSED);

    lv_obj_t* img = lv_img_create(icon);
    lv_img_set_src(img, img_src);
    lv_obj_set_width(img, LV_SIZE_CONTENT);   /// 48
    lv_obj_set_height(img, LV_SIZE_CONTENT);    /// 48

    lv_obj_t* label_name = lv_label_create(icon);
    lv_label_set_text(label_name, name);
    lv_obj_set_width(label_name, LV_SIZE_CONTENT);   /// 48
    lv_obj_set_height(label_name, LV_SIZE_CONTENT);    /// 48
	lv_obj_add_style(label_name, &style.font, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t* label_data = lv_label_create(box);
    lv_obj_remove_style_all(label_data);
    lv_label_set_text(label_data, infos);
    lv_obj_set_width(label_data, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(label_data, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(label_data, LV_ALIGN_LEFT_MID);
	lv_obj_add_style(label_data, &style.font, LV_PART_MAIN | LV_STATE_DEFAULT);

    /* get real max height */
    lv_obj_update_layout(label_data);
    lv_coord_t height = lv_obj_get_height(label_data);
    height = LV_MAX(height, ITEM_HEIGHT_MIN);
    lv_obj_set_height(box, height);
    lv_obj_set_height(icon, height);
}
static void Create(lv_obj_t* root)
{
    lv_obj_remove_style_all(root);
    lv_obj_set_width(root, 240);
    lv_obj_set_height(root, 240);
    lv_obj_set_align(root, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(root, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    Item_Create(root,
        "USB HID",
        &ui_img_pc_png,

        "Surface Dial\n"
		"KeyBoard\n"
		"Mouse\n"
		"Media\n"
        );
    Item_Create(root,
        "Setting",
        &ui_img_set_png,

        "Setting"
    );
    Item_Create(root,
        "PowerOff",
        &ui_img_power_png,

        "PowerOff"
    );
    Item_Create(root,
        "Custom",
        &ui_img_customize_png,

        "Custom HID"
    );

	ui_ArcScreen1 = lv_arc_create(ui_Screen1);
    lv_obj_set_width(ui_ArcScreen1, 240);
    lv_obj_set_height(ui_ArcScreen1, 240);
    lv_obj_set_x(ui_ArcScreen1, 3);
    lv_obj_set_y(ui_ArcScreen1, -3);
    lv_obj_set_align(ui_ArcScreen1, LV_ALIGN_CENTER);
    lv_arc_set_value(ui_ArcScreen1, 50);
    lv_arc_set_bg_angles(ui_ArcScreen1, 108, 169);
    lv_obj_set_style_arc_color(ui_ArcScreen1, lv_color_hex(0x303030), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_ArcScreen1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_ArcScreen1, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui_ArcScreen1, true, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_ArcScreen1, lv_color_hex(0xB0E14A), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_ArcScreen1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_ArcScreen1, 6, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui_ArcScreen1, true, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_ArcScreen1, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ArcScreen1, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
}
void ui_Screen1_dial_event(uint8_t state)
{
	switch (state)
    {
        case DIAL_STA_RELEASE:
            
        	break;
        case DIAL_STA_PRESS:

            break;
        case DIAL_STA_R:
            enc_num++;
            break;
        case DIAL_STA_L:
            enc_num--;
            break;
        case DIAL_STA_CLICK:
            int8_t index = icon_index;//change screen1
            ESP_LOGI(TAG, "current:%d ,state: %d", index,state);
            switch (index)
            {
                case 0://usb hid
                    _ui_screen_change(&ui_Screen2, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen2_screen_init);
                    break;
                case 1://setting
                    break;
                case 2://poweroff
                    power_off();
                    break;
                case 3://Customize HID Interface
                    _ui_screen_change(&ui_Screen3, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, &ui_Screen3_screen_init);
                default:
                    break;
            }
            break;
        
        default:
            break;
    }
                    /* code */
}
void scr_Screen1_unloaded_cb(lv_event_t * e)
{
	lv_obj_t ** var = lv_event_get_user_data(e);
	lv_timer_del(task1);
    lv_group_del(group);
    lv_obj_del(*var);
    (*var) = NULL;
}
void scr_Screen1_loaded_cb(lv_event_t * e)
{
    ui_state.index = UI_MENU_INTERFACE; 
    lv_indev_set_group(encoder_indev, group);
    foc_knob_set_param(screen1_foc_knob_param);
}
void task_bat_cb(lv_timer_t * tmr)
{
	uint8_t value = bat_val_get();
	lv_arc_set_value(ui_ArcScreen1, value);
}
void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_add_event_cb(ui_Screen1, scr_Screen1_loaded_cb, LV_EVENT_SCREEN_LOADED, &ui_Screen1);
	// lv_obj_add_event_cb(ui_Screen1, scr_Screen1_unloaded_cb, LV_EVENT_SCREEN_UNLOADED, &ui_Screen1);
	lv_obj_set_style_bg_img_src(ui_Screen1, &ui_img_bg1_png, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
	Style_Init();
    lv_obj_t* container = lv_obj_create(ui_Screen1);
    Create(container);
    uint32_t mid_btn_index = (lv_obj_get_child_cnt(container) - 1) / 2;
    for (uint32_t i = 0; i < mid_btn_index; i++)
    {
        lv_obj_move_to_index(lv_obj_get_child(container, -1), 0);
    }
    group_init(container, icon_index);
	task1 = lv_timer_create(task_bat_cb, 200, 0);
	lv_timer_set_repeat_count(task1,-1);
}
