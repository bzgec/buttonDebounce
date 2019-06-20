/* 
 * Code copied/modified from:
 *   - https://hackaday.com/2015/12/09/embed-with-elliot-debounce-your-noisy-buttons-part-i/
 *   - https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/
 *   - https://pubweb.eng.utah.edu/~cs5780/debouncing.pdf
 *   - https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers
 * 
 */

#include "myDebounce.h"
#include "stdio.h"

void IRAM_ATTR debounce_update_button(register WORD *pwButton_history, register BOOL bCurrentState)
{
  *pwButton_history = (*pwButton_history << 1) | bCurrentState;
  //*pwButton_history = (*pwButton_history << 1);
  //*pwButton_history |= bCurrentState;
}

BOOL debounce_is_button_pressed(register WORD *pwButton_history)
{
  BOOL bPressed = 0;    
  if ((*pwButton_history & MASK_BUTTON_PRESSED) == MASK_BUTTON_PRESSED_CONFIRM)
  { 
    bPressed = 1;
    *pwButton_history = MASK_BUTTON_DOWN;
  }
  return bPressed;
}

BOOL debounce_is_button_released(register WORD *pwButton_history)
{
  BOOL bPressed = 0;    
  if ((*pwButton_history & MASK_BUTTON_RELEASED) == MASK_BUTTON_RELEASED_CONFIRM)
  { 
    bPressed = 1;
    *pwButton_history = MASK_BUTTON_UP;
  }
  return bPressed;
}

BOOL debounce_is_button_down(register WORD *pwButton_history)
{
  return (*pwButton_history == MASK_BUTTON_DOWN);
}

BOOL debounce_is_button_up(register WORD *pwButton_history)
{
  return (*pwButton_history == MASK_BUTTON_UP);
}

EWIHCH_PRESS debounce_whichPress(register WORD *pwButton_history, register BYTE *byCounter)
{
  EWIHCH_PRESS ePress = eNOT_PRESSED;

  // debounce_is_button_down
  if(*pwButton_history == MASK_BUTTON_DOWN)
  {
    if(*byCounter < MAX_COUNTER_NUMBER)
    {
      (*byCounter)++;
    }
    #ifdef PRINT_COUNTER_VALUE
    printf("*byCounter: %d\n", *byCounter);
    #endif  // PRINT_COUNTER_VALUE
    ePress = eDOWN;
  }
  // debounce_is_button_up && ...
  else if(*pwButton_history == MASK_BUTTON_UP && *byCounter != 0)
  {
    if(*byCounter > LONG_PRESS_THRESHOLD)
    {
      ePress = eLONG_PRESS;
    }
    else
    {
      ePress = eSHORT_PRESS;
    }
    *byCounter = 0;
  }
  else if(debounce_is_button_released(pwButton_history))
  {
    ePress = eRELEASED;
  }
  else if(debounce_is_button_pressed(pwButton_history))
  {
    ePress = ePRESSED;
  }

  return ePress;  
}


/*inline void update_button(register BYTE *byButtonCounter, register BOOL bCurrentState)
{
  if(bCurrentState)
  {
    (*byButtonCounter)++;
  }
}*/
