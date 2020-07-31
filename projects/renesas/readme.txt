----------------------------------------------
Getting Start for RX MCUs Amazon FreeRTOS
----------------------------------------------
Thank you for interesting about RX MCUs (RX65N is main) and Amazon FreeRTOS.
This guide is for your environment setup and confirm demos behavior.

If you have any question/suggestion/opinion, please visit following site and post it.
https://github.com/renesas/amazon-freertos

Thanks for Amazon team provides great software as Open Source and great IoT Cloud Service.
And Thanks for NoMaY-san provides many many useful information to port the Amazon FreeRTOS to RX65N,
and manages GitHub repository for this project.
And Thanks for Renesas RX MCUs business unit member funds for me,
and helps some hardware/software development.
I hope this solution will be helpful for embedded system developer in W/W.

Renesas Electronics, Ishiguro

--------------------------------------------------------------------------
Change Logs
--------------------------------------------------------------------------
v202002.00-rx-1.0.0
 [UPDATED] Updated FreeRTOS version 202002.00.
 [TESTED] demos MQTT echo behavior for
          RX65N RSK CC-RX e2 studio with E2 Emulator Lite.
          RX65N Cloud kit CC-RX e2 studio with E2 Emulator Lite (on board).

--------------------------------------------------------------------------
Development Environment (recommended)
--------------------------------------------------------------------------
Board: Renesas Starter Kit+ for RX65N-2MB
    [en] https://www.renesas.com/us/en/products/software-tools/boards-and-kits/renesas-starter-kits/renesas-starter-kitplus-for-rx65n-2mb.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/boards-and-kits/renesas-starter-kits/renesas-starter-kitplus-for-rx65n-2mb.html

Board: Renesas RX65N Cloud Kit
    [en] https://www.renesas.com/us/en/products/software-tools/boards-and-kits/eval-kits/rx65n-cloud-kit.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/boards-and-kits/eval-kits/rx65n-cloud-kit.html

--------------------------------------------------------------------------
RX65N Device Introduction
--------------------------------------------------------------------------
RX65N is powerful device for IoT embedded system.
RX65N has some useful peripheral for getting sensor data, control some motors,
and communication with some devices using USB/Ether/SD card/SDIO/UART/I2C/SPI/etc,
and original security IP can make "Root of Trust" as device security,
and LCD controller that can connect to generic LCD
(recommended 480x272 resolution with double buffer (256x2KB)),
with huge internal memory (max ROM=2MB, RAM=640KB)
with powerful/low current consumption (34 CoreMark/mA),
Renesas original CPU(RXv2 core)@120MHz using 40nm process.
https://www.renesas.com/en-us/about/press-center/news/2017/news20171113.html

--------------------------------------------------------------------------
EOF
--------------------------------------------------------------------------
