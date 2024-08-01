#ifndef AHT20_H
#define AHT20_H

#include <esp_err.h>

// Define the I2C pins
#define I2C_MASTER_SDA 21
#define I2C_MASTER_SCL 22

// Error codes
#define AHT20_OK       0
#define AHT20_ERR_INIT -1
#define AHT20_ERR_READ -2

/**
 * Starts the moisture sensor task
 */
void hum_temp_sensor_task_start(void);

float getTemp(void);
float getHum(void);
#endif  // AHT20_H