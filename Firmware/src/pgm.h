#ifndef INC_PGM_H_
#define INC_PGM_H_


/****** CONSTANTS ******/
#define EE_OUT1VAL		0xF800 // 1b
#define EE_OUT2VAL		0xF801 // 1b
#define EE_CRC			0xF802  // 1b
#define EE_CODELASTADDR	0x3FFF	// last address of the code flash

extern uint8_t FlashKey1, FlashKey2;

#define flkey_prepare	FlashKey1 = 0xA5; FlashKey2=0xF1;
#define flkey_clear		FlashKey1 = 0x00; FlashKey2=0x00;
//SI_LOCATED_VARIABLE_NO_INIT(conf_saved, static const volatile conf_saved_t, SI_SEG_CODE, 0x1800);


void defaultSettingsEE(void);
void storeSettingsEE(void);
void loadSettingsEE(void);
//void clearDataEE(bool force);
//void readEE2XDATA(void);
//void writeXDATA2EE(void);

#endif /* INC_PGM_H_ */
