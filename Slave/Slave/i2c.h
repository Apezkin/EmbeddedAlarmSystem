//
// Created by aksu-lut on 28.4.2022.
//

#ifndef SLAVE_I2C_H
#define SLAVE_I2C_H
#include <avr/io.h>

#define SLAVE_READ 0
#define SLAVE_WRITE 1
#define SLAVE_COMMON 2

#define MASTER_STOP 1
#define MASTER_ACK_RECEIVED 0
#define MASTER_NACK_RECEIVED 2
#define MASTER_ACK_LAST_BYTE_RECEIVED 3
#define MASTER_ERROR 4
/**
 @brief    Initializes slave for I2C communication.


 @param    slave_address the address of the slave.
 @return   none
*/
void I2C_slave_init(char slave_address);


/**
 @brief    Waits for master.

 @param    none
 @return   \b SLAVE_READ Master wants slave to read. \n
           \b SLAVE_WRITE Master wants slave to write. \n
           \b SLAVE_COMMON Common message from master. \n
*/
int8_t I2C_slave_listen();

/**
 @brief    Sends byte to the master with ack.

 @param    data byte to send to master device.
 @return   \b MASTER_STOP Master responds with stop signal. \n
           \b MASTER_ACK_RECEIVED Master responds with ack. \n
           \b MASTER_NACK_RECEIVED Master responds with nack. \n
           \b MASTER_ACK_LAST_BYTE_RECEIVED Master responds with ack and says that it doesn't want more. \n
           \b MASTER_ERROR Master responds with unknown signal. \n
*/
int8_t I2C_slave_transmit(uint8_t data);

/**
 @brief    Sends byte to the master with nack.

 @param    data byte to send to master device.
 @return   \b MASTER_STOP Master responds with stop signal. \n
           \b MASTER_ACK_RECEIVED Master responds with ack. \n
           \b MASTER_NACK_RECEIVED Master responds with nack. \n
           \b MASTER_ACK_LAST_BYTE_RECEIVED Master responds with ack and says that it doesn't want more. \n
           \b MASTER_ERROR Master responds with unknown signal. \n
*/
int8_t I2C_slave_transmit_nack(char data);

/**
 @brief    Reads byte from master.

 @param    none
 @return   \b MASTER_STOP Master responds with stop signal. \n
           \b MASTER_ERROR Master responds with unknown signal. \n
           \b otherwise returns the data byte from master. \n
*/
char I2C_slave_receive();


/**
 @brief    Reads data to buffer.

 @param    buffer where the data is stored
 @param    count how many bytes are read to buffer. Overflowing ones are disregarded.
 @return   none
*/
void I2C_read_to_buffer(char *buffer, uint8_t count);
#endif //SLAVE_I2C_H
