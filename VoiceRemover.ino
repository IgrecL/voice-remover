#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include "MyDsp.h"

AudioPlaySdWav           playWav1;
AudioOutputI2S           audioOutput;
MyDsp                    dsp;

AudioConnection          patchCord1(playWav1, 0, dsp, 0);
AudioConnection          patchCord2(playWav1, 1, dsp, 1);
AudioConnection          patchCord3(dsp, 0, audioOutput, 0);
AudioConnection          patchCord4(dsp, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#define BUTTON_PIN       2

void setup() {
  Serial.begin(9600);
  AudioMemory(8);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  pinMode(BUTTON_PIN, INPUT);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);
  playWav1.play(filename);
  delay(25);
  while (playWav1.isPlaying()) {
    dsp.setParamValue("stereo", analogRead(A0));
    dsp.setParamValue("volume", analogRead(A1));
    dsp.setParamValue("voice", digitalRead(BUTTON_PIN));
    Serial.println(digitalRead(BUTTON_PIN));
    delay(100);
  }
}


void loop() {
  playFile("EXAMPLE.WAV");
  delay(10);

}
