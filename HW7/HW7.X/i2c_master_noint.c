#include <proc/p32mx250f128b.h>
#include "i2c_master_noint.h"

// I2C Master utilities, 100 kHz, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// Change I2C1 to the I2C channel you are using
// I2C pins need pull-up resistors, 2k-10k



void i2c_master_setup(void) {
    //Turn off analog function from Pin B2 and B3. B2 is pin 6, and B3 is pin 7
    ANSELBbits.ANSB2 = 0; //SDA2
    ANSELBbits.ANSB3 = 0; //SCL2

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
    //CTRL1_XL: Turn on accelerometer 
    setExpander(CTRL1_XL, 0x82); //Sample Rate at 1.66kHz, 2g sensitivity and 100 Hz filter
    setExpander(CTRL2_G, 0x88); //Sample rate at 1.66kHz, 1000 dps sensitivity
    setExpander(CTRL3_C, 0x04); //Turn IF_INC bit to 1 to read multiple registers in a row. IF_INC bit is 3rd bit
}

void setExpander(unsigned char address, char value) {
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR << 1 | 0);
    i2c_master_send(address);
    i2c_master_send(value);
    i2c_master_stop();
}

char getExpander(unsigned char address) {
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

void I2C_read_multiple(unsigned char address, unsigned char reg, unsigned char *data, int length)
{
    int i;
    int l = length;
    i2c_master_start();
    i2c_master_send(address << 1 | 0);
    i2c_master_send(reg);
    i2c_master_restart();
    i2c_master_send(SLAVE_ADDR << 1 | 1);
    for(i=0; i<l;i++)
    {
        data[i] = i2c_master_recv();
        if(i != l - 1)
        {
            i2c_master_ack(0);
        }
        else
        {
            i2c_master_ack(1);   
        }
        
    }
    i2c_master_stop();
}