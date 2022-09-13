#include <Arduino.h>
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = D1;
const int LOADCELL_SCK_PIN = D2;

HX711 scale;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("taring the scale... remove any weight from the scale");
  delay(3000);
  scale.set_scale(-478.507); // this value is obtained by calibrating the scale with known weights
  scale.tare();               // reset the scale to 0
  Serial.println("Taring done.");

  Serial.println("Readings:");
}

void loop() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 5);

  scale.power_down();             // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}
