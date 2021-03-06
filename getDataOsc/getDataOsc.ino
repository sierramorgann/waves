// Need apple air tower connected to router for ethernet network between computer and arduino 
// make sure both computer and arduino are listening on the same port
// start the plist in terminal to run the python script scraper.py every hour 

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
#define MS1 11
#define MS2 12
#define MS3 13



#define STEPS_PER_ROTATION 400

Stepper motor(STEPS_PER_ROTATION, STEP, DIR, MS1, MS2, MS3);

// the media access control (ethernet hardware) address for the shield:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//the IP address for the shield:
byte ip[] = { 10, 0, 1 , 3 };

//MAKE SURE THIS PORT MATCHES WITH COMPUTER PORT
const unsigned int localPort = 4380;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold incoming packet
EthernetUDP Udp;

float height;
int period;
int sec = 6; // test representation of period
float ft = 2; // test representation of height
int interval;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time the motor was updated

void setup()
{
  //start the ethernet connection 
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  //start the communication port
  Serial.begin(57600);

  // These pins are set above to send instructions to the arduino to operate the stepper motor
  pinMode(ENABLE, OUTPUT);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  //  motor.setSpeed(1);
}

// This function calculates the delay in seconds to make sure the wave makes its full peak and flat line in the 
// directed amount of time read in from the OSC data (not 100% acurate, but whatever the data is an averaged num)

void delayTime() {
  if (period <= 5 ) { // fastest speed : 2.5 sec / foot
    interval = 16;
  }
  else if (period > 5 && period <= 7) { // faster speed : 5 sec / foot
    interval = 31;
  }
  else if (period > 7 && period <= 10) { // average speed : 10 sec / foot
    interval = 62;
  }
  else if (period > 10) { // slower speed : 20 sec / foot
    interval = 125;
  }
  else { // slowest speed as default
    interval = 1000; //represents 1 full second delay for one step
  }
}

// Controls the stepper motor 

void stepNow(int totalSteps) {
  digitalWrite(MS1, HIGH); //Pull MS1,MS2, and MS3 high to set logic to 1/16th microstep resolution
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
  int i;
  delayTime();
  Serial.println(interval);
  for (i = 0; i < totalSteps; ++i) {
    digitalWrite(STEP, HIGH);
    delay(interval);
    digitalWrite(STEP, LOW);
  }
}

// if you just want to move the stepper in one direction call either of these functions

//void walkRight() {
//  digitalWrite(DIR, HIGH);
//  stepNow(STEPS_PER_ROTATION * ft);
//}
//
//void walkLeft() {
//  digitalWrite(DIR, LOW);
//  stepNow(STEPS_PER_ROTATION * ft);
//}


//move the stepper motor one direction and then back the other direction (this creates the wave effect)

void walkBothDirections() {
  Serial.println(F("dir LOW"));
  digitalWrite(DIR, LOW);
  stepNow(STEPS_PER_ROTATION * height);

  Serial.println(F("dir HIGH"));
  digitalWrite(DIR, HIGH);
  stepNow(STEPS_PER_ROTATION * height);
}


void loop () {
  
  //Gets OSC data over ethernet port from python script
  OSCMessage msg;
  int packetSize = Udp.parsePacket();
  // miliseconds will always be greater than interval... currentMillis will get reset to 0 every loop
  unsigned long currentMillis = millis();

  //wraps the OSC function in a time wrapper so the data gets updated almost immediately when there is new data coming in  
  if (currentMillis - previousMillis >= interval) {
    //set the previous mills to current to reset the time
    previousMillis = currentMillis;

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

    // if the motor is turned off turn on and run
    if (period > 0) {
      walkBothDirections();
    } else {
      Serial.println("there was an error with the stepper motor");
    }
  }
}




