# Actuated-Puck

##How To:
Connect and Command
 1. In order to connect to the puck you must have a bluetooth serial console.
 2. After installing a serial console such as putty, connect to the puck via bluetooth.
 a) It will be called BT-42
 3. From there the command set is a set of 4 digits. The first digit is a switch variable and the last 3 are the command digits. The possible combinations are:
 
a) s000
Sets a new degree for the Set point for the PID algorithm.
In order to set for -10 degrees, command s350.
10 degrees would be s010.
 
b) p000
this would set the p variable to 0, in order to set the p variable to 5, command p500 as the number will be divided by 100.
 
c) i000
Same as the p variable but for the integral constant.
 
d) D000
Same as the p variable but for the derivative constant.
 
e) where the 000 is any possible combination of 3 digits.
 4. There will never be any feedback from the puck as the transmit line of the UART port is used to feed information to the sd card.
