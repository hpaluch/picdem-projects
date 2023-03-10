# Project: PIC16F88 ADC Sound

Summary:
- read potentiometer angle using ADC input
- produce sound where frequency varies on potentiometer angle

Used Hardware:
- [DM163045][DM163045] - `PICDEM Lab Development Kit`
- [PIC16F88][PIC16F88] (included with selected versions of PICDEM kit)

Details
- read potentiometer angle using ADC - values are 0 to 1023 (0x3ff)
- output on speaker where period is 1 ms + every $1 \\, \mu s$ (micro-second)
  for +1 increment from ADC.
- example for: ADC=0 (potentiometer set to ground), speaker period
  is 1 ms (1 000 Hz)
- example for: ADC=1023 (0x3ff) - when potentiometer set to +5V voltage,
  speaker period is $2023 \\, \mu s$ ( $1000 \\, \mu s + 1023$ ).
- also ADC value is send to UART so we can see on Digital Analyzer what ADC value
  was used for Speaker period.

![PIC16F88 ADC Sound Schematic](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/ExpressPCB/pic16f88-adc-sound.png)

Here are results from scope:

![PIC16F88 ADC Sound ADC UART](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/pic16f88-adc-sound.X/assets/ad2-adc-sound.png)

In above example:
- trigger set on UART pin, failing edge.
- potentiometer set to maximum Voltage, ADC has maximum value 0x3FF (1023) - see that on `UART_ADC` decoded
  digital pin
- measured period is 2.015 ms, should be 2.023 ms, However it matches instruction cycle frequency error
  (1% in my case - 2.01 MHz instead of 2.00 MHz)

List of used [PIC16F88][PIC16F88] pins:
- `RA0/AN0/PIN17` ADC potentiometer input, channel 0 (AN0)
- `RA1/AN1/PIN18` sound output, period from 1.000 ms to 2.023 ms ( 1.000ms + ADC value, where ADC max value is 1023)
- `RA6/OSC2/CLKO/PIN15` - $f_{OSC}/4$ => 8 MHz /4 =>  2 MHz
- `RB5/SS/TX/CK/PIN11` - UART transmitting 2 bytes of ADC value,8-data bits,1-stop bit, no-parity

Best lesson learned:
- Unlike Timer0, the Timer1 and Timer2 are considered as *peripherals*,
  so additionally PEIE must be enabled to receive interrupts(!)
- The only way to to have fine period control on Timer1 AND automatically
  RESET Timer1 on Compare is to use so called:

  > `CCP1CON=0x0b` => Compare mode, trigger special event (CCP1IF bit is set,
  > CCP1 pin is unaffected); CCP1 resets TMR1 and starts
  > an A/D conversion (if A/D module is enabled
  >
  > Also note that  `CCP1CON=0x0a` => "Compare mode,
  > generate software interrupt on match (CCP1IF bit is set, CCP1 pin is
  > unaffected)" - does not Reset Timer0 on 
  > Compare event (so next period is always 65536).
  > However there is nice trick in [AN594][AN594] - rolling over CCP registers
  > on each interrupt (adding new period to existing value in CCP). However I still prefer using special
  > event where all Timer1 housekeeping is done automatically (with the exception of flipping pulse
  > in IRQ handler)

Please note that unlike many other examples we use INTRC frequency 8 MHz, so Instruction
clock is 2 MHz, this allow as to increment Timer1 period in 0.5us steps and thus speaker
period by 1us  steps (2 times longer). Here is scope from `RA6/OSC2/CLKO/PIN15`:

![PIC16F88 ADC Sound Instruction clock](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/pic16f88-adc-sound.X/assets/ad2-fosc-div4.png)

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

