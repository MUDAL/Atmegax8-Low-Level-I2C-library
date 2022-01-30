# Atmegax8-Low-Level-I2C-library  

This I2C device driver links a high level application with the I2C (or TWI) peripheral  
of the Atmega48/88/168/328 microcontroller(s). The library is written in C and can be used  
by any development board containing the aforementioned microcontroller(s) e.g. Arduino Uno,  
Arduino Nano etc. The project was developed in **Visual Studio Code** with the **PlatformIO**  
extension.  

## PlatformIO settings  
The **platformio.ini** file contains the necessary project settings for a particular platform.  
For example, the following settings apply to a dev board with the Atmega328 chip.  

[env:ATmega328P]  
platform = atmelavr  
board = ATmega328P  
upload_speed = 57600 

The **upload_speed** value can be adjusted based on the dev board being used. 57600 works for the  
Arduino Nano while 115200 works for the Uno. Both boards use the Atmega328 microcontroller. With the  
configuration above, the microcontroller on the dev board can be programmed at the bare metal level  
without depending on the **Arduino framework**.
