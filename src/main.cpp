#include <Arduino.h>

#include <SD.h>
#define SD_CS 5
#define SD_SCK 18
#define SD_MOSI 23
#define SD_MISO 19

#include <camera.h>

camera camera;

// put your setup code here, to run once:
void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  log_i("Starting SD card");

  if (!SD.begin(SD_CS))
  {
    log_e("No SD card found!");
    return;
  }

  
  // Initialize the camera
//  esp32cam_aithinker_config.frame_size = FRAMESIZE_SVGA;
  }

// put your main code here, to run repeatedly:
void loop()
{
  auto frame = camera.get_frame();
  

}