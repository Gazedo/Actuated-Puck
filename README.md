# Actuated-Puck

##How To:
Connect and Command
 1. In order to connect to the puck you must have a bluetooth serial console.
 2. After installing a serial console such as putty, connect to the puck via bluetooth.
   * It will be called BT-42
 3. From there the command set is a set of 4 digits. The first digit is a switch variable and the last 3 are the command digits. The possible combinations are:
   * s000
Sets a new degree for the Set point for the PID algorithm.
In order to set for -10 degrees, command s350.
10 degrees would be s010.
   * p000
this would set the p variable to 0, in order to set the p variable to 5, command p500 as the number will be divided by 100.
  * i000
Same as the p variable but for the integral constant.
  * d000
Same as the p variable but for the derivative constant.
  * where the 000 is any possible combination of 3 digits.
 4. There will never be any feedback from the puck as the transmit line of the UART port is used to feed information to the sd card.


##Planned upgrades
Teensy 3.1
  * Would eliminate the 5V regulator and the 3V3 regulator.
  * Enable the chip to output the logged information by bluetooth rather than having to use the sd card
  * Operates at 3V3 but is tolerant to 5V
    * Would eliminate the need for the level shifter
9 DoF IMU
  * Doesn't seem to be needed under testing however it is an option if excessive yaw drift is found which effects the readings.
##Driver Chip
  * This driver chip (L6234) needs a very specialized commutation sequence in order to make the motor move and that code has been the major obstacle but there don't seem to be any better options on the market. An ideal chip would take the hall effect signals as input, a pwm signal from the MCU as input, and output the correct drive current and voltage.
