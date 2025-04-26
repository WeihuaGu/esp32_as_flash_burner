#ifndef __SPI_CS_H__
#define __SPI_CS_H__

#define FRAM_PIN_NUM_CS 7
#define FLASH_PIN_NUM_CS 10
#define SPI_DEVICE_MAX 2
// 定义SPI设备的枚举
typedef enum {
    SPI_DEVICE_FRAM,
    SPI_DEVICE_FLASH
} spi_device_type_t;

#endif

