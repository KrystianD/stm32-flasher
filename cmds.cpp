/*
 * cmds.cpp
 * Copyright (C) 2014 Krystian Dużyński <krystian.duzynski@gmail.com>
 */

#include "cmds.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "proto.h"
#include "uart.h"
#include "devices.h"

extern stm32_dev_t dev;

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
	
	dev.version = uart_read_byte();
	dev.option1 = uart_read_byte();
	dev.option2 = uart_read_byte();
	
	res = uart_read_ack_nack();
	if (res != ACK)
	{
		printf("get version: NACK(2)\n");
		return -1;
	}
	
	printf("Device version: 0x%02x Option1: 0x%02x Option2: 0x%02x\n", dev.version, dev.option1, dev.option2);
	
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
	dev.bootVersion = d[0];
	
	for (int i = 0; i < 11; i++)
		dev.cmds[i] = d[i + 1];
	printf("Bootloader version: 0x%02x = v%d.%d\n", dev.bootVersion, dev.bootVersion >> 4, dev.bootVersion & 0x0f);
	printf("Get command    is 0x%02x\n", dev.cmds[GET]);
	printf("Get version    is 0x%02x\n", dev.cmds[GET_VERSION]);
	printf("Get ID         is 0x%02x\n", dev.cmds[GET_ID]);
	printf("Read memory    is 0x%02x\n", dev.cmds[READ_MEMORY]);
	printf("Go             is 0x%02x\n", dev.cmds[GO]);
	printf("Write memory   is 0x%02x\n", dev.cmds[WRITE_MEMORY]);
	printf("Erase command  is 0x%02x\n", dev.cmds[ERASE]);
	printf("Write Protect  is 0x%02x\n", dev.cmds[WRITE_PROTECT]);
	printf("Write Unpotect is 0x%02x\n", dev.cmds[WRITE_UNPROTECT]);
	printf("Read Protect   is 0x%02x\n", dev.cmds[READOUT_PROTECT]);
	printf("Read Unprotect is 0x%02x\n", dev.cmds[READOUT_UNPROTECT]);
	
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
	
	uart_send_cmd(dev.cmds[GET_ID]);
	
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
	
	dev.id = (d[0] << 8) | d[1];
	
	printf("Device ID: 0x%04x\n", dev.id);
	
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
	
	
	if (dev.cmds[ERASE] == 0x44)
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
	else if (dev.cmds[ERASE] == 0x43)
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
