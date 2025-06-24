//MOUSE
/*
MOUSE CODE
Code by Andrew R. Sass written for MICROMOUSE: THE GREAT CHEESE HUNT". AUTHOR grants permission to
   use all or parts of this code if this section is included. Oct. 1, 2024
   
RED OFF, GREEN OFF-BEACON NOT DETECTED.
OPTION 1 RED FLASHING, GREEN ON -BEACON DETECTED. MAY BE TOO QUICK
       TO SEE IF MOUSE NOT USING IR LED IN PIN 11 TO QUENCH BEACON.
RED OFF, GREEN ON. BEACON EXTIGUISHED
OPTION 2 RED ON, GREEN OFF BEACON DETECTED. RED OFF, GREEN ON BEACON QUENCHED
*/

#define OPTION 1   //PREPROCESSOR SWITCH

#define USER_OUTPUT_1 13
#define USER_OUTPUT 12
#define BEACON 11
#define PCINT 10
#define BEACON_ON_LIGHT_RED 9
#define BEACON_OFF_LIGHT_GREEN  8
#define PULSE_WIDTH  1000// 40 kHz = 25 us period; 25 us X 1000 = 25 ms 

int n = 0;

bool LOCK = HIGH;
bool PULSE = LOW;

bool RISING_EDGE=1;

bool BEACON_OFF = LOW;//BEACON IS ON

bool MOUSE_1_STATE = LOW;  //"LOW" BEFORE BEACON DETECTED, "HIGH" AFTER BEACON DETECTED

short int MOUSE_2_STATE = 0;  // "0" BEFORE BEACON DETECTED, "1" DURING APPROACH TO BEACON, 
                              // "2" AFTER BEACON EXTINGUISHED
short int MOUSE_2_STATE_PREV = 0;


#if OPTION == 2

short int MOUSE_GOAL_2(void){
  bool temp = CHECK_BEACON();
  //Serial.print("MOUSE 2 STATE PREV = ");
  //Serial.println(MOUSE_2_STATE_PREV);
  if(temp == HIGH){
    Serial.println("MOUSE FORWARD");
    digitalWrite(BEACON_ON_LIGHT_RED,HIGH);
    MOUSE_2_STATE = 1;
    MOUSE_2_STATE_PREV = MOUSE_2_STATE;
    delay(1000);
    return MOUSE_2_STATE; 
  }
  if(temp == LOW && MOUSE_2_STATE_PREV == 0){
    Serial.println("MOUSE STOP");
    digitalWrite(BEACON_ON_LIGHT_RED,LOW);
    digitalWrite(BEACON_OFF_LIGHT_GREEN,LOW);
    MOUSE_2_STATE = 0;
    delay(1000);
    return MOUSE_2_STATE; 
  }
  if(temp == LOW && MOUSE_2_STATE_PREV == 1){
    Serial.println("MOUSE STOP 2");
    digitalWrite(BEACON_ON_LIGHT_RED,LOW);
    digitalWrite(BEACON_OFF_LIGHT_GREEN,HIGH);
    MOUSE_2_STATE = 2;
    delay(1000);
    return MOUSE_2_STATE; 
  }  
//  delay(1000); // necessary delay for synchronization with beacon
 // return MOUSE_2_STATE; 
}

#endif

#if OPTION == 1

bool MOUSE_GOAL_1(void){
 //Serial.println(CHECK_BEACON());
 //Serial.print("MOUSE_STATE = ");
 //Serial.println(MOUSE_STATE);
 
 if(CHECK_BEACON() == HIGH && BEACON_OFF == LOW){
    BEACON_OFF = HIGH;
  }
 if(BEACON_OFF == HIGH && MOUSE_1_STATE == LOW){
   //Serial.print("EEACON_OFF = ");
   //Serial.println(BEACON_OFF);
   digitalWrite(BEACON_OFF_LIGHT_GREEN,HIGH);
   MOUSE_1_STATE = HIGH; 
  }
  return MOUSE_1_STATE;  
}

#endif

bool CHECK_BEACON(void){
 digitalWrite(BEACON_ON_LIGHT_RED,LOW);

 if(LOCK == HIGH){
     return LOW;
    }

  
   //delay(2000);
  
 
 if(LOCK == LOW){ //
     digitalWrite(BEACON_ON_LIGHT_RED,HIGH);

  #if  OPTION == 1      
  /******************25 ms pulse, 40 kHz*****************************/   
 
     while(n <= PULSE_WIDTH && LOCK == LOW){   //generates approx 25 us square wave for 25 ms  
                                   //locks out irpt
       PULSE = HIGH;
       n = n + 1;
       PORTB |=  B00001000;            //pin 11 hi
       delayMicroseconds(12);
       PORTB &= B11110111;       //pin 11 lo
       delayMicroseconds(12);
     }//END WHILE
  
    
     n = n / PULSE_WIDTH;
     PULSE = LOW;
   /***************************end 25 ms pulse*****************/ 
   
   #endif
    LOCK = HIGH; 

  return HIGH;
 }//END IF LOCK 

}

ISR(PCINT0_vect){
  if((PINB & B00000100) && RISING_EDGE ){
    if(PULSE==LOW){
      LOCK = LOW;}
    RISING_EDGE = 0;
  } 

  if(!(PINB & B00000100) && !RISING_EDGE ){
    //LOCK = HIGH;    //LOCKED IN LOOP
    RISING_EDGE = 1;
  }
}

/*
void IR_triggerLatch(){     //irpt every other event
  if(PULSE == LOW){
    LOCK = LOW;
    ///LATCH = LATCH_PREVIOUS ^ HIGH;
    ///LATCH_PREVIOUS = LATCH;
  }
}
*/

void init_GPIO()
{
  pinMode(USER_OUTPUT_1,OUTPUT);//DEFINE ANY OUTPUT YOU WANT OR NONE AS LONG AS PIN IS AN OUTPUT
  pinMode(USER_OUTPUT,OUTPUT);//DEFINE ANY OUTPUT YOU WANT OR NONE AS LONG AS PIN IS AN OUTPUT
  pinMode(BEACON_ON_LIGHT_RED,OUTPUT);
  pinMode(BEACON_OFF_LIGHT_GREEN,OUTPUT);
  pinMode(BEACON,OUTPUT);
  pinMode(PCINT,INPUT);
  //DDRB = B00111011;  //DDRB = B00001000; 
} 

// the setup function runs once when you press reset or power the board
void setup() {
Serial.begin(9600);
init_GPIO();
PCICR |= B00000001;  //GRP 0
PCMSK0 |= B00000100;  //PIN D10
//attachInterrupt(digitalPinToInterrupt(IR_SENSOR),IR_triggerLatch,RISING);//IR SENSOR NEG PULSE
}

// the loop function runs over and over again forever
void loop(){

  #if OPTION == 1
  Serial.println(MOUSE_GOAL_1()); //"LOW" BEACON NOT DETECTED, "HIGH" BEACON EXTINGUISHED
  #endif

  #if OPTION == 2
  //MOUSE_GOAL_2();
  Serial.println(MOUSE_GOAL_2()); //"0" BEACON NOT DETECTED, "1" APPROACHING DETECTED BEACON
                                  //"2" BEACON EXTINGUISHED
  #endif
}//END LOOP

