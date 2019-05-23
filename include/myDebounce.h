#ifndef __myDebounce_h
#define __myDebounce_h

#include "Types.h"
#include "esp_attr.h"  // IRAM_ATTR for debounce_update_button() function

#define MASK_BUTTON_PRESSED            0xF00F  // FxxF x->is don't care
#define MASK_BUTTON_PRESSED_CONFIRM    0x000F  // 0xxF
#define MASK_BUTTON_RELEASED           0xF00F  // FxxF x->is don't care
#define MASK_BUTTON_RELEASED_CONFIRM   0xF000  // Fxx0
#define MASK_BUTTON_DOWN               0xFFFF  // all bits must be one (maybe bad?)
#define MASK_BUTTON_UP                 0x0000  // all bits must be zero (maybe bad?)

/*
 * DETECTING LONG PRESS ('eLONG_PRESS')
 *   Lets say that for long press button needs to be held down for 2s (2000ms):
 *     - 'debounce_update_button()' called every 5ms (ISR)
 *     - 'debounce_whichPress()' called every 20ms (task)
 *     - Simple metod (good enough):
 *         2000ms / 20ms = 100 
 *         This means that 'LONG_PRESS_THRESHOLD' should be set to 100
 *     - Correct metod:
 *         16*5ms = 80ms (this long button needs to be '1' to increment '*byCounter', 
 *                        meaning that 80ms is needed to detect 'eSHORT_PRESS' or 'eLONG_PRESS' button press)
 *         (2000ms - 80ms) / 20ms = 96
 *         This means that 'LONG_PRESS_THRESHOLD' should be set to 96
 */
#define LONG_PRESS_THRESHOLD 192  // the number that separates the short press from long press
#define MAX_COUNTER_NUMBER   255  // max byte number...

// uncomment to test button counter (to determine whether it is long or shor press)
// some tweaking needs to be done to match your needs, also change LONG_PRESS_THRESHOLD
//#define PRINTF_COUNTER_VALUE

typedef enum
{
  eNOT_PRESSED = 0,
  eSHORT_PRESS,
  eLONG_PRESS,
  ePRESSED,
  eRELEASED
} EWIHCH_PRESS;

#ifdef __cplusplus
extern "C" {
#endif

void IRAM_ATTR debounce_update_button(register WORD *pwButton_history, register BOOL bCurrentState);
BOOL debounce_is_button_pressed(register WORD *pwButton_history);
BOOL debounce_is_button_released(register WORD *pwButton_history);
BOOL debounce_is_button_down(register WORD *pwButton_history);
BOOL debounce_is_button_up(register WORD *pwButton_history);
EWIHCH_PRESS debounce_whichPress(register WORD *pwButton_history, register BYTE *byCounter);

#ifdef __cplusplus
}
#endif

#endif  // __myDebounce_h
