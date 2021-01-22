/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* System Name : [RZ/A2M] Example of Initialization
* File Name   : readme-e.txt
* Rev        : Rev.1.40.00
* Date       : Nov. 20,2019
*******************************************************************************/
/*******************************************************************************
*
* History     : Oct. 04,2018 Rev.1.00.00    First edition issued
*             : Dec. 28,2018 Rev.1.10.00    Removed the loader program from the sample code.
*             :                             Updated loader program writing method.
*             : Apr. 15,2019 Rev.1.20.00    Changed settings to output CKIO clock.
*             :                             Changed reset handler processing
*             :                               - Changed the setting procedure of NEON and VFP 
*             :                                 in initialsetting process.
*             :                               - Added processing to clear IOKEEP bit.
*             : May. 17,2019 Rev.1.30.00    Changes project folder structure and 
*             :                             compiler option.
*             : Jul. 12,2019 Rev.1.31.00    Correct the style for source code such as indent and 
*             :                             line feed for the description contents of the source file 
*             :                             and the header file in sample code. 
*             :                             Improve the content of source comments and update sample code.
*             : Nov. 20,2019 Rev.1.40.00    Changed the processing of R_SC_HardwareSetup function.
*             :                               - Moved the processing to clear the IOKEEP bit to 
*             :                                 the Userdef_PreHardwareSetup function.
*             :                               - Moved the processing to cancel the write protection of 
*             :                                 on-chip data-retention RAM to Userdef_PostHardwareSetup function.
*******************************************************************************/

1. Before Use

  This sample code has been run and confirmed by the CPU board (RTK7921053C00000BE) 
  and the SUB board (RTK79210XXB00000BE) with the RZ/A2M group R7S921053VCBG. 
  Use this readme file, the application note, and the sample code as a reference 
  for your software development.


  ****************************** CAUTION ******************************
   This sample code are all reference, and no one to guarantee the 
   operation. Please use this sample code for the technical 
   reference when customers develop software.
  ****************************** CAUTION ******************************


2. System Verification

  This sample code was developed and tested with the following components:

    CPU                                 : RZ/A2M
    Board                               : RZ/A2M CPU board (RTK7921053C00000BE)
                                          RZ/A2M SUB board (RTK79210XXB00000BE)
    Compiler                            : GNU Arm Embedded Toolchain 6-2017-q2-update
    Integrated development environment  : e2 studio Version 7.6.0.
    Emulator                            : SEGGER J-Link Base
                                          (Customers are required to prepare J-Link emulator compatible with RZ/A2M.)


3. About Sample Code

  This sample code operates the following processing.

  (1) Application program [rza2m_blinky_sample_osless_gcc]
    After the RZ/A2M initial settings, outputs the character string to the terminal on the host PC connected via
    the USB Micro-B connector (CN5 on SUB board) by using the main function in order to activate the OS timer.
    The OS timer interrupt processing executed every 500ms causes LED1 on the CPU board to blink.

    The application program is executed by the loader program after performing the register setting processing of 
    the SPI multi I/O bus controller (SPIBSC) and the serial flash memory by the loader program.
    The loader program is stored in the following folder.
      [rza2m_blinky_sample_osless_gcc\bootloader\rza2_qspi_flash_ddr_bootloader.elf]

    This loader program conforms to the specification of Macronix MX25L51245G. When using another serial flash memory, 
    change the source code of the loader program project [rza2m_sflash_boot_loader_gcc] that is included in 
    "RZ/A2M Group Example of Booting from Serial Flash Memory" application note according to the specifications of 
    the flash memory, and then generate the new load module. The project debug configuration uses the loader program
    after changing the file name of the generated load module to "rza2_qspi_flash_ddr_bootloader.elf".

   Refer to application notes for the details about the sample code. 


4. Operation Confirmation Conditions

  (1) Boot mode
    - Boot mode 3
      (Boot from serial flash memory 3.3V)
      * The program can not be operated if the boot mode except the above is specified.

  (2) Operating frequency
      The RZ/A2M clock pulse oscillator module is set to ensure that the RZ/A2M clocks on the CPU board
      board have the following frequencies:
      (The frequencies indicate the values in the state that the clock with 24MHz
      is input to the EXTAL pin in RZ/A2M clock mode 1.)
      - CPU clock (I clock)                 : 528MHz
      - Image processing clock (G clock)    : 264MHz
      - Internal bus clock (B clock)        : 132MHz
      - Peripheral clock1 (P1 clock)        :  66MHz
      - Peripheral clock0 (P0 clock)        :  33MHz
      - QSPI0_SPCLK                         :  66MHz
      - CKIO                                : 132MHz

  (3) Setting for asynchronous communication
      - Bit rate        : 115200 bps
      - Data bit        : 8 bits
      - Parity bit      : none
      - Stop bit        : 1 bit

  (4) Serial flash memory used
    - Manufacturer  : Macronix Inc.
    - Product No.   : MX25L51245G

  (5) Setting for cache
      Initial setting for the L1 and L2 caches is executed by the MMU. 
      Refer to the "RZ/A2M group Example of Initialization" application note about "Setting for MMU" for 
      the valid/invalid area of L1 and L2 caches.


