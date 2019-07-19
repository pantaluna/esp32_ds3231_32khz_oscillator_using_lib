## Project Description
This project demonstrates the hardware and software setup to use a DS3231 as an external 32KHz external oscillator for the ESP32 (and get accurate timings in deep sleep).

=> The app gets the current datetime via SNTP once (Wifi on) and then goes into an endless cycle of power-on for a few seconds + deep sleep for a while. Let the app run for a day and check afterwards in the debug log how accurate the datetime is. If an external  oscillator would not have been used then the current datetime would have drifted quite a lot (as documented in the ESP32 data sheets).



This project can used with the following Real-Time Clock boards:

1. The blue circular ChronoDot V2 Real Time Clock Module (recommended).
2. A barebone DS3231MZ+ SOIC8 150_mil chip on a SOIC8 breakout board (soldered manually).
3. The blue rectangular large ZS042 DS3231 RTC Real Time Clock Module.



Use this project as well to get insights about the ESP-IDF component "mjd_ds3231". Check out the subdirectory "components/mjd_ds3231" for installation/wiring/usage instructions, data sheets, FAQ, photo's, etc. for the hardware and software.



## Why would you use an accurate external oscillator 32Khz for the ESP32?

The ESP32's internal 150Hz crystal oscillator is not very accurate  (as documented in the ESP32 data sheets) for e.g. data loggers which wake up from deep sleep at regular intervals.

The ESP32 datetime deviates typically **+23 seconds per every hour** from the real-world datetime.



Luckily the ESP32 also supports a more accurate RTC external 32kHz oscillator and the Maxim DS3231 provides that functionality.

The ESP32 hardware design guidelines (as documented in **KConfig ESP32_RTC_CLOCK_SOURCE**) state that:

- The external clock signal must be connected to the ESP32's 32K_XP pin. The amplitude should be < 1.0V in case of a square wave signal. The common mode voltage should be 0.1 < Vcm < 0.5Vamp, where Vamp is the signal amplitude (I have not yet figured out what the last sentence means).
- A 1 - 10 nF capacitor must be wired between the ESP32's 32K_XN pin and GND.



**The 32K pin of the DS3231 RTC board** is the square wave output signal of the 32K oscillator. It is **open-drain** so you need to wire up **a  pullup resistor** to read this signal correctly from a microcontroller pin. Pullup values between 1K and 10M are typical; i choose 100K.

The ESP32 requires specific voltages for the square wave signal (see above) so the pullup resistor cannot be wired to the 3.3V rail of the ESP32. The pullup must be wired to for example a 0.8V power rail which can be made from VCC 3.3V using a voltage divider with Rtop=680K and Rbottom=220K.

The wiring setup to use the DS3231 as a RTC Real Time Clock via the I2C protocol, as described in the main document for each RTC board, is not required. The DS3231 pins SCL and SDA are not used in this scenario.

The black/yellow RTC board cannot be used because the 32K output pin of the DS3231 chip is not exposed on that breakout board.



## What are the HW SW requirements of the ESP32 MJD Starter Kit?

### Hardware

- A decent ESP development board. I suggest to buy a popular development board with good technical documentation and a significant user base. Examples: [Adafruit HUZZAH32](https://www.adafruit.com/product/3405),  [Espressif ESP32-DevKitC](http://espressif.com/en/products/hardware/esp32-devkitc/overview), [Pycom WiPy](https://pycom.io/hardware/), [Wemos D32](https://wiki.wemos.cc/products:d32:d32).
- The peripherals that are used in the project.
  @tip The README of each component contains a section "Shop Products".
  @example A Bosch BME280 meteo sensor breakout board.



### Software: ESP-IDF v3.2

- A working installation of the **Espressif ESP-IDF *V3.2* development framework**** (detailed instructions @ http://esp-idf.readthedocs.io/en/latest/get-started/index.html).

```
mkdir ~/esp
cd    ~/esp
git clone -b v3.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v3.2
```

- A C language editor or the Eclipse IDE CDT (instructions also @ http://esp-idf.readthedocs.io/en/latest/get-started/index.html).



## Wiring

Check out the images.

![Wiring - ESP32 and DS3231 32KHz oscillator-01.png](Wiring - ESP32 and DS3231 32KHz oscillator-01.png)

```
- RTC board pin VCC => ESP32 pin VCC 3.3V
- RTC board pin GND => ESP32 pin GND

- ESP32 pin GPIO#32 (pin 32K_XP) => 100K pullup resistor => a custom power rail (> 0.5V; < 1.0V)
- ESP32 pin GPIO#32 (pin 32K_XP) => RTC board pin 32K
- ESP32 pin GPIO#33 (pin 32K_XN) => 1nF ceramic capacitor => GND

- Voltage divider circuit of 0.8V: InputVCC=3.3V, Rtop=680K, Rbottom=220K, GND.
```

https://learn.adafruit.com/adafruit-ds3231-precision-rtc-breakout



## Software Setup

- Goto the project directory, for example 
  `cd ~/esp32_ds3231_32khz_oscillator_using_lib`
- Run `make menuconfig`.
- Select "PROJECT CONFIGURATION" and enter the WiFi SSID and Password.
- Select "Component config"  => ESP32-specific  => RTC clock source := **External 32kHz oscillator at 32K_XP pin**
- Select "Component config"  => ESP32-specific  => Number of cycles for RTC_SLOW_CLK calibration := **5000**
- Exit menuconfig.



## Running the example
```
cd ~/esp32_ds3231_32khz_oscillator_using_lib
make flash monitor
```



Check the debug log and make sure the following lines are logged (the exact calibration value may vary). If the external oscillator is not found then an error message will be logged.

```
D (537) clk: waiting for 32k oscillator to start up
D (849) clk: RTC_SLOW_CLK calibration value: 16048920
```



Procedure:

1. Start the app.
2. Check the current datetime after some time (an hour, a day) and check that it still accurate.
3. Stop the app.
4. Unplug the wires to pins 32K_XN (#32) and 32K_XP (#33).
5. Start the app.
6. Check the current datetime after some time (an hour, a day). The datetime will have deviated a lot.



## Test Results

The ESP32 datetime does not deviate from the real-world datetime when the external 32Khz crystal oscillator is wired up and configured :)

The ESP32 datetime deviates typically **+23 seconds per every hour** from the real-world datetime when the external 32Khz crystal oscillator is not used.



## Notes

- **Status Register (0Fh) Bit 3 "Enable 32kHz Output (EN32kHz)"**: this bit controls the status of the 32kHz pin. When set to logic 1, the 32kHz pin is enabled and outputs a 32.768kHz square-wave signal. When set to logic 0, the 32kHz pin goes to a high-impedance state. **The initial power-up state of this bit is logic 1, and a 32.768khz square-wave signal appears at the 32khz pin** after power is applied to the DS3231.



## Reference: the ESP32 MJD Starter Kit SDK

Do you also want to create innovative IoT projects that use the ESP32 chip, or ESP32-based modules, of the popular company Espressif? Well, I did and still do. And I hope you do too.

The objective of this well documented Starter Kit is to accelerate the development of your IoT projects for ESP32 hardware using the ESP-IDF framework from Espressif and get inspired what kind of apps you can build for ESP32 using various hardware modules.

Go to https://github.com/pantaluna/esp32-mjd-starter-kit



## Related ESP-IDF projects

`my_ds3231_clock_using_lib` This project demonstrates the basics of using the MJD component "mjd_ds3231" for the DS3231-based RTC Real Time Clock boards.

