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

volatile unsigned long pulseStartTime;
volatile unsigned long pulseTime = 0;

volatile unsigned long pulseStartTimePS;
volatile unsigned long pulseTimePS = 0;


bool LATCH = LOW;
bool PULSE = LOW;



void triggerLatch_Rising(){
  pulseStartTimePS = millis();
  //DO NOT SWITCH LATCH BACK SINCE BEACON ONLY TURNED OFF ONCE!!!
  attachInterrupt(digitalPinToInterrupt(PROX_SENSOR),triggerLatch_Falling,FALLING); 
} 

void triggerLatch_Falling(){  //IN THIS VERSION THE BEACON IS TURNED OFF AT FALLING NOT RISING
  pulseTimePS = millis() - pulseStartTimePS;
  if(pulseTimePS > 1000 && PULSE == LOW){ 
    LATCH = HIGH;}
  attachInterrupt(digitalPinToInterrupt(PROX_SENSOR),triggerLatch_Rising,RISING);      
}

void IR_triggerLatch_Rising(){ //IN THIS VERSION THE BEACON IS TURNED OFF AT RISING NOT FALLING
  pulseTime = millis() - pulseStartTime;
  if(pulseTime > 23 && pulseTime < 27 && PULSE == LOW){ 
    LATCH = HIGH;}
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR),IR_triggerLatch_Falling,FALLING);     
}

void IR_triggerLatch_Falling(){
  pulseStartTime = millis();
  //DO NOT SWITCH LATCH BACK TO LOW SINCE BEACON ONLY TURNED OFF ONCE!!!
  attachInterrupt(digitalPinToInterrupt(IR_SENSOR),IR_triggerLatch_Rising,RISING);
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
attachInterrupt(digitalPinToInterrupt(PROX_SENSOR),triggerLatch_Rising,RISING);//PROX SENSOR POS PULSE
attachInterrupt(digitalPinToInterrupt(IR_SENSOR),IR_triggerLatch_Falling,FALLING);//IR SENSOR NEG PULSE
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("BEACON OPERATIMG");
  //digitalWrite(BEACON_ON_LIGHT_RED,HIGH);
  //digitalWrite(BEACON_OFF_LIGHT_GREEN,LOW);
  Serial.println("PULSE WIDTH  = ");
  Serial.println(pulseTime);
  Serial.println("PULSE WIDTH PS  = ");
  Serial.println(pulseTimePS);    
  while(LATCH == LOW){
    digitalWrite(BEACON_ON_LIGHT_RED,HIGH);
    digitalWrite(BEACON_OFF_LIGHT_GREEN,LOW);
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
    delay(50);
  }//END 1ST WHILE
  digitalWrite(BEACON_ON_LIGHT_RED,LOW);
  digitalWrite(BEACON_OFF_LIGHT_GREEN,HIGH);  
}//END LOOP

