#include <SPI.h>
#include <stdint.h>

#define MOSI_PIN 16
#define MISO_PIN 14
#define CLK_PIN 15
#define DEVICE_1 8
#define DEVICE_2 9

int count = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DEVICE_1, OUTPUT);
  pinMode(DEVICE_2, OUTPUT);
  pinMode(10, OUTPUT);          // Set SS to master mode

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setBitOrder(MSBFIRST);
  digitalWrite(DEVICE_1, HIGH);
  digitalWrite(DEVICE_2, HIGH); 
}

uint16_t mcp3008_read(uint8_t device, uint8_t  channel) {  
  digitalWrite(DEVICE_1, HIGH);
  digitalWrite(DEVICE_2, HIGH);
  
  delay(1);
  
  digitalWrite(device, LOW);
  SPI.transfer(0x01);
  uint8_t  msb = SPI.transfer(0x80 + (channel  << 4 ));
  uint8_t  lsb = SPI.transfer(0x00);
  digitalWrite(device, HIGH);

  delay(1);
    
  return (( msb & 0x03 ) << 8) + lsb;     
}

void loop() {
  for (int chan=0; chan<8; chan++) {
    Serial.print(mcp3008_read(DEVICE_1, chan)); Serial.print("\t");
  }
  Serial.print("["); Serial.print(count); Serial.print("], ");

  for (int chan_2=0; chan_2<8; chan_2++) {
    Serial.print(mcp3008_read(DEVICE_2, chan_2)); Serial.print("\t");
  }

  Serial.print("["); Serial.print(count); Serial.println("]");
  
  count++;
  Serial.println("");
  delay(20);
}
