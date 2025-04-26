#ifndef __BUS_SPI_H__
#define __BUS_SPI_H__
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define USE_SPI_HOST    SPI2_HOST
#define PIN_NUM_CLK  4
#define PIN_NUM_MISO 5
#define PIN_NUM_MOSI 6

// 初始化 VSPI 总线
esp_err_t bus_spi_init(void);

esp_err_t only_one_cmd(int spidevice, uint8_t cmd, uint8_t* out_data ,size_t out_len);

esp_err_t send_command_array(int spidevice, const uint8_t *commands, size_t command_len);

esp_err_t send_command_array_withreturn(int spidevice, const uint8_t *commands, size_t command_len, uint8_t *rx_data, size_t rx_len);



#endif // __BUS_SPI_H__

