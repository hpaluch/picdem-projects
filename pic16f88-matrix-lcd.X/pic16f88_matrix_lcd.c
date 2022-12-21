/*
 * File:   pic16f88_matrix_lcd.c
 * Summary: PIC16F88 connected to matrix 1x16 LCD MC16011A
 * Function:
 *   - enumerates all characters on display as "ASCII: %c 0x%02x"
 *   - blinks LED on RB5
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
 *   - RB2/SDO/RX/DT/PIN8  - D6/PIN13   I/O
 *   - RB3/PGM/CCP1        - D7/PIN14   I/O
 *   
 *   - RA4/AN4/T0CKI/PIN3  - RS/PIN4    O
 *   - RA6/OSC2/CLKO/PIN15 - R/W/PIN5   O
 *   - RA7/OSC1/CLKI/PIN16 - E/PIN6     O
 * Free PINs:
 *   - RB4/SCK/SCL/PIN10
 * Reserved PINs:
 *   - RA5/MCLR/VPP/PIN4 - Master Clear (Reset). Input/programming voltage
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
//#ifdef __DEBUG
#pragma config PWRTE = OFF       // Power-up Timer Enable bit (PWRT disabled fo debug)
/*#else
 #pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
 #endif*/
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

// show to user which mode is compiled
#ifdef __DEBUG
#warning Build in Debug mode
#else
#warning Build in Production (Run) mode
#endif 

#define  _XTAL_FREQ 4000000 // f_osc (4 MHz) for __delay_ms();
#include <xc.h>

// My I/O ports (i=input,o=output,io=input/output)
#define oLED_MASK  _PORTB_RB5_MASK
// for LCD display
#define oLCD_RS_MASK _PORTA_RA4_MASK
#define oLCD_RW_MASK _PORTA_RA6_MASK
#define oLCD_E_MASK _PORTA_RA7_MASK

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

u8 glob_error_code = 0;
void fatal_error(u8 error_code)
{
    glob_error_code = error_code; //for debugger
    while(1){
        // loop forever with very fast blinking LED
        __delay_ms(50);
        PORTB ^= oLED_MASK;
    }
}

void LCD_set_data(u8 value)
{
    vPORTA &= 0xf0;
    vPORTA |= (value & 0x0f);
    vPORTB &= 0xf0;
    vPORTB |= ( (value >> 4) & 0x0f);
    PORTA = vPORTA;
    PORTB = vPORTB;
}

// ensure that all data pins RA0-3,4,6,7,RB0-3 are in output mode
void LCD_data_output_mode(u8 value)
{
    // set output data LATCHes before switch
    LCD_set_data(value);
    // now set output mode
    TRISA = 0x20; // only RA5 input, all others are output
    TRISB &= 0xf0;
}


void LCD_strobe(void)
{
    NOP();NOP();NOP();
    vPORTA |= oLCD_E_MASK;
    PORTA = vPORTA;
    NOP();NOP();NOP();
    vPORTA &= (u8)~ oLCD_E_MASK;
    PORTA = vPORTA;
    NOP();NOP();NOP();
}

void LCD_send_cmd(u8 cmd)
{
    vPORTA &= (u8) ~ (oLCD_E_MASK | oLCD_RS_MASK | oLCD_RW_MASK);
    PORTA = vPORTA;
    LCD_data_output_mode(cmd);
    LCD_strobe();
    __delay_ms(2); // required when we are not checking BF (Busy Flag)
    // set all signals to idle mode
    vPORTA &= (u8) ~ (oLCD_E_MASK | oLCD_RS_MASK | oLCD_RW_MASK);
    PORTA = vPORTA;
}

// Function Set:
// 0 0 1 DL  N F x x
// 0 0 1 1   8 0 0 0 => 0x38 (our 16x1 display use 8x2 addressing)
#define LCD_FN_SET 0x38

void LCD_init(void)
{
    // trying "Initializing by Instruction" from HD44780.pdf
    // ensure that I/O pins are in correct mode
    LCD_data_output_mode(0);
    // 1. Wait for more than 15 ms after VCC rises to 4.5 V
    __delay_ms(100);
    // Function Set:
    LCD_set_data(LCD_FN_SET);
    LCD_strobe();
    // 2. Wait for more than 4.1 ms
    __delay_ms(8);
    LCD_strobe();
    // 3. Wait for more than 100 us
    __delay_ms(1);
    LCD_strobe();
    // finally do initialization sequence
    LCD_send_cmd( LCD_FN_SET );
    LCD_send_cmd( 0x0F );  // Display on/off control, cursor on, blinking
    LCD_send_cmd( 0x06 ); // entry mode set
    LCD_send_cmd( 0x01 ); // clear screen and reset position
}

void LCD_setpos(u8 pos)
{
    pos |= 0x80;
    LCD_send_cmd(pos);
}

void LCD_putc(u8 c)
{
    vPORTA &= (u8) ~ (oLCD_E_MASK | oLCD_RS_MASK | oLCD_RW_MASK);
    vPORTA |= oLCD_RS_MASK;
    PORTA = vPORTA;
    LCD_data_output_mode(c);
    LCD_strobe();
    __delay_ms(2); // required when we are not checking BF (Busy Flag)
    // set all signals to idle mode
    vPORTA &= (u8) ~ (oLCD_E_MASK | oLCD_RS_MASK | oLCD_RW_MASK);
    PORTA = vPORTA;
}

u8 HEX_digit_to_ascii(u8 digit)
{
    digit &= 0x0f;
    if (digit < 10){
        return '0'+digit;
    }
    return 'A'+(digit-10);
}

void LCD_put_hexbyte(u8 val)
{
    LCD_putc(HEX_digit_to_ascii( (val >> 4) & 0x0f  ));
    LCD_putc(HEX_digit_to_ascii( val & 0x0f  ));
}

void LCD_puts(const char *str)
{
    const char *p;
    for(p=str;*p != '\0';p++){
        LCD_putc(*p);
    }
}

void main(void) {
    u8 c='!';
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

    LCD_init();
    LCD_setpos(0);
    LCD_puts("Init...");
    
    TMR0 = 0;   // defined state for TMR0
    // setup OPTION_REG
    CLRWDT(); // even when disabled WDT - see DS30487D-page 69
    OPTION_REG = _OPTION_REG_nRBPU_MASK|0x7;     
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;  // enable interrupts for Timer0
    ei();                   // enable all interrupts
    LCD_setpos(0);
    LCD_puts("ASCII: ");
    LCD_putc(c);
    LCD_setpos(0x40);
    LCD_puts(" 0x");
    LCD_put_hexbyte(c);
    
    while(1){
        if (counter != oldCounter && (counter & 0xf) == 0xf){
            // put ascii char
            LCD_setpos(7);
            c++;
            LCD_putc(c);
            LCD_setpos(0x43);
            LCD_put_hexbyte(c);
            
            // flip LED on every char change
            vPORTB ^= oLED_MASK;
            PORTB = vPORTB;
            oldCounter = counter;
        }
    }
    // never reached
    return;
}
