# Arduino_Vibration_Logger_SSD1306 (震動紀錄器)
## 利用 MPU9250 的陀螺儀當作震動偵測器

效果如下
![image](https://github.com/Chihhao/Arduino_Vibration_Logger_SSD1306/blob/main/image/IMG_2965.jpg)

## 材料
(1) Arduino Nano  
(2) MPU9250  
(3) 0.96吋OLED (128*64)  
(4) DS3231時鐘  
(5) SPI Nor Flash (Mxic or Winbond)(1Gb)  
(6) 3D列印外殼 (https://www.thingiverse.com/thing:4861300)  

## 有用到的函式庫 (已包含，不須額外安裝)
https://github.com/asukiaaa/MPU9250_asukiaaa  
https://github.com/jarzebski/Arduino-DS3231  
https://github.com/adafruit/Adafruit_SSD1306  
https://github.com/Marzogh/SPIMemory  

## 注意
(1) 螢幕解析度實際上只用到128*32，因為使用128*64，Arduino會記憶體不足  
(2) 預設每0.2秒會記錄一筆資料，可以連續紀錄約9天，超過會自動回頭覆蓋，就像行車紀錄器  
(3) 我手上的這顆MPU9250的AD0必須上拉(可能有損壞)，上拉後I2C地址變成0x69  
    (若沒有上拉，I2C地址是0x68)  

![image](https://github.com/Chihhao/Arduino_Vibration_Logger_SSD1306/blob/main/image/wiring.png)
![image](https://github.com/Chihhao/Arduino_Vibration_Logger_SSD1306/blob/main/image/IMG_2961.jpg)
![image](https://github.com/Chihhao/Arduino_Vibration_Logger_SSD1306/blob/main/image/IMG_2962.jpg)
![image](https://github.com/Chihhao/Arduino_Vibration_Logger_SSD1306/blob/main/image/log.png)
