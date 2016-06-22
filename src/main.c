/*
 * Plantilla
*/

#include <cr_section_macros.h>
#include <stdbool.h>

#include "api_gpio.h"

#define TICKBASE_HZ      (1000)

// Target 10Hz
#define LED_MIN_PERIOD   (100)
// Period decreased in LED_PERIOD_DELTA ticks every time
#define LED_PERIOD_DELTA (100)

struct seconds_clock {
  unsigned int seconds;
  unsigned int ticks;
};

struct led_cycle_state {
  unsigned int ticks;
  bool on;
  unsigned int ticks_per_period;
};

struct button {
  bool pressed;
};

int systick = 0;

void SysTick_Handler(void)
{
	systick = 1;
}

void gpioButtonGetState(struct button * button_state, gpio_port_t const * gpio)
{
  button_state->pressed = !GPIO_read(gpio);
}

void ledCycleUpdate(struct led_cycle_state * state)
{
  if(state->ticks >= (state->ticks_per_period / 2)) {
    state->ticks  = 0;
    state->on = !state->on;
  }
  else {
    state->ticks++;
  }
}

void setHWLedState(struct led_cycle_state * state, gpio_port_t const * gpio)
{
  if(state->on) {
    GPIO_write(gpio, 1);
  }
  else {
    GPIO_write(gpio, 0);
  }
}

void secondsClockReset(struct seconds_clock * clock)
{
  clock->seconds = 0;
  clock->ticks   = 0;
}

void secondsClockTickUpdate(struct seconds_clock * clock)
{
  if(clock->ticks >= TICKBASE_HZ) {
    clock->seconds++;
    clock->ticks = 0;
  }
  else {
    clock->ticks++;
  }
}

int main(void) {
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock/TICKBASE_HZ);

  gpio_port_t gpio_led;
  gpio_port_t gpio_button;

  struct seconds_clock clock       = {.seconds = 0, .ticks = 0};
  struct led_cycle_state led_state = {.ticks = 0, .on = FALSE, .ticks_per_period = TICKBASE_HZ};
  struct button button_state = {.pressed = FALSE};

  GPIO_init(&gpio_led, 0, 22, GPIO_DIR_OUTPUT);
  GPIO_init(&gpio_button, 0, 17, GPIO_DIR_INPUT);

  secondsClockReset(&clock);

  while(1) {
    if(systick) {
      systick = 0;

      ledCycleUpdate(&led_state);
      secondsClockTickUpdate(&clock);

      gpioButtonGetState(&button_state, &gpio_button);

      // Every 5s
      if((clock.seconds != 0) && (clock.seconds % 5 == 0)) {
        secondsClockReset(&clock);
        // Increase led toggle frequency
        led_state.ticks_per_period -= LED_PERIOD_DELTA;

        if(led_state.ticks_per_period < LED_MIN_PERIOD) {
          led_state.ticks_per_period = LED_MIN_PERIOD;
        }
      }

      if (button_state.pressed) {
        setHWLedState(&led_state, &gpio_led);
      }
      else {
        GPIO_write(&gpio_led, false);
      }
    }
  }

  return 0;
}
