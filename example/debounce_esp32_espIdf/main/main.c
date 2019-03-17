
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include "soc/rtc.h"
#include "esp_clk.h"

#include "driver/timer.h"
#include "soc/timer_group_struct.h"
//#include "esp_types.h"

#define TOUCH_PAD_NO_CHANGE   (-1)
#define TOUCH_THRESH_NO_USE   (0)
#define TOUCH_FILTER_MODE_EN  (1)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

#include "main.h"
#include <string.h>
#include "myDebounce.h"

WORD g_wButtonHistory_left; 
WORD g_wButtonHistory_right; 
WORD g_wButtonHistory_set;
BYTE g_byButtonCounter_left;
BYTE g_byButtonCounter_right;
BYTE g_byButtonCounter_set;

BOOL g_bTouchSensor_flag = FALSE;


#ifdef ENABLE_BUTTONS
void IRAM_ATTR timer_group0_t1_isr_buttons(void* parameter)
{
	debounce_update_button(&g_wButtonHistory_left,  !READ_BUTTON(PIN_BUTTON_LEFT));
	debounce_update_button(&g_wButtonHistory_right, !READ_BUTTON(PIN_BUTTON_RIGHT));
	debounce_update_button(&g_wButtonHistory_set,   !READ_BUTTON(PIN_BUTTON_SET));

	// When servicing an interrupt within an ISR, the interrupt need to explicitly cleared.
  TIMERG0.int_clr_timers.t1 = 1;

  // After the alarm has been triggered
  // we need enable it again, so it is triggered the next time
  TIMERG0.hw_timer[TIMER_1].config.alarm_en = TIMER_ALARM_EN;
}
#endif  // ENABLE_BUTTONS

void app_main()
{
	init_gpio_LEDBUILTIN();  // turn OFF the LEDBUILTIN

#ifdef ENABLE_TOUCH_SENSOR
	init_gpio_touchpad();
	xTaskCreate(&task_read_touchpad, "Touch_pad_task", TASK_STACK_SIZE_TOUCHPAD, NULL, TASK_PRIORITY_TOUCHPAD, NULL);
#endif  // ENABLE_TOUCH_SENSOR

#ifdef ENABLE_BUTTONS
	init_gpio_buttons();
	init_timer_button_debounce();
	xTaskCreate(&task_read_buttons, "buttons_task", TASK_STACK_SIZE_BUTTONS, NULL, TASK_PRIORITY_BUTTONS, NULL);
#endif  // ENABLE_BUTTONS

  /*while(1)
  {
  }*/
}

#ifdef ENABLE_BUTTONS
void task_read_buttons(void *pvParameter)
{
	BYTE nn = 0;
  while(1) 
  {
		if (debounce_is_button_released(&g_wButtonHistory_left))
		{
			//printf("Buttons changed: %X\n", byDebouncedButtons);
			printf("L released: %d\n", ++nn);
		}
		if (debounce_is_button_down(&g_wButtonHistory_right))
		{
			//printf("Buttons changed: %X\n", byDebouncedButtons);
			printf("R down: %d\n", ++nn);
		}
		if (debounce_is_button_pressed(&g_wButtonHistory_set))
		{
			//printf("Buttons changed: %X\n", byDebouncedButtons);
			printf("S pressed: %d\n", ++nn);
		}

		EWIHCH_PRESS ePress;
		ePress = debounce_long_short_press(&g_wButtonHistory_set, &g_byButtonCounter_set);
		if(ePress == eLONG_PRESS)
		{
			printf("S long button press\n");
		}
		else if(ePress == eSHORT_PRESS)
		{
			printf("S short button press\n");
		}

    vTaskDelay(TASK_INTERVAL_BUTTONS / portTICK_PERIOD_MS);
  }
}
#endif  // ENABLE_BUTTONS

void init_gpio_LEDBUILTIN()
{
	// turn OFF the led that is built in...
	gpio_pad_select_gpio(LED_BUILTIN);
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);
	gpio_set_level(LED_BUILTIN, 1);
}

