# ESP32 MJD DS3231 Real Time Clock component
This is an ESP-IDF component for the ESP32 hardware platform of Espressif for the Maxim DS3231x RTC real-time clock chips.



The Maxim DS3231 chip comes in 2 variations:

* **DS3231/DS3131SN**: Real-time clock with **a temperature compensated crystal oscillator (TCXO)**. Accuracy: +/- 3ppm. Active supply current: 200uA. The 32KHz output signal on pin#1 is also temperature-compensated. Can generate a 1Hz, 1.024kHz, 4.096kHz, 8.192kHz SQW signal on the SQW/INT pin. Parts: **DS3231SN#**.
* **DS3231M**: Real-time clock with **a MEMS resonator**. Accuracy: +/- 5ppm. Active supply current: 200uA. The 32KHz output signal on pin#1 is **not**  temperature-compensated; meaning it will loose accuracy when the ambient temperature changes significantly (this has no effect on the accuracy of the RTC, only the 32K output signal). Can only generate a 1Hz SQW signal on the SQW/INT pin. Parts: **DS3231MZ+, DS3231M+**.

[Maxim App Note 5143 Feature Comparison of the DS323x Real-Time Clocks](https://www.maximintegrated.com/en/app-notes/index.mvp/id/5143)



The breakout boards are based on the Maxim **DS3231SN**. **Some boards also expose the 32K pin** and so can also be used as an accurate external 32KHz crystal oscillator for the ESP32 to improve the time keeping accuracy in deep sleep.



## Example ESP-IDF project(s)
`esp32_ds3231_clock_using_lib` This project demonstrates the basics of using the MJD component "mjd_ds3231" for the DS3231-based RTC Real Time Clock boards.

`esp32_ds3231_32khz_oscillator_using_lib` This project demonstrates the hardware and software setup to use a DS3231 as an external 32KHz external oscillator for the ESP32. This will result in accurate time keeping in periods of deep sleep.



## Supported Products.
1. The blue circular ChronoDot V2 Real Time Clock Module (recommended).
2. A barebone DS3231MZ+ SOIC8 chip on a SOIC8 breakout board (soldered manually) or on a SOIC socket adapter.
3. The blue rectangular large ZS042 DS3231 RTC Real Time Clock Module.
4. The black smaller model (with yellow capacitor) for Raspberry Pi RTC Real Time Clock Module.



## Instructions

**=> Go to the _doc directory and read the specs, instructions for installation and wiring of each  RTC Board.**



## Pinout

Check out the images.

```
DS3231M IC

PIN#  PIN NAME	NOTES
====  ========  =====
1     32KHZ     
2     VCC       
3     INT/SQW   Not Used
4     RST       Not Used
5     SCL      
6     SDA      
7     VBAT     VBAT+
8     GND
```

https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout



## Setup the Maxim DS3231 RTC as a 32Khz external oscillator for the ESP32

Checkout the project `esp32_ds3231_32khz_oscillator_using_lib` for detailed instructions and the wiring diagram.



## Sensor I2C

- The sensor acts as a I2C slave.
- I2C Device address:
  - **0x68** for both the DS3231 SOIC16 package (the off-the-shelf breakout boards documented in this project are all SOIC16) and the smaller DS3231M SOIC8 package.



## Reference: the ESP32 MJD Starter Kit SDK

Do you also want to create innovative IoT projects that use the ESP32 chip, or ESP32-based modules, of the popular company Espressif? Well, I did and still do. And I hope you do too.

The objective of this well documented Starter Kit is to accelerate the development of your IoT projects for ESP32 hardware using the ESP-IDF framework from Espressif and get inspired what kind of apps you can build for ESP32 using various hardware modules.

Go to https://github.com/pantaluna/esp32-mjd-starter-kit



## Issues

- https://github.com/espressif/esp-idf/issues/584 The ESP32 function mktime() does not support datetime values with year >= 2038! This does affect the DS3231 component when working with datetime values in that range.

