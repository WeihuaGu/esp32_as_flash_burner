#include <string.h>
#include "spi_cs.h"
#include "bus_spi.h"

// 存储每个设备的句柄
spi_device_handle_t spi_handles[SPI_DEVICE_MAX];
int cs_nums[SPI_DEVICE_MAX];

// 回调函数
void cs_high(spi_transaction_t *trans) {
    // 从 user 指针中获取 CS 引脚编号
    int *cs_pin_num_ptr = (int *)trans->user;
    int cs_pin_num = *cs_pin_num_ptr;
    gpio_set_level(cs_pin_num, 1);
}
void cs_low(spi_transaction_t *trans) {
    // 从 user 指针中获取 CS 引脚编号
    int *cs_pin_num_ptr = (int *)trans->user;
    int cs_pin_num = *cs_pin_num_ptr;
    gpio_set_level(cs_pin_num, 0);
}

esp_err_t bus_spi_init(void) {

    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096
    };
    spi_device_interface_config_t devcfg_fram = {
        .clock_speed_hz = 1 * 1000 * 1000, // 1 MHz
        .mode = 1,                          // SPI mode 0
        .spics_io_num = FRAM_PIN_NUM_CS,         // CS pin
        .queue_size = 1,                      // Transaction queue size
	.command_bits = 8,
	.pre_cb = cs_low,
        .post_cb = cs_high,

    };
    spi_device_interface_config_t devcfg_flash = {
        .clock_speed_hz = 1 * 1000 * 1000, // 1 MHz
        .mode = 1,                          // SPI mode 0
        .spics_io_num = FLASH_PIN_NUM_CS,         // CS pin
        .queue_size = 1,                      // Transaction queue size
	.command_bits = 8,
	.pre_cb = cs_low,
        .post_cb = cs_high,
    };
    gpio_set_direction(FRAM_PIN_NUM_CS, GPIO_MODE_OUTPUT);
    gpio_set_direction(FLASH_PIN_NUM_CS, GPIO_MODE_OUTPUT);
    cs_nums[SPI_DEVICE_FRAM] = FRAM_PIN_NUM_CS;
    cs_nums[SPI_DEVICE_FLASH] = FLASH_PIN_NUM_CS;

    // 初始化 SPI 总线
    esp_err_t ret = spi_bus_initialize(USE_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE("BUS_SPI", "Failed to initialize SPI bus");
        return ret;
    }
    // 添加设备到 SPI 总线
    ret = spi_bus_add_device(USE_SPI_HOST, &devcfg_fram, &spi_handles[SPI_DEVICE_FRAM]);
    if (ret != ESP_OK) {
        ESP_LOGE("BUS_SPI", "Failed to add fram device to SPI bus");
        return ret;
    }
    ret = spi_bus_add_device(USE_SPI_HOST, &devcfg_flash,&spi_handles[SPI_DEVICE_FLASH]);
    if (ret != ESP_OK) {
        ESP_LOGE("BUS_SPI", "Failed to add flash device to SPI bus");
        return ret;
    }

    return ESP_OK;
}

// send only 8 bits cmd
esp_err_t only_one_cmd(int spidevice, uint8_t cmd, uint8_t* out_data, size_t out_len)
{
    spi_transaction_t t = {
	.user = &cs_nums[spidevice],
        .cmd = cmd,
        .length = 8,
        .rx_buffer = NULL,
    };
    //esp_err_t err = spi_device_polling_transmit(spi_handles[spidevice], &t);
    esp_err_t err = spi_device_transmit(spi_handles[spidevice], &t);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t *tx_dummy = NULL;
    tx_dummy = (uint8_t*)malloc(out_len);
    memset(tx_dummy,0x00, out_len);
    spi_transaction_t rt = {
	.user = &cs_nums[spidevice],
        .tx_buffer = tx_dummy,
        .length = out_len * 8,
        .rxlength = out_len * 8,
        .rx_buffer = out_data
    };
    //err = spi_device_polling_transmit(spi_handles[spidevice], &rt);
    err = spi_device_transmit(spi_handles[spidevice], &rt);
    if (err != ESP_OK) {
        return err;
    }
    return ESP_OK;
}

esp_err_t send_command_array(int spidevice, const uint8_t *commands, size_t command_len) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.user = &cs_nums[spidevice];
    t.length = command_len * 8;  // 每个字节8位
    t.tx_buffer = commands;
    t.rx_buffer = NULL;
    printf("only commands: ");
    for(int i=0;i<command_len;i++){
             printf("0x%02X ", commands[i]);
    }
    printf("\n");
    //ret = spi_device_polling_transmit(spi_handles[spidevice], &t);
    ret = spi_device_transmit(spi_handles[spidevice], &t);
    if (ret != ESP_OK) {
        ESP_LOGE("BUS_SPI", "Failed to send command array: %s", esp_err_to_name(ret));
    }
    return ret;
}
esp_err_t send_command_array_withreturn(int spidevice, const uint8_t *commands, size_t command_len, uint8_t *rx_data, size_t rx_len) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.user = &cs_nums[spidevice];
    t.tx_buffer = commands;
    t.length = command_len * 8;
    t.rx_buffer = NULL;
    printf("commands: ");
    for(int i=0;i<command_len;i++){
             printf("0x%02X ", commands[i]);
    }
    printf("\n");
    //ret = spi_device_polling_transmit(spi_handles[spidevice], &t);
    ret = spi_device_transmit(spi_handles[spidevice], &t);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to send command array: %s", esp_err_to_name(ret));
    }

    spi_transaction_t rt;
    memset(&rt, 0, sizeof(rt));
    uint8_t *tx_dummy = NULL;
    tx_dummy = (uint8_t*)malloc(rx_len);
    memset(tx_dummy,0x00, rx_len);


    rt.user = &cs_nums[spidevice];
    rt.length = rx_len * 8;
    rt.tx_buffer = tx_dummy;
    rt.rxlength = rx_len * 8;
    rt.rx_buffer = rx_data;
    //ret = spi_device_polling_transmit(spi_handles[spidevice], &rt);
    ret = spi_device_transmit(spi_handles[spidevice], &rt);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to rev command array: %s", esp_err_to_name(ret));
    }

    printf("returned rx_data: ");
    for(int i=0;i<rx_len;i++){
        printf("0x%02X ", rx_data[i]);
    }
    printf("\n");

    return ret;
}


