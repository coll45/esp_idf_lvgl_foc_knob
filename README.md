# 开发日志
- [x] GC9A01亮屏 添加组件esp_lcd_gc9a01: "^2.0.0"
- - 添加GC9A01组件库
- - 修改对应屏幕引脚
- [x] 运行LVGL
- - 自己开发需要添加esp_lvgl_port1.4.0，lvgl版本8.4（会自动下载LVGL9）**后期官方更新可能会遇到自动下载LVGL9的问题，需要自己创建component文件夹然后在里面放入esp_lvgl_port、LVGL8**
- - 在SDK Configuration里面需要修改LVGL参数，16：RGB565、swap the 2 bytes等参数
- - 需要修改LVGL旋转参数，并且修改后要设置旋转方向
```cpp
const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = lcd_io,
        .panel_handle = lcd_panel,
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_DRAW_BUFF_HEIGHT * sizeof(uint16_t),
        .double_buffer = EXAMPLE_LCD_DRAW_BUFF_DOUBLE,
        .hres = EXAMPLE_LCD_H_RES,
        .vres = EXAMPLE_LCD_V_RES,
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation = {
            .swap_xy = false,//change here
            .mirror_x = true,//change here
            .mirror_y = false,//change here
        },
        .flags = {
            .buff_dma = true,
        }
    };
    lvgl_disp = lvgl_port_add_disp(&disp_cfg);
    lv_disp_set_rotation(lvgl_disp, LV_DISP_ROT_NONE);
```

- [x] Squareline导出ui文件运行
- - 在Sq设置中修改lvgl/lvgl.h头文件位置为lvgl.h
- [x] LVGL实现物理输入
- [x] 运行FOC
- [x] 电机数据做为LVGL输入
- [x] 运行Knob电机控制，电机力反馈控制LVGL
- [x] usb hid 实现surface dial、键盘音量、媒体、滚轮、鼠标、按键等功能
- - 尝试切换界面时候启用HID成功，但是使用tinyusb_driver_uninstall卸载设备会出现USB设备虽然卸载了，但是再次HID_init时候电脑不会再次获取HID设备报文
- - 最终解决办法是，切换界面HID init只调用一次，让HID功能常开，切回其他界面时也不卸载HID设备
- [] 蓝牙 hid
- [] wifiwebsever 
- - 配网：通过AP开启网页，输入wifi账号密码进行连接用户自己的wifi
- - web OTA：实现网页更新固件
- - 授权码：获取esp32的mac唯一地址，进行算法加密, 用户通过网页输入授权码，解锁固件全功能。
- - 提供完整版的bin，开启wifi后会在网页上看见mac地址，用户拿mac给我，作者解密给他授权码，然后用户拿授权码输入到网页里面，让esp32自己判断是否正确，后开启全功能
- - 页面实现自定义旋钮按键功能、力度、旋钮运动模式
- - 实现页面上传图片进行更换icon
- [] midi 输入
- [] mqtt 智能家居

。。。。