#ifdef ENABLE_TOUCH_SENSOR
void init_gpio_touchpad()
{
	// Initialize touch pad peripheral.
  // The default fsm mode is software trigger mode.
	touch_pad_init();
  // Set reference voltage for charging/discharging
  // In this case, the high reference valtage will be 2.7V - 1V = 1.7V
  // The low reference voltage will be 0.5
  // The larger the range, the larger the pulse count value.
  touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
  touch_pad_config(PIN_TOUCHPAD, TOUCH_THRESH_NO_USE);  // Touch pad channel 0 is GPIO4
	//touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);  // To start touch FSM by software trigger
	// if it is uncommented than it does not work with hardware timer...
	//touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);  // To start touch FSM by timer
#if TOUCH_FILTER_MODE_EN
  touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
#endif
}
#endif  // ENABLE_TOUCH_SENSOR

#ifdef ENABLE_BUTTONS
void init_gpio_buttons()
{
	gpio_config_t io_conf;
  //disable interrupt
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  //set as input mode
  io_conf.mode = GPIO_MODE_INPUT;
  //bit mask of the pins that you want to set,e.g.GPIO18/19
  io_conf.pin_bit_mask = (1UL<<PIN_BUTTON_LEFT);
  //disable pull-down mode
  io_conf.pull_down_en = 0;
  //enable pull-up mode
  io_conf.pull_up_en = 1;
  //configure GPIO with the given settings
  gpio_config(&io_conf);

  io_conf.pin_bit_mask = (1UL<<PIN_BUTTON_RIGHT);
  gpio_config(&io_conf);
  io_conf.pin_bit_mask = (1UL<<PIN_BUTTON_SET);
  gpio_config(&io_conf);
}
#endif  // ENABLE_BUTTONS

#ifdef ENABLE_BUTTONS
void init_timer_button_debounce()
{
	esp_err_t espRc;
	timer_config_t timerConfig;
	timerConfig.divider = TIMER_DIVIDER_DEBOUNCE;
  timerConfig.counter_dir = TIMER_COUNT_UP;
  timerConfig.counter_en = TIMER_PAUSE;
  timerConfig.alarm_en = TIMER_ALARM_EN;
  timerConfig.intr_type = TIMER_INTR_LEVEL;
	timerConfig.auto_reload = TRUE;

	espRc = timer_init(TIMER_GROUP_0, TIMER_1, &timerConfig);
	if (espRc == ESP_OK) 
	{
		ESP_LOGI("TIMER", "Debounce timer configured successfully");
	} else 
	{
		ESP_LOGE("TIMER", "Debounce timer configuration failed. code: 0x%.2X", espRc);
	}


	// Timer's counter will initially start from value below.
  // Also, if auto_reload is set, this value will be automatically reload on alarm
  timer_set_counter_value(TIMER_GROUP_0, TIMER_1, 0x00000000ULL);

  // Configure the alarm value and the interrupt on alarm.
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_1, TIMER_INTERVAL_DEBOUNCE_MILLI_SEC * TIMER_SCALE_DEBOUNCE);
  //timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000*1000);
  //timer_enable_intr(TIMER_GROUP_0, TIMER_0);  // enable interrupts for a specific timer 
  timer_isr_register(TIMER_GROUP_0, TIMER_1, timer_group0_t1_isr_buttons, 
      NULL, ESP_INTR_FLAG_IRAM, NULL);

	timer_start(TIMER_GROUP_0, TIMER_1);
}
#endif  // ENABLE_BUTTONS

#ifdef ENABLE_TOUCH_SENSOR
void task_read_touchpad(void* pvParameter)
{
	while(1)
	{
		//uint16_t touch_value;
	  uint16_t touch_filter_value;

		//touch_pad_sw_start();

	#if TOUCH_FILTER_MODE_EN
	  // If open the filter mode, please use this API to get the touch pad count.
	  //touch_pad_read_raw_data(TOUCH_PAD_NUM0, &touch_value);
	  touch_pad_read_filtered(TOUCH_PAD_NUM0, &touch_filter_value);
	  //printf("T%d:[%4d,%4d] \n", TOUCH_PAD_NUM0, touch_value, touch_filter_value);
	#else
	  touch_pad_read(i, &touch_value);
	  //printf("T%d:[%4d] \n", i, touch_value);
	#endif

		if(touch_filter_value <= 500)
		{
			g_bTouchSensor_flag = TRUE;
		}
		else
		{
			g_bTouchSensor_flag = FALSE;
		}

		vTaskDelay(TASK_INTERVAL_TOUCHPAD / portTICK_PERIOD_MS);
	}
}
#endif  // ENABLE_TOUCH_SENSOR