# catFeeder
This project is for an Arduino based automatic cat feeder!

## Hardware
* Arduino uno R3
* Real Time Clock Module (RTC)
* Big Easy Stepper Driver
* NEMA 17 Stepper Motor
* Push Button
* 1 small LED
* 3 state rotary switch

## Functionality
The hardware and user interface of this prototype design are meant to be as simple as possible. As such, the feedings are programmed to be 12 hours apart and those times are set based on time of the last manual feeding. A manual feeding is activated by a pushing the push button. The RTC keeps more exact time than the on-board Arduino clock is capable of for extended periods of time (hours/days). The RTC also keeps time with a small battery should the Arduino lose power. The LED is set to blink if there is a problem with the RTC. The rotary switch provides 3 feed settings: low, med, high. These settings were adjusted experimentally based on the motor/motor-driver combination and the desired output. I'll add some pictures and better explanation in here one day, and include any of the 3-D printed parts I end up using.

This project was meant to be a fun gift for my sister and parents. If anyone finds it useful, feel free to fork and customize it!
