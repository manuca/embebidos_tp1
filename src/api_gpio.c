#include "api_gpio.h"

void GPIO_init(gpio_port_t * gpio, uint8_t port, uint8_t pin, gpio_dir dir)
{
  gpio->port = port;
  gpio->pin  = port;
  gpio->dir  = (dir == gpio_dir_input) ? gpio_dir_input : gpio_dir_output;

  Chip_IOCON_PinMux(LPC_IOCON, gpio->port, gpio->pin, IOCON_MODE_INACT, IOCON_FUNC0);
  Chip_GPIO_SetPinDIR(LPC_GPIO, gpio->port, gpio->pin,
      (gpio->dir == gpio_dir_output));
}

bool GPIO_read(const gpio_port_t * gpio)
{
  return Chip_GPIO_ReadPortBit(LPC_GPIO, gpio->port, gpio->pin);
}

void GPIO_write(const gpio_port_t * gpio, bool value)
{
  if( gpio->dir == gpio_dir_output) {
		Chip_GPIO_WritePortBit(LPC_GPIO, gpio->port, gpio->pin, value);
  }
}
