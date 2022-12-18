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

## Project: PIC16F88 ADC Sound

* [pic16f88-adc-sound.X/](pic16f88-adc-sound.X/)
  - read potentiometer angle using ADC - values are 0 to 0x3ff (1023)
  - output on speaker where period is 1 ms + every 2us (micro-second)
    for +1 increment from ADC.
  - example ADC=0 (potentiometer set to ground), speaker period
    is 1 ms (1 000 Hz)
  - example ADC=1023 (0x3ff) - when potentiometer set to +5V voltage,
    speaker period is 3048 us (1000us + 2*1024).

![PIC16F88 ADC Soun Schematic](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/ExpressPCB/pic16f88-adc-sound.png)

Best lesson learned:
- Unlike Timer0, the Timer1 and Timer2 are considered as *peripherals*,
  so additionally PEIE must be enabled to receive interrupts(!)
- The only way to to have fine period control on Timer1 AND automatically
  RESET Timer1 on overflow is to use so called:

  > `CCP1CON=0x0b` => Compare mode, trigger special event (CCP1IF bit is set,
  > CCP1 pin is unaffected); CCP1 resets TMR1 and starts
  > an A/D conversion (if A/D module is enabled

Again please note that  `CCP1CON=0x0b` => "Compare mode,
generate software interrupt on match (CCP1IF bit is set, CCP1 pin is
unaffected)" - is unusable, because it does not Reset Timer0 on 
Compare event.

# List of MCUs

Officially [PICDEM Lab Development Kit][DM163045]  includes these MCUs:

* [PIC12F615][PIC12F615]
* [PIC16F616][PIC16F616]

My revised PICDEM board additionally includes these MCUs:
* [PIC10F206][PIC10F206]
* [PIC16F630][PIC16F630]
* [PIC16F690][PIC16F690]
* [PIC16F88][PIC16F88] - my favorite, has ICD (hardware debugger support)


[DM163045]: http://www.microchip.com/Developmenttools/ProductDetails/DM163045 "PICDEM Lab Development Kit"
[PIC10F206]: https://www.microchip.com/en-us/product/PIC10F206
[PIC12F615]: https://www.microchip.com/en-us/product/PIC12F615
[PIC16F616]: https://www.microchip.com/en-us/product/PIC16F616 
[PIC16F630]: https://www.microchip.com/en-us/product/PIC16F630
[PIC16F690]: https://www.microchip.com/en-us/product/PIC16F690
[PIC16F88]: https://www.microchip.com/wwwproducts/en/PIC16F88 "PIC16F88 Overview"
[ExpressPCB]:  https://www.expresspcb.com/pcb-cad-software/
