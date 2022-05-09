//
// Created by aksu-lut on 28.4.2022.
//

#ifndef MASTER_I2C_H
#define MASTER_I2C_H

#include <avr/io.h>

#ifndef SLAVE_WRITE_ADDR
#define SLAVE_WRITE_ADDR 0x20
#endif
#ifndef SLAVE_READ_ADDR
#define SLAVE_READ_ADDR 0x21
#endif

#define TWI_OK 0
#define TWI_ACK_RECEIVED 1
#define TWI_NACK_RECEIVED 2
#define TWI_ERROR 3
#define TWI_START_FAILED 4


/**
 @brief   Inits I2C connection.

 @param   none
 @return  none
*/
void I2C_init();

/**
 @brief    Starts writing to the slave.

 Starts the connection, sends character, and stops the connection.
 @param    slave_write_addr the write address of the slave.
 @return   status   \b TWI_ACK_RECEIVED Slave responded with ACK \n
                    \b TWI_NACK_RECEIVED Slave responded with NACK \n
                    \b TWI_START_FAILED Failed to start. \n
                    \b TWI_ERROR unexpected error. \n
*/
uint8_t I2C_write_start(char slave_write_addr);

/**
 @brief    Starts reading from the slave.

 @param    slave_read_addr the read address of the slave.
 @return   status   \b TWI_ACK_RECEIVED Slave responded with ACK. \n
                    \b TWI_NACK_RECEIVED Slave responded with NACK. \n
                    \b TWI_START_FAILED Failed to start. \n
                    \b TWI_ERROR unexpected error. \n
*/
uint8_t I2C_read_start(char slave_read_addr);

/**
 @brief   Reads character and responses with ACK.

 @param   none
 @return  data  character from slave.
*/
char I2C_read_ack();

/**
 @brief   Reads character and responses with NACK.

 @param   none
 @return  data  character from slave.
*/
char I2C_read_nack();


/**
 @brief   Stops the I2C connection by sending TWSTO.

 @param   none
 @return  none
*/
void I2C_stop();

/**
 @brief    Sends byte.

 Only sends byte, the connection has to be started and stopped.
 @param    data the byte to write to slave address.
 @return   status   \b TWI_ACK_RECEIVED was able to send the byte and received ACK from slave. \n
                    \b TWI_NACK_RECEIVED was able to send the byte and received NACK from slave. \n
                    \b TWI_ERROR error sending byte. \n
*/
uint8_t I2C_write(char data);

/**
 @brief    Send command (=character) to defined slave address.

 Starts the connection, sends character, and stops the connection.
 @param    command the character to write to slave address.
 @return   status   \b TWI_OK was able to send the command. \n
                    \b TWI_ERROR could not send the command. \n
*/
int8_t I2C_command_write(char command);

#endif //MASTER_I2C_H
