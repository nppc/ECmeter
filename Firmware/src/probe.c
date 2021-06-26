#include "probe.h"
#include "gen.h"

int16_t GetProbeADC(void){
  int16_t counter, adcval;
  int32_t sum1, sum2;
  ADC0MX = ADC0MX_ADC0MX__ADC0P7; // probe
  sum1=0;
  sum2=0;
  adcval=0;
  pinsPushPull();
  PIN_1 = 1;
  PIN_2 = 0;
  ADC0CN0_ADINT = 0;  // reset ADC complete interrupt
  ADC0CN0_ADBUSY = 1; // start ADC conversions
  for(counter=0;counter<500;counter++){
      while(ADC0CN0_ADINT==0); // wait for adc to complete
      adcval = ADC0;
      PIN_1 = !PIN_1;
      PIN_2 = !PIN_2;
      ADC0CN0_ADINT = 0;  // reset ADC complete interrupt
      ADC0CN0_ADBUSY = 1; // start ADC conversions
      sum1+=adcval;
      while(ADC0CN0_ADINT==0); // wait for adc to complete
      adcval = ADC0;
      PIN_1 = !PIN_1;
      PIN_2 = !PIN_2;
      ADC0CN0_ADINT = 0;  // reset ADC complete interrupt
      ADC0CN0_ADBUSY = 1; // start ADC conversions
      sum2+=adcval;
  }
  pinsHighZ();
  sum1=sum1/500;
  sum2=4095-(sum2/500);
#ifdef DEBUGUART
      sendUART(sum1,0);
      prnUART(" ",0);
      sendUART(sum2,0);
      prnUART(" ",0);
      sendUART(sum1-(sum1-sum2)/2,1);
#endif
  return sum1-(sum1-sum2)/2;
}

int16_t convert2EC(int16_t adcval){
  uint8_t i;
  // check value for the range
  if(adcval>=calib_data[0].ADCval && adcval<4000) return 0; // distilled water
  if(adcval>=4000) return adcval; // probe in air
  // find value in a table
  for(i=0;i<(CALIBRATIONVALUES-1);i++){
      if(adcval>calib_data[i+1].ADCval) break;
  }
  // calculate the value from found range
  // 100-(adcval-[lowerboundinrange])*256/divconst
  if(i==(CALIBRATIONVALUES-1)){
      // EC greather than 5
      return (CALIBRATIONVALUES-1)*100+(calib_data[CALIBRATIONVALUES-1].ADCval-adcval);
  }else{
      // EC is in a range
      return i*100+(100-div32round((uint32_t)(adcval-calib_data[i+1].ADCval) * 256,calib_data[i].divconst));
  }
}
