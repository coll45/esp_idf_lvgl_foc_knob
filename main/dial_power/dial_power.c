#include "dial_power.h"

static TaskHandle_t s_task_handle;
static const char *TAG = "POWER";
uint32_t adc_val;
static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    BaseType_t mustYield = pdFALSE;
    //Notify that ADC continuous driver has done enough number of conversions
    vTaskNotifyGiveFromISR(s_task_handle, &mustYield);

    return (mustYield == pdTRUE);
}
static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)
{
    //adc init
    adc_continuous_handle_t handle = NULL;

    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,
        .conv_frame_size = EXAMPLE_READ_LEN,
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &handle));

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = 20 * 1000,
        .conv_mode = EXAMPLE_ADC_CONV_MODE,
        .format = EXAMPLE_ADC_OUTPUT_TYPE,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++) {
        adc_pattern[i].atten = EXAMPLE_ADC_ATTEN;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = EXAMPLE_ADC_UNIT;
        adc_pattern[i].bit_width = EXAMPLE_ADC_BIT_WIDTH;

        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%"PRIx8, i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%"PRIx8, i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%"PRIx8, i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_continuous_config(handle, &dig_cfg));

    *out_handle = handle;
}
uint32_t AvgFilter(uint32_t data)
{
    /* filter_buff, FILTER_SIZE */
    static uint32_t fifo[16];
    static uint8_t data_index = 0;
    static uint32_t sum=0;
    uint32_t da;
    /* find the index-9' data */
    da = fifo[data_index];
    fifo[data_index] = data;
    data_index = (data_index + 1) % 16;
    sum += data;
    sum -= da;
    return(sum>>4);
}
void adc_read_task()
{
    esp_err_t ret;
    uint32_t ret_num = 0;
    uint8_t result[EXAMPLE_READ_LEN] = {0};
    memset(result, 0xcc, EXAMPLE_READ_LEN);

    s_task_handle = xTaskGetCurrentTaskHandle();

    adc_continuous_handle_t handle = NULL;
    continuous_adc_init(adc_channel, sizeof(adc_channel) / sizeof(adc_channel_t), &handle);

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = s_conv_done_cb,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    ESP_ERROR_CHECK(adc_continuous_start(handle));

    while (1) {

        /**
         * This is to show you the way to use the ADC continuous mode driver event callback.
         * This `ulTaskNotifyTake` will block when the data processing in the task is fast.
         * However in this example, the data processing (print) is slow, so you barely block here.
         *
         * Without using this event callback (to notify this task), you can still just call
         * `adc_continuous_read()` here in a loop, with/without a certain block timeout.
         */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        char unit[] = EXAMPLE_ADC_UNIT_STR(EXAMPLE_ADC_UNIT);

        while (1) {
            ret = adc_continuous_read(handle, result, EXAMPLE_READ_LEN, &ret_num, 0);
            if (ret == ESP_OK) {
                // ESP_LOGI("TASK", "ret is %x, ret_num is %"PRIu32" bytes", ret, ret_num);
                // for (int i = 0; i < ret_num; i += SOC_ADC_DIGI_RESULT_BYTES) {
                    int i = 0;
                    adc_digi_output_data_t *p = (adc_digi_output_data_t*)&result[i];
                    uint32_t chan_num = EXAMPLE_ADC_GET_CHANNEL(p);
                    // adc_val = EXAMPLE_ADC_GET_DATA(p);
                    adc_val = AvgFilter(EXAMPLE_ADC_GET_DATA(p));
                    /* Check the channel number validation, the data is invalid if the channel num exceed the maximum channel */
                    // if (chan_num < SOC_ADC_CHANNEL_NUM(EXAMPLE_ADC_UNIT)) {
                    //     ESP_LOGI(TAG, "Unit: %s, Channel: %"PRIu32", Value: %"PRIu32, unit, chan_num, adc_val);
                    // } else {
                    //     ESP_LOGW(TAG, "Invalid data [%s_%"PRIu32"_%"PRIx32"]", unit, chan_num, adc_val);
                    // }
                // }
                /**
                 * Because printing is slow, so every time you call `ulTaskNotifyTake`, it will immediately return.
                 * To avoid a task watchdog timeout, add a delay here. When you replace the way you process the data,
                 * usually you don't need this delay (as this task will block for a while).
                 */
                vTaskDelay(10);
            } else if (ret == ESP_ERR_TIMEOUT) {
                //We try to read `EXAMPLE_READ_LEN` until API returns timeout, which means there's no available data
                break;
            }
        }
    }
    ESP_ERROR_CHECK(adc_continuous_stop(handle));
    ESP_ERROR_CHECK(adc_continuous_deinit(handle));
}
void power_gpio_init()
{
    // power gpio init
    gpio_set_direction(POWER_GPIO, GPIO_MODE_OUTPUT);
    power_on();

    xTaskCreate(adc_read_task, "adc_read_task", 4096, NULL, 7, NULL);
}
void power_on()
{
    gpio_set_level(POWER_GPIO, 1);
}
void power_off()
{
    gpio_set_level(POWER_GPIO, 0);
}
uint32_t bat_adc_get()
{
    uint32_t data = (adc_val*3300*2)>>12;
    return data;
}

#define ARRAY_DIM(a) (sizeof(a) / sizeof((a)[0]))
const static int Battery_Level_Percent_Table[11] = {3400, 3650, 3700, 3740, 3760, 3795, 3840, 3910, 3980, 4070, 4200};
int toPercentage(uint32_t voltage)
{
	if(voltage < Battery_Level_Percent_Table[0])
		return 0;
    
	for(int i = 0; i<ARRAY_DIM(Battery_Level_Percent_Table); i++)
    {
		if(voltage < Battery_Level_Percent_Table[i])
			return i*10 - (10UL * (int)(Battery_Level_Percent_Table[i] - voltage)) / 
			(int)(Battery_Level_Percent_Table[i] - Battery_Level_Percent_Table[i-1]);;
	}
	return 100;
}
uint8_t bat_val_get()
{
    uint32_t bat_val = bat_adc_get();
    int val = toPercentage(bat_val);
    ESP_LOGI(TAG, "adc:%ld    percent: %d",bat_val,val);
    return val;
}