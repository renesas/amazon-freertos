PLEASE REFER TO THE APPLICATION NOTE FOR THIS MIDDLEWARE FOR MORE INFORMATION

r_tfat_rx
=========

Document Number
---------------
R20AN0038JJ0303-RX-TFAT
R20AN0038EJ0303-RX-TFAT

Version
-------
v3.03

Overview
--------
TFAT library is a FAT file system which just requres small memory.
TFAT library is based on FatFs.

Please refer to the 
http://www.renesas.com/mw/tfat
http://japan.renesas.com/mw/tfat
"R20AN0293JJxxxx-RX-TFAT/R20AN0293EJxxxx-RX-TFAT" too.

These configuration options can be found in "r_config\r_tfat_rx_config.h". 
An original copy of the configuration file is stored in 
"r_tfat_rx\ref\r_tfat_rx_config_reference.h".


Features
--------
* TFAT library is a FAT file system which just requres small memory.

Supported MCUs
--------------
* RX family

Boards Tested On
----------------
* RSKRX71M
* RSKRX64M
* RSKRX63N
* RSKRX62N
* RSKRX610
* RSKRX231
* RSKRX210
* RSKRX113
* RSKRX111
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
"r01an1723ju0111_rx.pdf/r01an1826jj0102_rx.pdf(R20AN0038JJ0303-RX-TFAT)"
"r01an1723eu0111_rx.pdf/r01an1826eu0102_rx.pdf(R20AN0038EJ0303-RX-TFAT)".

Toolchain(s) Used
-----------------
* Renesas RXC V.2.05.00

File Structure
--------------
r_tfat_rx
|   readme.txt
|   r_tfat_lib.h
|
+---doc
|   |
|   +---ja
|   |         r20uw0078jj0301_tfat.pdf
|   |         r20an0038jj0303-rx-tfat.pdf
|   +---en
|             r20uw0078ej0301_tfat.pdf
|             r20an0038ej0303-rx-tfat.pdf
|
+---lib
|         r_mw_version.h
|         r_stdint.h
|         tfat_rx200_big.lib
|         tfat_rx200_little.lib
|         tfat_rx600_big.lib
|         tfat_rx600_little.lib
|
+---ref
          r_tfat_rx_config_reference.h

r_config
    r_tfat_rx_config.h