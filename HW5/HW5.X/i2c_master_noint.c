#include <proc/p32mx250f128b.h>
#include "i2c_master_noint.h"

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k

#define SLAVE_ADDR 0x20

void i2c_master_setup(void) {
    //Turn off analog function from Pin B2 and B3. B2 is pin 17, and B3 is pin 18
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB3 = 0;

    // I2CBRG = [1/(2*Fsck) - PGD]*Pblck - 2 
    I2C2BRG = 233;

    //PGD is 104 ns
    I2C2CONbits.ON = 1; // turn on the I2C2 module
}

// Start a transmission on the I2C bus

void i2c_master_start(void) {
    I2C2CONbits.SEN = 1; // send the start bit
    while (I2C2CONbits.SEN) {
        ;
    } // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C2CONbits.RSEN = 1; // send a restart 
    while (I2C2CONbits.RSEN) {
        ;
    } // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C2TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while (I2C2STATbits.TRSTAT) {
        ;
    } // wait for the transmission to finish
    if (I2C2STATbits.ACKSTAT) { // if this is high, slave has not acknowledged
        // ("I2C2 Master: failed to receive ACK\r\n");
    }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1; // start receiving data
    while (!I2C2STATbits.RBF) {
        ;
    } // wait to receive the data
    return I2C2RCV; // read and return the data
}

void i2c_master_ack(int val) { // sends ACK = 0 (slave should send another byte)
    // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1; // send ACKDT
    while (I2C2CONbits.ACKEN) {
        ;
    } // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) { // send a STOP:
    I2C2CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while (I2C2CONbits.PEN) {
        ;
    } // wait for STOP to complete
}

void initExpander() {
    //Set pins GP0-3 to output, GP4-7 to inputs
    setExpander(0x00, 0xF0); //0x00 is register controlling which pins are input or outputs
    //Turn on pull up resistor on pin GP7
    //    setExpander(0x06, 0b10000000); //0x06 is register controlling pull up resistors

}

void setExpander(char address, char value) {
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1 | 0);
    i2c_master_send(address);
    i2c_master_send(value);
    i2c_master_stop();
}

char getExpander(char address) {
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1 | 0);
    i2c_master_send(address);
    i2c_master_restart();
    i2c_master_send(SLAVE_ADDR << 1 | 1);
    char val = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    return val;
}