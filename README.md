# Debouncing library

## Detecting long button press (`eLONG_PRESS`)
Lets say that for long press button needs to be held down for 2s (2000ms):
  * `debounce_update_button()` called every 5ms (ISR)
  * `debounce_whichPress()` called every 20ms (task)

1. Simple metod (good enough):
    * `2000ms / 20ms = 100`
    * This means that `LONG_PRESS_THRESHOLD` should be set to 100
2. Correct metod:
    * `16*5ms = 80ms` (this long button needs to be _1_ to increment `*byCounter`,
                     meaning that 80ms is needed to detect `eSHORT_PRESS` or `eLONG_PRESS` button press)
    * `(2000ms - 80ms) / 20ms = 96`
    * This means that `LONG_PRESS_THRESHOLD` should be set to 96

#### Code copied/modified from:
  * https://hackaday.com/2015/12/09/embed-with-elliot-debounce-your-noisy-buttons-part-i/
  * https://hackaday.com/2015/12/10/embed-with-elliot-debounce-your-noisy-buttons-part-ii/
  * https://pubweb.eng.utah.edu/~cs5780/debouncing.pdf
  * https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers

### If you have any suggestions please let me know.
