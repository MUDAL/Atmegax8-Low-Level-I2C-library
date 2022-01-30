#ifndef I2C_H
#define I2C_H

#include <avr/io.h>
//Communication status
typedef enum 
{
  I2C_FAILURE = 0,
  I2C_SUCCESS
}I2C_Status;
//Functions
extern void I2C_Init(void);
extern I2C_Status I2C_SimpleWrite(uint8_t slaveAddr,uint8_t* pData,uint8_t len);
extern I2C_Status I2C_Write(uint8_t slaveAddr,uint8_t regAddr,uint8_t* pData,uint8_t len);
extern I2C_Status I2C_SimpleRead(uint8_t slaveAddr,uint8_t* pBuffer,uint8_t len);
extern I2C_Status I2C_Read(uint8_t slaveAddr,uint8_t regAddr,uint8_t* pBuffer,uint8_t len);

#endif 
