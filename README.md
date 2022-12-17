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
  - output speaker tone based on ADC - TODO
  - TODO: currently there is fixed sound using Timer1

![PIC16F88 ADC Soun Schematic](https://raw.githubusercontent.com/hpaluch/picdem-projects/master/ExpressPCB/pic16f88-adc-sound.png)

Best lesson learned:
- Unlike Timer0, the Timer1 and Timer2 are considered as *peripherals*,
  so additionally PEIE must be enabled to receive interrupts(!)

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
