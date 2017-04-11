#include <proc/p32mx250f128b.h>

#include "spi_lib.h"
#define CS LATBbits.LATB7

void setVoltage(unsigned int channel, unsigned int voltage)
{
    unsigned int bit1 = 0, bit2 = 0;
    //channel: 0 = DAC A, 1 = DAC B
    bit1 = (channel << 3) | 0b0111; // Create the configuration bits
    bit1 = (bit1 << 4)| (voltage >> 4); // Add the first 4 bit values for voltage to bit1
    bit2 = voltage << 4; // Add last 4 bit values for voltage to first 4 bits of bit 2
    
    CS = 0;
    spi1_io(bit1);
    spi1_io(bit2);
    CS = 1;
}

void initSPI1()
{
//    RPB7Rbits.RPB7R = 0b0011; //Set up Pin B7 to be SS1
    SPI1CON = 0;
    SPI1BUF;
    SPI1BRG = 1; // Can make 0x1000 to see on N-scope
//    SPI1BRG = 0x1000; // Can make 0x1000 to see on N-scope
    SPI1STATbits.SPIROV = 0;
    SPI1CONbits.CKP = 0;
    SPI1CONbits.CKE = 1;
    SPI1CONbits.MSTEN = 1;
    SPI1CONbits.ON = 1;
    
    ANSELBbits.ANSB13 = 0;
    RPB13Rbits.RPB13R = 0b0011; //Set up Pin B13 to be SDO1
    SDI1Rbits.SDI1R = 0b0100; //Set up Pin B8 (Pin 17) to be SDI1
    TRISBbits.TRISB7 = 0; // Set up Pin B7 to be CS (Slave select Digital Output pin)   
}

char spi1_io(unsigned char write)
{
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF)
    {
        ;
    }
    return SPI1BUF;
    
}