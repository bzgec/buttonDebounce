#ifndef __main_h
#define __main_h

#include "Types.h"


#define LED_BUILTIN       GPIO_NUM_2
#define PIN_BUTTON_LEFT   GPIO_NUM_17
#define PIN_BUTTON_RIGHT  GPIO_NUM_18
#define PIN_BUTTON_SET    GPIO_NUM_19
#define PIN_TOUCHPAD      TOUCH_PAD_NUM0  // Touch pad channel 0 is GPIO4

// enable what you use
#define ENABLE_BUTTONS
#define ENABLE_TOUCH_SENSOR

// intervals at which task is run (in milli seconds)
#define TASK_INTERVAL_BUTTONS 10
#define TASK_INTERVAL_TOUCHPAD 10

// Each task is assigned a priority from 0 to ( configMAX_PRIORITIES - 1 )
// Low priority numbers denote low priority tasks. The idle task has priority zero (tskIDLE_PRIORITY). 
#define TASK_PRIORITY_BUTTONS  5
#define TASK_PRIORITY_TOUCHPAD  5

#define TASK_STACK_SIZE_BUTTONS configMINIMAL_STACK_SIZE*5
#define TASK_STACK_SIZE_TOUCHPAD configMINIMAL_STACK_SIZE


void app_main();
void init_gpio_LEDBUILTIN();
void init_gpio_touchpad();
void init_gpio_buttons();
void init_timer_button_debounce();
void task_read_buttons(void* pvParameter);
void task_read_touchpad(void* pvParameter);

#define GPIO_REG_ADDR 0x3FF44000
#define GPIO_REG_IN_ADDR_OFFSET 0x003c
#define GPIO_REG_IN_ADDR (GPIO_REG_ADDR + GPIO_REG_IN_ADDR_OFFSET)
#define GPIO_REG_IN (*((DWORD*)GPIO_REG_IN_ADDR))

#define READ_BUTTON(gpio_numb)  ((GPIO_REG_IN & (1 << (gpio_numb))) != 0)

// TIMER FOR BUTTON DEBOUNCE
#define TIMER_DIVIDER_DEBOUNCE            80  //  Hardware timer clock divider
#define TIMER_SCALE_DEBOUNCE              (TIMER_BASE_CLK / TIMER_DIVIDER_DEBOUNCE / 1000)  // convert counter value to seconds
#define TIMER_INTERVAL_DEBOUNCE_MILLI_SEC 5 


#endif  // __main_h

