# Project: PIC16F88 matrix LCD

Summary:
- displays whole character set (from 0x00 to 0xff) on LCD matrix display 16.x

Used Hardware:
- [DM163045][DM163045] - `PICDEM Lab Development Kit`
- [PIC16F88][PIC16F88] (included with selected versions of PICDEM kit)
- [MC16011A][MC16011A] matrix LCD display 16x1 with "CPU" interface - not included

Here is whole kit in action:

![PIC16F88 with LCD 16x1](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/pic16f88-matrix-lcd.X/assets/pic16f88-lcd.jpg)

Detail of display:

![Detail of LCD 16x1](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/pic16f88-matrix-lcd.X/assets/lcd16011a-detail.jpg)

I bought this display around 20 years ago:

![MC16011A LCD 16x1 - Top](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/assets/mc16011a-top.jpg)

![MC16011A LCD 16x1 - Bottom](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/assets/mc16011a-bottom.jpg)


Details:
- project [pic16f88-matrix-lcd.X/](pic16f88-matrix-lcd.X/)
- LCD display: [MC16011A][MC16011A]
  - probably uses chip: [HD44780][HD44780]
  - it is 16x1 character display operating as 8x2 - see last image from
    - http://www.everbouquet.com.tw/MC16011A.html
- Existing PIC LCD projects:
  - https://web.archive.org/web/20010212034722/http://home.iae.nl/users/pouweha/lcd/lcd2.shtml
  - https://www.microchip.com/en-us/application-notes/an587
- Existing 16-bit PIC projects:
  - https://www.microchip.com/en-us/application-notes/an1525 (it is oxymeter,
    but it seems to use similar display)
  - https://www.microchip.com/en-us/development-tool/DM240001 (Explorer 16
    board seems to usae similar display)

Please see header of [pic16f88_matrix_lcd.c](pic16f88_matrix_lcd.c) for used PINs.

Done:
- Enumerates all characters and their HEX values
- output is *like* printf `ASCII: %c 0x%02x`

# Navigation

Go [back to project page](../../..).

[AN594]: https://www.microchip.com/en-us/application-notes/an594 "Using the CCP Module(s)"
[DM163045]: http://www.microchip.com/Developmenttools/ProductDetails/DM163045 "PICDEM Lab Development Kit"
[PIC10F206]: https://www.microchip.com/en-us/product/PIC10F206
[PIC12F615]: https://www.microchip.com/en-us/product/PIC12F615
[PIC16F616]: https://www.microchip.com/en-us/product/PIC16F616 
[PIC16F630]: https://www.microchip.com/en-us/product/PIC16F630
[PIC16F690]: https://www.microchip.com/en-us/product/PIC16F690
[PIC16F88]: https://www.microchip.com/wwwproducts/en/PIC16F88 "PIC16F88 Overview"
[ExpressPCB]:  https://www.expresspcb.com/pcb-cad-software/

[MC16011A]: http://www.everbouquet.com.tw/MC16011A.html
[HD44780]: https://www.sparkfun.com/datasheets/LCD/HD44780.pdf

