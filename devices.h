#ifndef __DEVICES_H__
#define __DEVICES_H__

#include <stdint.h>

enum ECmd
{
	GET, GET_VERSION, GET_ID, READ_MEMORY, GO, WRITE_MEMORY,
	ERASE, WRITE_PROTECT, WRITE_UNPROTECT, READOUT_PROTECT, READOUT_UNPROTECT
};

class stm32_dev_t
{
	uint16_t id;
	uint8_t version, option1, option2, bootVersion;
	uint8_t cmds[11];
	
};
struct stm32_dev_info_t
{
	uint16_t id;
	uint32_t flashStart, flashEnd;
	uint32_t optStart, optEnd;
	int sectors;
	void (*update)(stm32_dev_t* dev);
};

extern stm32_dev_info_t devices[];

#endif
