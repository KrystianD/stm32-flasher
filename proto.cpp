#include "proto.h"

#define TIMEOUT (1000)

SerialHandle handle = -1;

int uart_send_cmd(uint8_t cmd)
{
	char buf[] = { (char)cmd, (char)~cmd };
	return uart_tx(handle, buf, 2);
}

int uart_read_ack_nack()
{
	char buf[1];
	int r = uart_rx(handle, buf, 1, TIMEOUT);
	if (r == -1) return -1;
	return buf[0];
}
int uart_read_ack_nack(int timeout)
{
	char buf[1];
	int r = uart_rx(handle, buf, 1, timeout);
	if (r == -1) return -1;
	return buf[0];
}
int uart_read_ack_nack_fast()
{
	char buf[1];
	int r = uart_rx(handle, buf, 1, 100);
	if (r == -1) return -1;
	return buf[0];
}

int uart_read_byte()
{
	char b;
	int r = uart_rx(handle, &b, 1, TIMEOUT);
	return r == -1 ? -1 : b;
}
int uart_read_data(char* data, int len)
{
	int r = uart_rx(handle, data, len, TIMEOUT * len);
	return r == -1 ? -1 : r;
}

int uart_write_data_checksum(const char* data, int len)
{
	char chk = data[0];
	for (int i = 1; i < len; i++)
		chk ^= data[i];
	int w = uart_tx(handle, data, len);
	if (w == -1) return -1;
	w = uart_tx(handle, &chk, 1);
	return w == -1 ? -1 : len;
}
int uart_write_data(const char* data, int len)
{
	int w = uart_tx(handle, data, len);
	return w == -1 ? -1 : len;
}
int uart_write_byte(char data)
{
	int w = uart_tx(handle, &data, 1);
	return w == -1 ? -1 : 1;
}
