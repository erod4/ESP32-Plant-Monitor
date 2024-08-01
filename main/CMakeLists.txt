idf_component_register(SRCS "app_nvs.c" "wifi_app.c" "rgb_led.c" "main.c" "http_server.c" "AHT20.c" "moisture_sensor.c" "toggle_sleep_button.c"  "aws_iot.c" "sntp_time_sync.c"
                    INCLUDE_DIRS "." EMBED_FILES "webpage/app.css" "webpage/index.html" "webpage/jquery-3.3.1.min.js"  "webpage/app.js" "webpage/favicon.ico" )
target_add_binary_data(${COMPONENT_TARGET} "certs/aws_root_ca_pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/certificate_pem_crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/private_pem_key" TEXT)