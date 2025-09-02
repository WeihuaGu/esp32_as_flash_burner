#include "main.h"
#include "bus_spi.h"
#include "flash_base.h"
void app_main(void)
{
	bus_spi_init();
	flash_test();

}
