/*
 * File:   pic16f88_matrix_lcd.c
 * Summary: PIC16F88 connected to matrix LCD MC16011A
 * Used PINs:
 *   - RB5/SS/TX/CK/PIN11 - blinking LED using Timer1
 *  for LCD:
 *     PIC16F88            LCD          Direction
 *   - RA0/AN0/PIN17       - D0/PIN7    I/O
 *   - RA1/AN1/PIN18       - D1/PIN8    I/O
 *   - RA2/AN2/CVREF/PIN1  - D2/PIN9    I/O
 *   - RA3/AN3/VREF+/PIN2  - D3/PIN10   I/O
 *   - RB0/INT/CCP1/PIN6   - D4/PIN11   I/O
 *   - RB1/SDI/SDA/PIN7    - D5/PIN12   I/O
 *   - RB2/SDO/RX/DT       - D6/PIN13   I/O
 *   - RB3/PGM/CCP1        - D7/PIN14   I/O
 *   
 *   - RA4/AN4/T0CKI/PIN3  - RS/PIN4    O
 *   - RA6/OSC2/CLKO/PIN15 - R/W/PIN5   O
 *   - RA7/OSC1/CLKI/PIN16 - E/PIN6     O
 * Free PINs:
 *   - RB4/SCK/SCL/PIN10
 * Reserved PINs:
 *   - RA5/MCLR/VPP/PIN4 - Master Clear (Reset). Input/programming voltage
 *   - RB3/PGM/CCP1/PIN9 - Low-Voltage ICSP Enable - UNUSED by PicKit3
 *   - RB6/AN5/PGC/T1OSO/T1CKI/PIN12 - ICD (Debugger) clock
 *   - RB7/AN6/PGD/T1OSI/PIN13 - In-Circuit Debugger and ICSP programming data
 *  DevKit: DM163045 - PICDEM Lab Development Kit
 *     MCU: PIC16F88 PDIP
 *      SW: MPLAB X IDE v6.05, XC8 v2.40, DFP 1.3.42
 * Created on December 20, 2022, 9:59 AM
 */

// CONFIG1
// Oscillator Selection bits (INTRC oscillator;
// port I/O function on both RA6/OSC2/CLKO pin and RA7/OSC1/CLKI pin)
#pragma config FOSC = INTOSCIO
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#ifdef __DEBUG
#pragma config PWRTE = OFF       // Power-up Timer Enable bit (PWRT disabled fo debug)
#else
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#endif
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)

// My I/O ports
#define iLED_MASK  _PORTB_RB5_MASK

// show to user which mode is compiled
#ifdef __DEBUG
#warning Build in Debug mode
#else
#warning Build in Production (Run) mode
#endif 

#define  _XTAL_FREQ 4000000 // f_osc (4 MHz) for __delay_ms();
#include <xc.h>

// use short typedef aliases as in Linux kernel
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

// shadow variables to avoid PORTx (non-latch read-modify-write) problem
u8 vPORTA = 0;
u8 vPORTB = 0;
// must be volatile because it is modified in interrupt handler
volatile u8 counter = 0;
u8 oldCounter = 0;
// ISR from DS50002400C, page 7
void __interrupt() pic16f_irq(void){
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
         INTCONbits.TMR0IF = 0; // must acknowledge Timer0 interrupt
         // increment counter
         // DO NOT MODIFY PORTs here - it will cause race with main thread !!!
         counter++;
    }
} 

void main(void) {
    
    vPORTA = 0;
    vPORTB = 0;
    PORTA = 0;
    PORTB = 0;

    // ANS5=RB6,ANS6=RB - input, used by Debugger as PGC and PGD
    ANSEL = 0x60; // set RA0-RA4 (ANS0-toANS4) as Digital I/O
    TRISA = 0x20; // only RA5 input (/MCLR), others as Output
    // RB4, RB6, RB7 inputs, other outputs
    TRISB = (u8) 0xd0; // 1101 0000

    OSCCONbits.IRCF = 0b110;    // 1 MHz instruction clock
    // wait until OSC is stable, otherwise we will screw up 1st
    // call of __delay_ms() !!! it will be much slower then expected!!
    while(OSCCONbits.IOFS == 0){/*nop*/};
    
    TMR0 = 0;   // defined state for TMR0
    // setup OPTION_REG
    CLRWDT(); // even when disabled WDT - see DS30487D-page 69
    OPTION_REG = _OPTION_REG_nRBPU_MASK|0x7;     
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;  // enable interrupts for Timer0
    ei();                   // enable all interrupts     
    while(1){
        if (counter != oldCounter){
            // flip PORTA on every tick
            vPORTA ^= 0xff;
            PORTA = vPORTA;
            if (counter & 1){
                // flip PORTB every second tick
                vPORTB ^= 0xff;
                PORTB = vPORTB;
            }
            oldCounter = counter;
        }
    }
    // never reached
    return;
}
