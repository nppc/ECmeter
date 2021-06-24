#include "main.h"
#include "SSD1306.h"
#include "i2c.h"

void ssd1306_printDigitLine(uint8_t line, uint8_t digit);


void ssd1306_init(void){
  uint8_t i,d;
  ssd1306_send_command_start();
  for (i = 0; i < init_sequence_length; i++) {
    d=initSSD1306sq[i];
    I2C_Write(d);
  }
  I2C_Stop();
}


void ssd1306_send_command_start(void){
  i2cBegin(0x3C, 0); // address of SSD1306
  I2C_Write(SSD1306_COMMAND);
}

void ssd1306_send_command(uint8_t cmd){
  ssd1306_send_command_start();
  I2C_Write(cmd);
  I2C_Stop();
}

void ssd1306_write_display_start(void){
  i2cBegin(0x3C, 0); // address of SSD1306
  I2C_Write(SSD1306_DATA);
}


void setCol(uint8_t col) {
  ssd1306_send_command(SSD1306_SETLOWCOLUMN | (col & 0XF));
  ssd1306_send_command(SSD1306_SETHIGHCOLUMN | (col >> 4));
}


void setRow(uint8_t row) {
  ssd1306_send_command(SSD1306_SETSTARTPAGE | row);
}

void ssd1306_clear_display(void){
uint8_t i,i1;
  for(i1=0;i1<4;i1++){
    setCol(0);
    setRow(i1);
    ssd1306_write_display_start();
    for(i=0;i<128;i++){
      I2C_Write(0);
    }
    I2C_Stop();
  }
  setCol(0);
  setRow(0);
}

// 0.00 - 9.99
void ssd1306_printNumber(int16_t num){
  // convert binary number to BCD
  uint8_t i, a[3],tmp3, col;
  int16_t tmp;
  if(num<1000){
      tmp = num;
      a[0] = tmp / 100;
      tmp = tmp % 100;
      a[1] = tmp / 10;
      tmp = tmp % 10;
      a[2] = tmp;
  }else{
      a[0]=10;
      a[1]=10;
      a[2]=10;
  }
  // now print to LCD
  // we print 4 rows and 3 digits with dot
  for(tmp=0;tmp<4;tmp++){
    col = 128-((number_width+2)*3+dot_width);
    setCol(col); // set the position
    setRow(tmp);
    // first digit
    ssd1306_write_display_start();
    ssd1306_printDigitLine(tmp,a[0]);
    I2C_Write(0);
    I2C_Write(0);
    // print dot
    for(i=0;i<dot_width;i++){
      tmp3 = i+tmp*dot_width;
      I2C_Write(dot_bitmap[tmp3]);
    }
    I2C_Write(0);
    I2C_Write(0);
    // print second digit
    ssd1306_printDigitLine(tmp,a[1]);
    I2C_Write(0);
    I2C_Write(0);
    // print third digit
    ssd1306_printDigitLine(tmp,a[2]);
    I2C_Stop();
  }

}

void ssd1306_printDigitLine(uint8_t line, uint8_t digit){
  uint8_t i,tmp1;
  uint16_t tmp;
  for(i=0;i<number_width;i++){
    tmp = (i+line*number_width)+digit*number_width*4;
    tmp1 = num_font_bitmap[tmp];
    I2C_Write(tmp1);
  }
}

// x,y - position (x:0-127,y:0-3)
//w,h - size (w:0-127,h:0-3)
void ssd1306_printBitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t code *bmp){
  uint8_t ih, iw;
  uint16_t addr=0;
  for(ih=0;ih<h;ih++){
    setCol(x); // set the position
    setRow(y);
    ssd1306_write_display_start();
    for(iw=0;iw<w;iw++){
      I2C_Write(bmp[addr]);
      addr++;
    }
    I2C_Stop();
    y++;
  }
}

// x,y - position (x:0-127,y:0-3)
//w,h - size (w:0-127,h:0-3)
void ssd1306_printBitmapClear(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
  uint8_t ih, iw;
  for(ih=0;ih<h;ih++){
    setCol(x); // set the position
    setRow(y);
    ssd1306_write_display_start();
    for(iw=0;iw<w;iw++){
      I2C_Write(0);
    }
    I2C_Stop();
    y++;
  }
}
