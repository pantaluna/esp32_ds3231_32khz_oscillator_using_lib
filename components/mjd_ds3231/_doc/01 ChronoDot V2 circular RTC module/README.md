# Adafruit ChronoDot V2.0
This board is typically used as an RTC Real Time Clock. It can also be used as an accurate RTC external 32Khz oscillator for the ESP32.

Contains 1x chip Maxim Integrated "**DS3231SN#** I2C-Integrated RTC/TCXO/Crystal" (the biggest chip on the PCB) in package SOIC16.

Differences with the ZS042 board:

- Much smaller board.
- Less power consumption.
- Does NOT contain an EEPROM 24C342 memory chip from Atmel. Which I do not need.
- Does not contain a battery charger circuit. Which I do not need.



## Example ESP-IDF project

`my_ds3231_clock_using_lib` This project demonstrates the basics of using the MJD component "mjd_ds3231" for the DS3231-based RTC Real Time Clock boards.

`esp32_ds3231_32khz_oscillator_using_lib` This project demonstrates the hardware and software setup how to use a DS3231 as an external 32KHz external oscillator for the ESP32 (and get accurate timings in deep sleep).



## Shop Product.
1pcs RTC real-time clock module DS3231SN ChronoDot V2.0 I2C for Arduino Memory DS3231 module

https://www.adafruit.com/product/255



### Board PIN layout
Pins @ left side:

```
1 BAT (not used)
2 32K (only used when the board functions as an external 32KHz crystal oscillator for the ESP32)
3 SQW (not used)
4 RST (not used)
```

Pins @ right side:

```
1 GND
2 VCC 3.3V
3 I2C SCL
4 I2C SDA
```

See also the images.



### Wiring instructions for using the I2C protocol
- Connect RTC board pin VCC to the MCU pin VCC 3V.
- Connect RTC board pin GND to the MCU pin GND.
- Connect RTC board pin SCL to a MCU GPIO#21 (Huzzah32 #21 bottomleft).
- Connect RTC board pin SDA to a MCU GPIO#17 (Huzzah32 #17 bottomleft-1).

https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout



## Sensor I2C
- I2C Device address 0x68 = IC RTC Maxim Integrated DS3231.
- Sensor acts as a slave.



## Data Sheet
[Go to the _doc directory for more documents and images.]

http://docs.macetech.com/doku.php/chronodot_v2.0

https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html



## Setup the Maxim DS3231 RTC as an external oscillator 32Khz for the ESP32

Checkout the project `esp32_ds3231_32khz_oscillator_using_lib`.



## FAQ

- OK 3.3V - 5V.
- The battery holder is designed for a CR1220 3V lithium coin cell.
- RTC Accuracy: 1 second.
- The contents of the time and calendar registers are in *the binary-coded decimal (BCD) format*.
- The board does not contain pull-up resistors when using the I2C protocol. The ESP32's internal pullups are activated in the mjd_ds3231 component.
- The DS3231 can be run in either 12-hour or 24-hour mode. Bit 6 of the hours register is defined as the 12- or 24-hour mode select bit.
     When high, the 12-hour mode is selected, and bit 5 is the AM/PM bit with logic-high being PM.
     ***When low,  the 24-hour mode is selected***, and bit 5 is the 20-hour bit (20–23 hours).

- The IC contains **a digital temperature sensor**. Accuracy: +-3 degrees Celsius. The temperature reading (Celsius) of the on-board temperature sensor is exposed via the I2C protocol. This is handy for having for example a temperature reading of the PCB inside a project box.



## Known Issues
- The board does not fit on a small breadboard if you want to use the pin headers on both the left side (power + I2C pins) and the right side (32K pin). 

