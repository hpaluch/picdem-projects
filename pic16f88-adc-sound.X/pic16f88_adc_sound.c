/*
 * File:   pic16f88_adc_sound.c
 * Summary:
 *          1. read potentiometer value with ADC on AN0/PIN17
 *          3. produce sound on speaker on RA1
 *     I/O: - RA0/AN0/PIN17 ADC potentiometer input, channel 0
 *          - RA1/AN1/PIN18 sound output, period by pot
 *  DevKit: DM163045 - PICDEM Lab Development Kit
 *     MCU: PIC16F88 PDIP
 *      SW: MPLAB X IDE v6.05, XC8 v2.40, DFP 1.3.42
 * Created on December 17, 2022, 2:10 PM
 */

// PIC16F88 Configuration Bit Settings
// 'C' source line config statements
// Window -> Target Memory Views -> Configuration Bits
// CONFIG1 and 2 mostly generated from:
// Window -> Target Memory Views -> Configuration Bits
// CONFIG1
#pragma config FOSC = INTOSCCLK // Oscillator Selection bits (INTRC oscillator; CLKO function on RA6/OSC2/CLKO pin and port I/O function on RA7/OSC1/CLKI pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON        // Low-Voltage Programming Enable bit ON
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off)
#pragma config CCPMX = RB0      // CCP1 Pin Selection bit (CCP1 function on RB0)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// CONFIG2
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode disabled)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
#define  _XTAL_FREQ 4000000 // f_osc (4MHz) for __delay_ms();
#include <xc.h>

// use short typedef aliases as in Linux kernel
#include <stdint.h>

// my types - like Linux kernel
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

const u16 ADC_MAX_VALUE = 0x3ff;

// show to user which mode is compiled
#ifdef __DEBUG
#warning Build in Debug mode
#else
#warning Build in Production (Run) mode
#endif

#define fSPKR_MASK  _PORTA_RA1_MASK
volatile unsigned char vLATA = 0;

void toggle_SPKR(void)
{
    vLATA = vLATA ^ fSPKR_MASK;
    PORTA = vLATA;
}

u16 read_ADC(void)
{
    u16 tmp;
    
    ADCON0bits.GO_DONE = 1;
    while(ADCON0bits.GO_DONE){
        // nop
    }
    tmp = ADRESH;
    tmp <<= 8;
    tmp |= ADRESL;
    return tmp;
}

void main(void) {
    
        // initialize PINs as soon as possible
    PORTA = 0; // ensure defined values on output latches
    TRISA = (u8) ~ fSPKR_MASK; // disable all outputs, expect LED on RA1
    PORTB = 0;
    TRISB = (u8) ~0; // all PORTB are inputs

    // ADC setup
    // 1. enable Digital I/O on RA1 (Flash LED), ensure that RA0 is Analog Input
    ANSEL = (u8) ~ _ANSEL_ANS1_MASK; 
    // 2. set VREF/ADCON1: ADFM=1 (result Right justified),
    //    ADCS2 = divide disabled, Vref VCFG = 00B - AVdd AVss
    ADCON1 = _ADCON1_ADFM_MASK;
    // 3. set AD Input channel ADCON0
    // 4. set AD conversion clock ADCON0
    //    use FRC clock, channel 0 RA0/AN0
    ADCON0 = _ADCON0_ADCS_MASK;
    // 5. enable AD module - do this as separated step!
    ADCON0bits.ADON = 1;
 
    // OSC setup
    OSCCONbits.IRCF = 0b110;    // f = 4 MHz => 1 MHz instruction clock
    // wait until OSC is stable, otherwise we will screw up 1st
    // call of __delay_ms() !!! it will be much slower then expected!!
    while(OSCCONbits.IOFS == 0){/*nop*/};
    
    // default: output 500 Hz Tone on speaker using Delay loop
    while(1){
        toggle_SPKR();
        __delay_ms(1);
    }
    
    return;
}
