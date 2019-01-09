extern "C" {
  // AVR LibC Includes
  #include <inttypes.h>
  #include <avr/interrupt.h>
}
#include <Arduino.h>
#include <RGB.h>
ICSensor* sensor[6];
  byte preva=0;
  unsigned long timeLimit;
  byte byteActiveColorSensor;
  byte tmp;
  byte totalColor;
  byte byteType ;
  boolean allDone;
  unsigned long color_loop_breaker;
  unsigned long color_loop_checker ;
  byte* result;
  byte Cal_RGB=0; 
byte*  get_color (byte a)//
{     

      if(!Cal_RGB){
    sensor[1]  = new ColorSensor(DIGITAL_PIN_1);
    sensor[2]  = new ColorSensor(DIGITAL_PIN_2);
    sensor[3]  = new ColorSensor(DIGITAL_PIN_3);
    sensor[4]  = new ColorSensor(DIGITAL_PIN_4);
    sensor[5]  = new ColorSensor(DIGITAL_PIN_5);
    Cal_RGB=1;
  }
  
  if(a==DIGITAL_PIN_1)
  a=1;
  else if(a==DIGITAL_PIN_2)
  a=2;
  else if(a==DIGITAL_PIN_3)
  a=3;
  else if(a==DIGITAL_PIN_4)
  a=4;
  else if(a==DIGITAL_PIN_5)
  a=5;
  else
  return 0;
  byteActiveColorSensor = 0;
   tmp = 0;
  totalColor = 0;
  byteType = sensor[a] -> getType();
    if (byteType == SENSOR_TYPE_COLOR) {
      if (totalColor == byteActiveColorSensor) {
        sensor[a] -> reset();
      }
      totalColor++;
    }
  tmp = 0;
  timeLimit = micros();
  
  while (totalColor > 0) {
    //Serial.print("F");
      allDone = true;
      byteType = sensor[a] -> getType();
      if (byteType == SENSOR_TYPE_COLOR) {
        if (tmp < byteActiveColorSensor) {
          tmp++;
          continue;
        }
         color_loop_breaker = micros();
         color_loop_checker = micros();
        
        while (!sensor[a] -> isReady()) {
          sensor[a] -> iteration(0, totalColor);
          color_loop_checker = micros();
          if (( color_loop_checker - color_loop_breaker) > 1000 * 10){
            //Serial.println("FUCK_BREAK");
            if(result[0]!=0&&result[1]!=0)
             break;
          }
        }
        break;
      }
  if (allDone || abs(micros() - timeLimit) > sensor[a]->INTERVAL_BYTE * 10) break;
  }
  byteActiveColorSensor++;
  if (byteActiveColorSensor == totalColor) {
    byteActiveColorSensor = 0;
  } 
    result = sensor[a] -> getResult();
    result[0]=result[1];
    result[1]=result[2];
    result[2]=result[3];
    return(result);
}
