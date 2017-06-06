#include <xc.h>
#include <proc/p32mx250f128b.h>
#include "motor_control.h"

void setUpMotors()
{
    //Set up for only DC Motors. Servo motor setup only requires timers for PWM.
    RPA0Rbits.RPA0R = 0b0101; // A0 is OC1
    TRISAbits.TRISA1 = 0;
    LATAbits.LATA1 = 0; // A1 is the direction pin to go along with OC1

    RPB2Rbits.RPB2R = 0b0101; // B2 is OC4
    TRISBbits.TRISB3 = 0;
    LATBbits.LATB3 = 0; // B3 is the direction pin to go along with OC4
    
    RPB14Rbits.RPB14R = 0b0101; // B14 is OC3
}

void setTimers()
{
    //Setting up timers for DC Motors
    T2CONbits.TCKPS = 2; // prescaler N=4 
    PR2 = 1200 - 1; // 10kHz
    TMR2 = 0;
    OC1CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC4CONbits.OCM = 0b110;
    OC1RS = 0; // max allowed value is 1119
    OC1R = 0; // read-only initial value
    OC4RS = 0; // max allowed value is 1119
    OC4R = 0; // read-only initial value
    T2CONbits.ON = 1;
    OC1CONbits.ON = 1;
    OC4CONbits.ON = 1;
    
    //Setting up timers for Servo Motors
    T3CONbits.TCKPS = 4; // prescaler N=16
    PR3 = 60000 - 1; // 50Hz
    TMR3 = 0;
    OC3CONbits.OCM = 0b110; // PWM mode without fault pin; other OC1CON bits are defaults
    OC3CONbits.OCTSEL = 1; // use timer3
    OC3RS = 4500; // should set the motor to 90 degrees (0.5ms to 2.5ms is 1500 to 7500 for 0 to 180 degrees)
    OC3R = 4500; // read-only
    T3CONbits.ON = 1;
    OC3CONbits.ON = 1;
}