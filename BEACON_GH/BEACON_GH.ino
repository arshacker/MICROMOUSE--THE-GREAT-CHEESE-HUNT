//BEACON CODE

/*
  40 kHz output modulated by 25 ms wide pulse. pulse duty cycle, 525 ms
  DIAGRAM CORRECT AFTER COPYING INTO ARDUINO IDE
           <------------525 ms----------->
           25 ms
           *****                          *****
           *   *                          *   *
        ****   ************| |*************   *****       
        *   
   Signal can be extinguished by interrupts from a proximity sensor or an ir sensor.
   
   Code by Andrew R. Sass writted for MICROMOUSE: THE GREAT CHEESE HUNT". AUTHOR grants permission to use
   code if this section is included. Oct. 1, 2024
        
*/

#define BEACON 11 
#define PROX_SENSOR 3
#define IR_SENSOR 2
#define BEACON_ON_LIGHT_RED 7
#define BEACON_OFF_LIGHT_GREEN 5
#define PULSE_WIDTH  1000// 40 kHz = 25 us period; 25 us X 1000 = 25 ms 


unsigned int n = 0;


bool LATCH = LOW;
bool PULSE = LOW;



void triggerLatch_LockOn(){
  LATCH = HIGH; 
}

void IR_triggerLatch_LockOn(){
  if(PULSE == LOW){
    LATCH = HIGH;
  } 
}

void init_GPIO()
{
  pinMode(BEACON,OUTPUT);
  DDRB = B00001000; 
  pinMode(BEACON_ON_LIGHT_RED,OUTPUT);
  pinMode(BEACON_OFF_LIGHT_GREEN,OUTPUT);
  pinMode(PROX_SENSOR,INPUT); //INPUT is default but this is a reminder

}


// the setup function runs once when you press reset or power the board
void setup() {
Serial.begin(9600);
init_GPIO();
attachInterrupt(digitalPinToInterrupt(PROX_SENSOR),triggerLatch_LockOn,RISING);//PROX SENSOR POS PULSE
attachInterrupt(digitalPinToInterrupt(IR_SENSOR),IR_triggerLatch_LockOn,FALLING);//IR SENSOR NEG PULSE
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("BEACON OPERATIMG");
  digitalWrite(BEACON_ON_LIGHT_RED,HIGH);
  digitalWrite(BEACON_OFF_LIGHT_GREEN,LOW);
  while(LATCH == LOW){
    while(n <= PULSE_WIDTH && LATCH == LOW){   //generates approx 25 us square wave
      PULSE = HIGH;  
      n = n + 1;
      PORTB |=  B00001000;            //pin 11 hi
      delayMicroseconds(12);
      PORTB &= B11110111;       //pin 11 lo
      delayMicroseconds(12);
    }//END 2ND WHILE
    PULSE = LOW;
    n = n / PULSE_WIDTH;
    delay(500);
  }//END 1ST WHILE
  digitalWrite(BEACON_ON_LIGHT_RED,LOW);
  digitalWrite(BEACON_OFF_LIGHT_GREEN,HIGH);  
}//END LOOP


