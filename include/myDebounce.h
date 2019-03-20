#ifndef __myDebounce_h
#define __myDebounce_h

#include "Types.h"


#define MASK_BUTTON_PRESSED            0xF00F  // FxxF x->is don't care
#define MASK_BUTTON_PRESSED_CONFIRM    0x000F  // 0xxF
#define MASK_BUTTON_RELEASED           0xF00F  // FxxF x->is don't care
#define MASK_BUTTON_RELEASED_CONFIRM   0xF000  // Fxx0
#define MASK_BUTTON_DOWN               0xFFFF  // all bits must be one (maybe bad?)
#define MASK_BUTTON_UP                 0x0000  // all bits must be zero (maybe bad?)

#define LONG_PRESS_THRESHOLD 150  // the number that separates the short press from long press
#define MAX_COUNTER_NUMBER   255  // max byte number...

// uncomment to test button counter (to determine whether it is long or shor press)
// some tweaking needs to be done to match your needs, also change LONG_PRESS_THRESHOLD
//#define PRINTF_COUNTER_VALUE

typedef enum
{
  eNOT_PRESSED = 0,
  eSHORT_PRESS,
  eLONG_PRESS
} EWIHCH_PRESS;

#ifdef __cplusplus
extern "C" {
#endif

void IRAM_ATTR debounce_update_button(register WORD *wButton_history, register BOOL bCurrentState);
BOOL debounce_is_button_pressed(register WORD *wButton_history);
BOOL debounce_is_button_released(register WORD *wButton_history);
BOOL debounce_is_button_down(register WORD *wButton_history);
BOOL debounce_is_button_up(register WORD *wButton_history);
EWIHCH_PRESS debounce_long_short_press(register WORD *wButton_history, register BYTE *byCounter);

#ifdef __cplusplus
}
#endif

#endif  // __myDebounce_h
