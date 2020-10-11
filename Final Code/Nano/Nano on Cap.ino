#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include "SoftwareSerial.h"

#define outputA 2
#define outputB 3
#define Servopin 6
#define potpin 7

SoftwareSerial BTserial(5, 4); 
RF24 radio(9, 10); // CE, CSN


Servo myservo;
const byte address[][6] = {"00001", "00002"};
boolean button_state = 0;
int button_last = 1;
int button_1_last = 1;
int button = 0;
int button_1 = 0;
int button_A = 0;
int button_B = 0;
int state = 0;
int count = 0;
int count1 = 0;
int count2 = 0;
int count3 = 0;
int count4 = 0;
int counter = 0; 
int val; 
int servo_angle = 90;
int flag = 0;
char c = ' ';
unsigned long previousMillis = 0;
 
void setup() {
  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);
  Serial.begin(9600);
  radio.begin();
  //Setting the address at which we will receive the data
  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);
  radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
  BTserial.begin(38400);  
  Serial.println("BTserial started at 38400");
  myservo.attach(Servopin);
  delay(1000);
}
void loop()
{ 
  delay(5); 
  radio.startListening();
  if (radio.available())              //Looking for the data from two buttons
  {
    serialcommunication();
    char text[2] = "";                 //Saving the incoming data
    radio.read(&text, sizeof(text));    //Reading the data
    radio.read(&button_state, sizeof(button_state));    //Reading the data
    //Serial.println(text);
    if(text[0] == '1')
    {
      //Serial.println(text);
      button = 1;
    }
    else if(text[0] == '0')
    {
      //Serial.println(text);
      button = 0;
    }
    if(text[1] == '1')
    {
      //Serial.println(text);
      button_1 = 1;
    }
    else if(text[1] == '0')
    {
      //Serial.println(text);
      button_1 = 0;
    }  
  }
  if (button != button_last)
  {
    button_A = 1;
    count++;
  }
  else
  {
    button_A = 0;
  }
  if (button_1 != button_1_last)
  {
    myservo.write(0);
    Serial.println("State Change");
    count1 ++;
    button_B = 1;
    state = 0;
    delay(1000);
  }
  else
  {
    button_B = 0;
  }
  //Serial.println("button_A, button_B: ");
  //Serial.println(button_A);
  //Serial.println(button_B);
  //Serial.println("count!!!!!!!!!!!!");
  //Serial.println(count);
  //Serial.println("state!!!!!!!!!!!!");
  //Serial.println(state);
  button_last = button;
  button_1_last = button_1;
  switch(state){
    case 0: // initial state
    { 
      Serial.println("Initial State\n");
      servo_angle = 0;
      myservo.detach();
      if (button_A == 1)
      {
        Serial.println("Set the position of the mask...\n");
        myservo.attach(Servopin);
        if (count1 == 1)
        {
          count1 --;
          state = 0;
          Serial.println(count1);
        }
        else
        {
          count1 = 10;
          state = 1;
          count = 1;
        }
      }
      Listen(state);
      break;
    }
    case 1:
    {
      Serial.println("State 1 \n");
      if (count == 1)
      {
        val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
        val = map(val, 0, 1023, 0, 90);     // scale it to use it with the servo (value between 0 and 180)
        myservo.write(val);                  // sets the servo position according to the scaled value
        delay(15);
        servo_angle = val;   
      }
      else
      {
        state = 6;
        myservo.write(0);
      }
      Listen(state);
      break;
    }
    case 2:
    {
      Serial.print("Case 2");
      if (button_A == 1)
      {
        Serial.println("Mask On...");
        myservo.attach(Servopin);
        myservo.write(servo_angle);
        flag = 1;
        state = 4;
      }
      else if(c == '1')
     {
        Serial.println("Person Behind Detected!");
        myservo.attach(Servopin);
        myservo.write(servo_angle);
        state = 4;
     }
     Listen(state);
     break;
    }
    case 3:
    {
      if (button_A == 1 && c != '1')
      {
        
        Serial.println("Button pressed to lift the mask!");
        myservo.attach(Servopin);
        myservo.write(0);
        flag = 0;
        state = 5;
        previousMillis = millis();
      }
      else if(c == '0' && flag == 0)
      {
        Serial.println("No Person is Detected Behind!");
        myservo.attach(Servopin);
        myservo.write(0);
        state = 5;
        previousMillis = millis();
      }
      Listen(state);
      break; 
    }
    case 4:
    {
      count2 = count2 + 1;
      Serial.println(count2);
      if (count2 > 100)
      {
        state = 3;
        myservo.detach();
        count2 = 0;
      }
      Listen(3);
      break;
    }
    case 5:
    {
      count3 = count3 + 1;
      if (count3 > 100)
      {
        state = 2;
        myservo.detach();
        count3 = 0;
      }
      Listen(2);
      break;
    }
    case 6:
    {
      count4 = count4 + 1;
      if (count4 > 100)
      {
        state = 2;
        myservo.detach();
        count4 = 0;
      }
      Listen(2);
      break;
    }
    
  }
}  
  

void Listen(int state)
{  
  delay(5);
  radio.stopListening();
  Serial.println("state = ");
  Serial.println(state);
  if (state == 0)
  {
    //Serial.println("test = 0");
    const char test = '0';
    radio.write(&test, sizeof(test));
  }
  else if (state == 1)
  {
    const char test = '1';
    //Serial.println("test = 1");
    radio.write(&test, sizeof(test));
  }
  else if (state == 2)
  {
    const char test = '2';
    //Serial.println("test = 1");
    radio.write(&test, sizeof(test));
  }
  else if (state == 3)
  {
    const char test = '3';
    //Serial.println("test = 1");
    radio.write(&test, sizeof(test));
  }
}

void serialcommunication()
{
  if (BTserial.available())
  {  
    c = BTserial.read();
  }
  if (c == '1')
  {
    Serial.write("Person Detected\n");
  }
  else
  {
    Serial.write("No Person Detected\n");
  }
}
