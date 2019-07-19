# Small black (with yellow capacitor) for Raspberry Pi RTC Real Time Clock Module.
This board  is designed for fitting on a header of the 5V Raspberry PI (https://pinout.xyz/pinout/i2c) but it also works fine on 3.3V microcontrollers.

Contains 1x chip Maxim Integrated "DS3231 I2C-Integrated RTC/TCXO/Crystal" (the biggest chip on the PCB) in package SOIC16.

I2C address: 0x68.

Differences with the ZS042 board:

- Much smaller board.
- Less power consumption.



## Example ESP-IDF project
esp32_ds3231_clock_using_lib



## Shop Product.
DS3231 for Raspberry Pi DS3231 RTC Real Time Clock Module



### Board PIN layout
```
1 : VCC 3.3V
2 : I2C SDA
3 : I2C CLK
4 : Not Connected
5 : GND
```

See also the images.



### Wiring instructions for using the I2C protocol
- Connect RTC board pin VCC to the MCU pin VCC 3V.
- Connect RTC board pin SDA to a MCU GPIO#17 (Huzzah32 #17 bottomleft-1).
- Connect RTC board pin SCL to a MCU GPIO#21 (Huzzah32 #21 bottomleft).
- Connect RTC board pin GND to the MCU pin GND.



## Sensor I2C
- I2C Device address 0x68.

- Sensor acts as a slave.



## Data Sheet
[Go to the _doc directory for more documents.]

https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS3231.html



## SOP: setup the RTC board as an external 32Khz oscillator for the ESP32

Not possible.



## FAQ
- OK 3.3V - 5V.
- The board contains a CR927 Lithium battery (not rechargeable); it is tack-welded so it cannot be removed/replaced easily. A CR1032 battery also fits.
- Accuracy: 1 second.
- The contents of the time and calendar registers are in *the binary-coded decimal (BCD) format*.
- The board does not contain pull-up resistors when using the I2C protocol. The ESP32's internal pullups are activated in the mjd_ds3231 component.
- The DS3231 can be run in either 12-hour or 24-hour mode. Bit 6 of the hours register is defined as the 12- or 24-hour mode select bit.
     When high, the 12-hour mode is selected, and bit 5 is the AM/PM bit with logic-high being PM.
     ***When low,  the 24-hour mode is selected***, and bit 5 is the 20-hour bit (20–23 hours).

- The IC contains **a digital temperature sensor**. Accuracy: +-3 degrees Celsius. The temperature reading (Celsius) of the on-board temperature sensor is exposed via the I2C protocol. This is handy for having for example a temperature reading of the PCB inside a project box.



## Known Issues
* The 32K output pin of the DS3231 chip is not exposed on this breakout board.

