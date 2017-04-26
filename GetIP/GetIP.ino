#include <Stepper.h>

#include <Arduino.h>
#include <OSCBoards.h>
#include <OSCBundle.h>
#include <OSCMessage.h>

#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet2.h>
#include <EthernetUdp2.h>
#include <SPI.h>

int ENABLE = LOW;

#define ENABLE 10
#define STEP 9
#define DIR 8

#define STEPS_PER_ROTATION 200

Stepper motor(STEPS_PER_ROTATION, STEP, DIR);

// the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//the IP address for the shield:
byte ip[] = { 10, 0, 1 , 3 };

const unsigned int localPort = 200;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet
EthernetUDP Udp;

float height;
int period;
int sec = 6; // test representation of period
float ft = 2; // test representation of height
int interval;      //turn into miliseconds

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

void setup()
{
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  Serial.begin(57600);

  pinMode(ENABLE, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  motor.setSpeed(1);
}

void delayTime() {
  if (period <= 5 ) { // fastest speed : 2.5 sec / foot
    interval = 16;
  }
  else if (period > 5 && sec <= 7) { // faster speed : 5 sec / foot
    interval = 31;
  }
  else if (period > 7 && sec <= 10) { // average speed : 10 sec / foot
    interval = 62;
  }
  else if (period > 10) { // slower speed : 20 sec / foot
    interval = 125;
  }
  else { // slowest speed as default
    interval = 1000; //represents 1 full second delay for one step
  }
}

void stepNow(int totalSteps) {
  int i;
  delayTime();
  Serial.println(interval);
  for (i = 0; i < totalSteps; ++i) {
    digitalWrite(STEP, HIGH);
    delay(interval);
    digitalWrite(STEP, LOW);
  }
}

//void walkRight() {
//  digitalWrite(DIR, HIGH);
//  stepNow(STEPS_PER_ROTATION * ft);
//}
//
//void walkLeft() {
//  digitalWrite(DIR, LOW);
//  stepNow(STEPS_PER_ROTATION * ft);
//}

void walkBothDirections() {
  Serial.println(F("dir LOW"));
  digitalWrite(DIR,LOW);
  stepNow(STEPS_PER_ROTATION * height);
  
  Serial.println(F("dir HIGH"));
  digitalWrite(DIR,HIGH);
  stepNow(STEPS_PER_ROTATION * height);
}

void loop () {
  OSCMessage msg;
  int packetSize = Udp.parsePacket();

  if (packetSize) {

    // read the packet into packetBufffer
    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    for (int i = 0; i < UDP_TX_PACKET_MAX_SIZE; i++) {
      msg.fill(packetBuffer[i]);
      Serial.print(packetBuffer[i]);
      packetBuffer[i] = '\0';
    }

    //returns true if the data in the first position is an integer
    if (msg.isFloat(0)) {
      height = msg.getFloat(0);
      Serial.print(height);
    }

    if (msg.isInt(1)) {
      period = msg.getInt(1);
      Serial.println(period);
    } else {
      Serial.print("error in OSC msg");
    }
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (period > 0) {
      walkBothDirections();
    } else {
      //Serial.println("there was an error with the stepper motor");
    }
  }
}




