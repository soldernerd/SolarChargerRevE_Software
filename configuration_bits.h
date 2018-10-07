/* 
 * File:   configuration_bits.h
 * Author: luke
 *
 * Created on 7. Oktober 2018, 13:35
 */

#ifndef CONFIGURATION_BITS_H
#define	CONFIGURATION_BITS_H

// CONFIG1L
#pragma config WDTEN = ON       // Watchdog Timer (Enabled)
#pragma config PLLDIV = 2       // PLL Prescaler Selection (Divide by 2 (8 MHz oscillator input))
#pragma config CFGPLLEN = ON    // PLL Enable Configuration Bit (PLL Enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset (Enabled)
#pragma config XINST = OFF      // Extended Instruction Set (Disabled)

// CONFIG1H
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide)
#pragma config CP0 = OFF        // Code Protect (Program memory is not code-protected)

// CONFIG2L
#pragma config OSC = HSPLL      // Oscillator (HS+PLL, USB-HS+PLL)
#pragma config SOSCSEL = HIGH   // T1OSC/SOSC Power Selection Bits (High Power T1OSC/SOSC circuit selected)
#pragma config CLKOEC = OFF     // EC Clock Out Enable Bit  (CLKO output disabled on the RA6 pin)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor (Disabled)
#pragma config IESO = ON        // Internal External Oscillator Switch Over Mode (Enabled)

// CONFIG2H
#pragma config WDTPS = 512       // Watchdog Postscaler (1:512)

// CONFIG3L
#pragma config DSWDTOSC = T1OSCREF  // DSWDT Clock Select (DSWDT uses T1OSC/T1CKI)
#pragma config RTCOSC = T1OSCREF    // RTCC Clock Select (RTCC uses T1OSC/T1CKI)
#pragma config DSBOREN = OFF        // Deep Sleep BOR (Disabled)
#pragma config DSWDTEN = OFF        // Deep Sleep Watchdog Timer (Disabled)
#pragma config DSWDTPS = 8192       // Deep Sleep Watchdog Postscaler (1:8,192 (8.5 seconds))

// CONFIG3H
#pragma config IOL1WAY = OFF    // IOLOCK One-Way Set Enable bit (The IOLOCK bit (PPSCON<0>) can be set and cleared as needed)
#pragma config ADCSEL = BIT10   // ADC 10 or 12 Bit Select (10 - Bit ADC Enabled)
#pragma config MSSP7B_EN = MSK7 // MSSP address masking (7 Bit address masking mode)

// CONFIG4L
#pragma config WPFP = PAGE_1    // Write/Erase Protect Page Start/End Location (Write Protect Program Flash Page 1)
#pragma config WPCFG = OFF      // Write/Erase Protect Configuration Region  (Configuration Words page not erase/write-protected)

// CONFIG4H
#pragma config WPDIS = OFF      // Write Protect Disable bit (WPFP<6:0>/WPEND region ignored)
#pragma config WPEND = PAGE_0   // Write/Erase Protect Region Select bit (valid when WPDIS = 0) (Pages 0 through WPFP<6:0> erase/write protected)
#pragma config LS48MHZ = SYS48X8// Low Speed USB mode with 48 MHz system clock bit (System clock at 48 MHz USB CLKEN divide-by is set to 8)
 

#endif	/* CONFIGURATION_BITS_H */

