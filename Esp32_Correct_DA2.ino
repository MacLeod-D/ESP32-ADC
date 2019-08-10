// (C) 2019 Helmut Weber
//
// Build a LookUpTable for ESP32-ADC to get corrected values


// Uncomment to get Text for including in programm:
//#define GRAPH


#include <driver/dac.h>

float Results[4096];
float Res2[4095*5];

void Dotest(int j) {
unsigned int a1;
  //Serial.print("DoTest "); Serial.println(j);
  for (int i=0;i<256;i++) {  
      dac_output_voltage(DAC_CHANNEL_1, (i) & 0xff);
      delayMicroseconds(100);
      adcStart(35);
      delayMicroseconds(100);
      a1 = adcEnd(35);
      Results[i*16]=0.9*Results[i*16] + 0.1*a1;
      //Results[i*16]=a1;
  }
}

void setup() {
    dac_output_enable(DAC_CHANNEL_1);  // pin 25
    dac_output_voltage(DAC_CHANNEL_1, 0);
    adcAttachPin(35);
    adcStart(35);

    Serial.begin(921600);

   
}



void loop() {
float r;
int x, delta, deltaMin, deltaMinIndex;
    Serial.println("Test Liniarity ...");
    for (int i=0; i<500; i++) {
        if ((i%100)==0) Serial.println(i);
        Dotest(i);
    } 
    for (int i=0; i<256; i++) {
       r=Results[i*16]+0.5;
       x=(int)r;
       Serial.print(i*16); Serial.print(" "); Serial.println(Results[i*16]); 
    } 
    
    Serial.println("Calculate interpolated values ..");

    for (int i=0; i<256; i++) {
       for (int j=1; j<16; j++) {
          Results[i*16+j] = Results[i*16] + (Results[(i+1)*16] - Results[(i)*16])*(float)j / (float)16.0;
       }
    }

    for (int i=0; i<4096; i++) {
        Serial.print(i); Serial.print(" "); Serial.println(Results[i]);
    }

    for (int i=0; i<4096; i++) {
        Results[i]=0.5 + Results[i];
    } 

    for (int i=0; i<4096; i++) {
        Serial.print(i); Serial.print(" "); Serial.println(Results[i],6);
    }

    for (int i=0; i<4096; i++) {
       for (int j=0; j<5; j++) {
          Res2[i*5+j] = Results[i] + (Results[(i+1)] - Results[i])*(float)j / (float)10.0;
       }
    } 

    for (int i=0; i<(5*4096); i++) {
        Serial.print(i); Serial.print(" "); Serial.println(Res2[i],6);
    }

    for (int i=1; i<4096; i++) {
        float diff, minDiff;
        int index;
        minDiff=99999;
        for (int j=0; j<(5*4096); j++) {
            diff=abs((float)(i) - Res2[j]);
            if(diff<minDiff) {
                minDiff=diff;
                index=j;
            }
        }

        Results[i]=index;
    }

    for (int i=0; i<(4096); i++) {
        Serial.print(i); Serial.print(" "); Serial.println(Results[i]/5,6);
        Results[i]/=5;
    }

#ifdef GRAPH   
    
    while(1) {
      int a1;
      for (int i=2; i<256;i++) {
        dac_output_voltage(DAC_CHANNEL_1, (i) & 0xff);
        delayMicroseconds(100);
        adcStart(35);
        delayMicroseconds(100);
        a1 = adcEnd(35);
        float r= Results[a1];
        Serial.print(i*16); Serial.print(" "); Serial.println(r);
      }
    }
    
#else
    for (int i=0; i<10; i++) Serial.println();
    Serial.println(" float ADC_LUT[4096] = { 0,");
    for (int i=1; i<4095; i++) {
      Serial.print(Results[i],4); Serial.print(",");
      if ((i%16)==0) Serial.println();
    }
    Serial.println(Results[4095]); Serial.println("} ;");


    for (int i=0; i<10; i++) Serial.println();
    Serial.println(" int ADC_LUT[4096] = { 0,");
    for (int i=1; i<4095; i++) {
      Serial.print((int)Results[i]); Serial.print(",");
      if ((i%16)==0) Serial.println();
    }
    Serial.println((int)Results[4095]); Serial.println("} ;");


    while(1);

#endif
    


}
