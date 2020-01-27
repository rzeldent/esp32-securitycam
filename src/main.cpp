#include <Arduino.h>

#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

#include "FS.h"     //sd card esp32
#include "SD_MMC.h" //sd card esp32

#include <camera.h>

camera camera;

// Pin to tigger the camera. Is done by a PIR sensor (needs a pull-up)
const byte pir_pin = GPIO_NUM_16;

// Time to stay active after trigger
const int remain_active_milliseconds = 5000;

// Interval to take pictures in milliseconds
const int picture_interval_milliseconds = 0;

// Last time milliseconds when motion was detected
volatile unsigned long last_triggered = ULONG_MAX - remain_active_milliseconds;

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

  // Get last id used for the image
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
    if (id == ULONG_MAX || strcmp(end_parse, ".JPG") != 0)
      continue;

    if (id >= image_id)
      image_id = id + 1;
  }

  log_i("Next image id to be used: %u", image_id);

  // Set pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(pir_pin), [](){
    log_i("Motion detected");
    last_triggered = millis();
  }, RISING);

  // Initialize the camera
  camera.initialize();
}

// put your main code here, to run repeatedly:
void loop()
{
  auto now = millis();
  if (last_triggered + remain_active_milliseconds < now)
  {
    // Take a picture
    auto frame = camera.get_frame();
    log_i("Took picture. size: %ld.", frame->as_jpeg()->size());
    String path = "/" + String(image_id++) + ".JPG";

    auto file = SD_MMC.open(path, FILE_WRITE);
    file.write(frame->as_jpeg()->data(), frame->as_jpeg()->size());
    file.close();
    log_i("Picture save as %s.", path.c_str());

    // If remain_active_milliseconds is zero, it means a one single shot
    if (remain_active_milliseconds == 0)
    {
      last_triggered = ULONG_MAX - remain_active_milliseconds;
      log_i("Single shot.");
    }
    else
    {
      sleep(picture_interval_milliseconds);
      log_i("Sleeping for %d milliseconds.", picture_interval_milliseconds);
    }
  }
}