5. Operational Procedure

  Use the following procedure to execute this sample code.

  (1) Setting for DIP switches and jumpers  
    Set the DIP switches and jumpers of the CPU board as follows.

     <<Setting for CPU board>>
      - SW1-1  : ON  (Disabled SSCG function)
        SW1-2  : OFF (Setting to clock mode 1(EXTAL input clock frequency : 20 to 24MHz))
        SW1-3  : ON  (MD_BOOT2 = L)
        SW1-4  : OFF (MD_BOOT1 = H)
        SW1-5  : OFF (MD_BOOT0 = H)
        SW1-6  : ON  (BSCANP normal mode (CoreSight debug mode))
        SW1-7  : ON  (CLKTEST OFF)
        SW1-8  : ON  (TESTMD OFF)
      - JP1 :   1-2  (Setting to supply 3.3V power for PVcc_SPI of RZ/A2M and U2)
      - JP2 :   2-3  (Setting to supply 1.8V power for PVcc_HO of RZ/A2M and U3)
      - JP3 :   Open (Use USB ch 0 in the function mode (Not supply VBUS0 power))

    Set the DIP switches and jumpers of the SUB board as follows.

     <<Setting for SUB board>>
      - SW6-1  : OFF (Setting to use P9_[7:0], P8_[7:1], P2_2, P2_0, P1_3, P1_[1:0], P0_[6:0], P6_7, P6_5, P7_[1:0] and P7[5:3] 
                      as DRP, audio, UART, CAN and USB interface terminals respectively)
        SW6-2  : OFF (Setting to use P8_4, P8_[7:6], P6_4 and P9_[6:3] as audio interface terminals)
        SW6-3  : OFF (Setting to use P9_[1:0], P1_0 and P7_5 as UART and USB interface terminals respectively)
        SW6-4  : OFF (Setting to use P6_[3:1] and PE_[6:0] as CEU terminals)
        SW6-5  : ON  (Setting to use P3_[5:1], PH_5 and PK_[4:0] as Ethernet PHY2 terminals)
        SW6-6  : ON  (Setting to use PJ_[7:6] as VDC6 terminals)
        SW6-7  : ON  (Setting to use P7_[7:4] as VDC6 terminals)
        SW6-8  : OFF (NC)
        SW6-9  : OFF (P5_3 = "H")
        SW6-10 : OFF (PC_2 = "H")

      - JP1 : 2-JP2  (Setting to use PJ_1 as interrupt terminal for IRQ0 switch (SW3))

    Refer to the CPU board and the SUB board user's manual for more details about
    setting for the DIP switches and jumpers.

  (2) Setting up sample code
    Copy the directories of [rza2m_blinky_sample_osless_gcc] to the e2 studio workspace directory
    in the host PC (eg: "C:\e2studio_Workspace_v760").

  (3) Activating integrated development environment
    Activate the integrated development environment e2 studio.

  (4) Building application program ([rza2m_blinky_sample_osless_gcc] project)
    After importing [rza2m_blinky_sample_osless_gcc] project by the e2 studio menu, build the project 
    and generate the executable file named as rza2m_sflash_boot_sample_osless_gcc.elf.

  (5) Connecting with emulator
    Connect the J-Link Base and the connector on the CPU board with JTAG cable. 
    When connecting you will require the conversion adapter "J-Link 19-pin Cortex-M Adapter" made by SEGGER.

  (6) Downloading sample code
    Select "Debug Configuration" by the e2 studio "Run" menu and open the "Debug Configurations" dialog box.
    Select "Renesas GDB Hardware Debugging" from the list and display the detail list.
    Select the debug configuration [rza2m_blinky_sample_osless_gcc HardwareDebug] of the application program, 
    and download both the executable file of the application program generated in (4) and the loader program 
    to the serial flash memory by pressing the "Debug" button.

  (7) Executing sample code
    Press reset on the e2 studio menu bar to start the debug session and the "Resume" button to run the code,
    Application code will be run after the boot loader process completes.


/* End of File */
