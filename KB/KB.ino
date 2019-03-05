#include <SPI.h>
#include <stdint.h>
#include "Keyboard.h"

#define MOSI_PIN 16
#define MISO_PIN 14
#define CLK_PIN 15
#define DEVICE_1 10

#define COL_1 3
#define COL_2 5
#define COL_3 6 
#define COL_4 9

int Columns[4] = { COL_1, COL_2, COL_3, COL_4 };

const byte rows = 5;
const byte cols = 4;

char keys[rows][cols] = {
  {'1','2','3','4'},
  {'q','w','e','r'},
  {'a','s','o','f'},
  {'v',' ',' ','x'},
  {' ',' ',' ','c'}
};

int keyCodes[rows][cols] = {
  {0,1,2,3},
  {4,5,6,7},
  {8,9,10,11},
  {12,13,14,15},
  {16,17,18,19}
};

unsigned long lastDebounceTime[20]; 
unsigned long debounceDelay = 250;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Keyboard.begin();

  // Setup Analog CHIP
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DEVICE_1, OUTPUT);
  pinMode(10, OUTPUT);          // Set SS to master mode

  // Start the lighting sequence
  pinMode(COL_1, OUTPUT);
  pinMode(COL_2, OUTPUT);
  pinMode(COL_3, OUTPUT);
  pinMode(COL_4, OUTPUT);

  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setBitOrder(MSBFIRST);
  digitalWrite(DEVICE_1, HIGH);
}

void light_it_up(int col) {
  analogWrite(col, 255);
}

void kill_it(int col) {
  analogWrite(col, 0);
}

uint16_t mcp3008_read(uint8_t device, uint8_t  channel) {  
  digitalWrite(device, LOW);
  SPI.transfer(0x01);
  uint8_t  msb = SPI.transfer(0x80 + (channel  << 4 ));
  uint8_t  lsb = SPI.transfer(0x00);
  digitalWrite(device, HIGH);
  return (( msb & 0x03 ) << 8) + lsb;     
}

void loop() {
  for (int col=0; col<4; col++) {
    kill_it(Columns[col]);
    delay(1);
  }
  
  for (int col=0; col<4; col++) {
    light_it_up(Columns[col]);
    for (int chan=0; chan<8; chan++) {
      int read = mcp3008_read(DEVICE_1, chan);
      if (read < 300) {
        continue;
      }
      
      int keyCode = keyCodes[chan][col];
      if (!lastDebounceTime[keyCode]) {
        Keyboard.print(keys[chan][col]);
        lastDebounceTime[keyCode] = millis();
      }
 
      if (lastDebounceTime[keyCode] && (millis() - lastDebounceTime[keyCode]) > debounceDelay) {
        Keyboard.print(keys[chan][col]);
        lastDebounceTime[keyCode] = millis();
      }

      Serial.println("COL: " + String(col) + ", CHAN: " + String(chan) + ", VAL:" + String(read) + ", KEYCODE: " + String(keyCode));
    }
    kill_it(Columns[col]);
    delay(1);
  }
}
