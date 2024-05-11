#include "wifi_config.h"
static bool g_wifi_sta_inited = false;
static bool g_wifi_ap_inited = false;
static bool g_wifi_sta_ap_state = false;
static char wifi_current_src[100] = "";
// SemaphoreHandle_t ap_sem;
esp_event_handler_instance_t instance_any_id;
esp_event_handler_instance_t instance_got_ip;

#define SCRATCH_BUFSIZE 8192
static const char *TAG = "TAG";
#define EXAMPLE_ESP_MAXIMUM_RETRY 10
#define PRIFIX_NAME "SuperDial"

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
esp_netif_t *esp_netif;
httpd_handle_t server = NULL;

#define EXAMPLE_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS ""
#define EXAMPLE_MAX_STA_CONN CONFIG_LWIP_MAX_SOCKETS

#define EXAMPLE_ESP_MAXIMUM_RETRY 5
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif
extern const char root_start[] asm("_binary_root_html_start");
extern const char root_end[] asm("_binary_root_html_end");
static void wifi_deinit(); // 关闭wifi热点
static void wifi_ap_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_station_task(void *pvParameters)
{
    uint32_t result = 0;
    while (1)
    {
        // result = xSemaphoreTake(ap_sem, portMAX_DELAY);
        if (result == pdPASS)
        {
            esp_restart();
            esp_wifi_stop();
            esp_event_handler_unregister(WIFI_EVENT,
                                         ESP_EVENT_ANY_ID,
                                         &wifi_ap_event_handler);
            esp_netif_destroy_default_wifi(esp_netif);
            esp_event_loop_delete_default();
            esp_wifi_deinit();
            esp_netif_deinit();
            httpd_stop(server);
            wifi_init_sta();
            ESP_LOGI(TAG, "wifi station inited...");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
//---------------------nvs-----------------------//
void nvs_get_str_log(esp_err_t err, char *key, char *value)
{
    switch (err)
    {
    case ESP_OK:
        // ESP_LOGI(TAG, "%s = %s", key, value);
        ESP_LOGI(TAG, "nvs get %s value success!", key);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGI(TAG, "%s : Can't find in NVS!", key);
        break;
    default:
        ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
    }
}

esp_err_t from_nvs_set_value(char *key, char *value)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("memory", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ESP_FAIL;
    }
    else
    {
        err = nvs_set_str(my_handle, key, value);
        ESP_LOGI(TAG, "set %s is %s!,the err is %d\n", key, (err == ESP_OK) ? "succeed" : "failed", err);
        nvs_close(my_handle);
        ESP_LOGI(TAG, "NVS close Done\n");
    }
    return ESP_OK;
}

esp_err_t from_nvs_get_value(char *key, char *value, size_t *size)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("memory", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ESP_FAIL;
    }
    else
    {
        err = nvs_get_str(my_handle, key, value, size);
        nvs_get_str_log(err, key, value);
        nvs_close(my_handle);
    }
    return err;
}
/**************读取Nvs中WIFI账号密码***************/
esp_err_t http_get_nvs_wifi_config(wifi_config_t *wifi_config)
{
    char str1[64] = "";
    char str2[64] = "";
    char str3[64] = "";
    size_t str1_size = sizeof(str1);
    size_t str2_size = sizeof(str2);
    size_t str3_size = sizeof(str3);

    esp_err_t err = from_nvs_get_value("ssid", str1, &str1_size);
    if (err == ESP_OK)
    {
        strncpy(&wifi_config->sta.ssid, str1, str1_size);
        // sprintf(start_data.wifi_name, str1);
    }
    err = from_nvs_get_value("password", str2, &str2_size);
    if (err == ESP_OK)
    {
        strncpy(&wifi_config->sta.password, str2, sizeof(wifi_config->sta.password));
    }
    err = from_nvs_get_value("decive_code", str3, &str3_size);
    if (err == ESP_OK)
    {
        // strncpy(start_data.qcp, str3, sizeof(start_data.qcp));
    }

    ESP_LOGI(TAG, "%s\r\n", wifi_config->sta.ssid);
    ESP_LOGI(TAG, "%s\r\n", wifi_config->sta.password);
    // ESP_LOGI(TAG, "%s\r\n", start_data.qcp);
    return ESP_OK;
}

//---------------------wifi_sta-----------------------//
static int s_retry_num = 0;

static void sta_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_event_sta_disconnected_t *disconnected = (wifi_event_sta_disconnected_t *)event_data;
        // ESP_LOGE(TAG, "Disconnect reason : %d", disconnected->reason);
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            char src[6] = "......";
            char src_num[6] = "";
            strncpy(src_num, src, s_retry_num);
            sprintf(wifi_current_src, "retry to connect %s", src_num);
            ESP_LOGI(TAG, "retry to connect to the AP ,s_retry_num:%d", s_retry_num);
        }
        else
        {
            // xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            // wifi_deinit();
            sprintf(wifi_current_src, "connect to the AP fail");
            ESP_LOGI(TAG, "connect to the AP fail");
        }
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_BEACON_TIMEOUT)
    {
        ESP_LOGI(TAG, "WIFI_EVENT_STA_BEACON_TIMEOUT");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        sprintf(wifi_current_src, "IP:%d.%d.%d.%d", IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        g_wifi_sta_ap_state = 0;
        // xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
void wifi_init_sta()
{
    // s_wifi_event_group = xEventGroupCreate();

    // ESP_ERROR_CHECK(esp_event_loop_create_default());
    // ESP_ERROR_CHECK(esp_netif_init());
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            // .password = "12345678",
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    // wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    // printf("%s\r\n",wifi_config.sta.ssid);
    // printf("%s\r\n",wifi_config.sta.password);
    http_get_nvs_wifi_config(&wifi_config);
    if(strlen((char*)wifi_config.sta.ssid) == 0)
    {
        ESP_LOGI(TAG, "No wifi config found in NVS");
        sprintf(wifi_current_src, " wifi name is null");
        return;
    }
    sprintf(wifi_current_src, "connect :%s", wifi_config.sta.ssid);
    

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    s_retry_num = 0;

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    esp_wifi_connect();

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    // EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
    //                                        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
    //                                        pdFALSE,
    //                                        pdFALSE,
    //                                        portMAX_DELAY);

    // /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
    //  * happened. */
    // if (bits & WIFI_CONNECTED_BIT)
    // {
    //     ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
    //              wifi_config.sta.ssid, wifi_config.sta.password);
    // }
    // else if (bits & WIFI_FAIL_BIT)
    // {
    //     ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
    //              wifi_config.sta.ssid, wifi_config.sta.password);
    // }
    // else
    // {
    //     ESP_LOGE(TAG, "UNEXPECTED Eprotocol_examples_common.h:ENT");
    // }

    // /* The event will not be processed after unregister */
    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    // ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    // vEventGroupDelete(s_wifi_event_group);
}
static void wifi_deinit() // 关闭wifi热点
{
    if (g_wifi_ap_inited == 0 && g_wifi_sta_inited == 0)
    {
        return;
    }
    if (g_wifi_sta_inited)
    {
        /* The event will not be processed after unregister */
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
        g_wifi_sta_inited = 0;
        s_retry_num = 0;
    }
    if (g_wifi_ap_inited)
    {
        esp_event_handler_unregister(WIFI_EVENT,
                                     ESP_EVENT_ANY_ID,
                                     &wifi_ap_event_handler);
        g_wifi_ap_inited = 0;
    }
    esp_wifi_stop();
    esp_netif_destroy_default_wifi(esp_netif);
    // esp_event_loop_delete_default();
    esp_wifi_deinit();
    // esp_netif_deinit();
    // httpd_stop(server);
    ESP_LOGI(TAG, "wifi deinit...");
}
void wifi_init_softap(void)
{
    char wifi_ap_name[32];
    sprintf(wifi_ap_name, "%s-%s", PRIFIX_NAME, sys_config.mac);

    wifi_config_t wifi_config = {
        .ap = {
            // .ssid = wifi_ap_name,
            .ssid_len = strlen(wifi_ap_name),
            .password = "",
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };
    memcpy(wifi_config.ap.ssid, wifi_ap_name, sizeof(wifi_config.ap.ssid));
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA)); // AP+STA
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
    sprintf(wifi_current_src, "Wifi:%16s  \nIP:%s", wifi_ap_name, ip_addr);
    ESP_LOGI(TAG, "Set up softAP with IP: %s", ip_addr);

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:'%s' password:'%s'",
             wifi_ap_name, EXAMPLE_ESP_WIFI_PASS);
    g_wifi_sta_ap_state = 1;
}

