#include "devices.h"

void stm32f40xxx_stm32f41xxx_update(stm32_dev_t* dev);

stm32_dev_info_t devices[] =
{
	{
		.id = 0x0413,
		.flashStart = 0x08000000,
		.flashEnd = 0x0805ffff,
		.optStart = 0x1fffc000,
		.optEnd = 0x1fffc00f,
		.sectors = 11,
		.update = stm32f40xxx_stm32f41xxx_update,
	},
	{
		.id = 0,
	}
};

void stm32f40xxx_stm32f41xxx_update(stm32_dev_t* dev)
{
}

stm32_dev_info_t* findDeviceInfo(const stm32_dev_t& device)
{
	for (int i = 0; devices[i].id != 0; i++)
		if (devices[i].id == device.id)
			return &devices[i];
	return 0;
}
