#ifndef __PROTO_H__
#define __PROTO_H__

#include <stdint.h>

#include "uart.h"

#define ACK 0x79
#define NACK 0x1f

extern SerialHandle handle;

int uart_send_cmd(uint8_t cmd);

int uart_read_ack_nack();
int uart_read_ack_nack(int timeout);
int uart_read_ack_nack_fast();

int uart_read_byte();
int uart_read_data(char* data, int len);

int uart_write_data_checksum(const char* data, int len);
int uart_write_data(const char* data, int len);
int uart_write_byte(char data);

#endif
