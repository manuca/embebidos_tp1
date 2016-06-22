#include "api_gpio.h"

void GPIO_init(gpio_port_t * gpio, uint8_t port, uint8_t pin, gpio_dir dir)
{
  gpio->port = port;
  gpio->pin  = pin;
  gpio->dir  = (dir == GPIO_DIR_INPUT) ? GPIO_DIR_INPUT : GPIO_DIR_OUTPUT;

  Chip_IOCON_PinMux(LPC_IOCON, gpio->port, gpio->pin, IOCON_MODE_INACT, IOCON_FUNC0);
  Chip_GPIO_SetPinDIR(LPC_GPIO, gpio->port, gpio->pin,
      (gpio->dir == GPIO_DIR_OUTPUT));
}

bool GPIO_read(const gpio_port_t * gpio)
{
  return Chip_GPIO_GetPinState(LPC_GPIO, gpio->port, gpio->pin);
}

void GPIO_write(const gpio_port_t * gpio, bool value)
{
  if( gpio->dir == GPIO_DIR_OUTPUT) {
		Chip_GPIO_SetPinState(LPC_GPIO, gpio->port, gpio->pin, value);
  }
}
