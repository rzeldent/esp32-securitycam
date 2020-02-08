#include <Arduino.h>

#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

#include "FS.h"     //sd card esp32
#include "SD_MMC.h" //sd card esp32

#include <camera.h>

camera camera;

// Pin to tigger the camera. Is done by a PIR sensor (needs a pull-up). GPIO13 and GPIO16 do not work!
const byte pir_pin = GPIO_NUM_12;

//const char *extension = ".JPG";
const char *extension = ".BMP";

// Next image id to use for file name on SD card
unsigned long image_id = 1;

// put your setup code here, to run once:
void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  pinMode(pir_pin, INPUT_PULLUP);

  // Look for the higest frame numer found
  if (!SD_MMC.begin())
  {
    log_e("SD card mount failed");
    sleep(10);
    ESP.restart();
  }

  // Get last id used for the images on the SD card
  log_i("Opening: /");
  auto root = SD_MMC.open("/");
  File entry;
  char *end_parse;
  while ((entry = root.openNextFile()))
  {
    log_i("Found file: %s", entry.name());
    // Skip the / (+1) and try to parse;
    auto id = strtoul(entry.name() + 1, &end_parse, 10);
    // Check if parsing a number was succesful and remainder is .JPG
    if (id == ULONG_MAX || strcmp(end_parse, extension) != 0)
      continue;

    if (id >= image_id)
      image_id = id + 1;
  }

  log_i("Next image id to be used: %u", image_id);

  SD_MMC.end();

  // Initialize the camera
  if (!camera.initialize(FRAMESIZE_QVGA, PIXFORMAT_GRAYSCALE))
  {
    log_e("Camera initialization failed");
    // sleep(10);
    // ESP.restart();
  }

  log_i("Camera initialized");
}

bool lastPirState = false;

// put your main code here, to run repeatedly:
void loop()
{
  auto state = digitalRead(pir_pin);
  if (!state)
  {
    // PIR is not triggered
    if (lastPirState)
      log_i("Armed");

    lastPirState = false;
    return;
  }

  if (lastPirState)
  {
    // PIR is still triggered
    return;
  }

  // Rising: trigger
  lastPirState = true;
  log_i("Triggered");

  // Take a picture
  camera::frame frame;
  log_i("Took picture. size: %ld.", frame.len);

  // Save the image
  String path = "/" + String(image_id++) + extension;
  if (SD_MMC.begin())
  {
    auto file = SD_MMC.open(path, FILE_WRITE);
    frame.write_bitmap(file);
    file.close();
    log_i("Picture save as %s.", path.c_str());
    SD_MMC.end();
  }
}