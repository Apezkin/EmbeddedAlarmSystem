//
//https://www.electronicwings.com/avr-atmega/atmega1632-i2c
//

#include "i2c.h"
#include "millis.h"

void I2C_Init()			/* I2C initialize function */
{
    TWSR = 0x00; // TWI status register prescaler value set to 1
    TWBR = 0x03; //0x03; // TWI bit rate register.
}

uint8_t I2C_Start_Write(char slave_write_addr)/* I2C start function */
{
    uint8_t twi_status;		/* Declare variable */
    TWCR=(1<<TWSTA)|(1<<TWEN)|(1<<TWINT); /* Enable TWI, generate START */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    twi_status= TWSR & 0xF8;		/* Read TWI twi_status register */
    if(twi_status != 0x08)		/* Check weather START transmitted or not? */
        return TWI_START_FAILED;			/* Return 0 to indicate start condition fail */
    TWDR=slave_write_addr;		/* Write SLA+W in TWI data register */
    TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI & clear interrupt flag */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    twi_status= TWSR & 0xF8;		/* Read TWI twi_status register */
    if(twi_status == 0x18)		/* Check for SLA+W transmitted &ack received */
        return TWI_ACK_RECEIVED;			/* Return 1 to indicate ack received */
    if(twi_status == 0x20)		/* Check for SLA+W transmitted &nack received */
        return TWI_NACK_RECEIVED;			/* Return 2 to indicate nack received */
    else
        return TWI_ERROR;			/* Else return 3 to indicate SLA+W failed */
}
uint8_t I2C_Start_Read(char slave_read_addr)/* I2C start function */
{
    uint8_t twi_status;		/* Declare variable */
    TWCR=(1<<TWSTA)|(1<<TWEN)|(1<<TWINT); /* Enable TWI, generate START */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    twi_status= TWSR & 0xF8;		/* Read TWI twi_status register */
    if(twi_status != 0x08)		/* Check weather START transmitted or not? */
        return TWI_START_FAILED;			/* Return 0 to indicate start condition fail */
    TWDR=slave_read_addr;		/* Write SLA+W in TWI data register */
    TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI & clear interrupt flag */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    twi_status= TWSR & 0xF8;		/* Read TWI twi_status register */
    if(twi_status == 0x40)		/* Check for SLA+W transmitted &ack received */
        return TWI_ACK_RECEIVED;			/* Return 1 to indicate ack received */
    if(twi_status == 0x48)		/* Check for SLA+W transmitted &nack received */
        return TWI_NACK_RECEIVED;			/* Return 2 to indicate nack received */
    else
        return TWI_ERROR;			/* Else return 3 to indicate SLA+W failed */
}

uint8_t I2C_Write(char data){
    uint8_t tw_status;		/* Declare variable */
    TWDR=data;			/* Copy data in TWI data register */
    TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI and clear interrupt flag */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    tw_status= TWSR & 0xF8;		/* Read TWI tw_status register */
    if(tw_status == 0x28)		/* Check for data transmitted &ack received */
        return TWI_ACK_RECEIVED;			/* Return 0 to indicate ack received */
    if(tw_status == 0x30)		/* Check for data transmitted &nack received */
        return TWI_NACK_RECEIVED;			/* Return 1 to indicate nack received */
    else
        return TWI_ERROR;			/* Else return 2 for data transmission failure */
}

char I2C_Read_Ack()		/* I2C read ack function */
{
    TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA); /* Enable TWI, generation of ack */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    return TWDR;			/* Return received data */
}

char I2C_Read_Ack_Timeout(unsigned long timeout)		/* I2C read ack function */
{
    unsigned long start_time = millis();
    TWCR=(1<<TWEN)|(1<<TWINT)|(1<<TWEA); /* Enable TWI, generation of ack */
    while(!(TWCR&(1<<TWINT))){
        unsigned long time = millis();
        if(start_time + timeout < time)
        {
            TWCR=(0<<TWEN)|(0<<TWINT)|(0<<TWEA);
            return -1;
        }
    };	/* Wait until TWI finish its current job */
    return TWDR;			/* Return received data */
}

char I2C_Read_Nack()		/* I2C read nack function */
{
    TWCR=(1<<TWEN)|(1<<TWINT);	/* Enable TWI and clear interrupt flag */
    while(!(TWCR&(1<<TWINT)));	/* Wait until TWI finish its current job */
    return TWDR;		/* Return received data */
}

void I2C_Stop()			/* I2C stop function */
{
    TWCR=(1<<TWSTO)|(1<<TWINT)|(1<<TWEN);/* Enable TWI, generate stop */
    while(TWCR&(1<<TWSTO));	/* Wait until stop condition execution */
}

void I2C_Read_To_Buffer(char *buffer, uint8_t count)
{
    I2C_Start_Read(SLAVE_READ_ADDR);
    for (int i = 0; i < count; ++i) {
        buffer[i] = I2C_Read_Ack();
    }
    I2C_Read_Nack();
    I2C_Stop();
}

uint8_t I2C_Write_From_Buffer(char *buffer, uint8_t count)
{
    uint8_t status;
    status = I2C_Start_Write(SLAVE_WRITE_ADDR);
    if (status == TWI_START_FAILED || status == TWI_ERROR)
        return TWI_ERROR;
    for (int i = 0; i < count; ++i) {
        status = I2C_Write(buffer[i]);
        if (status == TWI_NACK_RECEIVED || status == TWI_ERROR)
            return TWI_ERROR;
    }
    I2C_Stop();
    return 0;
}

