#ifndef INC_SSD1306_H_
#define INC_SSD1306_H_

#define SSD1306_COLUMNS 128
#define SSD1306_PAGES 4
#define SSD1306_ROTATION 180  // 0 and 180

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40


extern void ssd1306_init(void);
void ssd1306_send_command_start(void);
void ssd1306_send_command(uint8_t cmd);
void ssd1306_write_display_start(void);
void ssd1306_clear_display(void);
void setCol(uint8_t col);
void setRow(uint8_t row);
void ssd1306_printNumber(int16_t num);
#ifdef DEBUG
void ssd1306_printNumberDebug(int16_t num);
#endif
void ssd1306_printBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t code *bmp);
void ssd1306_printBitmapClear(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

/** Set Lower Column Start Address for Page Addressing Mode. */
#define SSD1306_SETLOWCOLUMN 0x00
/** Set Higher Column Start Address for Page Addressing Mode. */
#define SSD1306_SETHIGHCOLUMN 0x10
/** Set Memory Addressing Mode. */
#define SSD1306_MEMORYMODE 0x20
/** Set display RAM display start line register from 0 - 63. */
#define SSD1306_SETSTARTLINE 0x40
/** Set Display Contrast to one of 256 steps. */
#define SSD1306_SETCONTRAST 0x81
/** Enable or disable charge pump.  Follow with 0X14 enable, 0X10 disable. */
#define SSD1306_CHARGEPUMP 0x8D
/** Set Segment Re-map between data column and the segment driver. */
#define SSD1306_SEGREMAP 0xA0
/** Resume display from GRAM content. */
#define SSD1306_DISPLAYALLON_RESUME 0xA4
/** Force display on regardless of GRAM content. */
#define SSD1306_DISPLAYALLON 0xA5
/** Set Normal Display. */
#define SSD1306_NORMALDISPLAY 0xA6
/** Set Inverse Display. */
#define SSD1306_INVERTDISPLAY 0xA7
/** Set Multiplex Ratio from 16 to 63. */
#define SSD1306_SETMULTIPLEX 0xA8
/** Set Display off. */
#define SSD1306_DISPLAYOFF 0xAE
/** Set Display on. */
#define SSD1306_DISPLAYON 0xAF
/**Set GDDRAM Page Start Address. */
#define SSD1306_SETSTARTPAGE 0XB0
/** Set COM output scan direction normal. */
#define SSD1306_COMSCANINC 0xC0
/** Set COM output scan direction reversed. */
#define SSD1306_COMSCANDEC 0xC8
/** Set Display Offset. */
#define SSD1306_SETDISPLAYOFFSET 0xD3
/** Sets COM signals pin configuration to match the OLED panel layout. */
#define SSD1306_SETCOMPINS 0xDA
/** This command adjusts the VCOMH regulator output. */
#define SSD1306_SETVCOMDETECT 0xDB
/** Set Display Clock Divide Ratio/ Oscillator Frequency. */
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
/** Set Pre-charge Period */
#define SSD1306_SETPRECHARGE 0xD9
/** Deactivate scroll */
#define SSD1306_DEACTIVATE_SCROLL 0x2E
/** No Operation Command. */
#define SSD1306_NOP 0XE3
//------------------------------------------------------------------------------

// this section is based on https://github.com/adafruit/Adafruit_SSD1306

#define init_sequence_length 24 //bytes
static const uint8_t code initSSD1306sq[] = { // Initialization Sequence
    // Init sequence for Adafruit 128x32 OLED module
    SSD1306_DISPLAYOFF,
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,  // the suggested ratio 0x80
    SSD1306_SETMULTIPLEX, 0x1F,        // ratio 32
    SSD1306_SETDISPLAYOFFSET, 0x0,     // no offset
    SSD1306_SETSTARTLINE | 0x0,        // line #0
    SSD1306_CHARGEPUMP, 0x14,          // internal vcc
    SSD1306_MEMORYMODE, 0x02,          // page mode
#if SSD1306_ROTATION == 0
    SSD1306_SEGREMAP | 0x1,            // column 127 mapped to SEG0
    SSD1306_COMSCANDEC,                // column scan direction reversed
#elif SSD1306_ROTATION == 180
    SSD1306_SEGREMAP | 0x0,            // column 127 mapped to SEG0
    SSD1306_COMSCANINC,                // column scan direction reversed
#endif
    SSD1306_SETCOMPINS, 0x02,          // sequential COM pins, disable remap
    SSD1306_SETCONTRAST, 0x7F,         // contrast level 127
    SSD1306_SETPRECHARGE, 0xF1,        // pre-charge period (1, 15)
    SSD1306_SETVCOMDETECT, 0x40,       // vcomh regulator level
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_NORMALDISPLAY,
    //SSD1306_INVERTDISPLAY,
    //SSD1306_DISPLAYON
};

// https://diyusthad.com/image2cpp
// Font constants
#define number_width 18
#define dot_width 6

#endif /* INC_SSD1306_H_ */
