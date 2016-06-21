/*
 * Plantilla
*/

#include "chip.h"
#include <cr_section_macros.h>

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

void ledOn()
{
  Chip_GPIO_SetPinState(LPC_GPIO, 0, 22, true);
}

void ledOff()
{
  Chip_GPIO_SetPinState(LPC_GPIO, 0, 22, false);
}

void buttonStateRead(struct button * button_state)
{
  button_state->pressed = !Chip_GPIO_GetPinState(LPC_GPIO, 0, 17);
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

void setHWLedState(struct led_cycle_state * state)
{
  if(state->on) {
    ledOn();
  }
  else {
    ledOff();
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

  Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 22);

  struct seconds_clock clock       = {.seconds = 0, .ticks = 0};
  struct led_cycle_state led_state = {.ticks = 0, .on = FALSE, .ticks_per_period = TICKBASE_HZ};
  struct button button_state = {.pressed = FALSE};

  secondsClockReset(&clock);

  while(1) {
    if(systick) {
      systick = 0;

      ledCycleUpdate(&led_state);
      secondsClockTickUpdate(&clock);
      buttonStateRead(&button_state);

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
        setHWLedState(&led_state);
      }
      else {
        ledOff();
      }
    }
  }

  return 0;
}
