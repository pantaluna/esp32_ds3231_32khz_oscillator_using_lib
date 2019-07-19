# ESP32 MJD DS3231 Real Time Clock component
This is component based on ESP-IDF for the ESP32 hardware from Espressif for the Maxim DS3231x Real-time clock chips.



The Maxim DS3231 chip comes in 2 variations:

* **DS3231/DS3131SN**: Real-time clock with **a temperature compensated crystal oscillator** (TCXO). Accuracy: +/- 3ppm. Active Supply Current: 200uA. The 32KHz output signal on pin#1 is also temperature-compensated. Can generate a 1Hz, 1.024kHz, 4.096kHz, 8.192kHz SQW signal on the SQW/INT pin. Parts: **DS3231SN#**.
* **DS3231M**: Real-time clock with **a MEMS Resonator**. Accuracy: +/- 5ppm. Active Supply Current: 200uA. The 32KHz output signal on pin#1 is **not**  temperature-compensated; meaning it will loose accuracy when the ambient temperature changes (this has no effect on the accuracy of the RTC, only the 32K output). Can only generate a 1Hz SQW signal on the SQW/INT pin. Parts: **DS3231MZ+, DS3231M+**.

[Maxim App Note 5143 Feature Comparison of the DS323x Real-Time Clocks](https://www.maximintegrated.com/en/app-notes/index.mvp/id/5143)



The breakout boards are based on the Maxim DS3231SN. **Some boards** can also be used as an accurate external 32KHz crystal oscillator for the ESP32 to improve time keeping accuracy in deep sleep.



## Example ESP-IDF project(s)
`my_ds3231_clock_using_lib` This project demonstrates the basics of using the MJD component "mjd_ds3231" for the DS3231-based RTC Real Time Clock boards.

`esp32_ds3231_32khz_oscillator_using_lib` This project demonstrates the hardware and software setup to use a DS3231 as an external 32KHz external oscillator for the ESP32 (and get accurate timings in deep sleep).



## Supported Products.
1. The blue circular ChronoDot V2 Real Time Clock Module (recommended).
2. A barebone DS3231MZ+ SOIC8 chip on a SOIC8 150_mil breakout board (soldered manually) or on a SOIC socket adapter.
3. The blue rectangular large ZS042 DS3231 RTC Real Time Clock Module.
4. The black smaller model (with yellow capacitor) for Raspberry Pi RTC Real Time Clock Module.



## Instructions

**=> Go to the _doc directory and read the specs, instructions for installation and wiring for each  RTC Board.**



## Sensor I2C

- I2C Device address:
  - 0x68 for both the DS3231 SOIC16 package (the off-the-shelf breakout boards documented in this project are all SOIC16) and the smaller DS3231M SOIC8 package.
- Sensor acts as a slave.



## Setup the Maxim DS3231 RTC as an external oscillator 32Khz for the ESP32

Checkout the project `esp32_ds3231_32khz_oscillator_using_lib`.



## Reference: the ESP32 MJD Starter Kit SDK

Do you also want to create innovative IoT projects that use the ESP32 chip, or ESP32-based modules, of the popular company Espressif? Well, I did and still do. And I hope you do too.

The objective of this well documented Starter Kit is to accelerate the development of your IoT projects for ESP32 hardware using the ESP-IDF framework from Espressif and get inspired what kind of apps you can build for ESP32 using various hardware modules.

Go to https://github.com/pantaluna/esp32-mjd-starter-kit

