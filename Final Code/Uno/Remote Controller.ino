#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define led_pin 2
#define led_pin_1 3
#define button_pin 5
#define button_pin_1 4
#define red_pin 8
#define green_pin 7
#define blue_pin 6


RF24 radio(9, 10); // CE, CSN        
const byte address[][6] = {"00001", "00002"};     //Byte of array representing the address. This is the address where we will send the data. This should be same on the receiving side.

 
int count = 0;
int count1 = 0;
boolean button_state = 0;
boolean button_state_1 = 0;
boolean last_button = 0;
boolean last_button_1 = 0;

void setup() {
  pinMode(led_pin, OUTPUT);
  pinMode(led_pin_1, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(button_pin, INPUT);
  pinMode(button_pin_1, INPUT);
  Serial.begin(9600);
  radio.begin();                  //Starting the Wireless communication
  radio.openWritingPipe(address[1]);   //Setting the address at which we will receive the data
  radio.openReadingPipe(1, address[0]);
  radio.setPALevel(RF24_PA_MIN);  //You can set it as minimum or maximum depending on the distance between the transmitter and receiver.  
  Serial.println("test");
}

void loop()
{
  delay(5);
  radio.stopListening();
  button_edge_detection();
  delay(5);
  radio.startListening();
  if(radio.available())
  {
    char test;
    radio.read(&test, sizeof(test));
    Serial.println(test);
    if(test == '0')
    {
      digitalWrite(led_pin, HIGH);
      digitalWrite(led_pin_1, LOW);
      digitalWrite(blue_pin, 0);
      digitalWrite(red_pin, 0);
      digitalWrite(green_pin, 0);
    }
    else if(test == '1')
    {
      digitalWrite(led_pin_1, HIGH);
      digitalWrite(led_pin, LOW);
    }
    else if(test == '2')
    {
      digitalWrite(led_pin_1, LOW);
      digitalWrite(blue_pin, 0);
      digitalWrite (green_pin, 255);
      digitalWrite (red_pin, 0);  
    }
    else if(test == '3')
    {
      //digitalWrite(led_pin_1, LOW);
      digitalWrite(blue_pin, 255);
      digitalWrite (red_pin, 255);
      digitalWrite (green_pin, 0);
    }
  }
}


void button_edge_detection()
{
  button_state = digitalRead(button_pin);
  button_state_1 = digitalRead(button_pin_1);
  //
  if(button_state == HIGH && last_button == LOW)
  {
    count ++;
  }
  if(button_state_1 == HIGH && last_button_1 == LOW)
  {
    count1 ++;
  }

  if (count % 2 !=0 )
  {  
    if (count1 % 2 !=0 )
    {
      const char text[] = "11";
      //Serial.println(text);
      radio.write(&text, sizeof(text));                  //Sending the message to receiver
    }
    else
    {
      const char text[] = "10";
      //Serial.println(text);
      radio.write(&text, sizeof(text));
    }
  }
  else
  {
    if (count1 % 2 !=0 )
    {
      const char text[] = "01";
      //Serial.println(text);
      radio.write(&text, sizeof(text));                  //Sending the message to receiver
    }
    else
    {
      const char text[] = "00";
      //Serial.println(text);
      radio.write(&text, sizeof(text));
    }                   //Sending the message to receiver
  }
  last_button = button_state;
  last_button_1 = button_state_1;
    //Sending the message to receiver
}
