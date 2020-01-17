#include <Arduino.h>

#include <soc/soc.h>
#include <soc/rtc_cntl_reg.h>

#include "FS.h"     //sd card esp32
#include "SD_MMC.h" //sd card esp32

#define SD_CS 5
#define SD_SCK 18
#define SD_MOSI 23
#define SD_MISO 19

#include <camera.h>

camera camera;

// put your setup code here, to run once:
void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  esp_log_level_set("*", ESP_LOG_VERBOSE);

  camera.initialize();

  if (!SD_MMC.begin())
  {
    log_e("SD card mount failed");
    return;
  }
  switch (SD_MMC.cardType())
  {
  case CARD_NONE:
    log_i("No SD_MMC card attached.");
    break;
  case CARD_MMC:
    log_i("MMC card present");
    break;
  case CARD_SD:
    log_i("SDSC card present");
    break;
  case CARD_SDHC:
    log_i("SDHCC card present");
    break;
  default:
    log_i("Unknown card present");
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  log_i("SD_MMC Card Size: %lluMB\n", cardSize);
}

// put your main code here, to run repeatedly:
void loop()
{
  //auto frame = camera.get_frame();
  sleep(1);
}