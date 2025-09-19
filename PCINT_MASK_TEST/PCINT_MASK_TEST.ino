/*
MOUSE CODE
Code by Andrew R. Sass writted for MICROMOUSE: THE GREAT CHEESE HUNT". AUTHOR grants permission to
   use all or parts of this code if this section is included. Oct. 1, 2024
   
RED OFF, GREEN OFF-BEACON NOT DETECTED.
RED FLASHING, GREEN ON -BEACON DETECTED. MAY BE TOO QUICK
       TO SEE IF MOUSE NOT USING IR LED IN PIN 11 TO QUENCH BEACON.
RED OFF, GREEN ON. BEACON EXTIGUISHED
*/

#define OUT 12
#define BEACON 11
#define PCINT 10
#define PCINT_2 5
#define PCINT_1 A2
#define SAMPLE_INPUT A4 //7 //9
#define HWINT_2 2
#define BEACON_ON_LIGHT_RED 7
#define BEACON_OFF_LIGHT_GREEN  5
#define PULSE_WIDTH  1000// 40 kHz = 25 us period; 25 us X 1000 = 25 ms 

int n = 0;

bool LOCK = HIGH;
bool LOCK1 = HIGH;
bool LOCK2 = HIGH;
bool HWLOCK = HIGH;

bool RISING_EDGE=1;

bool BEACON_OFF = LOW;//BEACON IS ON

void HW_triggerLatch(){     //irpt every other event
    HWLOCK = LOW;
}


/*
ISR(PCINT0_vect){
  if(PULSE == LOW){
    LOCK = LOW;
  }    
}
*/
/*
ISR(PCINT0_vect){
  if(digitalRead(IRPT_2)==1 && RISING_EDGE == 1 ){
    LOCK = LOW;
    RISING_EDGE = 0;
  } 

  if(digitalRead(IRPT_2)==0 && RISING_EDGE == 0 ){
    LOCK = HIGH;
    RISING_EDGE = 1;
  }
}
*/

ISR(PCINT0_vect){
  if((PINB & B00000100) && RISING_EDGE ){
    LOCK = LOW;
    RISING_EDGE = 0;
  } 

  if(!(PINB & B00000100) && !RISING_EDGE ){
    //LOCK = HIGH;    //LOCKED IN LOOP
    RISING_EDGE = 1;
  }
}


ISR(PCINT1_vect){
  if((PINC & B00000100) && RISING_EDGE ){
    LOCK1 = LOW;
    RISING_EDGE = 0;
  } 

  if(!(PINC & B00000100) && !RISING_EDGE ){
    //LOCK = HIGH;    //LOCKED IN LOOP
    RISING_EDGE = 1;
  }  
}

ISR(PCINT2_vect){
  if((PIND & B00100000) && RISING_EDGE ){
    LOCK2 = LOW;
    RISING_EDGE = 0;
  } 

  if(!(PIND & B00100000) && !RISING_EDGE ){
    //LOCK = HIGH;    //LOCKED IN LOOP
    RISING_EDGE = 1;
  }  
}

void init_GPIO()
{
  pinMode(BEACON_ON_LIGHT_RED,OUTPUT);
  pinMode(BEACON_OFF_LIGHT_GREEN,OUTPUT);
  pinMode(BEACON,OUTPUT);
  pinMode(HWINT_2,INPUT);
  pinMode(SAMPLE_INPUT,INPUT);
  pinMode(PCINT,INPUT);  //DDRB = B00111011;//DDRB = B00001000; 
  pinMode(PCINT_2,INPUT);
  pinMode(PCINT_1,INPUT);
} 


// the setup function runs once when you press reset or power the board
void setup() {
Serial.begin(9600);
init_GPIO();
attachInterrupt(digitalPinToInterrupt(HWINT_2),HW_triggerLatch,RISING);//IR SENSOR NEG PULSE

PCICR |= B00000111;  //GRP 0,1,2
PCMSK0 |= B00000100;  //PIN D10
PCMSK2 |= B00100000;  //PIN 5
PCMSK1 |= B00000100;  //pin A2
}

// the loop function runs over and over again forever
void loop(){
  /*
  if(LOCK==HIGH){
    Serial.println("RUNNING");
    delay(1000);
  }
 */

  /*
  Serial.print("digitalRead(IRPT_2)  =  ");
  Serial.println(digitalRead(IRPT_2));
  */

  bool b = digitalRead(SAMPLE_INPUT);
  digitalWrite(OUT,b);
  
  if(LOCK==LOW){
    Serial.println("IRPT PCINT");
    LOCK=HIGH;
  }

  if(LOCK1==LOW){
    Serial.println("IRPT PCINT1");
    LOCK1=HIGH;
  }

  if(LOCK2==LOW){
    Serial.println("IRPT PCINT2");
    LOCK2=HIGH;
  }

  
  
  if(HWLOCK==LOW){
    Serial.println("IRPT HWINT");
    HWLOCK=HIGH;
  }  
 
  

}//END LOOP
