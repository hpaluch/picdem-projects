# Henryk's PICDEM projects

Here are various projects for [PICDEM Lab Development Kit][DM163045] -
Microchips Development board with 8-bit MCUs.

There are 2 programming guides for PICDEM board:
* very old using MPLAB IDE and HI-TECH C: 
  - http://ww1.microchip.com/downloads/en/DeviceDoc/41369A.pdf
* recent using MPLAB X IDE  and XC C compiler:
  - https://ww1.microchip.com/downloads/en/DeviceDoc/40001369C.pdf

Here are my Components created in [ExpressPCB Classic][ExpressPCB]
version 7.9.0.
To install those Custom components copy:
- `ExpressPCB\SchComponents_Custom\` to `%USERPROFILE%\Documents\ExpressPCB\SchComponents_Custom\` (packages)

# List of Projects

## Project: [PIC16F88 ADC Sound](pic16f88-adc-sound.X/)

Summary:
- read potentiometer angle using ADC input
- produce sound where frequency varies on potentiometer angle

Used Hardware:
- [DM163045][DM163045] - `PICDEM Lab Development Kit`
- [PIC16F88][PIC16F88] (included with selected versions of PICDEM kit)

Details:
- please visit folder [pic16f88-adc-sound.X/](pic16f88-adc-sound.X/) and
  read the [README](pic16f88-adc-sound.X/README.md) file for details.

## Project: [PIC16F88 matrix LCD](pic16f88-matrix-lcd.X/)

Summary:
- displays whole character set (from 0x00 to 0xff) on LCD 16x1 matrix display MC16011A

![PIC16F88 with LCD 16x1](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/pic16f88-matrix-lcd.X/assets/pic16f88-lcd.jpg)

Detail of display:

![Detail of LCD 16x1](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/pic16f88-matrix-lcd.X/assets/lcd16011a-detail.jpg)

Used Hardware:
- [DM163045][DM163045] - `PICDEM Lab Development Kit`
- [PIC16F88][PIC16F88] (included with selected versions of PICDEM kit)
- [MC16011A][MC16011A] matrix LCD display 16x1 with "CPU" interface - not included

Details:
- please visit folder [pic16f88-matrix-lcd.X/](pic16f88-matrix-lcd.X/) and
  read the [README](pic16f88-matrix-lcd.X//README.md) file for details.

# My other projects with PICDEM board

Here is list of my PICDEM board projects that are hosted elsewhere:
* https://github.com/hpaluch/pic16f88-adc-pwm
  - PIC16F88, ADC (to read potentiometer value), and set LED brightness using PWM
* https://github.com/hpaluch/pic16f88-comp-ccp-ex.X
  - PIC16F88 using comparator and Capture to detect signal period - NOT finished
* https://github.com/hpaluch/temp_meter_16f630
  - temperature meter with PIC16F630, DS18B20 1-wire temp sensors and 2 segment LED display
  - moved from bitbucket.org, because they busted Mercurial SCM
* https://github.com/hpaluch/sound10f206.x
  - generate sound with 10F206
  - moved from bitbucket.org, because they busted Mercurial SCM
* https://github.com/hpaluch/motor_pic10f206
  - PIC10F206 drive motor using Software pwm
  - moved from bitbucket.org, because they busted Mercurial SCM
* https://bitbucket.org/hpaluch/pic16f88-demo.x/src/master/
  - PIC16F88 UART example - control LED from PC with Putty, requires "USB Console cable"
    (USB virtual COM port with TTL(!) pinout)
  - Assembler version
* https://bitbucket.org/hpaluch/pic16f88-c-demo.x/src
  - PIC16F88 UART example - control LED from PC with Putty, requires "USB Console cable"
    (USB virtual COM port with TTL(!) pinout)
  - C version
* https://bitbucket.org/hpaluch/pic-usb-serial/src/master/
  - PIC16F206 and Software UART transmitter (only).

# List of MCUs

Officially [PICDEM Lab Development Kit][DM163045]  includes these MCUs:

* [PIC12F615][PIC12F615]
* [PIC16F616][PIC16F616]

My revised PICDEM board additionally includes these MCUs:
* [PIC10F206][PIC10F206]
* [PIC16F630][PIC16F630]
* [PIC16F690][PIC16F690]
* [PIC16F88][PIC16F88] - my favorite, has ICD (hardware debugger support)

Additionally I have also:
* [PIC16F84][PIC1684] - legend that I started with around '98 (under DOS using DIY RS232 port programmer published
  by Sam J. Engström)
* [PIC16F819][PIC16F819]

# Notes

It was observed that pull-up resistor (value from 4.7 kOhm to 10 kOhm - as recommended by PicKit3 docs)
from `/MCRL` to `Vdd` (+5V) is *required for proper power-up startup* (especially with disconnected PicKit3)!

See also https://ww1.microchip.com/downloads/aemtest/OTH/ProductDocuments/UserGuides/52116A.pdf,
DS52116A-page 22, 2.5.2 Target Connection Circuitry, FIGURE 2-4: STANDARD CONNECTION TARGET CIRCUITRY


# Other notes

GitHub.com incorrectly parses TeX spacing characters in inline math (`$ ... $`), for example `$1 \, s$`
as reported on https://github.com/community/community/discussions/17143.
Workaround is to double `\` which however breaks TeX syntax...

[AN594]: https://www.microchip.com/en-us/application-notes/an594 "Using the CCP Module(s)"
[DM163045]: http://www.microchip.com/Developmenttools/ProductDetails/DM163045 "PICDEM Lab Development Kit"
[PIC10F206]: https://www.microchip.com/en-us/product/PIC10F206
[PIC12F615]: https://www.microchip.com/en-us/product/PIC12F615
[PIC16F616]: https://www.microchip.com/en-us/product/PIC16F616 
[PIC16F630]: https://www.microchip.com/en-us/product/PIC16F630
[PIC16F690]: https://www.microchip.com/en-us/product/PIC16F690
[PIC16F819]: https://www.microchip.com/en-us/product/PIC16F819
[PIC16F84]: https://www.microchip.com/en-us/product/PIC16F84
[PIC16F88]: https://www.microchip.com/wwwproducts/en/PIC16F88 "PIC16F88 Overview"
[ExpressPCB]:  https://www.expresspcb.com/pcb-cad-software/

[MC16011A]: http://www.everbouquet.com.tw/MC16011A.html
[HD44780]: https://www.sparkfun.com/datasheets/LCD/HD44780.pdf

