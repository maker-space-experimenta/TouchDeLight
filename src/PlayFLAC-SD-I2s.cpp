#include "Arduino.h"
#include <FS.h>
#include <HTTPClient.h>
#include <SD.h>
#include <SPI.h>
#include <SPIFFS.h>
#include "AudioFileSourceSD.h"
#include <AudioOutputI2S.h>
#include "AudioGeneratorFLAC.h"

// For this sketch, you need connected SD card with '.flac' music files in the root
// directory. Some samples with various sampling rates are available from i.e. 
// Espressif Audio Development Framework at:
// https://docs.espressif.com/projects/esp-adf/en/latest/design-guide/audio-samples.html
//
// On ESP8266 you might need to reencode FLAC files with max '-2' compression level 
// (i.e. 1152 maximum block size) or you will run out of memory. FLAC files will be 
// slightly bigger but you don't loose audio quality with reencoding (lossles codec).

// You may need a fast SD card. Set this as high as it will work (40MHz max).
#define SPI_SPEED SD_SCK_MHZ(40)



File dir;
AudioFileSourceSD *source = NULL;
AudioOutputI2S *output = NULL;
AudioGeneratorFLAC *decoder = NULL;

void setup() {
  Serial.begin(115200);
  Serial.println();
  delay(1000);
  audioLogger = &Serial;  
  source = new AudioFileSourceSD();
  output = new AudioOutputI2S();
  decoder = new AudioGeneratorFLAC();

  
    SD.begin();

  dir = SD.open("/"); 
}

void loop() {
  if ((decoder) && (decoder->isRunning())) {
    if (!decoder->loop()) decoder->stop();
  } else {
    File file = dir.openNextFile();
    if (file) {      
      if (String(file.name()).endsWith(".flac")) {
        source->close();
        if (source->open(file.name())) { 
          Serial.printf_P(PSTR("Playing '%s' from SD card...\n"), file.name());
          decoder->begin(source, output);
        } else {
          Serial.printf_P(PSTR("Error opening '%s'\n"), file.name());
        }
      
    } else {
      Serial.println(F("Playback form SD card done\n"));
      delay(1000);
    }       
  }
}
}