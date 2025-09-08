#ifndef	_I2C_H_
#define _I2C_H_


extern void I2CStart(void);
extern void I2CStop(void);
extern void I2CRestart(void);
extern void I2CAck(void);
extern void I2CNak(void);
extern void I2CWait(void);
extern void I2CSend(unsigned char dat);
extern unsigned char I2CRead(void);
extern void I2CInit(void);
extern void I2C_Open (unsigned int FCLOCK);

extern unsigned char ReadByte(unsigned char address);
extern void WriteByte(unsigned char address, unsigned char data);



#endif			/* _I2C_H_ */