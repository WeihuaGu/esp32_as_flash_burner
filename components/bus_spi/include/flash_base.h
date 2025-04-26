#ifndef __FLASH_BASE_H__
#define __FLASH_BASE_H__
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

// 定义 W25Q128JV 指令集
#define W25Q128JV_WRITE_ENABLE 0x06
#define W25Q128JV_WRITE_DISABLE 0x04
#define W25Q128JV_READ_ID 0x9F
#define W25Q128JV_READ_DATA 0x03
#define W25Q128JV_PAGE_PROGRAM 0x02
#define W25Q128JV_SECTOR_ERASE 0x20

static const char *TAG = "Tflash";

// 读取 FRAM 数据
//esp_err_t fram_read(uint16_t addr, uint8_t *data, size_t len);


// 写入 FRAM 数据
//esp_err_t fram_write(uint16_t addr, uint8_t *data, size_t len);


void flash_test();





#endif

