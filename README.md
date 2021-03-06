# esp32-securitycam

## Description
ESP Camera module that will wake up from deep sleep when the PIR sensor is triggered.
A PIR sensor (**P**assive **i**nfra**r**ed) detects the presence of humand and other animals.
While triggered it takes pictures and writes them to the SD Card.
To prevent flashing of the LED (connected to one of the SD interface pins) it has been desoldered.

Because the esp32 is in deep sleep mode and the PIR sensor uses little energy, it can be battery powered.

![Overview](https://github.com/rzeldent/esp32-securitycam/blob/master/front%20view.jpg)

## Hardware
Bill of materials:

| # | Description | $ | AliExpress link | Remarks |
| --: | ------------- | --: | ----------------- | --------- |
| 1 | esp32-cam module | $7.34 | [https://www.aliexpress.com/item/32994696562.html](https://www.aliexpress.com/item/32994696562.html) | 
| 1 | PIR AM312 | $6.20 | [https://www.aliexpress.com/item/32922605847.html](https://www.aliexpress.com/item/32922605847.html) | lot of 5 | 
| 1 | Dupont cable female-female  (10cm) | $0.58 | [https://www.aliexpress.com/item/33007698478.html](https://www.aliexpress.com/item/33007698478.html) | 40 wires |
| 1 | USB to TTL programming cable | $3.24 | [https://www.aliexpress.com/item/4000276930065.html](https://www.aliexpress.com/item/4000276930065.html) | Development only|

## Wiring
The PIR sensor must be powered (using the GND and 3.3V) and its output connected to GPIO12.

![Wiring](https://github.com/rzeldent/esp32-securitycam/blob/master/side%20view.jpg)



## Preparation
