#include "main.h"
#include "bus_spi.h"
#include "fram_base.h"
#include "flash_base.h"
void app_main(void)
{
	bus_spi_init();
	fram_test();
	flash_test();

}
