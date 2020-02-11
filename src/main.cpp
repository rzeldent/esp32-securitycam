#include <Arduino.h>

#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

#include "FS.h"     //sd card esp32
#include "SD_MMC.h" //sd card esp32

#include <camera.h>

Camera camera;

// Pin to tigger the camera. Is done by a PIR sensor (needs a pull-up). GPIO13 and GPIO16 do not work!
const byte pir_pin = GPIO_NUM_12;

const char *extension = ".jpg";

// Next image id to use for file name on SD card
unsigned long image_id = 1;

// put your setup code here, to run once:
void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  // AM312 needs a pullup resistor
  pinMode(pir_pin, INPUT_PULLUP);

  // Look for the higest frame number found
  if (!SD_MMC.begin())
  {
    log_e("SD card mount failed");
    sleep(10);
    ESP.restart();
  }

  // Get last id used for the images on the SD card
  log_i("Looking for last image id used");
  auto root = SD_MMC.open("/");
  File entry;
  char *end_parse;
  while ((entry = root.openNextFile()))
  {
    log_d("Found file: %s", entry.name());
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
  if (!camera.initialize(FRAMESIZE_SXGA))
  {
    log_e("Camera initialization failed");
    sleep(10);
    ESP.restart();
  }

  log_d("Camera initialized");
}

void loop_singleshot(uint delayMilliseconds = 0)
{
  if (digitalRead(pir_pin) == LOW)
    return;

  // Optional delay
  delayMicroseconds(1000 * delayMilliseconds);

  // Take a picture
  Camera::Frame frame;
  // Save the image
  String path = "/" + String(image_id++) + extension;
  if (!SD_MMC.begin())
  {
    auto file = SD_MMC.open(path, FILE_WRITE);
    frame.write_jpeg(file);
    file.close();
    log_i("Picture save as %s.", path.c_str());
    SD_MMC.end();
  }
  else
  {
    log_e("Unable to open SD card");
  }

  do
  {
    delayMicroseconds(1000 * 10);
  } while (digitalRead(pir_pin) == HIGH);
}

void loop_continuous()
{
  if (digitalRead(pir_pin) == LOW)
    return;

  if (SD_MMC.begin())
  {
    do
    {
      // Take a picture
      Camera::Frame frame;
      // Save the image
      String path = "/" + String(image_id++) + extension;
      auto file = SD_MMC.open(path, FILE_WRITE);
      frame.write_jpeg(file);
      file.close();
      log_i("Picture save as %s.", path.c_str());
    } while (digitalRead(pir_pin) == HIGH);

    SD_MMC.end();
  }
  else
  {
    log_e("Unable to open SD card");
  }
}

// put your main code here, to run repeatedly:
void loop()
{
  // Uncomment below the desired behavior: SingleShot or Continuous

  //loop_singleshot();
  loop_continuous();
}