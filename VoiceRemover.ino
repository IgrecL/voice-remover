#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "MyDsp.h"

AudioPlaySdWav           playWav;
AudioInputI2S            audioInput;
AudioOutputI2S           audioOutput;
MyDsp                    dsp;

AudioConnection          patchCord1(audioInput, 0, dsp, 0);
AudioConnection          patchCord2(audioInput, 1, dsp, 1);
AudioConnection          patchCord3(playWav, 0, dsp, 2);
AudioConnection          patchCord4(playWav, 1, dsp, 3);
AudioConnection          patchCord5(dsp, 0, audioOutput, 0);
AudioConnection          patchCord6(dsp, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#define BUTTON_PIN1       2
#define BUTTON_PIN2       3
#define BUTTON_PIN3       4

int last = 0;
int mode = 0;
int button1Pressed = 0;
int button2Pressed = 0;
int button3Pressed = 0;

void setup() {
  Serial.begin(9600);
  AudioMemory(12);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  pinMode(BUTTON_PIN1, INPUT);
  pinMode(BUTTON_PIN2, INPUT);
  pinMode(BUTTON_PIN3, INPUT);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void update() {
  
  // Update parameters values
  dsp.setParamValue("stereo", analogRead(A0));
  dsp.setParamValue("volume", analogRead(A1));
  Serial.println(analogRead(A0));
  Serial.println(analogRead(A1));

  // Button to turn on/off voice removal
  Serial.println(digitalRead(BUTTON_PIN1));
  if ((digitalRead(BUTTON_PIN1) == 1) && (button1Pressed == 0)) {
    dsp.setParamValue("voice", !last);
    last = !last;
    button1Pressed = 1;
  } else if (digitalRead(BUTTON_PIN1) == 0) {
    button1Pressed = 0;
  }

  // Button to switch from jack mode to SD card mode
  Serial.println(digitalRead(BUTTON_PIN2));
  if ((digitalRead(BUTTON_PIN2) == 1) && (button2Pressed == 0)) {
    if (mode) {
      mode = 0;
    } else {
      mode = 1;
    }
    Serial.print("Mode: ");
    Serial.println(mode);
    button2Pressed = 1;
  } else if (digitalRead(BUTTON_PIN2) == 0) {
    button2Pressed = 0;
  }
  
  
  Serial.println(digitalRead(BUTTON_PIN3));
  Serial.println();
  delay(100);
}

void playFile(const char *filename)
{
  patchCord1.disconnect();
  patchCord2.disconnect();
  Serial.print("Playing file: ");
  Serial.println(filename);
  playWav.play(filename);
  delay(25);
  while (playWav.isPlaying()) {
    update();
    if (mode == 0) {
      playWav.stop();
    }
  }
  patchCord1.connect();
  patchCord2.connect();
}

void loop() {
  if (mode == 1) {
    playFile("EXAMPLE.WAV");
  }
  delay(100);
  update();
}
