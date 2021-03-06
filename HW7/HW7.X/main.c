#include <xc.h>           // processor SFR definitions
#include <sys/attribs.h>  // __ISR macro
#include <math.h>
#include "ILI9163C.h"
#include "i2c_master_noint.h"
#include <stdio.h>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0 // some 16bit userubmit the link to your repid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    SPI1_init();
    LCD_init();
    i2c_master_setup();
    initExpander();
    
    __builtin_enable_interrupts();
    char message[100];
    LCD_clearScreen(BLACK);
    int arrLength = 14;
    unsigned char data[arrLength];    
    int time;
    
    sprintf(message, "WHOAMI:%d", getExpander(WHO_AM_I));
    LCD_string(message, 10, 80, WHITE, BLACK);
    
    while(1)
    {
        time = _CP0_GET_COUNT();
        I2C_read_multiple(SLAVE_ADDR , OUT_TEMP_L, data, arrLength);
        signed short temp = (data[1] << 8) | data[0];
        signed short gyro_x = (data[3] << 8) | data[2];
        signed short gyro_y = (data[5] << 8) | data[4];
        signed short gyro_z = (data[7] << 8) | data[6];
        signed short accel_x = (data[9] << 8) | data[8];
        signed short accel_y = (data[11] << 8) | data[10];
        signed short accel_z = (data[13] << 8) | data[12];      
        
        sprintf(message, "AX: %.2f   ", accel_x*.0061);
        LCD_string(message, 10, 100, WHITE, BLACK);
        
        sprintf(message, "AY: %.2f   ", accel_y*.0061);
        LCD_string(message, 10, 90, WHITE, BLACK);
        
        //xbar
        LCD_barX(64,64,-(accel_x*.0061),4,50, RED,BLACK);
        //ybar
        LCD_barY(64,64,-(accel_y*.0061),4,50, GREEN,BLACK);
        
        
        while(_CP0_GET_COUNT() - time < (48000000/10))
        {
            ;
        }
    }
    
    
}

