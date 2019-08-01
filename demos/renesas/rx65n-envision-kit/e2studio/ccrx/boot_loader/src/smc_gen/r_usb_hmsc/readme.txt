PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_usb_hmsc
=======================

Document Number 
---------------
R01AN2026EJ
R01AN2026JJ

Version
-------
v1.23

Overview
--------
USB Host Mass Storage Class Driver (HMSC)

Features
--------
The USB host mass storage class driver comprises a USB mass storage class 
Bulk-Only Transport (BOT) protocol. When combined with a file system and 
storage device driver, it enables communication with a BOT-compatible USB 
storage device.


Supported MCUs
--------------
* RX64M Group
* RX71M Group
* RX63N Group
* RX65N Group

Boards Tested On
----------------
* RSKRX64M
* RSKRX71M
* RSKRX63N
* RSKRX65N
* RSKRX65N_2MB
 
Limitations
-----------

Peripherals Used Directly
-------------------------


Required Packages
-----------------
* r_bsp
* r_usb_basic

How to add to your project
--------------------------

Toolchain(s) Used
-----------------
* Renesas RX v.2.07.00

File Structure
--------------
r_usb_hmsc
|   readme.txt
|   r_usb_hmsc_if.h
|
+---doc
|     \en
|     |   r01an2026ej0123_usb.pdf
|     \jp
|         r01an2026jj0123_usb.pdf
|
+---ref
|       r_usb_hmsc_config_reference.h
|
\---src
     |  r_usb_hmsc_api.c
     |  r_usb_hmsc_driver.c
     |  r_usb_hstorage_driver.c
     |  r_usb_hstorage_driver_api.c
     |
     \---inc
             r_usb_hmsc.h
