/*
 * File:   pic16f88_matrix_lcd.c
 * Summary: PIC16F88 connected to matrix LCD MC16011A
 * PINS:
 *   - RA6/OSC2/CLKO/PIN15 - output instruction clock 1 MHz (to check frequency)
 *   - RB5/SS/TX/CK/PIN11 - blinking LED using Timer1
 *  DevKit: DM163045 - PICDEM Lab Development Kit
 *     MCU: PIC16F88 PDIP
 *      SW: MPLAB X IDE v6.05, XC8 v2.40, DFP 1.3.42
 * Created on December 20, 2022, 9:59 AM
 */

// CONFIG1
#pragma config FOSC = INTOSCCLK // Oscillator Selection bits (INTRC oscillator; CLKO function on RA6/OSC2/CLKO pin and port I/O function on RA7/OSC1/CLKI pin)
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


// ISR from DS50002400C, page 7
void __interrupt() pic16f_irq(void){
    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
         INTCONbits.TMR0IF = 0; // must acknowledge Timer0 interrupt
         // toggle LED
         PORTBbits.RB5 ^= 1;
    }
} 

void main(void) {
    //PORTA = 0;                  // set latches on PORTA to known value
    //ANSEL = 0;                  //
    TRISB = (u8) ~ iLED_MASK; // output our I/O ports

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
        // NOP
    }
    // never reached
    return;
}
