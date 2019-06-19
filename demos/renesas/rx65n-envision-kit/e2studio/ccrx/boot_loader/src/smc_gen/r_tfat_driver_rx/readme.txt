PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_tfat_driver_rx
=========

Document Number
---------------
R20AN0335JJ0103-RX-TFAT
R20AN0335EJ0103-RX-TFAT

Version
-------
v1.03

Overview
--------
This memory driver interface connects TFAT library and Device Drivers(USB etc).
TFAT library is a FAT file system which just requres small memory.
TFAT library is based on FatFs.
<br/>
Please refer to the
 http://www.renesas.com/mw/tfat
 http://japan.renesas.com/mw/tfat

Features
--------
* This memory driver interface connects TFAT library and Device Drivers(USB etc).


Supported MCUs
--------------
* RX family

Boards Tested On
----------------
* RSKRX111
* RSKRX113
* RSKRX64M
* RSKRX71M
* RSKRX63N
* RSKRX62N
* RSKRX610
* RSKRX231
* RSKRX210
* RSKRX65N

Limitations
-----------
None

Peripherals Used Directly
-------------------------
None

Required Packages
-----------------
None

How to add to your project
--------------------------
Please refer to the Adding Firmware Integration Technology Modules to Projects.
"r01an1723ju0111_rx.pdf/r01an1826jj0102_rx.pdf(R20AN0335JJ0103-RX-TFAT)"
"r01an1723eu0111_rx.pdf/r01an1826ej0102_rx.pdf(R20AN0335EJ0103-RX-TFAT)".

Toolchain(s) Used
-----------------
* Renesas RX V.2.05.00

File Structure
--------------
r_config
    r_tfat_driver_rx_config.h

r_tfat_driver_rx
|   readme.txt
|   r_tfat_driver_if.h
|
+---doc
|   |
|   +---en
|   |   r20an0335ej0103-rx-tfat.pdf
|   +---ja
|       r20an0335jj0103-rx-tfat.pdf
+---src
|   |   r_tfat_drv_if.c
|   |   r_tfat_drv_if_dev.h
|   |
|   +---device
|       |
|       +---sdhi
|       |      r_tfat_drv_if_sdhi.c
|       +---usb
|       |      r_tfat_drv_if_usb.c
|       +---usb_mini
|              r_tfat_drv_if_usb_mini.c
|
+---ref
          r_tfat_driver_rx_config_reference.h

