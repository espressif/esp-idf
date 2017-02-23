# LEDC(LED control) Example

###This example shows:

 * init LEDC module:
 
     a.	You need to set the timer of LEDC first, this decide the frequency and resolution of PWM.
     
     b.	You need to set the LEDC channel you want to use, and bind the channel with one of the timers.
     
 * You can install a default fade function, then you can use fade APIs.
 
 * You can also set a target duty directly without fading.
 
 * This example use GPIO18/19/4/5 as LEDC ouput, and it will change the duty repeatedly.

