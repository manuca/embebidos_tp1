#include <stdint.h>
#include <stdbool.h>
#include "chip.h"

typedef enum {GPIO_DIR_INPUT, GPIO_DIR_OUTPUT} gpio_dir;

typedef struct {
  uint8_t port;
  uint8_t pin;
  gpio_dir dir;
} gpio_port_t;

void GPIO_init(gpio_port_t * port, uint8_t port_no, uint8_t pin_no, gpio_dir dir);
bool GPIO_read(const gpio_port_t * gpio);
void GPIO_write(const gpio_port_t * gpio, bool value);
bool GPIO_getState(const gpio_port_t * gpio);