// HTTP GET Handler
static esp_err_t root_get_handler(httpd_req_t *req)
{   
    ESP_LOGI(TAG, "Serve root");
    if(g_wifi_sta_ap_state) // 热点模式
    {
        const uint32_t root_len = root_end - root_start;
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, root_start, root_len);
    }
    else // 客户端模式
    {
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, "ok", HTTPD_RESP_USE_STRLEN);
    }

    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
};

// HTTP Error (404) Handler - Redirects all requests to the root page
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGI(TAG, "Redirecting to root");
    return ESP_OK;
}
// WiFi Scan Handler 热点扫描
#define DEFAULT_WIFI_SCAN_COUNT 20
static wifi_ap_record_t wifi_scan_list[DEFAULT_WIFI_SCAN_COUNT];
static uint16_t wifi_scan_count = DEFAULT_WIFI_SCAN_COUNT;

static void get_wifi_list(void)
{
    esp_wifi_scan_start(NULL, true);
    esp_wifi_scan_get_ap_num(&wifi_scan_count);
    // printf("Number of access points found: %d\n", wifi_scan_count);
    if (wifi_scan_count > 0)
    {
        if (wifi_scan_count > DEFAULT_WIFI_SCAN_COUNT)
            wifi_scan_count = DEFAULT_WIFI_SCAN_COUNT;
        ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&wifi_scan_count, wifi_scan_list));
        // printf("  SSID             | Channel | RSSI | Authmode\n");
        // for (int i = 0; i < wifi_scan_count; i++)
        // {
        //     printf("%32s | %7d | %4d | %s\n", (char *)wifi_scan_list[i].ssid, wifi_scan_list[i].primary, wifi_scan_list[i].rssi, (wifi_scan_list[i].authmode == WIFI_AUTH_OPEN) ? "open" : "unknown");
        // }
    }
}
static esp_err_t wifi_scan_handler(httpd_req_t *req)
{
    cJSON *wifi_infos_obj = cJSON_CreateObject();
    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    get_wifi_list();
    cJSON *wifi_infos = cJSON_CreateArray(); // 创建一个数组
    char *ssid;
    for (int i = 0; i < wifi_scan_count; i++)
    {
        cJSON *wifi_info = cJSON_CreateObject();
        cJSON_AddNumberToObject(wifi_info, "rssi", wifi_scan_list[i].rssi);
        cJSON_AddStringToObject(wifi_info, "ssid", (char *)(wifi_scan_list[i].ssid));
        cJSON_AddItemToArray(wifi_infos, wifi_info);
    }
    cJSON_AddItemToObject(wifi_infos_obj, "wifi_infos", wifi_infos); // 各模块数据
    char *wifi_infos_obj_json = cJSON_Print(wifi_infos_obj);         // JSON数据结构转换为JSON字符串
    httpd_resp_send(req, wifi_infos_obj_json, strlen(wifi_infos_obj_json));
    ESP_LOGI(TAG, "wifi scan get successfully!");
    return ESP_OK;
}
static const httpd_uri_t wifi_scan = {
    .uri = "/wifi_scan", // Match all URIs of type /upload/path/to/file
    .method = HTTP_GET,
    .handler = wifi_scan_handler,
};
static esp_err_t send_wifi_handler(httpd_req_t *req)
{
    char user_id[32] = "";
    char user_code[64] = "";
    char qcp_code[10] = "";
    int total_len = req->content_len;
    int cur_len = 0;
    char buf[SCRATCH_BUFSIZE];
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE)
    {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len)
    {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0)
        {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    // char *str_u = "abcdefg";
    // for(int i = 0; i<sizeof(str_u);i++){
    //     putchar(str_u[i]);
    // }
    buf[total_len] = '\0';
    for (int i = 0; i < total_len; i++)
    {
        putchar(buf[i]);
    }
    cJSON *root = cJSON_Parse(buf);
    //  int ssid = cJSON_GetObjectItem(root, "wifi_name")->valueint;
    //  int code = cJSON_GetObjectItem(root, "wifi_code")->valueint;

    char *ssid = cJSON_GetObjectItem(root, "wifi_name")->valuestring;
    char *code = cJSON_GetObjectItem(root, "wifi_code")->valuestring;
    char *decive_code = cJSON_GetObjectItem(root, "decive_code")->valuestring;
    int len1 = strlen(ssid);
    int len2 = strlen(code);
    int len3 = strlen(decive_code);
    memcpy(user_id, ssid, strlen(ssid));
    memcpy(user_code, code, strlen(code));
    memcpy(qcp_code, decive_code, strlen(decive_code));
    user_id[len1] = '\0';
    user_code[len2] = '\0';
    qcp_code[len3] = '\0';
    cJSON_Delete(root);
    //  ESP_LOGI(TAG, "json load  finished. SSID:%d password:%d ",ssid,code);
    ESP_LOGI(TAG, "json load  finished. SSID:%s password:%s ", user_id, user_code);

    // printf("\r\nwifi_ssid:");
    // for(int i = 0;i<len1;i++){
    //     printf("%c",user_id[i]);
    // }

    // printf("\r\nwifi_code:");
    // for(int i = 0;i<len2;i++){
    //     printf("%c",user_code[i]);
    // }
    ESP_ERROR_CHECK(from_nvs_set_value("ssid", user_id));
    ESP_ERROR_CHECK(from_nvs_set_value("password", user_code));
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    if (strcmp(user_id, "\0") != 0 && strcmp(user_code, "\0") != 0)
    {
        // xSemaphoreGive(ap_sem);
        ESP_LOGI(TAG, "set wifi name and password successfully! goto station mode");
    }

    return ESP_OK;
}
static const httpd_uri_t wifi_data = {
    .uri = "/wifi_data", // Match all URIs of type /upload/path/to/file
    .method = HTTP_POST,
    .handler = send_wifi_handler,
};
static esp_err_t index_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "uri: /");
    const char *html = "<div><a href=\"/long\">long</a></div>"
                       "<div><a href=\"/quick\">quick</a></div>";
    httpd_resp_sendstr(req, html);
    return ESP_OK;
}
httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // config.stack_size = 8192 * 4; // 32KB
    config.max_open_sockets = 10;
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &wifi_data);
        httpd_register_uri_handler(server, &wifi_scan);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    return server;
}
void wifi_init(uint8_t mode)
{
    static bool wifi_init_flag = false;
    if (wifi_init_flag == false)
    {
        wifi_init_flag = true;
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();
        esp_netif_create_default_wifi_ap();
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &sta_event_handler,
                                                            NULL,
                                                            &instance_any_id));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                            IP_EVENT_STA_GOT_IP,
                                                            &sta_event_handler,
                                                            NULL,
                                                            &instance_got_ip));

        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_ap_event_handler, NULL));
        start_webserver();
    }

    if (mode == 1) // STA
    {
        wifi_init_sta();
    }
    else if (mode == 2) // AP
    {
        wifi_init_softap();
    }
    else
    {
        ESP_LOGE(TAG, "wifi mode error");
    }
    
}
void wifi_stop(void)
{
    esp_wifi_stop();
    // httpd_stop(server);
}
void get_wifi_current_status(char *src)
{
    strcpy(src, wifi_current_src);
}