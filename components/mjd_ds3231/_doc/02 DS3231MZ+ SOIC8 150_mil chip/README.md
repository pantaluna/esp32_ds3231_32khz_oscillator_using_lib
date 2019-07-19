# DS3231MZ+ mini breakout board
This is a manually soldered breakout board. It can also be made using an SOIC8 socket adapter (see images).

Contains 1x chip Maxim Integrated "**DS3231SMZ+** RTC" in package SOIC8.

It only needs one extra 100nF ceramic capacitor between the pins VCC and GND.

The breakout board does not contain a socket for a battery but if you use the board as a 32KHz external crystal oscillator for the ESP32 then the battery is not needed.



## Example ESP-IDF project

`my_ds3231_clock_using_lib` This project demonstrates the basics of using the MJD component "mjd_ds3231" for the DS3231-based RTC Real Time Clock boards.

`esp32_ds3231_32khz_oscillator_using_lib` This project demonstrates the hardware and software setup how to use a DS3231 as an external 32KHz external oscillator for the ESP32 (and get accurate timings in deep sleep).



## Shop Product.
DS3231MZ+ IC

100nF XR7 ceramic capacitor



### Board PIN layout

Go to the images.

[Wiring - DS3231MZ breakout-01.png](Wiring - DS3231MZ breakout-01.png)



### Wiring instructions for using the I2C protocol
- Connect RTC board pin VCC to the MCU pin VCC 3V.
- Connect RTC board pin GND to the MCU pin GND.
- Connect RTC board pin SCL to a MCU GPIO#21 (Huzzah32 #21 bottomleft).
- Connect RTC board pin SDA to a MCU GPIO#17 (Huzzah32 #17 bottomleft-1).



## Sensor I2C
- I2C Device address 0x68 = IC RTC Maxim Integrated DS3231.
- Sensor acts as a slave.



## Data Sheet
[Go to the _doc directory for more documents and images.]

https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html



## Setup the Maxim DS3231 RTC as an external oscillator 32Khz for the ESP32

Checkout the project `esp32_ds3231_32khz_oscillator_using_lib`.



## FAQ

- OK 3.3V - 5V.
- RTC Accuracy: 1 second.
- The contents of the time and calendar registers are in *the binary-coded decimal (BCD) format*.
- The board does not contain pull-up resistors when using the I2C protocol. The ESP32's internal pullups are activated in the mjd_ds3231 component.
- The DS3231 can be run in either 12-hour or 24-hour mode. Bit 6 of the hours register is defined as the 12- or 24-hour mode select bit.
     When high, the 12-hour mode is selected, and bit 5 is the AM/PM bit with logic-high being PM.
     ***When low,  the 24-hour mode is selected***, and bit 5 is the 20-hour bit (20–23 hours).

- The IC contains **a digital temperature sensor**. Accuracy: +-3 degrees Celsius. The temperature reading (Celsius) of the on-board temperature sensor is exposed via the I2C protocol. This is handy for having for example a temperature reading of the PCB inside a project box.



## Known Issues
