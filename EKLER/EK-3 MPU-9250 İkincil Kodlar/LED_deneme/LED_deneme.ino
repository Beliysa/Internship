#include <Tone32.h>


#define RED1 25
#define RED2 32
#define GREEN 33
#define BUZZER 27

void setup() {
 pinMode(RED1, OUTPUT);   //RED
 pinMode(RED2, OUTPUT);   //RED2
 pinMode(GREEN, OUTPUT);  //GREEN
 pinMode(BUZZER,OUTPUT);   //Buzzer
}

void loop() {
  
  digitalWrite(RED1, HIGH); 
  digitalWrite(RED2, HIGH);
  digitalWrite(GREEN, HIGH); 
  digitalWrite(BUZZER, LOW);
  
  
  
  delay(500);            // waits for a second
  
  
  digitalWrite(RED1,LOW); 
  digitalWrite(RED2,LOW);
  digitalWrite(GREEN,LOW); 
  digitalWrite(BUZZER,HIGH);
  
  
  delay(500);            // waits for a second
  
}
