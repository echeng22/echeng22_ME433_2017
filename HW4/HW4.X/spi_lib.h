#ifndef SPI_LIB_H_
#define SPI_LIB_H_

void initSPI1();
void setVoltage(unsigned int channel, unsigned int voltage);
char spi1_io(unsigned char write);

#endif