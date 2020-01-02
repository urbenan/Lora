/*
  Lora Send And Receive
  This sketch demonstrates how to send and receive data with the MKR WAN 1300 LoRa module.
  This example code is in the public domain.
*/

#include <MKRWAN.h>
#include <CayenneLPP.h>


LoRaModem modem;
// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

CayenneLPP lpp(51);

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;

#define ADC_NTC 0

float temp_celsius;
float temp_steinhart;
float A0_Float;
float A0_Voltage;
float U_VCC;
float U_NTC;
float R_1;
float R_NTC;
float NTC_A;
float NTC_B;
float NTC_C;
// int ADC_raw;
int ADC_raw;

void setup() {
  U_VCC=3.3;
  R_1=10000;

  NTC_A=1.3108309e-3;
  NTC_B=2.3257810e-4;
  NTC_C=1.2682369e-7;
  // put your setup code here, to run once:
  // Serial.begin(115200);
  // while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(EU868)) {
    // Serial.println("Failed to start module");
    while (1) {}
  };
  // Serial.print("Your module version is: ");
  // Serial.println(modem.version());
  // Serial.print("Your device EUI is: ");
  // Serial.println(modem.deviceEUI());
  // Serial.print("Text Hallo");

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    // Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independently by this setting the modem will
  // not allow to send more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.


}

void loop() {
  ADC_raw = analogRead(ADC_NTC);
  A0_Float=float(ADC_raw);
  A0_Voltage = (A0_Float/1023)*3.3;
  temp_celsius=(580.0-ADC_raw)/10;
  // temp_celsius=(768.0-ADC_raw)/10;
  U_NTC=A0_Voltage;
  R_NTC=(U_NTC*R_1)/(U_VCC-U_NTC);
  temp_steinhart=1/(NTC_A+NTC_B*log(R_NTC)+(NTC_C*pow(log(R_NTC),3)))-273.15;
  
  Serial.println(A0_Float);
  Serial.println(A0_Voltage);
  Serial.println(R_NTC);
  Serial.println(temp_steinhart);
  // Serial.println();
  // Serial.println("Enter a message to send to network");
  // Serial.println("(make sure that end-of-line 'NL' is enabled)");

  // while (!Serial.available());
  // String msg = "Hallo";

  lpp.reset();
  lpp.addBarometricPressure(1, A0_Float);
  lpp.addBarometricPressure(2, A0_Voltage);
  lpp.addAnalogInput(2, A0_Voltage);
  lpp.addTemperature(1, temp_celsius);
  lpp.addTemperature(2, temp_steinhart);
  // String msg = lpp.getBuffer();

  // Serial.println();
  // Serial.print("Sending: " + msg + " - ");
  // for (unsigned int i = 0; i < msg.length(); i++) {
  //  Serial.print(msg[i] >> 4, HEX);
  //  Serial.print(msg[i] & 0xF, HEX);
  //  Serial.print(" ");
  // }
  // Serial.println();

  int err;
  modem.beginPacket();
  // modem.print(msg);
  modem.write(lpp.getBuffer(),lpp.getSize());
  
  err = modem.endPacket(true);
  // if (err > 0) {
    // Serial.println("Message sent correctly!");
  // } else {
    // Serial.println("Error sending message :(");
    // Serial.println("(you may send a limited amount of messages per minute, depending on the signal strength");
    // Serial.println("it may vary from 1 message every couple of seconds to 1 message every minute)");
  // }
  delay(60000);
  if (!modem.available()) {
    // Serial.println("No downlink message received at this time.");
    return;
  }
  char rcv[64];
  int i = 0;
  while (modem.available()) {
    rcv[i++] = (char)modem.read();
  }
  // Serial.print("Received: ");
  // for (unsigned int j = 0; j < i; j++) {
    // Serial.print(rcv[j] >> 4, HEX);
    // Serial.print(rcv[j] & 0xF, HEX);
    // Serial.print(" ");
  // }
  // Serial.println();
}
