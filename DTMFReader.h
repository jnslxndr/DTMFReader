/* 
  DTMFDecoder reading class for Arduino
  -------------------------------------
  Use to read Pins from the MD8870D decoder into something useful
  
  This work is copyleft!
  
  (cl) Copyleft 2012 jens alexander ewald, jens@lea.io.
  
  NO WARRANTY at all!
   -----
*/
#ifndef ____DTMFREADER_H
#define ____DTMFREADER_H

#include <stdlib.h>
#include <arduino.h>
#include <avr/pgmspace.h>

PROGMEM  prog_uchar ____dtmf_keys[]  = { 
  'D','1','2','3','4','5','6','7','8','9','0','*','#','A','B','C'
};

class DTMFReader {
  
public:
  inline DTMFReader(int _detect_pin, int pin1, int pin2, int pin3, int pin4) {
    detect_pin = _detect_pin;
    value_pins[0] = pin1;
    value_pins[1] = pin2;
    value_pins[2] = pin3;
    value_pins[3] = pin4;
    
    DTMF_USE_IRQ = 0; // Set this 0 for now!
    reset();
  }
  
  inline void reset() {
    detect_state = LOW;
    cleanUp();
  };
  
  inline ~DTMFReader(){};
  
  inline int available() {
    return detect_state == HIGH;
  }
  
  inline void update()
  {
    detectChange();
    if      (wentUp()  ) onDetectUp(getChar());
    else if (wentDown()) onDetectDown(getChar());
    cleanUp();
  };
  
  inline void onKeyUp   (int (*_onKeyUp)(char key)   ) {onDetectUp   = _onKeyUp;   };
  inline void onKeyDown (int (*_onKeyDown)(char key) ) {onDetectDown = _onKeyDown; };
  
private:
  
  static const char NO_DTMF_AVAIL = 0;
  static const char DTMF_AVAIL = 1;
  
  int (*onDetectUp)(char key);
  int (*onDetectDown)(char key);

  // int (*onDetectChange)(char key);
  
  int detect_pin;
  int value_pins[4];
  
  int detect_state;
  int last_state;
  
  // Experimental Support! Not yet implemented
  int DTMF_USE_IRQ;
  
  inline void getState(){
    detect_state = digitalRead(detect_pin);
  }
  
  inline void detectChange(){
    getState();
  }
  
  inline int wentUp(){
    return last_state < detect_state;
  }
  
  inline int wentDown(){
    return last_state > detect_state;
  }
  
  inline void cleanUp(){
    last_state = detect_state;
  }
  
  char getChar(){
    char _current = which_key(read_state());
    return _current;
  }
  
  int read_state(){
    int _state = 0;
    int i=5;
    while(i--){
      _state |= digitalRead(value_pins[i]) << i;
    }
    return _state;
  }
  
  // pick the char from the progmem
  char which_key(int index) {
    if (index > 15 && index < 0) return '\0'; // return \0 if no index found!
    return pgm_read_byte_near(____dtmf_keys+index);
  }
  
};

#endif /* end of include guard: ____DTMFREADER_H */
