#include <SPI.h>
#include <RH_RF95.h>

#define NUM_STEPS 12
#define T1 70 //Lo recibe y multiplcia por 10; 90 => 900ms
#define T_UMBRAL 10 //200ms
#define UMBRAL 40

#define REQUEST_ID 0x21
#define RESPONSE_ID 0x22

#define OUT_SPEED1  50
#define OUT_SPEED2  70
#define OUT_SPEED3  85

#define SPEED1 10
#define SPEED2 11
#define SPEED3 12
// for feather32u4
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define RF95_FREQ 915.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Need this on Arduino Zero with SerialUSB port (eg RocketScream Mini Ultra Pro)
#define RF95_FREQ 915.0
int led = 2;//;
uint8_t currentSpeed = 0;
uint8_t lastCurrentSpeed = 0;

// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(100);
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  pinMode(led, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SPEED1, INPUT_PULLUP);
  pinMode(SPEED2, INPUT_PULLUP);
  pinMode(SPEED3, INPUT_PULLUP);

  Serial.begin(9600);
  //while (!Serial);

  while (!rf95.init()) {
    while (!Serial);
    Serial.println(F("LoRa radio init failed"));
    while (1);
  }
  //  Serial.println(F("LoRa radio init OK!"));

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    while (!Serial);
    Serial.println(F("setFrequency failed"));
    while (1);
  }
  //  Serial.print(F("Set Freq to: ")); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop()
{
  readSpeed();
  if (rf95.available())
  {
    readSpeed();
    Serial.print("lastSpeed" + String(lastCurrentSpeed) + " ");
    Serial.println("currentSpeed" + String(currentSpeed));
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
    {
      digitalWrite(led, HIGH);
      digitalWrite(LED_BUILTIN, HIGH);
      if (len && buf[0] == REQUEST_ID) {
        //      RH_RF95::printBuffer("request: ", buf, len);
        Serial.print("got request: ");
        Serial.println((char*)buf);
        //      Serial.print("RSSI: ");
        //      Serial.println(rf95.lastRssi(), DEC);

        // Send a reply
        uint8_t data[] = {RESPONSE_ID, lastCurrentSpeed, currentSpeed, NUM_STEPS, T1, T_UMBRAL, UMBRAL};
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
        Serial.println("Sent a reply");
        digitalWrite(led, LOW);
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    else
    {
      digitalWrite(led, HIGH);
      Serial.println("recv failed");
    }
    lastCurrentSpeed = currentSpeed;
  }

}
