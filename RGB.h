#include <inttypes.h>

#define portOfPin(P)\
  (((P)>=0&&(P)<8)?&PORTD:(((P)>7&&(P)<14)?&PORTB:&PORTC))
#define ddrOfPin(P)\
  (((P)>=0&&(P)<8)?&DDRD:(((P)>7&&(P)<14)?&DDRB:&DDRC))
#define pinOfPin(P)\
  (((P)>=0&&(P)<8)?&PIND:(((P)>7&&(P)<14)?&PINB:&PINC))
#define pinIndex(P)((uint8_t)(P>13?P-14:P&7))
#define pinMask(P)((uint8_t)(1<<pinIndex(P)))
#define pinAsInput(P) *(ddrOfPin(P))&=~pinMask(P)
#define pinAsInputPullUp(P) *(ddrOfPin(P))&=~pinMask(P);digitalHigh(P)
#define pinAsOutput(P) *(ddrOfPin(P))|=pinMask(P)
#define digitalLow(P) *(portOfPin(P))&=~pinMask(P)
#define digitalHigh(P) *(portOfPin(P))|=pinMask(P)
#define isHigh(P)((*(pinOfPin(P))& pinMask(P))>0)
#define isLow(P)((*(pinOfPin(P))& pinMask(P))==0)
#define digitalState(P)((uint8_t)isHigh(P))
#define SENSOR_COUNT 5
#define SENSOR_RESPONSE_LENGTH 4
#define SENSOR_TYPE_COLOR 7
#define DIGITAL_PIN_1 4
#define DIGITAL_PIN_2 7
#define DIGITAL_PIN_3 8
#define DIGITAL_PIN_4 11
#define DIGITAL_PIN_5 12

class ICSensor {
  public:
    int INTERVAL_BIT=115;
    int INTERVAL_BYTE = INTERVAL_BIT*8;
    virtual byte getType();
    virtual void reset();
    virtual void iteration(byte data, byte data2);
    virtual boolean isReady();
    virtual byte* getResult();
   // virtual void debugSetValue(byte value);
};
class ColorSensor: public ICSensor {

    int pin;
    boolean booleanReady;
    unsigned long firstCAL =0;
    unsigned long secondCAL=0;
    unsigned long timeLowStart = 0;
    unsigned long timeLowEnd = 0;
    unsigned long timeHighStart = 0;
    unsigned long timeHighEnd = 0;
    byte  flag=0;
    unsigned long timeSync = 0;
    int INTERVAL_BIT_ALL=0;
    int INTERVAL_BIT_NEW=0;
    byte TRIES=0;

    byte byteCounter = 0;
    byte bitCounter = 0;
    byte result[3] = {0, 0, 0};
    byte _result[3] = {0, 0, 0};
    byte iSynchronized = 0;
  public:
    ColorSensor(int pin) {
      this -> pin = pin;
      pinAsInput(pin);
    };
    byte getType() {
      return SENSOR_TYPE_COLOR;
    }
    void reset() {
      booleanReady = false;
      timeLowStart = 0;
      timeLowEnd = 0;
      timeHighStart = 0;
      timeHighEnd = 0;
      timeSync = 0;
      byteCounter = 0;
      bitCounter = 0;
      //      result[0] = 0;
      //      result[1] = 0;
      //      result[2] = 0;
      _result[0] = 0;
      _result[1] = 0;
      _result[2] = 0;
      iSynchronized = 0;
      firstCAL=0;

      if(TRIES<25&&flag){
             TRIES++;
             if(TRIES>5){
      INTERVAL_BIT_ALL+=INTERVAL_BIT_NEW;
      INTERVAL_BIT=round(INTERVAL_BIT_ALL/(TRIES-5));
      INTERVAL_BYTE=INTERVAL_BIT*8;
    //  Serial.println(INTERVAL_BIT_ALL);
    //       Serial.println(TRIES); 
     // Serial.println(INTERVAL_BIT);
             }
      }
      else
      flag=1;
    }

    void iteration(byte data, byte data2) {
      switch (iSynchronized) {
        case 0: {
            if (digitalState(pin) == HIGH) {
              iSynchronized = 1;
              timeLowStart = micros();
            }
            else {
              iSynchronized = 0;
            }
            break;
          }
        case 1: {
            if (digitalState(pin) == LOW) {
              timeLowEnd = micros();
              if (timeLowEnd - timeLowStart >= (110)*14) {
                iSynchronized = 2;
              }
            }
            else {
              iSynchronized = 0;
            }
            break;
          }
        case 2: {
               if (digitalState(pin) == HIGH) {
      //                Serial.println(timeLowEnd - timeLowStart);
       timeHighStart = micros();
              iSynchronized = 3;
            }
            else {
              timeLowEnd = micros();
            }
            break;
          }
        case 3: {
          INTERVAL_BIT_NEW=round((timeLowEnd-timeLowStart)/14);//mb +114*2/16
          timeSync=timeLowEnd+INTERVAL_BIT+20;
   //       Serial.println(INTERVAL_BIT);
          iSynchronized = 4;
          }
        case 4: {
            if (timeSync <= micros()) {
              timeSync += INTERVAL_BIT;
              if (digitalState(pin) == HIGH) {
                _result[byteCounter] |= (1 << bitCounter);
              }
              else {
                _result[byteCounter] &= ~(1 << bitCounter);
              }
              bitCounter++;
              if (bitCounter > 7) {
                bitCounter = 0;
                byteCounter++;
                if (byteCounter > 2) {
                  //We done
                  byteCounter = 0;
                     if(abs(result[0] - _result[0]) >= 32 && abs(result[1] - _result[1]) < 5 && abs(result[2] - _result[2]) < 5
                          || abs(result[0] - _result[0]) < 5 && abs(result[1] - _result[1]) >= 32 && abs(result[2] - _result[2]) < 5
                          || abs(result[0] - _result[0]) < 5 && abs(result[1] - _result[1]) < 5 && abs(result[2] - _result[2]) >= 32){

                          }
                          else
                          {
                           
                  result[0] = _result[0];
                  result[1] = _result[1];
                  result[2] = _result[2];
                  
                  }
              iSynchronized =0;
               booleanReady = true;
                }
              }
            }
            break;
          }
      }
    }
    boolean isReady() {
      return booleanReady;
    }
    byte dwa[4];
    byte* getResult() {
      dwa[0]=0;
      dwa[1]= result[0];
      dwa[2]= result[1];
      dwa[3]=result[2];
      return dwa ;
    }
};

////////////////////////////////1111/////222222////////////////////////////////////////////
byte* get_color(byte a);
byte* get_color_by_sensor(ICSensor** sensors, byte a);
