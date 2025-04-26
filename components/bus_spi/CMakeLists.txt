idf_component_register(SRCS "bus_spi.c" "fram_base.c" "flash_base.c"
                       INCLUDE_DIRS "include"
                       REQUIRES driver freertos)

