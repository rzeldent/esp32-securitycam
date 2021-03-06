#include <Arduino.h>

#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

#include "WiFi.h"
#include "EEPROM.h"
#include "FS.h"
#include "SD_MMC.h"

#include <camera.h>

// Pin to tigger the camera. Is done by a PIR sensor (needs an internal pull-up).
// Tried GPIO13 and GPIO16 but theu did not work!
const gpio_num_t pir_pin = GPIO_NUM_12;

const char *extension = ".jpg";

// put your setup code here, to run once:
void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  // Disable brownout detector
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  // Disable Bluetooth and WiFi radio's to save power
  btStop();
  WiFi.mode(WIFI_OFF);

  // AM312 PIR sensor needs a pullup resistor
  pinMode(pir_pin, INPUT_PULLUP);

  // Retrieve the last image_id from EEPROM
  unsigned long image_id;
  EEPROM.begin(sizeof(image_id));
  image_id = EEPROM.readULong(0);
  log_i("Last image id used: %u", image_id);

  Camera camera;
  // Initialize the camera
  if (!camera.initialize(FRAMESIZE_SXGA))
  {
    log_e("Camera initialization failed");
    sleep(10);
    ESP.restart();
  }

  log_d("Camera initialized");

  if (SD_MMC.begin())
  {
    // Take pictures while triggered
    do
    {
      Camera::Frame frame;
      // Save the image
      String path = "/" + String(++image_id) + extension;
      auto file = SD_MMC.open(path, FILE_WRITE);
      frame.write_jpeg(file);
      file.close();
      log_i("Picture saved as %s. Len=%d bytes.", path.c_str(), frame.fb_->len);
    } while (digitalRead(pir_pin) == HIGH);

    SD_MMC.end();
  }
  else
  {
    log_e("Unable to open SD card");
  }

  // Save last used id to EEPROM
  EEPROM.writeULong(0, image_id);
  EEPROM.commit();

  // Wake when PIR is triggered again
  esp_sleep_enable_ext0_wakeup(pir_pin, true);

  Serial.println("Sleep....");
  esp_deep_sleep_start();
}

// put your main code here, to run repeatedly:
void loop()
{
}