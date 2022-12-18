/*
 * File:   pic16f88_adc_sound.c
 * Summary:
 *          1. read potentiometer value with ADC on AN0/PIN17
 *          3. produce sound on speaker on RA1, period is 1,000.0 us + ADC
 *             for ADC=3 period is 1000 + 3 = 1,003.0 us
 *     I/O: - RA0/AN0/PIN17 ADC potentiometer input, channel 0
 *          - RA1/AN1/PIN18 sound output, period by pot
 *          - RA6/OSC2/CLKO/PIN15 - fOSC/4 => 8 MHz /4 =>  2 MHz
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

// Timer1 Period Base
const u16 PERIOD_BASE = 1000;
const u16 ADC_MAX_VALUE = 0x3ff;

// show to user which mode is compiled
#ifdef __DEBUG
#warning Build in Debug mode
#else
#warning Build in Production (Run) mode
#endif

#define fSPKR_MASK  _PORTA_RA1_MASK
// must be volatile because it is shared between main thread
// and interrupt routine
volatile unsigned char vLATA = 0;
// flag from interrupt, when ADC conversion is finished
volatile u8 AdcDone = 0;
// Compare Period shadow register
u16 wCcPr = 0;

void click_few_times(u16 cycles)
{
    u16 i;
    for(i=0;i!=cycles;i++){
        vLATA = vLATA ^ fSPKR_MASK;
        PORTA = vLATA;
        __delay_ms(10);
    }   
}

void __interrupt() pic8int(void)
{
    // when CCP1 Interrupt is enabled
    // && CCP1 Compare Interrupt occurred
    if(PIE1bits.CCP1IE && PIR1bits.CCP1IF){
        // flip speaker output
        vLATA = vLATA ^ fSPKR_MASK;
        PORTA = vLATA;
        PIR1bits.CCP1IF = 0; // ack Compare interrupt
    }
    if(PIE1bits.ADIE && PIR1bits.ADIF){
        AdcDone = 1;
        PIR1bits.ADIF = 0;
    }
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
    u16 oldCcPr=0;
    u16 adc=0;
    
    // initialize PINs as soon as possible
    PORTA = 0; // ensure defined values on output latches
    TRISA = (u8) ~ fSPKR_MASK; // disable all outputs, expect LED on RA1
    PORTB = 0;
    TRISB = (u8) ~0; // all PORTB as outputs - to avoid floating Digital Inputs

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
    // interrupt stuff
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    // 5. enable AD module - do this as separated step!
    ADCON0bits.ADON = 1;
 
    // OSC setup
    OSCCONbits.IRCF = 0b111;    // f = 8 MHz => 2 MHz instruction clock
    // wait until OSC is stable, otherwise we will screw up 1st
    // call of __delay_ms() !!! it will be much slower then expected!!
    while(OSCCONbits.IOFS == 0){/*nop*/};

    click_few_times(100); // click for around 1s
    // We use CCP in Compare mode with Software Interrupt
    // RESET CCP first
    CCP1CON = 0;
    // Timer1: 1:1 prescaler, using system clock (1MHz)
    T1CON = 0;
    // Timer1: Reset current Timer value
    TMR1H = 0;
    TMR1L = 0;
    // resets TMR1 and starts an A/D conversion (if A/D module is enabled)
    // NOTE: We can't use "Software Interrupt mode", because it
    // does NOT reset TMR1 on compare
    CCP1CON = 0x0b;
    // preload Capture Compare Period for 500us (2000 Hz) - need 2 interrupts
    wCcPr = PERIOD_BASE;
    oldCcPr = wCcPr;
    CCPR1H = (u8)(wCcPr >> 8);
    CCPR1L = (u8)(wCcPr & 0xff);
    PIR1bits.TMR1IF = 0;
    PIR1bits.CCP1IF = 0;
    // Enable CCP1  interrupts
    PIE1bits.CCP1IE = 1;
    // Enable Timer1
    T1CONbits.TMR1ON = 1;
    // Unlike Timer0, both Timer1 and Timer2 are considered as PERIPHERAL
    // so PEIE must be set to receive interrupts !!!
    // see "Figure 8-1: Interrupt Logic"
    //     in "PICmicro Mid-Range MCU Family Reference Manual"
    INTCONbits.PEIE = 1;
    // Enable interrupts globally
    ei();   
    while(1){
        // delay ADC reading loop to reduce output Jitter...
        __delay_ms(100);
        // wait until AdcConversion is finished (set to 1 from IRQ handler)
        while(AdcDone==0);
        AdcDone = 0; // prepare for next event
        // read ADC
        adc = ADRESH;
        adc <<= 8;
        adc |= ADRESL;

        // basically we add ADC to 1 ms, so for ADC = 100,
        // we have 1000us+100us
        // so every ADC step adds 1us
        // Please note that speaker frequency is /2, so we use Instruction
        // clock 2 MHz instead of 1 MHz to compensate
        wCcPr = PERIOD_BASE + adc;
        if (wCcPr != oldCcPr){
            CCPR1H = (u8)(wCcPr >> 8);
            CCPR1L = (u8)(wCcPr & 0xff);
            oldCcPr = wCcPr;
        }
    }
    
    return;
}
