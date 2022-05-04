### Hardware
[ESP-8266](https://docs.ai-thinker.com/esp8266)
* <img src='https://docs.ai-thinker.com/_media/esp8266/spec/esp8266_all.png?w=700&tok=97ee19' style='width:200px;height:400px'/>
[ESP-01S-Relay](https://github.com/IOT-MCU/ESP-01S-Relay-v4.0)
* ![ESP-01S-Relay](https://www.geekering.com/wp-content/uploads/2020/03/Screenshot_6-296x300.png)

### Dependencies
* [ESP8266 core for Arduino](https://github.com/esp8266/Arduino)
* [Blinker Library](https://github.com/blinker-iot/blinker-library)

### Modifications Required
* [your wifi ssid](https://github.com/whSwitching/SampleStores/blob/0128dd4dbf22048f32eaae3154ce9a12966e0414/ESP-01S-Relay/ESP-01S-Relay.ino#L16)
* [your wifi password](https://github.com/whSwitching/SampleStores/blob/0128dd4dbf22048f32eaae3154ce9a12966e0414/ESP-01S-Relay/ESP-01S-Relay.ino#L17)
* [blinker device secret](https://github.com/whSwitching/SampleStores/blob/0128dd4dbf22048f32eaae3154ce9a12966e0414/ESP-01S-Relay/ESP-01S-Relay.ino#L18)
* [and other wifi config](https://github.com/whSwitching/SampleStores/blob/0128dd4dbf22048f32eaae3154ce9a12966e0414/ESP-01S-Relay/ESP-01S-Relay.ino#L19)
* [user-defined switch button name on blinker app](https://github.com/whSwitching/SampleStores/blob/0128dd4dbf22048f32eaae3154ce9a12966e0414/ESP-01S-Relay/ESP-01S-Relay.ino#L31)

### 3 ways to control
+ web request
  + http://192.168.2.66/gpio to get state
  + http://192.168.2.66/gpio?swi=on to set relay on
  + http://192.168.2.66/gpio?swi=off to set relay off
+ [Blinker app](https://github.com/blinker-iot/app-release)
+ [TmallGenie](https://diandeng.tech/doc/tmallgenie)
