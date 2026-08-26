#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_event.h"

#define DEFAULT_MAX_SCAN_APS 5

int app_main(void) {

    // Setup the NVS flash and re-flash if necessary
    esp_err_t flash = nvs_flash_init();
    if (flash == ESP_ERR_NVS_NO_FREE_PAGES || flash == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        flash = nvs_flash_init();
    }
    ESP_ERROR_CHECK(flash);

    // Create the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Setup default wifi config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&cfg) != ESP_OK) {
        printf("WIFI init failed.\n");
        return 1;
    }

    // Set the wifi mode before we turn it on
    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) {
        printf("Error setting wifi mode\n");
        return 1;
    }

    // Start the wifi
    if (esp_wifi_start() != ESP_OK) {
        printf("Error starting wifi\n");
        return 1;
    }

    // Start the scan for APs
    esp_err_t scan_start = esp_wifi_scan_start(NULL, 1);
    if (scan_start != ESP_OK) {
        switch(scan_start) {
            case ESP_ERR_WIFI_NOT_INIT:
                printf("Wifi not initialized\n");
                break;
            case ESP_ERR_WIFI_NOT_STARTED:
                printf("Wifi not started\n");
                break;
            case ESP_ERR_WIFI_TIMEOUT:
                printf("Wifi timeout\n");
                break;
            case ESP_ERR_WIFI_STATE:
                printf("Wifi still connecting\n");
                break;
            default:
                printf("Some other error occurred\n");
                printf("%s\n", esp_err_to_name(scan_start));
        }
        return 1;
    }

    // Get number of APs and print it
    uint16_t number_of_aps = 0;
    if (esp_wifi_scan_get_ap_num(&number_of_aps) != ESP_OK) {
        printf("An error getting AP count\n");
        return 1;
    }
    
    printf("Total APs: %i\n", number_of_aps);

    // Now let's get the AP information and print it to the screen
    wifi_ap_record_t ap_records[DEFAULT_MAX_SCAN_APS];

    if (number_of_aps > DEFAULT_MAX_SCAN_APS) {
        number_of_aps = DEFAULT_MAX_SCAN_APS;
    }

    if (esp_wifi_scan_get_ap_records(&number_of_aps, ap_records) != ESP_OK) {
        printf("Error getting records\n");
        return 1;
    }

    //printf("SSID           | Signal Strength | Channel | MAC\n");
    printf("%-20s %-8s %-7s %-17s\n", "SSID", "Strength", "Channel" , "MAC");
    printf("----------------------------------------------------\n");
    for (int i = 0; i < number_of_aps; i++) {
        //printf("%s           | %i | %i | ", ap_records[i].ssid, ap_records[i].rssi, ap_records[i].primary);
        printf("%-20s %-8i %-7i ", ap_records[i].ssid, ap_records[i].rssi, ap_records[i].primary);
        for (int j = 0; j < 6; j++) {
            printf("%02X", ap_records[i].bssid[j]);
            if (j != 5) {
                printf(":");      
            }
        }
        printf("\n");
    }

    // CLear the list for now to free up memory
    if (esp_wifi_clear_ap_list() != ESP_OK) {
        printf("Error clearing AP list\n");
        return 1;
    }

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
