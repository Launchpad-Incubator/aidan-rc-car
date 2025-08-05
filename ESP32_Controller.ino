/*
ESP32 RC Car Controller Code

  - Uses an ESP32 to send a GET request over Wi-Fi
  - Controls an RC Car with inputs from a joystick

Author: Aidan McMillan
Date: 08/05/2025
*/

#include <WiFi.h>
#include <Arduino.h>

const char* ssid = "Aidan's RC Car";
const char* password = "drivefast";
String pico_ip = "192.168.4.1";
String cmd = " ";
String past_cmd = " ";

WiFiClient Client;

bool input_changed;
int stick_input[2] = {0, 0};
int past_stick[2] = {-1, -1};
String stick_cmd[2] = {"null", "null"};

const int X_PIN = 3;
const int Y_PIN = 4;
const int LED = 2;
const int INT_CONTROL = 0;
const int MID_LOW = 2000;
const int MID_HIGH = 2100;

void getKeyInputs() {
  input_changed = 0;
  stick_input[0] = analogRead(X_PIN);
  stick_input[1] = analogRead(Y_PIN);
  
  if (stick_input[0] != past_stick[0] || stick_input[1] != past_stick[1]) {
    input_changed = 1;
  }
  
  if (input_changed == 1) {
    Serial.print("\nRecieved X input ");
    Serial.print(stick_input[0]);
    Serial.print(" and Y input ");
    Serial.print(stick_input[1]);
    if (stick_input[0] < MID_LOW) {
      stick_cmd[0] = "left";
    } else if (MID_LOW < stick_input[0] < MID_HIGH) {
      stick_cmd[0] = "center";
    } else if (MID_HIGH < stick_input[0]) {
      stick_cmd[0] = "right";
    } else {
      Serial.print("Error: Invalid Input!");
    }

    if (stick_input[1] < MID_LOW) {
      stick_cmd[1] = "forward";
    } else if (MID_LOW < stick_input[1] < MID_HIGH) {
      stick_cmd[1] = "stop";
    } else if (MID_HIGH < stick_input[1]) {
      stick_cmd[1] = "back";
    } else {
      Serial.print("Error: Invalid Input!");
    }
    Serial.print("\nSteering set to ");
    Serial.print(stick_cmd[0]);
    Serial.print("\nDrive set to ");
    Serial.print(stick_cmd[1]);
    past_stick[0] = stick_input[0];
    past_stick[1] = stick_input[1];
  }
}

void sendInputs() {
  cmd = "cmd=" + stick_cmd[1] + "&turn=" + stick_cmd[0];

  if (cmd != past_cmd) {
  
  String request = "GET /api?cmd=";
  request += cmd;
  request += " HTTP/1.1\r\nHost: ";
  request += pico_ip;
  request += "\r\nConnection: close\r\n\r\n";
  Client.print(request);
  past_cmd = cmd;

  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to wifi on network ");
  Serial.print(ssid);
  Serial.print(" and password ");
  Serial.print(password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
    digitalWrite(LED, !digitalRead(LED));
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.print("\nConnection Failed!");
    } else if (WiFi.status() == WL_NO_SSID_AVAIL) {
      Serial.print("\nNo SSID found!");
    }
  }
  Serial.print("\nConnected to wifi. \nIP address: ");
  Serial.print(WiFi.localIP());
}

void loop() {
  getKeyInputs();
  sendInputs();
}
