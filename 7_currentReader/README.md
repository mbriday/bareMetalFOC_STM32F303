# read the current

This simple application reads the current for the 3 shunt mode:

In the main.cpp file:

- The timer TIM1 is configured to generate an interrupt each 1s
- When there is an interrupt occurs, the ADC is started (in hardware) to get the current on each shunt.
- The interrupt handler print the results of the ADC (both RAW value from ADC and value in mA).

In the loop of the application, the phases U,V and W can be set (as an IO). The channel is updated each time the button is pushed (blue button of the board), and the channel get the maximum of current as long as the button remains pushed.

The currentReader global object configures the 3 ADCs (1 for each shunt), using an hardware trigger on TIM1, with an injected conversion. This object will be reused in the whole FOC application.

