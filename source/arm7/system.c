/*---------------------------------------------------------------------------------

  Copyright (C) 2005 - 2010
    Michael Noland (joat)
	Jason Rogers (Dovoto)
	Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1.  The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.
  2.  Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.
  3.  This notice may not be removed or altered from any source
      distribution.

---------------------------------------------------------------------------------*/
#include <nds/ndstypes.h>
#include <nds/system.h>
#include <nds/fifocommon.h>
#include <nds/fifomessages.h>
#include <nds/interrupts.h>
#include <nds/bios.h>
#include <nds/arm7/clock.h>
#include <nds/arm7/sdmmc.h>
#include <nds/arm7/i2c.h>

bool sleepIsEnabled = true;
extern bool __dsimode;

//---------------------------------------------------------------------------------
void powerValueHandler(u32 value, void* user_data) {
//---------------------------------------------------------------------------------
	u32 temp;
	u32 ie_save;
	int battery, backlight, power;

	switch(value & 0xFFFF0000) {
		//power control
	case PM_REQ_LED:
		temp = readPowerManagement(PM_CONTROL_REG);
		temp &= ~(3 << 4); //clear led bits
		temp |= value & 0xFF;
		writePowerManagement(PM_CONTROL_REG, temp);
		break;
	case PM_REQ_ON:
		temp = readPowerManagement(PM_CONTROL_REG);
		writePowerManagement(PM_CONTROL_REG, temp | (value & 0xFFFF));
		break;
	case PM_REQ_OFF:
		temp = readPowerManagement(PM_CONTROL_REG) & (~(value & 0xFFFF));
		writePowerManagement(PM_CONTROL_REG, temp);
		break;

	case PM_REQ_SLEEP:
			
		ie_save = REG_IE;
		// Turn the speaker down.
		swiChangeSoundBias(0,0x400);
		// Save current power state.
		power = readPowerManagement(PM_CONTROL_REG);
		// Set sleep LED.
		writePowerManagement(PM_CONTROL_REG, PM_LED_CONTROL(1));
		// Register for the lid interrupt.
		REG_IE = IRQ_LID;

		// Power down till we get our interrupt.
		swiSleep(); //waits for PM (lid open) interrupt

		//100ms
		swiDelay(838000);
		
		// Restore the interrupt state.
		REG_IE = ie_save;

		// Restore power state.
		writePowerManagement(PM_CONTROL_REG, power);

		// Turn the speaker up.
		swiChangeSoundBias(1,0x400); 

		// update clock tracking
		resyncClock();
		break;

	case PM_REQ_SLEEP_DISABLE:
		sleepIsEnabled = false;
		break;

	case PM_REQ_SLEEP_ENABLE:
		sleepIsEnabled = true;
		break;
	case PM_REQ_BATTERY:
		if (!__dsimode) {
			battery = (readPowerManagement(PM_BATTERY_REG) & 1)?3:15;
			backlight = readPowerManagement(PM_BACKLIGHT_LEVEL);
			if (backlight & (1<<6)) battery += (backlight & (1<<3))<<4;
		} else {
			battery = i2cReadRegister(I2C_PM,I2CREGPM_BATTERY);
		}
		fifoSendValue32(FIFO_SYSTEM, battery);
		break;
	case PM_DSI_HACK:
		__dsimode = true;
		break;
	}
}

//---------------------------------------------------------------------------------
void systemSleep(void) {
//---------------------------------------------------------------------------------
	if(!sleepIsEnabled) return;
	//puts arm9 to sleep which then notifies arm7 above (which causes arm7 to sleep)
	fifoSendValue32(FIFO_PM, PM_REQ_SLEEP);
}

//---------------------------------------------------------------------------------
int sleepEnabled(void) {
//---------------------------------------------------------------------------------
	return sleepIsEnabled;
}

//---------------------------------------------------------------------------------
void systemMsgHandler(int bytes, void *user_data) {
//---------------------------------------------------------------------------------
	FifoMessage msg;
	int retval;

	fifoGetDatamsg(FIFO_SYSTEM, bytes, (u8*)&msg);
	
	switch (msg.type) {

	case SYS_SD_READ_SECTORS:
		retval = sdmmc_sdcard_readsectors(msg.sdParams.startsector, msg.sdParams.numsectors, msg.sdParams.buffer);
		fifoSendValue32(FIFO_SYSTEM, retval);
		break;
	case SYS_SD_WRITE_SECTORS:
		retval = sdmmc_sdcard_writesectors(msg.sdParams.startsector, msg.sdParams.numsectors, msg.sdParams.buffer);
		fifoSendValue32(FIFO_SYSTEM, retval);
		break;
	
	}
}

//---------------------------------------------------------------------------------
void systemValueHandler(u32 value, void* user_data) {
//---------------------------------------------------------------------------------
	int result;
	
	switch(value) {

	case SYS_HAVE_SD:
		result = sdmmc_read16(REG_SDSTATUS0);
		fifoSendValue32(FIFO_SYSTEM, result);
		break;

	case SYS_SD_START:
		if (sdmmc_read16(REG_SDSTATUS0) == 0) {
			result = 1;
		} else {
			result = 0;
			sdmmc_controller_init();
			//result = sdmmc_sdcard_init();
		}
		fifoSendValue32(FIFO_SYSTEM, result);
		break;

	case SYS_SD_IS_INSERTED:
		result = sdmmc_cardinserted();
		fifoSendValue32(FIFO_SYSTEM, result);
		break;

	case SYS_SD_STOP:
		break;

	}
}

//---------------------------------------------------------------------------------
void installSystemFIFO(void) {
//---------------------------------------------------------------------------------
	fifoSetValue32Handler(FIFO_PM, powerValueHandler, 0);
	fifoSetValue32Handler(FIFO_SYSTEM, systemValueHandler, 0);
	fifoSetDatamsgHandler(FIFO_SYSTEM, systemMsgHandler, 0);
}


