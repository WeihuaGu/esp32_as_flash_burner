#include <string.h>
#include "bus_spi.h"
#include "spi_cs.h"
#include "fram_base.h"

// 写使能
esp_err_t send_WREN(){
    esp_err_t ret;
    uint8_t wren_cmd = WREN;
    ret = send_command_array(SPI_DEVICE_FRAM,&wren_cmd, 1);
    return ret;

}
esp_err_t send_WRDI(){
    esp_err_t ret;
    uint8_t wrdi_cmd = WRDI;
    ret = send_command_array(SPI_DEVICE_FRAM,&wrdi_cmd, 1);
    return ret;

}
esp_err_t fram_read_status_register(uint8_t *status_reg) {
    uint8_t cmd = RDSR; // 读取状态寄存器命令
    return only_one_cmd(SPI_DEVICE_FRAM,cmd,status_reg,1);
}
int print_status_register() {
    uint8_t status_reg;
    fram_read_status_register(&status_reg);
    printf("Status Register: 0x%02X\n", status_reg);
    if (status_reg == 0x00 || status_reg == 0xff) {
        return 1;
    }
    return 0;
}
// 写入 FRAM 数据
esp_err_t fram_write(uint16_t addr, uint8_t *data, size_t len) {
    esp_err_t ret;
    ret = send_WREN();
    if (ret != ESP_OK) {
        return ret;
    }

    uint8_t *tx_buffer = malloc(3 + len); // 命令 + 地址 + 数据
    tx_buffer[0] = WRITE;                 // 写命令
    tx_buffer[1] = (addr >> 8) & 0xFF;  // 地址高位
    tx_buffer[2] = addr & 0xFF;          // 地址低位
    memcpy(&tx_buffer[3], data, len);    // 数据
    // 发送命令和数据
    ret = send_command_array(SPI_DEVICE_FRAM,tx_buffer, len + 3);
    free(tx_buffer);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to write data: %s", esp_err_to_name(ret));
    }
    send_WRDI();
    return ret;
}

// 读取 FRAM 数据
esp_err_t fram_read(uint16_t addr, uint8_t *data, size_t len) {
    esp_err_t ret;

    uint8_t tx_buffer[3];
    tx_buffer[0] = READ;                 // 读命令
    tx_buffer[1] = (addr >> 8) & 0xFF;   // 地址高位
    tx_buffer[2] = addr & 0xFF;          // 地址低位
                                         
    ret = send_command_array_withreturn(SPI_DEVICE_FRAM,tx_buffer, sizeof(tx_buffer), data, len);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to read data: %s", esp_err_to_name(ret));
    }
    return ret;
}


void fram_test(){
    print_status_register();
     // 测试地址
    uint16_t addr1 = 0x0010;

    // 写入数据到 FRAM
    uint8_t write_data = 0x24; // 要写入的数据
    fram_write(addr1, &write_data, 1);
    //从 PSRAM 读取数据
    uint8_t read_data;
    fram_read(addr1, &read_data, 1);
}

