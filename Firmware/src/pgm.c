//#pragma src

#include <absacc.h>
#include "main.h"
#include "pgm.h"
#include "gen.h"

volatile uint8_t FlashKey1, FlashKey2;

uint8_t calculateCRCsettingsEE(void);
void erasePageEEflash(uint16_t addr);
void write_flash_byte (uint8_t source, uint8_t xdata * destination);
void write_flash_Word (uint16_t source, uint8_t xdata * destination);

void storeSettingsEE(void){
	uint8_t i, crc;
	uint8_t *calib_ptr = calib_data;
	uint8_t code *pflashwrite; // define empty pointer
	pflashwrite = EE_CALIBDATA; // assign address to a pointer
	//int16_t val1, val2;
	// store to flash
#ifdef PREVENTFLASHWRITE
	flkey_clear
#else
	flkey_prepare
#endif
	erasePageEEflash(EE_CALIBDATA);
	for(i=0;i<(CALIBRATIONVALUES*sizeof(calib_t));i++){
		write_flash_byte(calib_ptr[i], (uint16_t)&pflashwrite[i]);
	}
	//write_flash_Word((uint16_t)&mAsetLED1, EE_OUT1VAL);
	crc = calculateCRCsettingsEE();
	write_flash_byte(crc , EE_CRC);
	flkey_clear
}

void defaultSettingsEE(void){
	  calib_data[0].ADCval = 3855; calib_data[0].divconst = 6740; //=(ADCval[0]-ADCval[1])*256/100
	  calib_data[1].ADCval = 1222; calib_data[1].divconst = 1400;
	  calib_data[2].ADCval = 675; calib_data[2].divconst = 369;
	  calib_data[3].ADCval = 531; calib_data[3].divconst = 207;
	  calib_data[4].ADCval = 450; calib_data[4].divconst = 169; //=(ADCval[4]-ADCval[5])*256/100
	  calib_data[5].ADCval = 384; calib_data[5].divconst = 0; // no calculation after 5EC
}


void loadSettingsEE(void){
	// first check crc
	uint8_t i, crc_calc,crc_stored;
	uint8_t *calib_ptr = calib_data;
	uint8_t code *pflashread; // define empty pointer
	pflashread = EE_CALIBDATA; // assign address to a pointer

	crc_calc = calculateCRCsettingsEE();
	crc_stored = CBYTE[EE_CRC];
	if(crc_calc != crc_stored){
		defaultSettingsEE();
		delay_ms(100); // wait until voltage stabilized as next we write to flash
		storeSettingsEE();
	}
	// read flash

	for(i=0;i<(EE_CRC-EE_CALIBDATA);i++){
		calib_ptr[i]=pflashread[i];
	}
}

// calculate flash CRC
uint8_t calculateCRCsettingsEE(void){
	uint8_t i, crc=0;
	uint8_t code *pflashread; // define empty pointer
	pflashread = EE_CALIBDATA; // assign address to a pointer

	for(i=0;i<(EE_CRC-EE_CALIBDATA);i++){
		crc+=pflashread[i];
	}
	return crc=~crc+1;
}


void enable_flash_write(void){
	IE_EA = 0;                  // disable interrupts
	//VDM0CN |= (VDM0CN_VDMLVL__HIGH); // Set voltage Monitor to High
	PSCTL |= (PSCTL_PSWE__WRITE_ENABLED); // Set the PSWE bit
	PSCTL &= ~(PSCTL_PSEE__ERASE_ENABLED); // Clear the PSEE bit
	FLKEY = FlashKey1;            // FLASH lock and key sequence 1
	FLKEY = FlashKey2;            // FLASH lock and key sequence 2
}

void enable_flash_erase(void){
	IE_EA = 0;                  // disable interrupts
	//VDM0CN |= (VDM0CN_VDMLVL__HIGH); // Set voltage Monitor to High
	PSCTL |= (PSCTL_PSEE__ERASE_ENABLED); // Set the PSEE bit
	PSCTL |= (PSCTL_PSWE__WRITE_ENABLED); // Set the PSWE bit
	FLKEY = FlashKey1;            // FLASH lock and key sequence 1
	FLKEY = FlashKey2;            // FLASH lock and key sequence 2
}

void disable_flash_write(void){
	PSCTL &= ~(PSCTL_PSWE__WRITE_ENABLED); // Clear the PSWE bit
	PSCTL &= ~(PSCTL_PSEE__ERASE_ENABLED); // Clear the PSEE bit
	//VDM0CN &= ~(VDM0CN_VDMLVL__HIGH); // Set voltage monitor back to Low
	IE_EA = 1;   // re-enable interrupts
}

// store byte to address passed by value
void write_flash_byte (uint8_t source, uint8_t xdata * destination){
	if(destination>=EE_CALIBDATA){
		enable_flash_write();
#ifndef PREVENTFLASHWRITE
		*destination = source;  // Move the data to flash
#else
		source=source; // dummy operation to prevent warning
#endif
		disable_flash_write();
	}
}

void write_flash_Word (uint16_t source, uint8_t xdata * destination){
	write_flash_byte(source>>8, destination);
	write_flash_byte(source & 0xFF, destination+1);
}

void erasePageEEflash(uint16_t addr){
	uint8_t xdata * a = (uint8_t*)addr;
	enable_flash_erase();
	*a = 0;  // Move the data to flash
	disable_flash_write();
}
