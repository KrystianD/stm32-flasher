#include "uart.h"

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

SerialHandle uart_open (const char* path, int speed)
{
	struct termios options;

	int fd = open (path, O_RDWR | O_NOCTTY /*| O_NDELAY*/);

	if (fd < 0)
		return -1;

	tcgetattr (fd, &options);

	cfsetispeed (&options, speed);
	cfsetospeed (&options, speed);
	// cfsetispeed (&options, B9600);
	// cfsetospeed (&options, B9600);
	// cfsetispeed (&options, B230400);
	// cfsetospeed (&options, B230400);
	// printf ("230400\n");

	options.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS | HUPCL);
	options.c_cflag |= (CS8 | PARENB | CLOCAL | CREAD);
	options.c_lflag &= ~(ICANON | ISIG | ECHO | ECHOE | ECHOK | ECHONL | ECHOCTL | ECHOPRT | ECHOKE | IEXTEN);
	options.c_iflag &= ~(INPCK | IXON | IXOFF | IXANY | ICRNL);
	options.c_oflag &= ~(OPOST | ONLCR);

	for (uint i = 0; i < sizeof (options.c_cc); i++)
		options.c_cc[i] = _POSIX_VDISABLE;

	options.c_cc[VTIME] = 0;
	options.c_cc[VMIN] = 1;

	tcsetattr (fd, TCSAFLUSH, &options);

	return fd;
}
void uart_setspeed (SerialHandle handle, int speed)
{
	struct termios options;

	tcgetattr (handle, &options);

	cfsetispeed (&options, speed);
	cfsetospeed (&options, speed);

	tcsetattr (handle, TCSAFLUSH, &options);
}
int uart_tx (SerialHandle handle, const char *data, int len)
{
	while (len)
	{
		int written = write (handle, data, len);
		if (written < 0)
			return -1;
		len -= written;
		data += written;
	}
	return 0;
}
int uart_rx (SerialHandle handle, char *data, int len, int timeout_ms)
{
	struct termios options;
	int l = len;
	int r = 0;

	tcgetattr (handle, &options);
	options.c_cc[VTIME] = timeout_ms / 100;
	options.c_cc[VMIN] = 0;
	tcsetattr (handle, TCSANOW, &options);

	while (len)
	{
		int rread = read (handle, data, len);
		if (rread == 0)
			return r;
		if (rread < 0)
			return -1;

		len -= rread;
		data += rread;
		r += rread;
	}
	return r;
}
void uart_close (SerialHandle handle)
{
	close (handle);
}
