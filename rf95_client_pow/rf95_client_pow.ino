#include "DigiPot.h"
#include <SPI.h>
#include <RH_RF95.h>

#define REQUEST_ID 0x21
#define RESPONSE_ID 0x22
#define RFM95_RST 5
#define RF95_FREQ 915.0
#define UD A3
#define INC A4
#define CS A2
#define LED A5

// Singleton instance of the radio driver
RH_RF95 rf95;
DigiPot pot(INC, UD, CS);

//RH_RF95 rf95(5, 2); // Rocket Scream Mini Ultra Pro with the RFM95W
//RH_RF95 rf95(8, 3); // Adafruit Feather M0 with RFM95

uint8_t currentSpeed = 0;
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void setup()
{
  currentSpeed = 0;
  pot.reset(); //stop
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  Serial.begin(9600);
  delay(100);

  Serial.println("Start Client RFM95!");
  pinMode(LED, OUTPUT);
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop()
{
  uint8_t data[] = {REQUEST_ID};  // Send a message to rf95_server
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (rf95.waitAvailableTimeout(1000))    // Should be a reply message for us now
  {
    if (rf95.recv(buf, &len))
    {
      if (len >= 7 && buf[0] == RESPONSE_ID) {
        Serial.print("Last speed");
        Serial.print(buf[1], DEC);
        Serial.print(" current speed:");
        Serial.println( buf[2], DEC);
        setSpeed(buf[2], buf[3], buf[4], buf[5], buf[6]);
      }
    }
    else
    {
      digitalWrite(LED,  HIGH);
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
    digitalWrite(LED, !digitalRead(LED));
  }
  delay(400);

}

void setSpeed(uint8_t speed, uint8_t num_steps, uint8_t t1, uint8_t t2, uint8_t umbral)
{
  if (currentSpeed != speed) {
    const uint8_t maxStep = round(pow(1.6, num_steps - 1));
    for (uint8_t i = 0; i < num_steps; i++)
    {
      //accel grows exponentially as the for loop continues
      uint8_t  accel = map(round(pow(1.6, i)), 1, maxStep, currentSpeed, speed);
      pot.set(accel);
      Serial.println(accel);
      if (accel >= umbral)
        delay(t1 * 10);
      else
        delay(t2 * 10);
      digitalWrite(LED, !digitalRead(LED));
    } //close for
  }
  currentSpeed = speed;
  pot.set(speed);
}
