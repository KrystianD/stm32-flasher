#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "proto.h"
#include "uart.h"
#include "devices.h"

uint8_t firmwareData[128 * 1024];
int firmwareLen;
uint8_t eraseCmd;

// device info
uint16_t deviceId;
uint8_t deviceVer, deviceOption1, deviceOption2, bootVer;
uint8_t cmds[11];

#include <sys/time.h>
#include <unistd.h>
uint32_t getTicks()
{
	timeval tv;
	gettimeofday(&tv, 0);
	uint32_t val = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return val;
}

int getCommand();
int getID();
int readoutUnprotect();
int writeUnprotect();
int readoutProtect();
int getVersion();
int start()
{
	printf("sending init...\n");
	uart_tx(handle, "\x7f", 1);
	
	int res = uart_read_ack_nack_fast();
	if (res == ACK || res == NACK)
	{
		printf("device answered with 0x%02x\n", res);
		
		if (getVersion())
			return -1;
		if (getCommand())
			return -1;
		if (getID())
			return -1;
			
		return 0;
	}
	return -1;
}
int getVersion()
{
	int res;
	
	printf(">>> get version\n");
	
	uart_send_cmd(0x01);
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("get version: NACK(1)\n");
		return -1;
	}
	
	deviceVer = uart_read_byte();
	deviceOption1 = uart_read_byte();
	deviceOption2 = uart_read_byte();
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("get version: NACK(2)\n");
		return -1;
	}
	
	printf("Device version: 0x%02x Option1: 0x%02x Option2: 0x%02x\n", deviceVer, deviceOption1, deviceOption2);
	
	return 0;
}
int getCommand()
{
	int res;
	
	printf(">>> get command\n");
	
	uart_send_cmd(0x00);
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("get command: NACK(1)\n");
		return -1;
	}
	
	int len = uart_read_byte() + 1;
	// printf ("getcmd len: %d\n", len);
	
	char d[50];
	memset(d, 0, 50);
	uart_read_data(d, len);
	bootVer = d[0];
	
	for (int i = 0; i < 11; i++)
		cmds[i] = d[i + 1];
	printf("Bootloader version: 0x%02x = v%d.%d\n", bootVer, bootVer >> 4, bootVer & 0x0f);
	printf("Get command    is 0x%02x\n", cmds[GET]);
	printf("Get version    is 0x%02x\n", cmds[GET_VERSION]);
	printf("Get ID         is 0x%02x\n", cmds[GET_ID]);
	printf("Read memory    is 0x%02x\n", cmds[READ_MEMORY]);
	printf("Go             is 0x%02x\n", cmds[GO]);
	printf("Write memory   is 0x%02x\n", cmds[WRITE_MEMORY]);
	printf("Erase command  is 0x%02x\n", cmds[ERASE]);
	printf("Write Protect  is 0x%02x\n", cmds[WRITE_PROTECT]);
	printf("Write Unpotect is 0x%02x\n", cmds[WRITE_UNPROTECT]);
	printf("Read Protect   is 0x%02x\n", cmds[READOUT_PROTECT]);
	printf("Read Unprotect is 0x%02x\n", cmds[READOUT_UNPROTECT]);
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("get cmd: NACK(2)\n");
		return -1;
	}
	
	return 0;
}
int getID()
{
	int res;
	
	printf(">>> get id\n");
	
	uart_send_cmd(cmds[GET_ID]);
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("get id: NACK(1)\n");
		return -1;
	}
	
	int len = uart_read_byte() + 1;
	printf("get id: length: %d\n", len);
	
	char d[50];
	memset(d, 0, 50);
	uart_read_data(d, len);
	
	deviceId = (d[0] << 8) | d[1];
	
	printf("Device ID: 0x%04x\n", deviceId);
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("get id: NACK(2)\n");
		return -1;
	}
	
	return 0;
}
int readoutUnprotect()
{
	int res;
	
	uart_send_cmd(0x92);
	
	res = uart_read_ack_nack(2000);
	if (res != ACK)
	{
		printf("Readout Unprotect command first NACK'ed\n");
		return -1;
	}
	
	res = uart_read_ack_nack(2000);
	if (res != ACK)
	{
		printf("Readout Unprotect command second NACK'ed\n");
		return -1;
	}
	
	printf("Readout Unprotect succeed\n");
	
	usleep(1000 * 1000);
	
	return 0;
}
int readoutProtect()
{
	int res;
	
	uart_send_cmd(0x82);
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("Readout Protect command first NACK'ed\n");
		return -1;
	}
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("Readout Protect command second NACK'ed\n");
		return -1;
	}
	
	printf("Readout Protect succeed\n");
	
	return 0;
}
int writeUnprotect()
{
	int res;
	
	uart_send_cmd(0x73);
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("Write Unprotect command first NACK'ed\n");
		return -1;
	}
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("Write Unprotect command second NACK'ed\n");
		return -1;
	}
	
	printf("Write Unprotect succeed\n");
	
	usleep(1000 * 1000);
	
	return 0;
}
int erase()
{
	int res;
	
	
	if (cmds[ERASE] == 0x44)
	{
		printf("erasing device with Extended Erase command\n");
		
		uart_send_cmd(0x44);
		res = uart_read_ack_nack();
		if (res != ACK)
		{
			printf("Erase NACK'ed\n");
			return -1;
		}
		
		printf("Erase ACK'ed\n");
		
		uint16_t pages = 1;
		// uint16_t pages = 16;
		//
		uint8_t data[1000];
		data[0] = pages >> 8;
		data[1] = pages & 0xff;
		for (uint16_t i = 0; i <= pages; i++)
		{
			data[2 + i * 2] = i >> 8;
			data[2 + i * 2 + 1] = i & 0xff;
		}
		
		uart_write_data_checksum("\xff\xff", 2);
		// uart_write_data_checksum (data, 2 + pages * 2 + 2);
		
		// usleep(1000000);
		
		res = uart_read_ack_nack();
		if (res != ACK)
		{
			printf("Mass Erase NACK'ed\n");
			return -1;
		}
		
		printf("Mass Erase ACK'ed\n");
		
		return 0;
	}
	else if (cmds[ERASE] == 0x43)
	{
		printf("erasing device with standard Erase command\n");
		
		uart_send_cmd(0x43);
		int res = uart_read_ack_nack();
		if (res == ACK)
		{
			uart_send_cmd(0xff);
			res = uart_read_ack_nack();
			if (res == NACK)
			{
				printf("device NACKed\n");
				return -1;
			}
			printf("device erased\n");
			return 0;
		}
		else
		{
			printf("NACK received 0x%02x\n", res);
			return -1;
		}
	}
	else
	{
		printf("unknown Erase command\n");
		return -1;
	}
}
int programm()
{
	uint32_t curAddr = 0x08000000;
	uint32_t sent = 0;
	
	uint32_t startTime = getTicks();
	
	while (sent < firmwareLen)
	{
		printf("sending page of addr: 0x%08x...\n", curAddr);
		uart_send_cmd(0x31);
		
		int res = uart_read_ack_nack();
		if (res == ACK)
		{
			uint32_t tmp = ((curAddr & 0x000000ff) << 24) | ((curAddr & 0x0000ff00) << 8) |
			               ((curAddr & 0x00ff0000) >> 8) | ((curAddr & 0xff000000) >> 24);
			uart_write_data_checksum((char*)&tmp, 4);
			
			res = uart_read_ack_nack();
			if (res == ACK)
			{
				int left = firmwareLen - sent;
				if (left > 256) left = 256;
				
				printf("to send: %d\n", left);
				
				char buf[1000];
				
				memcpy(buf + 1, firmwareData + sent, left);
				
				buf[0] = left - 1;
				
				uart_write_data_checksum(buf, left + 1);
				
				res = uart_read_ack_nack();
				printf("ack: 0x%02x\n", res);
				
				if (res == ACK)
				{
					sent += left;
					curAddr += left;
					printf("ok 0x%08x %d of %d\n", curAddr, sent, firmwareLen);
				}
				else
				{
					return -1;
				}
			}
		}
		else
		{
			return -1;
		}
	}
	
	uint32_t endTime = getTicks();
	float time = endTime - startTime;
	float avg = firmwareLen / (time / 1000.0f) / 1024.0f;
	
	printf("time: %d ms avg speed: %.2f KBps (%d bps)\n", endTime - startTime, avg, (int)(avg * 8.0f * 1024.0f));
	
	return 0;
}
int run(uint32_t addr)
{
	printf("go to 0x%08x...\n", addr);
	uart_send_cmd(0x21);
	
	int res = uart_read_ack_nack();
	if (res == ACK)
	{
		uint32_t tmp = ((addr & 0x000000ff) << 24) | ((addr & 0x0000ff00) << 8) |
		               ((addr & 0x00ff0000) >> 8) | ((addr & 0xff000000) >> 24);
		uart_write_data_checksum((char*)&tmp, 4);
		
		res = uart_read_ack_nack();
		if (res == ACK)
		{
			printf("acked 1\n");
			return 0;
		}
		else
		{
			return -1;
		}
		
		res = uart_read_ack_nack();
		if (res == ACK)
		{
			printf("acked 2\n");
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int main(int argc, char** argv)
{
	bool testOnly = 0;
	
	if (argc < 4)
	{
		printf("usage: %s port speed test|bin_file\n", argv[0]);
		return 1;
	}
	
	if (strcmp(argv[3], "test") == 0)
	{
		testOnly = 1;
	}
	else
	{
		FILE *file = fopen(argv[3], "rb");
		firmwareLen = fread(firmwareData, 1, sizeof(firmwareData), file);
		fclose(file);
	}
	
	while ((firmwareLen % 4) != 0)
	{
		firmwareData[firmwareLen] = 0xff;
		firmwareLen++;
	}
	
	int speed = 0;
	if (strcmp(argv[2], "230400") == 0) speed = B230400;
	else if (strcmp(argv[2], "115200") == 0) speed = B115200;
	else if (strcmp(argv[2], "460800") == 0) speed = B460800;
	else if (strcmp(argv[2], "500000") == 0) speed = B500000;
	else if (strcmp(argv[2], "1000000") == 0) speed = B1000000;
	else if (strcmp(argv[2], "1500000") == 0) speed = B1500000;
	
	for (;;)
	{
		if (handle != -1)
			uart_close(handle);
		handle = uart_open(argv[1], speed);
		if (handle == -1)
		{
			perror("unable to open");
			return 1;
		}
		int res;
		
		res = start();
		if (res == 0)
		{
			if (testOnly)
				break;
				
			res = erase();
			if (res == 0)
			{
				res = programm();
				if (res == 0)
				{
					res = run(0x08000000);
					if (res == 0)
					{
						break;
					}
				}
			}
		}
		usleep(100 * 1000);
	}
	
	return 0;
}

