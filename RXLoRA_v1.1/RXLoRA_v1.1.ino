// Receiver code which is plugged into a separate workstation.
#include <SPI.h>
#include <RH_RF95.h>
#define DEBUG 1
#define TXLIMIT 450

// Feather M0
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0


RH_RF95 rf95(RFM95_CS, RFM95_INT);

String txdata;
uint32_t timer = millis();

// For setup, it needs to take < 10 seconds
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  initLoRA();
  delay(1000);

}

void initLoRA(void) {

  pinMode( RFM95_RST, OUTPUT );
  digitalWrite( RFM95_RST, HIGH );

  delay(100);

  Serial.println("Feather LoRa RX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Start - Pre-requisites to ensure that LoRA is initialized properly
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  // End -

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

}


void loop() {
  
  if (rf95.available()) {
    uint8_t buf[TXLIMIT];
    uint8_t len = sizeof(buf);

    if ( rf95.recv( buf, &len ) ) {
      Serial.print("Got: ");
      Serial.println( (char*) buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
  }
}
