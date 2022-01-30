#include "i2c.h"

#define SCL_FREQ          100000
//Status codes
#define START             0x08
#define REPEATED_START    0x10
#define MT_SLA_ACK        0x18
#define MT_DATA_ACK       0x28
#define MR_SLA_ACK        0x40
#define MR_DATA_ACK       0x50
#define MR_DATA_NACK      0x58
//Commands
#define START_CONDITION   0
#define STOP_CONDITION    1
#define ACK               2
#define NACK              3

/**
 * @brief 
 * 
 */
static void SendCommand(uint8_t condition)
{
  switch(condition)
  {
    case START_CONDITION:
      TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); 
      break;
    case STOP_CONDITION:
      TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
      break;
    case ACK:
      TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
      break;
    case NACK:
      TWCR = (1<<TWINT)|(1<<TWEN);
      break;
  }
} 

/**
 * @brief 
 * 
 * @param addr 
 */
static void SendByte(uint8_t addr)
{
  TWDR = addr; //Load a byte into I2C data register
  TWCR = (1<<TWINT)|(1<<TWEN); //Start transmission of the loaded byte
}

/**
 * @brief 
 * 
 * @param statusCode 
 * @return I2C_Status 
 */
static I2C_Status CheckStatusCode(uint8_t statusCode)
{
  while((TWCR & (1<<TWINT)) != (1<<TWINT)); //Wait for completion of event due to status code
  if((TWSR & 0xF8) != statusCode)
  {
    return I2C_FAILURE; 
  }
  return I2C_SUCCESS;  
}

/**
 * @brief Initialize I2C peripheral
 * 
 */
void I2C_Init(void)
{
  TWSR &= ~((1<<TWPS0)|(1<<TWPS1)); //TWI prescaler = 1
  TWBR = ((F_CPU / SCL_FREQ) - 16) / 2; //Set serial clock = 100kHz
}

/**
 * @brief 
 * 
 * @param slaveAddr 
 * @param pData 
 * @param len 
 * @return I2C_Status 
 */
I2C_Status I2C_SimpleWrite(uint8_t slaveAddr,uint8_t* pData,uint8_t len)
{
  SendCommand(START_CONDITION);
  if(CheckStatusCode(START) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  SendByte(slaveAddr << 1);
  if(CheckStatusCode(MT_SLA_ACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  for(uint8_t i = 0; i < len; i++)
  {
    SendByte(pData[i]);
    if(CheckStatusCode(MT_DATA_ACK) == I2C_FAILURE)
    {
      return I2C_FAILURE;
    }  
  }
  SendCommand(STOP_CONDITION);
  return I2C_SUCCESS;  
}

/**
 * @brief 
 * 
 * @param slaveAddr 
 * @param regAddr 
 * @param pData 
 * @param len 
 * @return I2C_Status 
 */
I2C_Status I2C_Write(uint8_t slaveAddr,uint8_t regAddr,uint8_t* pData,uint8_t len)
{
  SendCommand(START_CONDITION);
  if(CheckStatusCode(START) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  SendByte(slaveAddr << 1);
  if(CheckStatusCode(MT_SLA_ACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  SendByte(regAddr);
  if(CheckStatusCode(MT_DATA_ACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  for(uint8_t i = 0; i < len; i++)
  {
    SendByte(pData[i]);
    if(CheckStatusCode(MT_DATA_ACK) == I2C_FAILURE)
    {
      return I2C_FAILURE;
    }  
  }
  SendCommand(STOP_CONDITION);
  return I2C_SUCCESS;  
}

/**
 * @brief 
 * 
 * @param slaveAddr 
 * @param pBuffer 
 * @param len 
 * @return I2C_Status 
 */
I2C_Status I2C_SimpleRead(uint8_t slaveAddr,uint8_t* pBuffer,uint8_t len)
{
  SendCommand(START_CONDITION);
  if(CheckStatusCode(START) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  SendByte((slaveAddr << 1) | 1);
  if(CheckStatusCode(MR_SLA_ACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  for(uint8_t i = 0; i < (len-1); i++)
  {
    SendCommand(ACK);
    if(CheckStatusCode(MR_DATA_ACK) == I2C_FAILURE)
    {
      return I2C_FAILURE;
    }
    pBuffer[i] = TWDR;
  }
  SendCommand(NACK);
  if(CheckStatusCode(MR_DATA_NACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  pBuffer[len-1] = TWDR;  //Read last byte
  SendCommand(STOP_CONDITION);
  return I2C_SUCCESS;
}

/**
 * @brief 
 * 
 * @param slaveAddr 
 * @param regAddr 
 * @param pBuffer 
 * @param len 
 * @return I2C_Status 
 */
I2C_Status I2C_Read(uint8_t slaveAddr,uint8_t regAddr,uint8_t* pBuffer,uint8_t len)
{
  SendCommand(START_CONDITION);
  if(CheckStatusCode(START) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  SendByte(slaveAddr << 1);
  if(CheckStatusCode(MT_SLA_ACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  SendByte(regAddr);
  if(CheckStatusCode(MT_DATA_ACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  //Repeated start
  SendCommand(START_CONDITION);
  if(CheckStatusCode(REPEATED_START) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }    
  SendByte((slaveAddr << 1) | 1);
  if(CheckStatusCode(MR_SLA_ACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  for(uint8_t i = 0; i < (len-1); i++)
  {
    SendCommand(ACK);
    if(CheckStatusCode(MR_DATA_ACK) == I2C_FAILURE)
    {
      return I2C_FAILURE;
    }
    pBuffer[i] = TWDR;
  }
  SendCommand(NACK);
  if(CheckStatusCode(MR_DATA_NACK) == I2C_FAILURE)
  {
    return I2C_FAILURE;
  }
  pBuffer[len-1] = TWDR;  //Read last byte
  SendCommand(STOP_CONDITION);    
  return I2C_SUCCESS;
}
