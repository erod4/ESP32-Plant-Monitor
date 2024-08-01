#include "AHT20.h"
#include "aws_iot.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "moisture_sensor.h"
#include "nvs_flash.h"
#include "sntp_time_sync.h"
#include "toggle_sleep_button.h"
#include "wifi_app.h"
static const char TAG[] = "main";
static SemaphoreHandle_t wifi_callback_semaphore;
// void wifi_application_connected_events_GPIO(void) {
//     ESP_LOGI(TAG, "WiFi Application Connected!!");
//     sntp_time_sync_task_start();
// }
void wifi_application_connected_events_TIMER(void) {
    ESP_LOGI(TAG, "WiFi Application Connected!!");
    sntp_time_sync_task_start();
    aws_iot_start();
    xSemaphoreGive(wifi_callback_semaphore);
}
// application entry point
void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    // Create the semaphore
    wifi_callback_semaphore = xSemaphoreCreateBinary();
    if (wifi_callback_semaphore == NULL) {
        ESP_LOGE(TAG, "Failed to create semaphore");
        return;
    }
    // start WiFi
    wifi_app_start();

    // if wake up source was timer continue as usual
    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
    switch (wakeup_cause) {
        case ESP_SLEEP_WAKEUP_UNDEFINED:
            ESP_LOGI(TAG, "Wakeup cause: Undefined (first boot or reset)");
            sleep_toggle_button_config();
            hum_temp_sensor_task_start();

            break;
        case ESP_SLEEP_WAKEUP_EXT0:
            ESP_LOGI(TAG, "Wakeup cause: GPIO (EXT0)");
            sleep_toggle_button_config();

            break;
        case ESP_SLEEP_WAKEUP_EXT1:
            ESP_LOGI(TAG, "Wakeup cause: GPIO (EXT1)");
            sleep_toggle_button_config();
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            ESP_LOGI(TAG, "Wakeup cause: Timer");
            moisture_sensor_task_start();
            hum_temp_sensor_task_start();
            // Set connected event callback
            wifi_app_set_callback(&wifi_application_connected_events_TIMER);
            // Wait for the callback to signal that it has finished
            if (xSemaphoreTake(wifi_callback_semaphore, portMAX_DELAY) == pdTRUE) {
                // Wait for some time before going to sleep
                vTaskDelay(10000 / portTICK_PERIOD_MS);
                start_sleep();  // Start deep sleep with timer and button as source
            } else {
                ESP_LOGE(TAG, "Failed to take semaphore");
            }
            break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            ESP_LOGI(TAG, "Wakeup cause: Touchpad");
            sleep_toggle_button_config();
            break;
        case ESP_SLEEP_WAKEUP_ULP:
            ESP_LOGI(TAG, "Wakeup cause: ULP");
            sleep_toggle_button_config();
            break;
        default:
            ESP_LOGI(TAG, "Wakeup cause: Other");
            sleep_toggle_button_config();
            break;
    }
}
