#include "bat.h"

int16_t getBatVoltageMv(void){
  int32_t sum = 0;
  int16_t counter, adcval;

  ADC0MX = ADC0MX_ADC0MX__ADC0P3; // voltage

  for(counter=0;counter<500;counter++){
      ADC0CN0_ADINT = 0;  // reset ADC complete interrupt
      ADC0CN0_ADBUSY = 1; // start ADC conversions
      while(ADC0CN0_ADINT==0); // wait for adc to complete
      adcval = ADC0;
      sum+=adcval;
  }
  adcval = sum/500;
  return (int32_t)adcval*4334/4092; //ADC*Vmax/ADCmax value in mV
}
