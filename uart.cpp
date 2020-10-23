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

	for (unsigned int i = 0; i < sizeof (options.c_cc); i++)
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

int getSpeedByBaud(int baud)
{
	switch (baud)
	{
	case 50:
		return B50;
	case 75:
		return B75;
	case 110:
		return B110;
	case 134:
		return B134;
	case 150:
		return B150;
	case 200:
		return B200;
	case 300:
		return B300;
	case 600:
		return B600;
	case 1200:
		return B1200;
	case 1800:
		return B1800;
	case 2400:
		return B2400;
	case 4800:
		return B4800;
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 57600:
		return   B57600;
	case 115200:
		return  B115200;
	case 230400:
		return  B230400;
	case 460800:
		return  B460800;
	case 500000:
		return  B500000;
	case 576000:
		return  B576000;
	case 921600:
		return  B921600;
	case 1000000:
		return B1000000;
	case 1152000:
		return B1152000;
	case 1500000:
		return B1500000;
	case 2000000:
		return B2000000;
	case 2500000:
		return B2500000;
	case 3000000:
		return B3000000;
	case 3500000:
		return B3500000;
	case 4000000:
		return B4000000;
	default:
		return 0;
	}
}
