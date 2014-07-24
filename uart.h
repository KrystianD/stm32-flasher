#ifndef UART_H
#define UART_H

typedef int SerialHandle;

SerialHandle uart_open (const char* path, int speed);
void uart_setspeed (SerialHandle handle, int speed);
int uart_tx (SerialHandle handle, const char* data, int len);
int uart_rx (SerialHandle handle, char* data, int len, int timeout_ms);
void uart_close (SerialHandle handle);

int getSpeedByBaud(int baud);

#endif
