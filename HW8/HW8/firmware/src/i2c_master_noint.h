#ifndef I2C_MASTER_NOINT_H__
#define I2C_MASTER_NOINT_H__
// Header file for i2c_master_noint.c
// helps implement use I2C1 as a master without using interrupts

#define SLAVE_ADDR  0b1101011
#define CTRL1_XL    0x10
#define CTRL2_G     0x11
#define CTRL3_C     0x12
#define OUT_TEMP_L  0x20
#define OUTX_L_XL   0x28
#define WHO_AM_I    0x0F

void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz
void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop

void initExpander();
void setExpander(unsigned char address, char level);
char getExpander(unsigned char address);
void I2C_read_multiple(unsigned char address, unsigned char reg, unsigned char *data, int length);

#endif