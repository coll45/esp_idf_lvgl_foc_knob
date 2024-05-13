# 开发日志
- [x] GC9A01亮屏 添加组件esp_lcd_gc9a01: "^2.0.0"
- - 添加GC9A01组件库
- - 修改对应屏幕引脚
- [x] 运行LVGL
- - 自己开发需要添加esp_lvgl_port1.4.0，lvgl版本8.4（会自动下载LVGL9）**后期官方更新可能会遇到自动下载LVGL9的问题，需要自己创建component文件夹然后在里面放入esp_lvgl_port、LVGL8**
- - 在SDK Configuration里面需要修改LVGL参数，16：RGB565、swap the 2 bytes等参数
- - 需要修改LVGL旋转参数，并且修改后要设置旋转方向
```c
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
- - foc的PID点特殊，用到了时间来算D和I，所以要使用foc自带的pid去控制foc运动
- [x] usb hid 实现surface dial、键盘音量、媒体、滚轮、鼠标、按键等功能
- - 尝试切换界面时候启用HID成功，但是使用tinyusb_driver_uninstall卸载设备会出现USB设备虽然卸载了，但是再次HID_init时候电脑不会再次获取HID设备报文
- - 最终解决办法是，切换界面HID init只调用一次，让HID功能常开，切回其他界面时也不卸载HID设备
- [x] 蓝牙 hid
- - 问题描述：蓝牙一次性上报了四个HID设备：键盘、鼠标、Surface Dial、用户自定义媒体，出现第一次配对后，四个设备正常工作。esp32断电重启后，重新连接已配对的蓝牙设备，会出现四个蓝牙设备会有某一个或者几个无法正常工作
- - 问题分析：蓝牙上报设备里面不上报Surface Dial，只有三个HID设备的时候非常的稳定，断电重连也不会有问题。加入Surface Dial就会出现问题。但是单独上报一个Surface Dial不会出现无法正常工作的情况。初步判断是Surface Dial的设备HID描述有关系，这个设备描述非常的小众，通常不会有人使用这个HID描述
- - 解决问题：换了一个蓝牙hid demo 实现Surface Dial的HID描述，可以正常工作。参考项目：https://www.bilibili.com/video/BV1AC41157nm
- [] wifiwebsever 
- -  [x] 配网：通过AP开启网页，输入wifi账号密码进行连接用户自己的wifi
- - []  web OTA：实现网页更新固件
- - []  授权码：获取esp32的mac唯一地址，进行算法加密, 用户通过网页输入授权码，解锁固件全功能。
- - []  提供完整版的bin，开启wifi后会在网页上看见芯片唯一mac，用户提供芯片唯一mac，作者解密给他授权码，然后用户拿授权码输入到网页里面，让esp32自己判断是否正确，后开启全功能
- - []  页面实现自定义旋钮按键功能、力度、旋钮运动模式
- - []  实现页面上传图片进行更换icon
- [] midi 输入
- [] mqtt 智能家居

。。。。