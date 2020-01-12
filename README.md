# bareMetalFOC_STM32F303
Bare Metal Brushless position control (using Field Oriented Control) on STM32F303

The goal of this repository is to understand how to control a brushless motor using the Field Oriented Control approach. The hardware part is based on a ST Nucleo Motor control pack with:

 * a Nucleo board [STM32 F303RE](https://my.st.com/content/my_st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-mpu-eval-tools/stm32-mcu-mpu-eval-tools/stm32-nucleo-boards/nucleo-f303re.html)
 * a motor driver shield [IHM16M1](https://my.st.com/content/my_st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-move-actuate-hw/x-nucleo-ihm16m1.html) based on the low power interface SPIN830
 * a brushless Gimbal motor (GBM2804H-100T)

For a position control algorithm, a [AS5147P](https://ams.com/as5147padapterboard) position sensor is added.

The motor will not use the ST Motor Control framework, but a bare-metal approach (to learn how it works).
