//#pragma src

#include <absacc.h>
#include "main.h"
#include "pgm.h"
#include "gen.h"

volatile uint8_t FlashKey1, FlashKey2;

uint8_t calculateCRCsettingsEE(void);
void erasePageEEflash(uint16_t addr);
void write_flash_byte (uint8_t source, uint8_t xdata * destination);
//void write_flash_Word (uint16_t data *source, uint8_t xdata * destination);

void storeSettingsEE(void){
	uint8_t crc;
	int16_t val1, val2;
	// first check did the values were changed?
	val1 = CBYTE[EE_OUT1VAL];
	val2 = CBYTE[EE_OUT2VAL];

	// store to flash
#ifdef PREVENTFLASHWRITE
	flkey_clear
#else
	flkey_prepare
#endif
	erasePageEEflash(EE_OUT1VAL);
	write_flash_byte(1 , EE_OUT1VAL);
	write_flash_byte(2 , EE_OUT2VAL);
	//write_flash_Word((uint16_t)&mAsetLED1, EE_OUT1VAL);
	//write_flash_Word((uint16_t)&mAsetLED2, EE_OUT2VAL);

	crc = calculateCRCsettingsEE();
	write_flash_byte(crc , EE_CRC);
	flkey_clear
}

void defaultSettingsEE(void){
	// minimum brightness
	//adcglob.LedStep1 = 1;
	//adcglob.LedStep2 = 1;
}


void loadSettingsEE(void){
	// first check crc
	uint8_t crc_calc,crc_stored;
	crc_calc = calculateCRCsettingsEE();
	crc_stored = CBYTE[EE_CRC];
	if(crc_calc != crc_stored){
		defaultSettingsEE();
		delay_ms(100); // wait until voltage stabilized as next we write to flash
		storeSettingsEE();
	}
	adcglob.LedStep1 = CBYTE[EE_OUT1VAL];
	adcglob.LedStep2 = CBYTE[EE_OUT2VAL];
}

// calculate flash CRC
uint8_t calculateCRCsettingsEE(void){
	uint8_t i, crc=0;
	uint8_t code *pflashread; // define empty pointer
	pflashread = EE_OUT1VAL; // assign address to a pointer

	for(i=0;i<(EE_CRC-EE_OUT1VAL-1);i++){
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
	//if(destination>=EE_OUT1VAL){
		enable_flash_write();
		*destination = source;  // Move the data to flash
		disable_flash_write();
	//}
}

/*
// store byte to address passed by variable
void write_flash_byteVar (uint8_t source, uint16_t destination){
	uint8_t xdata *dst = (uint8_t xdata*)destination;
		enable_flash_write();
		*dst = source;  // Move the data to flash
		disable_flash_write();
}

void write_flash_Word (uint16_t data *source, uint8_t xdata * destination){
	write_flash_byte(*source>>8, destination);
	write_flash_byte(*source & 0xFF, destination+1);
}
*/

void erasePageEEflash(uint16_t addr){
	uint8_t xdata * a = (uint8_t*)addr;
	enable_flash_erase();
	*a = 0;  // Move the data to flash
	disable_flash_write();
}

/*
void eraseLastNPagesEEflash(uint16_t addr){
	// exit if all required pages were erased
	while(addr<=EE_CODELASTADDR){
		erasePageEEflash(addr);
		addr+=512; // go to next page
	}
}

// find the address of the first byte
#define FIRSTADDRDATAEE  EE_CODELASTADDR-(AISIZE*sizeof(pwmAI_t)*2)

void clearDataEE(bool force){
	uint8_t i;
	uint16_t addr;

	addr = FIRSTADDRDATAEE; // space for 2 arrays
	// if first bytes not equal 0xFF then exit
	if(!force){
		for(i=0;i<10;i++){
			uint8_t b = CBYTE[addr];
			if(b!=0xFF){return;} // exit without clearing as we already have some data there
			addr++;
		}
	}
	// store to flash
#ifdef PREVENTFLASHWRITE
	flkey_clear
#else
	flkey_prepare
#endif
	// first erase occupied pages
	addr = FIRSTADDRDATAEE; // space for 2 arrays
	eraseLastNPagesEEflash(addr);
	// loop in the cycle to write 0...
	// find the address of the first byte
	if(!force){
		addr = FIRSTADDRDATAEE; // space for 2 arrays
		while(addr<=EE_CODELASTADDR){
			write_flash_byteVar(0, addr);
			addr++;
		}
	}
}

// read data
void readEE2XDATA(void){
	uint16_t addrEE; // Address in Flash
	uint16_t i;
	uint8_t i1;
	U16_U8 tmp;
	// check, do we need to prepare EE if it doesn't contain any data?
	clearDataEE(0); // will happen nothing if there is data already
	disable_flash_write(); // make sure we are not dealing with flash writes
	addrEE = FIRSTADDRDATAEE; // space for 2 arrays

	for(i=0;i<AISIZE;i++){
		for(i1=0;i1<5;i1++){
			tmp.u8[0] = CBYTE[addrEE];
			addrEE++;
			tmp.u8[1] = CBYTE[addrEE];
			addrEE++;
			pwm_AI_data1[i].pwm[i1]=tmp.u16;
		}
	}

	for(i=0;i<AISIZE;i++){
		for(i1=0;i1<5;i1++){
			tmp.u8[0] = CBYTE[addrEE];
			addrEE++;
			tmp.u8[1] = CBYTE[addrEE];
			addrEE++;
			pwm_AI_data2[i].pwm[i1]=tmp.u16;
		}
	}
}

// write data
void writeXDATA2EE(void){
	uint16_t addrEE; // Address in Flash
	uint16_t i;
	uint8_t i1;
	U16_U8 tmp;
	// first clear the flash
	clearDataEE(1); // force to erase flash

	addrEE = FIRSTADDRDATAEE; // space for 2 arrays

	for(i=0;i<AISIZE;i++){
		for(i1=0;i1<5;i1++){
			tmp.u16 = pwm_AI_data1[i].pwm[i1];
#ifdef DEBUGUART
			sendUART(tmp.u16,0);
#endif
			write_flash_byteVar(tmp.u8[0], addrEE);
			addrEE++;
			write_flash_byteVar(tmp.u8[1], addrEE);
			addrEE++;
		}
	}

	for(i=0;i<AISIZE;i++){
		for(i1=0;i1<5;i1++){
			tmp.u16 = pwm_AI_data2[i].pwm[i1];
#ifdef DEBUGUART
			sendUART(tmp.u16,0);
#endif
			write_flash_byteVar(tmp.u8[0], addrEE);
			addrEE++;
			write_flash_byteVar(tmp.u8[1], addrEE);
			addrEE++;
		}
	}
}

*/