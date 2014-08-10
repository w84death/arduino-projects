/*  
    P1X by Krzysztof Jankowski
    Advanced menu & memory bank

    abstract: One-button menu and memory bank read/write to EEPROM
    created: 09-08-2014
    license: do what you want and dont bother me

    webpage: http://p1x.in
    twitter: @w84death
*/

#include <EEPROM.h>

#define LED_A 4
#define LED_B 1
#define LED_C 3
#define SWITCH 2
#define BUZZ 0

byte memorySize = 3; // memory bank size
byte number = 0;
long switchTime = 0;
int actionTime = 1500; // time for toggling menu
int menu = 6; // off menu
unsigned long currentTime;
boolean writing = false;

void setup(){
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(SWITCH, INPUT);
  // beep on start
  play(false);
}

void loop(){
  // set current time
  currentTime = millis();
  
  // button pressed
  if(digitalRead(SWITCH)){  
    // play beep
    play(false);    
    if(!writing){
      // toggle menu
      toggleMenu();    
    }else{
      // toggle number to write
      incNumber();
    }
    // set new time for button
    switchTime = currentTime;
    delay(150);
  }
  
  // if out of menu, clear screen after a moment
  if(menu == -1 && currentTime - switchTime > actionTime*2){
    turnOffLights();
    // play beep
    play(false);
    // and go to off mode
    menu = 6;
  }
  
  // if in read-menu
  if(menu < 3 && menu > -1){
    // if time for button has passed
    if(currentTime - switchTime > actionTime){       
        // show the number from memory
        lightBinary(EEPROM.read(menu));
        // play rattle sound
        play(true);
        // go to out of menu
        menu = -1;     
    }else{
      // if there is button time show the menu position
      lightMenu();
    }
  }
  
  // write menu
  if(menu > 2 && menu < 6){
    // if time for button has passed and we are in menu
    if(currentTime - switchTime > actionTime && !writing){
      // read the number of this bank
      number = EEPROM.read(menu-3);
      // show        
      lightBinary(number);
      // set writing mode
      writing = true;
      switchTime = currentTime;
      // play rattle sound
      play(true);
    }else
    // if time for button has passed and we are in write menu
    if(currentTime - switchTime > actionTime && writing){
      // write the number
      EEPROM.write(menu-3, number);
      // disable writing mode
      writing = false;
      // turn off
      menu = 6;
      // play rattle sound
      play(true);
    }
    else
    if(!writing){
      // if not in writing show the menu position
      lightMenu();
    }
  }
    
  // turn off menu
  if(menu == 6){
    turnOffLights();
  }

}

// toggle throu menu positions
void toggleMenu(){
  menu++;
  if(menu>6){
    menu = 0;
  }
}

// toggle throu numbers
void incNumber(){
  number++;
  if(number>7){
    number = 0;
  }
  lightBinary(number);
}

// turn off all LEDs
void turnOffLights(){
  digitalWrite(LED_A,0);
  digitalWrite(LED_B,0);
  digitalWrite(LED_C,0);
}

// light the LEDs for each menu
void lightMenu(){ 
  turnOffLights();
  
  switch (menu) {
    // read menu  - just light each LED
    case 0:
      digitalWrite(LED_A,1);   
      break;
      
    case 1:      
      digitalWrite(LED_B,1);      
      break;
      
    case 2:
      digitalWrite(LED_C,1);
      break;
    
    // write menu - blink each LED
    case 3:
      if(currentTime % 2 == 0){
        digitalWrite(LED_A,1);
      }else{
        digitalWrite(LED_A,0);
      }
      break;
      
    case 4:
      if(currentTime % 2 == 0){
        digitalWrite(LED_B,1);
      }else{
        digitalWrite(LED_B,0);
      }
      break;
      
    case 5:
      if(currentTime % 2 == 0){
        digitalWrite(LED_C,1);
      }else{
        digitalWrite(LED_C,0);
      }
      break; 
      
    // off menu - turn LEDs off
    case 6:
      turnOffLights();
    break;
  }
}

// light LEDs in binary
void lightBinary(byte val){
  digitalWrite(LED_A,bitRead(val, 0));
  digitalWrite(LED_B,bitRead(val, 1));
  digitalWrite(LED_C,bitRead(val, 2));
}

// play sound
void play(boolean mode){
  if(!mode){
    for(byte i=0; i<4; i++){
        analogWrite(BUZZ, 128);
        delay(2);
        analogWrite(BUZZ, 0);
      }
  }else{
    for(byte j=0; j<8; j++){
      for(byte i=0; i<4; i++){
        analogWrite(BUZZ, 64 + (i*32));
        delay(2);
        analogWrite(BUZZ, 0);
      }
      delay(24);
    }
  }
  analogWrite(BUZZ, 0);
}
