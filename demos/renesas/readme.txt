----------------------------------------------
Getting Start for RX MCUs Amazon FreeRTOS
----------------------------------------------
Thank you for interesting about RX MCUs (RX65N is main) and Amazon FreeRTOS.
This guide is for your environment setup and confirm demos behavior.

SheltyDog@Renesas writes this guide and has responsibility of this project.
If you have any question/suggestion/opinion, please visit following site and post it.
https://github.com/renesas-rx/amazon-freertos

And, details discussion is progressing in following site. (Japanese)
https://japan.renesasrulz.com/cafe_rene/f/forum21/4772/amazon-freertos-rx

Now Amazon provides great real time OS with network software for
MCU (like some peripheral with 100MHz class CPU) as "Amazon FreeRTOS".
As you know "Amazon Web Service(AWS)" is one of biggest IoT Cloud Service in W/W.
"Amazon FreeRTOS" can make a way to connect the "Amazon Web Service" like following steps.

Download or Clone repository from GitHub.
The download zip file name is "amazon-freertos-master.zip".
Extract this zip or your clone directory has "demos" and "lib" directory readme, etc.
This is our root directory called <root>.

Getting Start Steps:
 step1:  Refer to the Development Environment (recommended) section to get the board and tools.
 step2:  Setup tools for your PC, Compiler and IDE are mandatory.
         Others are just reference information, already implemented into step3 data, no need setup.
 step3:  Download RX MCUs Amazon FreeRTOS from GitHub. (Maybe you already done)
         https://github.com/renesas-rx/amazon-freertos
 step4:  Make your AWS account, and make your "Things" on AWS,
         and enable Security Policy to allow all your device will connect to your "Things".
 step5:  Make your device certification and private key and settings put this into your source code.
         <root>/demos/common/include
          aws_clientcredential.h
           clientcredentialMQTT_BROKER_ENDPOINT[]
           clientcredentialIOT_THING_NAME
          aws_clientcredential_keys.h
           clientcredentialCLIENT_CERTIFICATE_PEM[]
           clientcredentialCLIENT_PRIVATE_KEY_PEM[]
 step6:  Start Renesas IDE (e2 studio) and import Amazon FreeRTOS project.
         import sequence: file->import->existing project into workspace -> select a root directory
         The project folder is placed into <root>/demos/renesas/rx65n-rsk/ccrx-e2studio
         Please specify this directory only.
         This directory is top of reference. You can also use other project.
         Please select tested project shown into Development Environment (tested or no matrix) section
         in this Readme.
         Note: "DO NOT SET" the check for "Copy projects into workspace" in "Import Projects" dialog.
               Because, some of source in "<root>/lib" folder are outside of project directory.
               So if this check would exist, some of source in "<root>/lib" folder will be missing.
 step7:  Build
 step8:  Execute, confirm console log will show the Echo message from AWS.
         The log will be output from G1CUSB (or other UART/USB) connector as UART/USB.
         Please set baud-rate as 115200bps, 8bit-data, no-parity, 1 stop-bit,
         and "LF" only as return code for your console.
  
Thanks for Amazon team provides great software as Open Source and great IoT Cloud Service.
And Thanks for NoMaY-san provides many many useful information to port the Amazon FreeRTOS to RX65N,
and manages GitHub repository for this project.
And Thanks for Renesas RX MCUs business unit member funds for me,
and helps some hardware/software development.
I hope this solution will be helpful for embedded system developer in W/W.

--------------------------------------------------------------------------
Change Logs
--------------------------------------------------------------------------
v0.1.6:
[TESTED] by Amazon FreeRTOS Qualification for
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
[MERGED] upstream Amazon FreeRTOS v1.4.5 master
[ADDED] RX63N GR-SAKURA supports
[ADDED] RX65N GR-ROSE GCC/C++ supports
[DELETED] RX65N RSK + Silex SX-ULPGN supports (come back soon)
[DELETED] RX65N Target Board + Silex SX-ULPGN supports (come back soon)
[TESTED] demos MQTT echo behavior for
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N RSK GCC e2 studio with E2 Emulator Lite
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite
         RX65N GR-ROSE GCC/C++ e2 studio with E2 Emulator Lite
         RX64M GR-KAEDE CC-RX e2 studio with E2 Emulator Lite
         RX64M GR-KAEDE CC-RX CS+ with E2 Emulator Lite
         RX64M GR-KAEDE GCC e2 studio with E2 Emulator Lite
         RX63N GR-SAKURA CC-RX e2 studio with E2 Emulator Lite
         RX63N GR-SAKURA CC-RX CS+ with E2 Emulator Lite
         RX63N GR-SAKURA GCC e2 studio with E2 Emulator Lite

v0.1.5:
[UPDATED] Unifying BSP(CC-RX/GCC/IAR)
[UPDATED] Increase configMINIMAL_STACK_SIZE value from 140 to 180
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N RSK GCC e2 studio with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite
         RX65N Target Board + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Target Board + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite (on board)
         RX65N Target Board + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite
         RX64M GR-KAEDE CC-RX e2 studio with E2 Emulator Lite
         RX64M GR-KAEDE CC-RX CS+ with E2 Emulator Lite
         RX64M GR-KAEDE GCC e2 studio with E2 Emulator Lite
         
v0.1.4:
[ADDED] Missing parts for RX65N Target Board + Silex SX-ULPGN PMOD CC-RX
        e2 studio with E2 Emulator Lite (on board).

v0.1.3:
[UPDATED] Follow the upstream from Amazon FreeRTOS v1.4.1.
[FIXED] no use ET_LINKSTA setting for RX65N GR-ROSE projects.
[ADDED] Following projetcs.
         RX65N Target Board + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Target Board + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite (on board)
         RX65N Target Board + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite (on board)
         RX65N RSK + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N RSK GCC e2 studio with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite
         RX65N Target Board + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Target Board + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite (on board)
         RX65N Target Board + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite
         RX64M GR-KAEDE CC-RX e2 studio with E2 Emulator Lite
         RX64M GR-KAEDE CC-RX CS+ with E2 Emulator Lite
         RX64M GR-KAEDE GCC e2 studio with E2 Emulator Lite

v0.1.2:
[UPDATED] Follow the upstream from Amazon FreeRTOS v1.3.2.
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N RSK GCC e2 studio with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite
         RX64M GR-KAEDE CC-RX e2 studio with E2 Emulator Lite
         RX64M GR-KAEDE CC-RX CS+ with E2 Emulator Lite
         RX64M GR-KAEDE GCC e2 studio with E2 Emulator Lite

v0.1.1:
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N RSK GCC e2 studio with E2 Emulator Lite
         RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite

v0.1.1-pre5:
[REMOVED] link to FreeRTOS+TCP from rx65n-rsk-uart-wifi.
[FIXED] Wrong behavior of rx65n-rsk-uart-wifi: can set optimize level 2, can set log-off.
[UPDATED] Follow the upstream v1.3.0 excluding tests folder.
[RESTRUCTUERED] Remove prototype for optimizing file structure.

v0.1.1-pre4:
[FIXED] Wrong project settings.
[ADDED] Explanation of Renesas Starter Kit+ for RX65N-2MB + Silex SX-ULPGN PMOD environment in this readme.

v0.1.1-pre3:
[ADDED] Renesas Starter Kit+ for RX65N-2MB + Silex SX-ULPGN PMOD environment.

v0.1.1-pre2:
[UPDATED] aws_secure_sockets.c in rx65n-envision-kit and rx65n-gr-rose.

v0.1.1-pre1:
[CONFIRMED] Following changes by NoMaY-san.
[UPDATED] Follow the upstream from Amazon FreeRTOS v1.3.0. <step 1/3>
[ADDED] RX65N GR-ROSE's project for CC-RX & CS+.
[MOVED] RX MCUs device driver HAL from /demos folder to /lib/third_party/mcu_vendor/renesas
[FIXED] lack of called DI/EI operation in R_BSP_RegisterProtectDisable() and R_BSP_RegisterProtectEnable().
[ADDED] Maybe a better way to get the start address of the _ETHERNET_BUFFERS section for GNURX projects.
[FIXED] Correction of the SCFGcompiler.h because the R_ATTRIB_INTERRUPT/R_ATTRIB_STATIC_INTERRUPT macros
        for GNURX projects were incorrect so that interrupt vectors were not generated.
[RESTRUCTUERED] Restructuring folder structure of projects for CC-RX/e2 studio & CC-RX/CS+ & GNURX/e2 studio,
                RX65N-RSK's trial of new folder structure reducing the effort to maintain various projects
                but still keeping good usability.
[ADDED] A workaround to avoid the problem that the BSP_CFG_RTOS_USED in the r_bsp_config.h
        is set to (0) every time of code generation by the Smart Configurator.
[RESTRUCTUERED] Restructuring folder structure of projects for CC-RX/e2 studio & CC-RX/CS+ & GNURX/e2 studio,
                Improve the batch files and the handling of them.

v0.1.0:
[UPDATED] RX Driver Package version from v114 to v115 for all projects.
[UPDATED] CS+, e2 studio to latest.
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N RSK GCC e2 studio with E2 Emulator Lite
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite

v0.1.0-pre10:
[UPDATED] RX Driver Package version from v114 to v115 RX65N GR-ROSE GCC/e2 studio project.
[TESTED] Following projetcs.
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite

v0.1.0-pre9:
[UPDATED] RX Driver Package version from v114 to v115 RX65N GR-ROSE CC-RX/e2 studio project.
[TESTED] Following projetcs.
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite

v0.1.0-pre8:
[UPDATED] RX Driver Package version from v114 to v115 RX65N Envision Kit GCC/e2 studio project.
[TESTED] Following projetcs.
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite

v0.1.0-pre7:
[UPDATED] RX Driver Package version from v114 to v115 RX65N Envision Kit CC-RX/e2 studio project.
[TESTED] Following projetcs.
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite

v0.1.0-pre6:
[UPDATED] RX Driver Package version from v114 to v115 RX65N Envision Kit CC-RX/CS+ project.
[TESTED] Following projetcs.
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite
         
v0.1.0-pre5:
[UPDATED] RX Driver Package version from v114 to v115 RX65N RSK GCC/e2 studio project.
           -> Copy smc_gen folder (excluding bsp) from RX65N RSK CC-RX CS+ project.
[TESTED] Following projetcs.
         RX65N RSK GCC e2 studio with E2 Emulator Lite
         
v0.1.0-pre4:
[UPDATED] RX Driver Package version from v114 to v115 RX65N RSK CC-RX/CS+ project.
           -> Re-generate the code from RX Driver Package v115.
           -> Merged unified macro code for multi compiler.
[TESTED] Following projetcs.
         RX65N RSK CC-RX CS+ with E2 Emulator Lite

v0.1.0-pre3:
[FIXED] RX65N RSK CC-RX/e2 studio project has Ether driver is not latest.
        Updated from v114 to v115.
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite

v0.1.0-pre2:
[UPDATED] RX Driver Package version from v114 to v115 RX65N RSK CC-RX/e2 studio project.
           -> Re-generate the code from RX Driver Package v115.
           -> Merged unified macro code for multi compiler.
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite

v0.1.0-pre1:
[UPDATED] e2 studio version from v630 to v700 on RX65N RSK CC-RX/e2 studio project.
[UPDATED] RX Driver Package version from v114 to v115 RX65N RSK CC-RX/e2 studio project.
[ADDED] RX65N RSK GCC/e2 studio project.
[RESTRUCTUERED] RX65N RSK CC-RX/CS+ project.
[RESTRUCTUERED] RX65N RSK CC-RX/e2 studio project.
[FIXED] Device name tag (R5F565NEHxFB --> R5F565NEDxFB) on aws_demos.scfg
        of each of RX65N Envision Kit project.
[RESTRUCTUERED] RX65N Envision Kit CC-RX/CS+ project.
[RESTRUCTUERED] RX65N Envision Kit GCC/e2 studio project.
[RESTRUCTUERED] RX65N Envision Kit CC-RX/e2 studio project.

v0.0.9:
[NOTICE] Sorry, v0.0.8 includes wrong commit massage.
         No differencies between v0.0.7 to v0.0.8, v0.0.9 excluding commit message.
         v0.0.7 has following update.
[UPDATED] Emulator settings to use the E2 Emulator Lite for all projects.
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite
v0.0.8:
[NOTICE] Sorry, v0.0.7 includes wrong commit massage.
         No differencies between v0.0.7 to v0.0.8 excluding commit message.
         v0.0.7 has following update.
[UPDATED] Emulator settings to use the E2 Emulator Lite for all projects.
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite
         
v0.0.7:
[UPDATED] Emulator settings to use the E2 Emulator Lite for all projects.
[TESTED] Following projetcs.
         RX65N RSK CC-RX e2 studio with E2 Emulator Lite
         RX65N RSK CC-RX CS+ with E2 Emulator Lite
         RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)
         RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)
         RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)
         RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
         RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite
         
v0.0.7-pre4:
[ADDED] RX65N GR-ROSE GCC e2 studio project code.

v0.0.7-pre3:
[FIXED] RX65N Envision Kit GCC e2 studio project code.

v0.0.7-pre2:
[FIXED] RX65N GR-ROSE CC-RX e2 studio project SCI/BSP settings.

v0.0.7-pre1:
[ADDED] GCC and IAR trial envirionment into Envision Kit project.
[OMITTED] Step7-8 from Getting Start Steps.
[ADDED] Configuired device driver from RX Driver Package to omit step7-8.

v0.0.6:
[ADDED] Support RX65N GR-ROSE.
[ADDED] Compiler pre-processor macro "__RX" to fix e2 studio local issue for pre-build code analysis.
[ADDED] Settings for indexer e2 studio local issue for pre-build code analysis.
        - e2 studio project for each of the board, RX65N RSK, RX65N Envision Kit, RX65N GR-ROSE.
[UPDATED] prvMiscInitialization() call timing to avoid the system bus error comes from corrupting printf() output.
[UPDATED] MAC address from temporary MAC address to renesas sample code MAC address.
[UPDATED] e2 studio version from v620 to v630.
[UPDATED] RX Driver Package version from v113 to v114.

v0.0.5:
[UPDATED] Follow the upstream from Amazon FreeRTOS v1.2.7.
[UPDATED] Readme text step8 for additional notice.

v0.0.4:
[UPDATED] Readme text encode from SJIS to UTF8.
[UPDATED] Readme text step8 for more details.

v0.0.3:
[UPDATED] Follow the upstream from Amazon FreeRTOS v1.2.6.

v0.0.2:
[UPDATED] Follow the upstream from Amazon FreeRTOS v1.2.5.
[REMOVED] Getting start step7 from #ifdef 0 to 1.
[UPDATED] Clarify Getting start root directory path same as download zip "amazon-freertos-master".

v0.0.1:
[ADDED] RX65N supports Amazon FreeRTOS Release Version 1.2.3 in tentative.
        Only confirmed Echo demo using Ethernet.

--------------------------------------------------------------------------
Development Environment (recommended)
--------------------------------------------------------------------------
Board: Renesas Starter Kit+ for RX65N-2MB
    [en] https://www.renesas.com/us/en/products/software-tools/boards-and-kits/renesas-starter-kits/renesas-starter-kitplus-for-rx65n-2mb.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/boards-and-kits/renesas-starter-kits/renesas-starter-kitplus-for-rx65n-2mb.html

Compiler: CC-RX V3.00 (you need non-expired evaluation license or product license to build Amazon FreeRTOS)
    [en] https://www.renesas.com/us/en/products/software-tools/tools/compiler-assembler/compiler-package-for-rx-family-e2studio.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/tools/compiler-assembler/compiler-package-for-rx-family-e2studio.html

IDE: e2 studio V7.3.0
    [en] https://www.renesas.com/us/en/products/software-tools/tools/ide/e2studio.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/tools/ide/e2studio.html
    
Debugger: E2 Emulator Lite (no need to buy because Renesas Starter Kit has this one into package)
    [en] https://www.renesas.com/us/en/products/software-tools/tools/emulator/e2-emulator-lite.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/tools/emulator/e2-emulator-lite.html
    
    Notice: You can also use E1 Emulator but you have to modify the debugger settings on your IDE.
    
Device Driver: RX Driver Package v115
    [en] https://www.renesas.com/us/en/products/software-tools/software-os-middleware-driver/software-package/rx-driver-package.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/software-os-middleware-driver/software-package/rx-driver-package.html

Special Device Driver for FreeRTOS: RX65N Group RX65N Real-time OS Package V1.1.00
    [en] https://www.renesas.com/us/en/products/software-tools/software-os-middleware-driver/software-package/rx-real-time-os-package.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/software-os-middleware-driver/software-package/rx-real-time-os-package.html
    
Comment:
 Renesas Starter Kit+ for RX65N-2MB is expensive/huge spec for just prototyping phase.
 I am preparing low-priced/small-spec kit as expansion board for "Target Board for RX Family".
    [en] https://www.renesas.com/us/en/products/software-tools/boards-and-kits/cpu-mpu-boards/rx-family-target-board.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/boards-and-kits/cpu-mpu-boards/rx-family-target-board.html
 Current developments is using Ethernet but I will move this to WiFi.
 And I am also preparing to corresponds to GCC for release from CC-RX compiler limitation.
 
--------------------------------------------------------------------------
Development Environment (optional)
--------------------------------------------------------------------------
Board: RX65N Envision Kit
    [en] https://www.renesas.com/us/en/products/software-tools/boards-and-kits/renesas-promotion-boards/rx65n-envision-kit.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/boards-and-kits/renesas-promotion-boards/rx65n-envision-kit.html

         The log will be output from CN14(PMOD:1-6pin) connector as PMOD UART/USB.
         Please set baud-rate as 115200bps, 8bit-data, no-parity, 1 stop-bit,
         and "LF" only as return code for your console.
         PMOD UART/USB convertor is provided by Digilent.
         https://store.digilentinc.com/pmod-usbuart-usb-to-uart-interface/

         Comment:
           RX65N Envision Kit has no parts related Ethernet.
           Please implements LAN8720A and RJ45 connector onto your RX65N Envision Kit.

Board: RX65N GR-ROSE proto2 (Board Circuit Document No.:X65A-M01-B , Document Rev.B8)
    [en] now preparing
    [ja] now preparing
         [event info] http://gadget.renesas.com/ja/event/2018/RenesasNight13.html

         The log will be output from CN4 10pin=TxD2(P50) connector as UART.
         Please set baud-rate as 115200bps, 8bit-data, no-parity, 1 stop-bit,
         and "LF" only as return code for your console.
         PMOD UART/USB convertor can be used for this signal.
         https://store.digilentinc.com/pmod-usbuart-usb-to-uart-interface/

         Please contact as following.
         PMOD UART/USB ----- RX65N GR-ROSE CN4
         1             -----
         2(TxD)        ----- 10pin
         3             -----
         4             -----
         5(GND)        ----- 5pin
         6(VCC)        ----- 6pin

         [How to connect E2 Emulator Lite]
         GR-ROSE CN3 4pins are connected to RX65N Debug Interface called FINE.
         You can connect E2 Emulator Lite to this pins as following.
         GR-ROSE CN3 ----- Emulator 14pin connector
         pin1(VCC)      ----- pin8
         pin2(GND)      ----- pin2, pin12, pin14
         pin3(MD)       ----- pin7
         pin4(#RES)     ----- pin13

Board: RX64M GR-KAEDE
    [en] http://gadget.renesas.com/en/product/kaede.html
    [ja] http://gadget.renesas.com/ja/product/kaede.html

         The log will be output from CN8 2pin=TxD7(P90) connector as UART.
         Please set baud-rate as 115200bps, 8bit-data, no-parity, 1 stop-bit,
         and "LF" only as return code for your console.
         PMOD UART/USB convertor is provided by Digilent.
         https://store.digilentinc.com/pmod-usbuart-usb-to-uart-interface/

         Please contact as following.
         PMOD UART/USB ----- RX64M GR-KAEDE CN8/CN10
         1             -----
         2(TxD)        ----- CN8-2pin
         3             -----
         4             -----
         5(GND)        ----- CN10-1pin
         6(VCC)        ----- CN10-2pin

Board: RX63N GR-SAKURA II
    [en] http://gadget.renesas.com/en/product/sakura.html
    [ja] http://gadget.renesas.com/ja/product/sakura.html

         The log will be output from CN8 2pin=TxD0(P20) connector as UART.
         Please set baud-rate as 115200bps, 8bit-data, no-parity, 1 stop-bit,
         and "LF" only as return code for your console.
         PMOD UART/USB convertor is provided by Digilent.
         https://store.digilentinc.com/pmod-usbuart-usb-to-uart-interface/

         Please contact as following.
         PMOD UART/USB ----- RX63N GR-SAKURA CN8/CN10
         1             -----
         2(TxD)        ----- CN8-2pin
         3             -----
         4             -----
         5(GND)        ----- CN10-1pin
         6(VCC)        ----- CN10-2pin
         
         Notice:
         GR-SAKURA has 2 types. GR-SAKURA has 128KB-RAM, GR-SAKURA II has 256KB-RAM.
         Please confirm your GR-SAKURA is GR-SAKURA II that has 256KB-RAM RX63N (R5F563NYDDFP).
         Because Amazon FreeRTOS needs RAM size 128KB over.
         
Board: RX65N Target Board + Cloud Option Board (with Silex SX-ULPGN)	
    [en] https://www.renesas.com/us/en/products/software-tools/boards-and-kits/cpu-mpu-boards/rx-family-target-board.html
    [ja] https://www.renesas.com/jp/ja/products/software-tools/boards-and-kits/cpu-mpu-boards/rx-family-target-board.html

         The log will be output from Cloud Option Board CN18 connector as UART.
         Please set baud-rate as 115200bps, 8bit-data, no-parity, 1 stop-bit,
         and "LF" only as return code for your console.

         The communication will be output/input from/to Cloud Option Board CN5(PMOD) as UART.
         Silex SX-ULPGN PMOD can be connected.
         Please contact as following.
         SX-ULPGN PMOD                ----- RX65N Cloud Option Board (PMOD)
         HSUART1-CTS(PMOD 1pin)       ----- PMOD 1pin
         HSUART1-MOSI(PMOD 2pin)      ----- PMOD 2pin
         HSUART1-MISO(PMOD 3pin)      ----- PMOD 3pin
         HSUART1-RTS(PMOD 4pin)       ----- PMOD 4pin
         GND(PMOD 5pin)               ----- PMOD 5pin
         VCC(PMOD 6pin)               ----- PMOD 6pin
         GPIO                         ----- PMOD 7pin
         PWD_L(PMOD 8pin)             ----- PMOD 8pin
         HSUART2-MOSI(SX-ULPGN 15pin) ----- PMOD 9pin(P26)
         HSUART2-MISO(SX-ULPGN 16pin) ----- PMOD 10pin(P30)
         GND(PMOD 11pin)              ----- PMOD 11pin
         VCC(PMOD 12pin)              ----- PMOD 12pin
         
         
WIFI Module: Silex SX-ULPGN PMOD
    [en] https://www.renesas.com/us/en/products/synergy/gallery/partner-projects/silex-wifi-pmod.html
    [ja] https://www.renesas.com/jp/ja/products/synergy/gallery/partner-projects/silex-wifi-pmod.html
    
         You can put this module into followin PMOD connector.
           Renesas Starter Kit+ for RX65N-2MB: PMOD1
         
         And you have to update following settings in <root>/demos/common/include/aws_clientcredential.h
           #define clientcredentialWIFI_SSID       "Paste Wi-Fi SSID here."
           #define clientcredentialWIFI_PASSWORD   "Paste Wi-Fi password here."
           
IDE: CS+ v8.01.00
    [en] https://www.renesas.com/en-us/products/software-tools/tools/ide/csplus.html
    [ja] https://www.renesas.com/ja-jp/products/software-tools/tools/ide/csplus.html

Smart Configurator v2.0.0 (Standalone for CS+): 
    [en] https://www.renesas.com/en-us/products/software-tools/tools/solution-toolkit/smart-configurator.html
    [ja] https://www.renesas.com/ja-jp/products/software-tools/tools/solution-toolkit/smart-configurator.html
    
IDE/Compiler: EWRX/IAR v410
    [en] https://www.iar.com/iar-embedded-workbench/#!?architecture=RX
    [ja] https://www.iar.com/jp/iar-embedded-workbench/#!?architecture=RX

Compiler: GCC for Renesas 4.8.4.201803-GNURX 
    [en] https://gcc-renesas.com/
    [ja] https://gcc-renesas.com/ja/
    
--------------------------------------------------------------------------
Connection Pattern
--------------------------------------------------------------------------
  pattern1: wifi module has TCP/IP and SSL/TLS, Amazon recommends this pattern as RAM<16KB.
  osi	protocol	implemented into
  7	aws		Amazon FreeRTOS
  7	mqtt		Amazon FreeRTOS
  6	ssl/tls		wifi module
  5	socket		wifi module
  4	tcp		wifi module
  3	ip		wifi module
  2	ether		wifi module
  1	phy		wifi module

  RX65N Target Board + Cloud Option Board (with Espressif ESP8266)
  RX231 Target Board + Cloud Option Board (with Espressif ESP8266)
  RX130 Target Board + Cloud Option Board (with Espressif ESP8266)
  RX65N GR-ROSE (with Espressif ESP8266)

  pattern2: MCU has TCP/IP and SSL/TLS and using MCU Ethernet, Amazon recommends this pattern as RAM<64KB.
  osi	protocol	implemented into
  7	aws		Amazon FreeRTOS
  7	mqtt		Amazon FreeRTOS
  6	ssl/tls		Amazon FreeRTOS
  5	socket		Amazon FreeRTOS
  4	tcp		Amazon FreeRTOS
  3	ip		Amazon FreeRTOS
  2	ether		Amazon FreeRTOS(RX Ether specific)
  1	phy		ether-phy

  RX65N RSK <first step>
  RX65N Envision Kit <first step>
  RX65N GR-ROSE
  RXxxN Envision Kit (Murata Type 1FX on board)
  
  pattern3: MCU has TCP/IP and SSL/TLS and using MCU SDIO for wifi, Amazon recommends this pattern as RAM<64KB.
  osi	protocol	implemented into
  7	aws		Amazon FreeRTOS
  7	mqtt		Amazon FreeRTOS
  6	ssl/tls		Amazon FreeRTOS
  5	socket		Amazon FreeRTOS
  4	tcp		Amazon FreeRTOS
  3	ip		Amazon FreeRTOS
  2	ether		Amazon FreeRTOS(RX SDIO-wifi specific)
  1	phy		wifi radio

  RX65N RSK + SDIO wifi SDK (with Murata Type 1FX)
  RX65N Envision Kit + SDIO wifi SDK (with Murata Type 1FX)
  RXxxN Envision Kit (Murata Type 1FX on board)

  patternX: wifi module has TCP/IP and MCU has SSL/TLS, Amazon does NOT recommend this pattern!
            This pattern needs RAM<64KB but only MCU SSL/TLS is running even if both MCU and wifi module
            have SSL/TLS as function. This is very compromised pattern.
            Because Silex wifi module does not have a SSL/TLS client authentication scheme that Amazon
            Web Service requires.
  osi	protocol	implemented into
  7	aws		Amazon FreeRTOS
  7	mqtt		Amazon FreeRTOS
  6	ssl/tls		Amazon FreeRTOS
  5	socket		Amazon FreeRTOS
  4	tcp		wifi module
  3	ip		wifi module
  2	ether		wifi module
  1	phy		wifi module

  RX65N Target Board + Cloud Option Board (with Silex SX-ULPGN)	<first step>
  RX231 Target Board + Cloud Option Board (with Silex SX-ULPGN)	<first step> -> cannot be realized of lack of RAM
  RX130 Target Board + Cloud Option Board (with Silex SX-ULPGN)	<first step> -> cannot be realized of lack of RAM

--------------------------------------------------------------------------
Development Environment (tested or no matrix)
--------------------------------------------------------------------------

Borad number:
 (1)Renesas Starter Kit+ for RX65N-2MB
 (2)RX65N Envision Kit
 (3)RX65N GR-ROSE proto1
 (4)RX64M GR-KAEDE
 (5)Renesas Starter Kit+ for RX65N-2MB + Silex SX-ULPGN PMOD
 (6)RX65N Target Board + Silex SX-ULPGN PMOD
 (7)RX63N GR-SAKURA II

Connection pattern number:
 (1)pattern1: wifi module has TCP/IP and SSL/TLS, Amazon recommends this pattern as RAM<16KB.
 (2)pattern2: MCU has TCP/IP and SSL/TLS and using MCU Ethernet, Amazon recommends this pattern as RAM<64KB.
 (3)pattern3: MCU has TCP/IP and SSL/TLS and using MCU SDIO for wifi, Amazon recommends this pattern as RAM<64KB.
 (4)patternX: wifi module has TCP/IP and MCU has SSL/TLS, Amazon does NOT recommend this pattern!
 
/////////////////////////////////////////////////////////////

IDE number:
 (1)e2 studio
 (2)CS+
 (3)EWRX(IAR)

Compiler number:
 (1)CC-RX
 (2)GCC
 (3)IAR

                 / IDE      (1)         (2)         (3)
Board Connection / Compiler (1) (2) (3) (1) (2) (3) (1) (2) (3)
(1)   (2)        /           X   x       x   -   -   -   -     
(2)   (2)        /           x   x       x   -   -   -   -   * 
(3)   (2)        /           x   x       x   -   -   -   -     
(4)   (2)        /           x   x       x   -   -   -   -     
(5)   (4)        /           *   *       *   -   -   -   -     
(6)   (4)        /           *   *       *   -   -   -   -     
(7)   (2)        /           x   x       x   -   -   -   -     

  X: tested (Amazon FreeRTOS Qualification)
  x: tested (MQTT echo demo)
  *: now trying(still junk)
  -: N/A

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
Following text is SheltyDog memo written in Japanese.
Please ignore this in basically.

----------------------------------------------
Amazon FreeRTOS ポーティング記録
----------------------------------------------
■背景
ルネサス公式コミュニティ「かふぇルネ」でAmazon FreeRTOS関連の
話題があり、NoMaY氏がまとめてくれた情報を元にRX65Nで
Amazon FreeRTOSを動作させることを目指す。
https://japan.renesasrulz.com/cafe_rene/f/forum21/4772/amazon-freertos-rx/

本記録は「かふぇルネ」で活動するシェルティが記す。

--------------------------------------------------------------------------
■進捗
--------------------------------------------------------------------------
RX65N Envision Kit、RX65N RSK(2MB版/暗号器あり品)をターゲットにコードメンテを維持します。
下記 Amazon FreeRTOS 1.2.x は適宜最新コードに更新していきます。
2018/04/30時点での適用コードは 1.2.3 です。

①ルネサスTCP/IPをターゲットで動作させる（Etherの動作確認）
②SDIO無線LANを動作確認した時期のFreeRTOS 8.2.2をターゲットで動作させる
③ルネサスのFreeRTOSパッケージ、9.0.0をターゲットで動作させる
④Amazon FreeRTOS 1.2.xのFreeRTOS部分をターゲットで動作させる
⑤Amazon FreeRTOS 1.2.xのFreeRTOS+TCP部分をターゲットで動作させる
⑥Amazon FreeRTOS 1.2.xのmbed TLS部分をターゲットで動作させる
⑦Amazon FreeRTOS 1.2.xのMQTT部分をターゲットで動作させる（AWSへの接続実験）
⑧Amazon FreeRTOS 1.2.xのFreeRTOS+TCP部分のネットワーク層の結合部分を工夫して、
　(1)Ether、(2)SPI接続無線LANモジュール、(3)SDIO無線LANモジュールの3種類を
　切り替えられるようにする　★一旦停止、後ほど(2)に絞って再開
⑨Amazon FreeRTOS 1.2.xのmbed TLS部分の暗号処理プリミティブの
　ソフトウェア実装（AESとかRSAとか）をRX65N内蔵暗号器を使った
　ハードウェア実装に置き換える　★一旦停止
⑩Ether層のゼロコピーに対応する　★一旦停止
⑪Amazon FreeRTOS本家環境にマージし、Amazon FreeRTOS本家コードへの追従を簡単にできるようにする
⑫Amazon FreeRTOS のGitのforkに登録する
⑬Amazon FreeRTOS のCertificationを受験し合格しGitの本家に登録する　★いまここ＠シェルティ
　https://docs.aws.amazon.com/freertos/latest/userguide/freertos-qualification-program.html

--------------------------------------------------------------------------
■課題まとめ★
--------------------------------------------------------------------------
　2019/03/02
　　e2 studio/CC-RX環境全般において、ファイル更新が無いのにすべてのファイルのビルドがかかる病気がでている。

　2018/12/15
　　スマートコンフィグレータの出力先がプロジェクトファイルの階層以下で固定ということと、
　　AmazonがMCUベンダ側に指定しているデバイスドライバ置き場がプロジェクトファイルの階層外であることを
　　バッチファイルで補正していたが、補正しきれなくなった。
　　スマートコンフィグレータの出力先を可変にできるように急ぐ。
　　
　2018/11/25
　　改行コードをLFにした方が良いのではないか。本家はLF、ルネサスはCRLFと
　　なっている。
　　
　2018/09/24
　　岡宮氏からGR-ROSE(GCC)用のリンカスクリプトをもらった。
　　ユーザアプリによりRAMが割りあたる設定になっている。
　　全体的にRAM割り当てを見直してから一斉に修正することにする。
　　
　2018/08/26
　　性能面で改善点がまだありそうな予感がするので tracealyzer を導入して詳細評価する予定。
　　RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Liteの
　　プロジェクトのスマートコンフィグレータ設定に不要なEtherドライバが含まれているので
　　スマートコンフィグレータの設定を変更する必要がある。NoMaY氏に相談。

　2018/08/05
　　スマートコンフィグレータでRX65N RSK 2MBボードの設定をしていると
　　SCI6でP01、P00を使おうとすると警告がでる。なぜなのか。ツール部門に問い合わせてみる。
　　
　　WIFI環境 の aws_secure_sockets.c が古い状態。最新状態のテンプレートで作り込む。
　　テンプレート：\lib\secure_sockets\portable\freertos_plus_tcp\aws_secure_sockets.c
　　
　2018/07/22
　　スマートコンフィグレータのボード設定で RX65N RSK-2MB を選んでいるのに、
　　BSPで選択されて出力されるボード毎のフォルダが generic_rx65n になっている。
　　⇒将来ジェネリックのみになる予定。ボード依存部はスマートコンフィグレータ出力となる。
　　　⇒8/5 解決。
　　　
　　r_bsp.h 以下ヘッダはAmazon FreeRTOSに存在しない。BSP開発者に修正依頼。
　　ローカルではコメントアウトしておく。
　　/*#include "croutine.h" Amazon FreeRTOS does not have this header file. */
　　/*#include "freertos_start.h" Amazon FreeRTOS does not have this header file. */
　　resetprg.c にも、#include "freertos_start.h" がある。上記と同様にコメントアウトしておく。
　　⇒8/5 BSP開発者に修正依頼済み。
　　
　　コード生成すると、r_bsp_config.h の以下項目が必ず (0)に戻るようだ。
　　コード生成するたびにこの値を確認しなければならない。
　　まだ未対応ならスマートコンフィグレータ側はこの定義を無視するようにした方が良い。
　　#define BSP_CFG_RTOS_USED               (1) // <-- Updated by GUI. Do not edit this value manually
　　⇒現時点では公式未対応機能のためあえてこうしているとのこと。
　　　⇒NoMaY氏に暫定処置していただいた。内容OK。
　　　　⇒8/5 解決。
　　　　
　2018/06/10
　　スマートコンフィグレータのデバイス設定(暗号有品のRX65N)と
　　プロジェクトのデバイス設定(暗号無品のRX65N)とで食い違っていて、
　　スマートコンフィグレータ上で警告が出ている。⇒7/22メンテで調整した。
　　
　2018/05/01
　　2018/05/01のポーティング記録参照
　
　2018/03/17
　　2018/03/17のポーティング記録参照
　　
　2018/03/03以前
　　NetworkInterface.c 
　　(2)ゼロコピーの概念を無視するが...★
　　(4)(送信側のゼロコピーの仕組みが不明。これもまた後で調べる★)
　　(5)LANケーブル挿抜のイベントをTCP/IP側に通知する仕組みがあるが...★

　　その他
　　・システムログをSCIから出力するようにコード追加すればよい。(★後でメンテ)⇒完了。
　　・コンパイル前処理の構文解析でエラーになっている感じ。→★ツール開発部門に問い合わせてみる。

　その他
　　NoMaY氏の提案の調整。2018/3/11 0:00 のポスト参照。
　　https://japan.renesasrulz.com/cafe_rene/f/forum21/4772/amazon-freertos-rx/27511#27511

--------------------------------------------------------------------------
■ポーティング記録	★印が解決すべき課題
--------------------------------------------------------------------------
2019/03/02
　Amazon FreeRTOS Qualification Program 合格し、認証が取れた。
　https://www.renesas.com/jp/ja/about/press-center/news/2019/news20190108.html
　
　OTA対応やSDIO無線LAN対応、スマートコンフィグレータとの連携、FITのGCCI/IAR対応など
　まだまだ整備が足りないところが多い。引き続きメンテしていく必要がある。
　スマートコンフィグレータとの連携、FITのGCCI/IAR対応は正式に仕事としてスタートできた。
　ところで本家のアップデートが溜まってきたようなので、最新版V147とマージを試みリリースビルドを作る。
　それはそうと、対応ボードが増えて動作確認が大変かつ、MQTTエコーの動作確認しかできないので、
　そろそろ自動テスト機構を作る必要がある。
　アメリカの開発部隊がGitLabを利用したマイコン実機活用の自動テスト機構を作っていたので
　これを参考に作ってみよう。ここら辺がゴールデンウィークのネタ。
　・OTA対応
　・SDIO無線LAN
　・自動テスト機構
　
　ひとまずしばらくは手動で我慢してテストしていく。

　以下プロジェクトの動作確認OK。
　このフォルダはAmazon FreeRTOS Qualification Programを通した神様。
　.cprojectや.projectの内容は後々他のプロジェクトに反映する必要がある。
　\demos\renesas\rx65n-rsk\e2studio
　
　以下プロジェクトの動作確認OK。スマートコンフィグレータも動かしなおしてみた。問題なし。
　\demos\renesas\rx65n-rsk\ccrx-e2studio
　ただし、ファイル更新が無いのにすべてのファイルのビルドがかかる病気がでている。
　そのうちプロジェクトを作り直す必要があるか。
　ここまででコミット。

　以下プロジェクトの動作確認OK。スマートコンフィグレータも動かしなおしてみた。問題なし。
　\demos\renesas\rx65n-rsk\gnurx-e2studio
　ここまででコミット。

　以下プロジェクトの動作確認OK。スマートコンフィグレータも動かしなおしてみた。問題なし。
　\demos\renesas\rx65n-rsk\ccrx-csplus
　ここまででコミット。

　以下プロジェクトの動作確認OK。スマートコンフィグレータも動かしなおしてみた。問題なし。
　\demos\renesas\rx65n-rsk\ccrx-e2studio
　ただし、ファイル更新が無いのにすべてのファイルのビルドがかかる病気がでている。
　そのうちプロジェクトを作り直す必要があるか。
　ここまででコミット。
　
　以下プロジェクトの動作確認OK。スマートコンフィグレータも動かしなおしてみた。問題なし。
　\demos\renesas\rx65n-rsk-uart-sx-ulpgn\ccrx-e2studio
　ただし、ファイル更新が無いのにすべてのファイルのビルドがかかる病気がでている。
　そのうちプロジェクトを作り直す必要があるか。
　ここまででコミット。

　以下プロジェクトの動作確認OK。スマートコンフィグレータも動かしなおしてみた。問題なし。
　\demos\renesas\rx65n-rsk-uart-sx-ulpgn\gnurx-e2studio
　ここまででコミット。
　
2018/12/23
　以下プロジェクトの動作確認OK。
　\demos\renesas\rx65n-envision-kit\ccrx-e2studio
　ここまででコミット。
　
　以下プロジェクトの動作確認NG。WIFIは25日週に担当者が確認する手はずなので今回は省略。
　\demos\renesas\rx65n-rsk-uart-sx-ulpgn\ccrx-e2studio
　ここまででコミット。
　
　以下プロジェクトの動作確認OK。
　\demos\renesas\rx65n-envision-kit\gnurx-e2studio
　以下プロジェクトのリンカスクリプトに不要な記述を発見。削除。
　\demos\renesas\rx65n-rsk\gnurx-e2studio
　ここまででコミット。

　以下プロジェクトの動作確認OK。
　\demos\renesas\rx65n-gr-rose\ccrx-e2studio
　ここまででコミット。

　以下プロジェクトの動作確認OK。
　\demos\renesas\rx65n-gr-rose\gnurx-e2studio
　ここまででコミット。

　以下プロジェクトの動作確認OK。
　\demos\renesas\rx65n-gr-rose\gnurx-e2studio-cpp
　ここまででコミット。

　以下プロジェクトの動作確認OK。
　\demos\renesas\rx65n-gr-rose\ccrx-csplus
　ここまででコミット。
　
　以下プロジェクトの動作確認OK。
　\demos\renesas\rx64m-gr-kaede\ccrx-e2studio
　データフラッシュの大きさがRX65Nと違うので、aws_pkcs11_pal.c で保持する
　PKCS11用のストレージ保管庫のハッシュ値の補正が必要だった。狙い通り他のコードは共通でいけた。
　ifdefで機種依存部分は切り分け、ファイル自体は1個で実現。
　aws_pkcs11_pal.cを他のプロジェクトにも同じものをコピーしておく。★
　ここまででコミット。
　
　以下プロジェクトの動作確認OK。
　\demos\renesas\rx64m-gr-kaede\gnurx-e2studio
　リンカスクリプトでB_ETHERNET_BUFFERS_1を16バイト境界に配置しているが、
　32バイト境界が正解。このプロジェクトでたまたま32の倍数ではない16バイト境界に配置され
　Etherが動かなくなっていた。以下補正。他のGCC系のプロジェクトも後でまとめて確認して修正する。★
　BLOCK(16)→BLOCK(32)
　ここまででコミット。

　以下プロジェクトの動作確認OK。
　\demos\renesas\rx64m-gr-kaede\ccrx-csplus
　ここまででコミット。

　以下プロジェクトの動作確認NG。
　\demos\renesas\rx63n-gr-sakura2\ccrx-e2studio
　PKCS11の実装でデータフラッシュ書き込みを行っている。
　RX65N、RX64Mのコードをそのまま動かそうとしたら動かない。
　ハードウェアマニュアルを読むと、RX63Nのデータフラッシュは読み出し・書き込み許可設定が
　ユーザに委ねられているようだ。R_FLASH_Control()コマンドのアクセスウィンドウ設定コマンドで
　データフラッシュは読み出し・書き込み許可設定が可能。RX63xのときだけ実行されるコードを追加し
　正常動作確認OK。
　ここのaws_pkcs11_pal.cを他のプロジェクトにも同じものをあとでコピーしておく。★
　ここまででコミット。
　
　あとふたつ。ゴールが見えてきた。どえらい疲れた。
　以下プロジェクトの動作確OK。
　\demos\renesas\rx63n-gr-sakura2\gnurx-e2studio
　ここまででコミット。

　以下プロジェクトの動作確OK。
　\demos\renesas\rx63n-gr-sakura2\ccrx-csplus
　ここまででコミット。
　
　以下見直し項目をまとめて見直す。
　・CC-RXを使用する場合、コンパイラバージョンをv3.00.00にしておく
　　　(途中で確認項目に追加したので後でまとめて見直す)★
　・aws_pkcs11_pal.cを他のプロジェクトにも同じものをコピーしておく。★
　　→ここ(\demos\renesas\rx63n-gr-sakura2\ccrx-e2studio)のaws_pkcs11_pal.cを
　　　他のプロジェクトにも同じものをあとでコピーしておく。★
　・Etherが動かなくなっていた。以下補正。他のGCC系のプロジェクトも後でまとめて確認して修正する。★
　ここまででコミット。
　
　最後、検定対象のRX65N RSK (Ether)のtestsプロジェクトでテストを通し最終確認、
　また、AWSアップ用のRX65N RSK (Ether)のdemosプロジェクトも最終確認。OK。
　
　ここでコミット/リリースタグ。
　　　
2018/12/22
　大詰め。
　Amazonからのフィードバックを片っ端から片付けていく。
　https://github.com/renesas-rx/amazon-freertos/commit/7e03502a679e74dd69a8978e320763914bccbcce
　https://github.com/renesas-rx/amazon-freertos/commit/aeaa186f8c68e7cf0b59bafdd82d1d4a93737de6
　https://github.com/renesas-rx/amazon-freertos/commit/7dfa70495ef383c0a751e822cd8492b8b53ff22d
　
　おおまかには以下3点が問題。
　　(1)PKCS11の実装において、バッファ8KB確保しているが、オーバーランして書き込みアクセスしている
　　(2)乱数取得関数が毎回同じ値を出力する実装になっている
　　(3)鍵・証明書を書き込んだ状態で、main.c にあるprovisioning関数を動かさず書き込んだデータから
　　　読み込みなおす動作ができていない
　
　対策を打っていく。
　　(1)バッファ8KBの制御を見直す。
　　(2)チップ内蔵の温度計から乱数性を期待して取得する。
　　　(実際は12bitの値で±3程度しか振れないので無いよりマシ程度)
　　　またOSのタスクのTickを取り出す関数を乱数値に足しこんでおく
　　(3)PKCSの初期化関数をオーバーライドするオプションをAmazonチームから教えてもらった
　　　C_Initialize()を実装し、そこでROMのデータをRAMのバッファに展開するようにした
　
　また、Amazon FreeRTOS最新版の v1.4.5とマージしておく。
　あとは、他の環境用のプロジェクトを全部整備すれば完了。

　現状、以下2プロジェクトの動作確認ができている状態。
　\demos\renesas\rx65n-rsk\e2studio
　\latest\tests\renesas\rx65n-rsk\ccrx-e2studio
　
　RX63Nのtests用プロジェクトは不要なのでもう消そう。
　ひとまずここまででコミット。
　
　ほかのプロジェクトの動作確認はおおむね以下を追加していけばよいはず。
　　(1)r_s12ad_rx モジュールをスマートコンフィグレータで追加する
　　　→CS+の場合はスマートコンフィグレータ出力側のr_s12ad_rx モジュールをビルド対象から外しておく
　　　→BSPのスタックの設定を 1stack にしておく。 2stack だとビルド時にエラーがでる。
　　(2)プロジェクト側から、r_s12ad_rx モジュールの改造版を登録する
　　　\lib\third_party\mcu_vendor\renesas\FIT\RDP_v1.15_modified\r_s12ad_rx
　　(3)プロジェクト側から、r_s12ad_rx モジュールの改造版へのインクルードパスを通す
　　(4)aws_pkcs11_pal.c をRX65N RSKのものからコピーする
　　　(機種依存が無いように書いたので、どのRXマイコンでも動くはず。
　　　 今は機種毎にコードをフォルダ分離しているが、うまくいけばマージしても良いであろう)
　　(5)PKCS11の初期化をオーバーライドしているので、コンフィグファイルで以下設定変更をコメントアウトし有効化
　　　\demos\renesas\XXXX-XXXX\common\config_files\aws_pkcs11_config.h
　　　#define pkcs11configC_INITIALIZE_ALT
　　(6)C_PKCS11_STORAGE* を0x00100000番地にセクション追加
　　(7)FreeRTOSのコンフィグでヒープとスタックの量を調整
　　　\demos\renesas\XXXX-XXXX\common\config_files\FreeRTOSConfig.h
　　　#define configMINIMAL_STACK_SIZE                   ( ( unsigned short ) 512 )
　　　#define configTOTAL_HEAP_SIZE                      ( ( size_t ) ( 128U * 1024U ) )
　　(8)CC-RXを使用する場合、コンパイラバージョンをv3.00.00にしておく
　　　(途中で確認項目に追加したので後でまとめて見直す)★
　
　以下プロジェクトの動作確認OK。
　\demos\renesas\rx65n-rsk\ccrx-e2studio
　ここまででコミット。

　以下プロジェクトの動作確認NG。
　\demos\renesas\rx65n-rsk\gnurx-e2studio
　フラッシュのEraseのAPIがエラーを吐いている。
　データフラッシュへのデータ配置がうまくいってない。リンカスクリプトを見直す。
　見直した結果、動作確認OK。
　ここまででコミット。
　
　以下プロジェクトの動作確認OK。
　\demos\renesas\rx65n-rsk\ccrx-csplus
　ここまででコミット。
　
2018/12/15
　09fox氏が以下2点解析。Amazonからもアドバイスが届いた。
　結果原因を突き止めることができた。
　(1)AFQP_KeyGenerationEcdsaHappyPath
　(2)AFQP_TLS_ConnectEC

　(1)はPKCSの実装がおかしく、Amazon FreeRTOSの期待値と合っていなかったのが原因。
　具体的には、「ラベル + 鍵種」の値毎にハンドルを作っていたところ、
　「ラベル」毎にハンドルを作る（鍵種が異なってもハンドルは同じ）のが正解だった。
　
　(2)はシェルティのAWSアカウントのテスト用証明書が有効化されていなかったのが原因。
　有効にしたらあっさり通った。
　
　これで、テスト全件OKとなった。以下コミットの状態でテスト全件OKを確認。
　https://github.com/renesas-rx/amazon-freertos/commit/f9a3ef02655fa0d6735379dd8b1516ff878db612
　
　残件整理した結果、今週末は以下のように進める。
　
　- 各種開発環境の更新（e2 studio, CS+, Smart Configurator, CC-RX）
　  →RX Driver Packageの更新は見送り。
　- PKCSの実装で、データ保存をRAM上にしているが、フラッシュ上に保存するよう変更
　- ルネサスアメリカ、Amazonからの各種フィードバックを改めて確認
　- WIFIのSSID/PASS、AWSのアカウント情報のProvisioningができるモードを実装する。
　　ただしこれは検定用ではなく、RX65N Amazon FreeRTOS対応ボードの初期ファーム用実装。
　　なので、ブランチを作ってそちらで機能追加を行う。
　- リリースタグを作って検定に回す
　- v145の適用は週明けから考える

　【PKCSの実装で、データ保存をRAM上にしているが、フラッシュ上に保存するよう変更】
　
　データフラッシュは、64*512 byte = 32KBある。
　ひとまずその1/4の8KBを2面持ちして電源断対策を施す。
　書き込み用のRAMバッファは同じ容量の8KB用意する。
　フラッシュAPIに用意されているデータフラッシュのブロックサイズと
　ブロック数を表すマクロを使って配列の要素数を決めておく。
　
　static uint8_t local_storage[60000];
　↓
　static uint8_t local_storage[(FLASH_DF_BLOCK_SIZE * FLASH_NUM_BLOCKS_DF)/4];
　
　RX65N Envision Kitの初期ファームのコードから、データフラッシュの電源断対策の
　コードを移植する。具体的には以下3関数。
　
　static void update_dataflash_data_from_image(void);
　static void update_dataflash_data_mirror_from_image(void);
　static void check_dataflash_area(uint32_t retry_counter);
　
　さらに、データ構造体も移植し、PKCS用に作り直す。

　typedef struct _pkcs_storage_control_block_sub
　{
	uint8_t local_storage[((FLASH_DF_BLOCK_SIZE * FLASH_NUM_BLOCKS_DF)/4)-PKCS_SHA256_LENGTH];	/* RX65N case: 8KB */
　}PKCS_STORAGE_CONTROL_BLOCK_SUB;

　typedef struct _PKCS_CONTROL_BLOCK
　{
	PKCS_STORAGE_CONTROL_BLOCK_SUB data;
	uint8_t hash_sha256[PKCS_SHA256_LENGTH];
　}PKCS_CONTROL_BLOCK;
　
　RX65N Envision Kitの初期ファームはルネサスのSHA1ライブラリを使っているので、
　こちらではmbed TLSのSHA256を使うよう、各種定義やコードを変更していく。
　
　以下コードフラッシュにデータを設置する必要がある。
　
　R_ATTRIB_SECTION_CHANGE(C, _PKCS11_STORAGE, 1)
　static const PKCS_CONTROL_BLOCK pkcs_control_block_data = {PKCS_CONTROL_BLOCK_INITIAL_DATA};
　R_ATTRIB_SECTION_CHANGE_END

　R_ATTRIB_SECTION_CHANGE(C, _PKCS11_STORAGE_MIRROR, 1)
　static const PKCS_CONTROL_BLOCK pkcs_control_block_data_mirror = {PKCS_CONTROL_BLOCK_INITIAL_DATA};
　R_ATTRIB_SECTION_CHANGE_END
　
　が、r_compiler.h の コンパイラ差分吸収マクロ群に、セクションC用の
　R_ATTRIB_SECTION_CHANGE()が無かったので追加。
　
　#define _R_ATTRIB_SECTION_CHANGE_C1(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C, C##section_tag) /* The CC-RX adds postfix '_1' automatically */
　#define _R_ATTRIB_SECTION_CHANGE_C2(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C, C##section_tag) /* The CC-RX adds postfix '_2' automatically */
　#define _R_ATTRIB_SECTION_CHANGE_C4(section_tag)           __R_ATTRIB_SECTION_CHANGE_V(C, C##section_tag) /* The CC-RX does not add postfix '_4' */
　
　GCC側も同じ法則で追加。IARは落ち着いたら対応しよう。
　
　ここまでで、テスト全件流してみる。問題なし。
　一旦コードを登録する。

　【ルネサスアメリカ、Amazonからの各種フィードバックを改めて確認】
　あらかた片付いているが、残っているフィードバックもある。以下。
　(1) This Renesas specific code should be in demos/renesas/rx65n-rsk/common/renesas_code folder or in lib/third_party/mcu_vendor/renesas
　(2) For the folder demos/renesas/rx65n-rsk/ccrx-e2studio
　    I would like to rename this to demos/renesas/rx65n-rsk/e2studio.
　    We do not typically put the toolchain prepending the IDE name.
　    If there are strong reasons to prepending the compiler to the project name then use the following: demos/renesas/rx65n-rsk/e2studio/ccrx/
　
　(1)はルートにぶら下がっているsrcフォルダを、ルネサスフォルダのどちらかに移動させるべし、という指摘。
　これは指摘通り。直した方が良いであろう。demos/renesas/rx65n-rsk/common/renesas_codeに移動する。
　実態を移動するわけではなくて、仮想フォルダを作って移動になるので、大した影響はなさそうだが、
　NoMaY氏のバッチファイル関連がうまく移動できるかが課題か？　とにかくやってみる。
　
　まずはプロジェクトファイルと同列にある srcフォルダをリソースフィルタで除外対象とする。
　つぎにdemos/renesas/rx65n-rsk/common/renesas_codeに\lib\third_party\mcu_vendor\renesas\amazon_freertos_commonを
　リンクされたフォルダ機能で登録する。
　つぎにdemos/renesas/rx65n-rsk/common/renesas_codeにFIT_modified_codeを仮想フォルダ機能で登録する。
　FIT_modified_codeに各種FITモジュールをリンクされたフォルダ機能で登録する。
　必要なリソースフィルタも適宜登録していく。
　つぎにdemos/renesas/rx65n-rsk/common/renesas_codeに\demos\renesas\rx65n-rsk\ccrx-e2studio\src\smc_genを
　リンクされたフォルダ機能で登録する。
　さらに、インクルードパスを調整。
　
　ビルド、実行。問題なし。
　一旦コードを登録する。
　
　この状態だと、スマートコンフィグレータでコード生成するとコンパイルが通らなくなる。
　原因を解析したところ、スマートコンフィグレータがインクルードパスを1行ずつチェックし、
　smc_genが含まれる行を全削除し、かならずプロジェクト配下のsmc_genにインクルードパスを
　通しなおす動きをしていることが原因と判明。
　
　NoMaY氏のバッチファイルを改造して、smc_gen以下のr_config、r_pincfg、generalの各フォルダ・内部のファイルを
　smc_genと同列にtmpフォルダを生成してここに移し替えることにした。
　tmpフォルダをリンクされたフォルダ機能で登録する。
　さらに、インクルードパスを調整。

　これで、スマートコンフィグレータでコード生成してもインクルードパスが消されずに済む。
　ビルド、実行、問題なし。
　
　tmpフォルダに既に各種ファイルがある状態からビルドすればOKであるが、
　tmpフォルダが無い状態で、ビルドすると、smc_genからコピーしてコンパイルが実行されるまでの間に、
　プロジェクトがtmp内のコードを認識してくれず、エラーになる。
　
　もう一度ビルドすればビルドできるが・・・。
　この状態でコミットするのはいかがなものか。GCCもIARもまだスマートコンフィグレータと
　連携できてないので、CC-RXも一旦スマートコンフィグレータ連携は保留にしよう。
　
　課題をスマートコンフィグレータのチームに連絡。
　早くスマートコンフィグレータの出力先を可変にできるようにしてもらいたい。
　
　Readmeのみ更新。
　
2018/12/01
　引き続きテスト環境の調整。
　平日に別メンバーが進めた進捗を確認。
　エラー件数は98個のテスト項目に対して全体で14件(TCP:3, MQTT:2,TLS:2,PKCS:7)。
　
　TCPのエラーを見てみる。
　-AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasks 
　-AFQP_SECURE_SOCKETS_Threadsafe_DifferentSocketsDifferentTasks
　-AFQP_SOCKETS_Socket_InvalidInputParams
　
　AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasksを動かしてみると、
　通信時にTCPウィンドウが足りずに再送等が発生しテストがスムーズに動いていないようだ。
　過去性能評価したときに調整した設定パラメータに変更することでテストOKになった。

　-----引用-----
　　まず、\demos\renesas\rx65n-rsk\common\config_files\FreeRTOSIPConfig.h
　デフォルトOFFになっているが、以下TCPウィンドウウィングメカニズムをONに
　することでハードウェアの性能を最大限に引き出すことができる。
　/* USE_WIN: Let TCP use windowing mechanism. */
　#define ipconfigUSE_TCP_WIN                            ( 0 )
　
　また、性能を出すためにはTCPウィンドウウィングのために多くのRAMが
　必要となる。以下設定変更を施すことでハードウェア性能が引き出せる。
　/* Define the size of Tx buffer for TCP sockets. */
　#define ipconfigTCP_TX_BUFFER_LENGTH                   ( 1460*8 )

　さらにEtherドライバの受信ディスクリプタも複数用意する必要がある。
　\demos\renesas\rx65n-rsk\ccrx-e2studio\src\smc_gen\r_config\r_ether_rx_config.h
　/* The number of Rx descriptors. */
　#define ETHER_CFG_EMAC_RX_DESCRIPTORS               (12)
　/* The number of Tx descriptors. */
　#define ETHER_CFG_EMAC_TX_DESCRIPTORS               (4)
　-----引用終わり-----
　
　AFQP_SECURE_SOCKETS_Threadsafe_DifferentSocketsDifferentTasksは、
　SSL/TLS接続を別ソケット別タスクで並行して10回行うテスト。
　Amazon FreeRTOSが想定する完了時間がタイムアウト時間として設定されているようで、
　RX65Nでは正しく動いてはいるが時間切れになっているようだ。
　最適化がOFFになっていたので、ONにして試してみる。
　ギリギリセーフ？　OKになった。
　
　AFQP_SOCKETS_Socket_InvalidInputParamsは異常なソケット値を入れて
　正しく検出するかのテスト。
　正しく検出してvAssertCalled()を呼び出されてはいるが、ここでTEST_ABORT()してないので
　そのまま次の処理に進んでいるようだ？
　FreeRTOSConfig.hのconfigASSERT()の実装周りが期待値に達していないようだ。
　configASSERT()からTEST_ABORT()が呼ばれている。unityのインクルードファイルを呼ばないと
　TEST_ABORT()のシンボルが見つからないので、#include "unity_internals.h" を追加。
　これでOKになった。
　
　次にTLSのエラーを見てみる。
　-AFQP_TLS_ConnectMalformedCert(落ちるべきテストが通る)
　-AFQP_TLS_ConnectUntrustedCert(落ちるべきテストが通る)
　
　TLSテスト側がテスト用の証明書と秘密鍵を登録しようとするが、PKCSの実装体(aws_pkcs11_pal.c)において
　PKCS11_PAL_SaveObject()で同じラベル名が指定されても、リストに追加する実装になっていた。
　同じラベル名が指定されたら、リスト中の同名のラベル名の登録を解除しなければならない。
　このテストでは、システムで使用するラベル①と、テスト用のラベル②があり、ラベル①はシステム初期化時に、
　ラベル②はTLSテスト初期化時にPKCS11_PAL_SaveObject()で渡される。
　TLSテストは後にPKCS11_PAL_FindObject()を使用してラベル②に紐づくハンドルが出力されることを期待するが、
　ラベル①に紐づくハンドルが出てきてしまう。ラベル①に紐づくハンドルに紐づく証明書、秘密鍵は正常なので
　落ちるべきテストが通ってしまう。
　PKCS11_PAL_SaveObject()とPKCS11_PAL_FindObject()の実装を修正してテストOKになった。
　逆に、以下3個がエラーになった。テスト用の証明書データと秘密鍵がうまく作れていないようだ。これは後で確認する。
　AFQP_TLS_ConnectEC
　AFQP_TLS_ConnectRSA
　AFQP_TLS_ConnectBYOCCredentials
　
　ここまでで一旦コードを登録。
　
　次にMQTTのエラーを見てみる。
　以下2個がエラーを出力している。
　AFQP_MQTT_Init_NULLParams
　AFQP_MQTT_Connect_NULLParams
　
　mqttconfigASSERT()がabortできていないことが原因のようだ。
　aws_mqtt_config.h のmqttconfigASSERT()の実装を修正し全件OKとなった。
　
　次にPKCSのエラーを見てみる。
　以下7個がエラーを出力している。
　AFQP_Verify_HappyPath
　AFQP_Verify_InvalidParams
　AFQP_TestRSAExport
　AFQP_TestECDSAExport
　AFQP_SignVerifyRoundTripWithCorrectRSAPublicKey
　AFQP_SignVerifyRoundTripWithCorrectECPublicKey
　AFQP_KeyGenerationEcdsaHappyPath
　
　AFQP_Verify_HappyPathを追いかけてみる。
　どうやらPKCS11関連の証明書、秘密鍵をストレージに保存する際に属性情報がうまく保持できていないようだ。
　public keyなのにprivate keyと判定されて属性エラーになって落ちている様子。
　
　PKCSの実装体(aws_pkcs11_pal.c)を見直す。
　pkcs_data[xHandle - 1].Label.type の値で判断していたが、他社の実装を参考に
　pkcs_data[xHandle - 1].Label.value の値がpkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLSなどと
　一致しているかどうかで判断するように変更。
　
　これでエラーは残り以下2点になった。
　
　AFQP_SignVerifyRoundTripWithCorrectECPublicKey
　AFQP_KeyGenerationEcdsaHappyPath
　
　local_storage[]の容量が足りないようなので増やしておく。
　PKCS11_PAL_SaveObject()ですでに登録済みのラベルと同じラベルが来た時に、
　すでに登録済みのラベルはPKCS_DATA_STATUS_DELETED状態にしているが、
　すでにPKCS_DATA_STATUS_DELETED状態のラベルが居る場合の実装が抜けていた。
　すでにPKCS_DATA_STATUS_DELETED状態のラベルは無視してサーチするように変更。
　
　これでエラーは残り以下1点になった。
　
　AFQP_KeyGenerationEcdsaHappyPath
　
　以下aws_pkcs11_mbedtls.c の1482行目あたりのコードが影響して署名検証に失敗している様子。
　意図が分からないのでAmazon側に確認。
　ひとまず保留。
　
        *pxPrivateKey = PKCS11_PAL_SaveObject( &pxPrivateTemplate->xLabel, pucDerFile + pkcs11KEY_GEN_MAX_DER_SIZE - xResult, xResult );
        /* FIXME: This is a hack.*/
        *pxPublicKey = *pxPrivateKey + 1;
        xResult = CKR_OK;

　ここまでで、全テストを通しで走らせてみる。エラーは残り2件。もう少しだ。

　AFQP_KeyGenerationEcdsaHappyPath
　AFQP_TLS_ConnectEC
　
　ここまでで一旦コードを登録。

2018/11/25
　3連休のまとめ。他の仕事があるので3日目はAmazon関連はこれにて終了。
　テスト環境はだいたい整った。
　　https://github.com/renesas-rx/amazon-freertos/tree/master/tests
　　→Amazon FreeRTOS Qualification Program Developer Guide.pdf
　　
　マニュアルが良くできている。これを見ればだいたい分かる。
　テストは最低限以下が必要。
　　testrunnerFULL_TCP_ENABLED
　　testrunnerFULL_MQTT_ENABLED
　　testrunnerFULL_PKCS11_ENABLED
　　testrunnerFULL_TLS_ENABLED
　　testrunnerFULL_WIFI_ENABLED
　　
　現時点でRX63N GR-SAKURAにてtestrunnerFULL_TCP_ENABLEDが概ね動いているところ。
　動いてないテストは以下2点。（AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasksの
　後にまだ動いてないテストがある可能性有り）
　　TEST(Full_TCP, AFQP_SOCKETS_Socket_InvalidInputParams)
　　TEST(Full_TCP, AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasks)
　
　AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasksのあとにヒープ不足になって
　システムが停止する状態。
　
　Full_TCPテストの通信相手は以下2種類。
　　TCPのエコーサーバ
　　TLSのエコーサーバ
　　
　これらはgo言語で書かれたプログラムで動作する。必要に応じてgo言語をPC環境にインストールが必要。
　サーバプログラムは以下に格納されている。
　　https://github.com/renesas-rx/amazon-freertos/tree/master/tools/echo_server
　
　これらを動かしておいて、マイコン上のソフトは以下コードにサーバ情報を設定しておけば良い。
　　https://github.com/renesas-rx/amazon-freertos/blob/master/tests/common/include/aws_test_tcp.h
　
　サーバ証明書をセットする必要があるが、PEMからC言語に変換するツールが準備されている。
　　https://github.com/renesas-rx/amazon-freertos/blob/master/tools/certificate_configuration/PEMfileToCString.html
　
　マニュアルを見てみるとあとややこしい感じがするのは、Appendix G: TLSくらいか。
　改ざんされた証明書データを準備する必要などがあるようだが、まあマニュアル通りに進めていけば大丈夫そう。
　
　ひとまず以上。

2018/11/24
　GR-SAKURAでテスト環境を構築。
　ひとまずビルドが通って何かしらテストログが出力されることを確認。
　ただし、DHCPが完了する前にテストが走り始めてしまい、うまくいかない。
　ネットワーク初期化周りを調整する必要がありそうだ。
　
　NetworkInterface.c で リンク状態をチェックするためにソフトウェアタイマを立ち上げているが
　起動後しばらくしないと動かないようだ。受信タスクは動いているので受信タスクと同じよう
　タスクでリンク状態を確認するよう変更。
　共通コードなので他の環境に影響していないか要確認。（多分大丈夫なはず）
　
　テストが動き出した。以下でFailになっている。
　TEST(Full_TCP, AFQP_SOCKETS_Socket_InvalidTooManySockets)
　FAIL: Expected 1 Was 0. Max num sockets test failed
　
　aws_test_tcp.c でテストOKになっているテストケースは#if 0で省略しておく。（コミットはしない）
　
　デバッガで中身を見てみると、2個目のソケットの生成でエラーを吐いている様子。
　→prvSOCKETS_Socket_InvalidTooManySockets()の1772行目
　　xSocket = SOCKETS_Socket( SOCKETS_AF_INET, SOCKETS_SOCK_STREAM, SOCKETS_IPPROTO_TCP );

　1746行目に次のようなコメントがあり、ifdef 対象環境ではテストが無効化されている。
　/* Socket can be created as much as there is memory */

　このテストはTCP/IPを無線LANモジュール側にオフロードしている場合に、無線LANモジュール側が
　生成できるソケットの限界値を確かめるためのテストだ。
　RXマイコンもメモリがあるだけソケットを作れるのでテストを無効化して良いはず。
　ただし、同じRXマイコンでも環境によってはTCP/IPを無線LANモジュール側にオフロードするので
　さらに分岐が必要。要検討。とりあえず __RX のときはテスト無効化とする。
　AFQP_SOCKETS_Socket_InvalidTooManySocketsのテストがOKとなった。
　
　次のエラーはAFQP_SOCKETS_Socket_InvalidInputParamsだがよく分からない。保留。
　ここまででTCPのパラメータ入出力関係のテストがOKになるようだ。
　
　次のテストは暗号化されたエコーサーバとの通信テスト。
　AWSのクレデンシャルを設定してそれを使ってテストするようだ。
　以下のようにaws_test_tcp.hにコメントがある。
　
　/* Encrypted Echo Server.
　* If tcptestSECURE_SERVER is set to 1, the following must be updated:
　* 1. aws_clientcredential.h to use a valid AWS endpoint.
　* 2. aws_clientcredential_keys.h with corresponding AWS keys.
　* 3. tcptestECHO_SERVER_TLS_ADDR0-3 with the IP address of an
　* echo server using TLS.
　* 4. tcptestECHO_PORT_TLS, with the port number of the echo server
　* using TLS.
　* 5. tcptestECHO_HOST_ROOT_CA with the trusted root certificate of the
　* echo server using TLS. */
　
　1と2は実験用のクレデンシャルデータを使えばOK。
　tcptestECHO_SERVER_TLS_ADDR0-3はなんぞ？
　デフォルトで入っている 34.218.25.197 は何だろう。

　whoisで調べたら以下がでてきた。
　ec2-34-218-25-197.us-west-2.compute.amazonaws.com
　
　よく分からないのでマニュアルを見てみよう。
　https://github.com/renesas-rx/amazon-freertos/tree/master/tests
　　→Amazon FreeRTOS Qualification Program Developer Guide.pdf
　　
　tcptestECHO_SERVER_TLS_ADDR0で検索したら出てきた。
　Appendix L: TLS Server Setup
　
　ローカルでTLSサーバを立ててそれを対向にしてテストを実行するようだ。
　OpenSSL の go を使うようだ。cygwinで動かしてみよう。
　go をWindowsにインストールしたら cygwin で動いた。
　https://golang.org/dl/
　
　RXマイコン側に設定を施す。ローカルのTLSサーバのIPアドレスを
　tcptestECHO_SERVER_TLS_ADDR0-3に入れて、tcptestECHO_PORT_TLSのポート番号をセット。
　TLSサーバ側のポート番号は9000番になった。
　tcptestECHO_HOST_ROOT_CAは、適当に作ったオレオレ証明書を貼れば良いようだ。

　確かAmazon FreeRTOSのパッケージの中のtoolsフォルダに
　PEMをC言語に変換する便利ツールが入っていたはず。気が利いてますな。
　https://github.com/renesas-rx/amazon-freertos/blob/master/tools/certificate_configuration/PEMfileToCString.html
　
　ここまでで以下テストがパスするようになった。
　TEST(Full_TCP, AFQP_SECURE_SOCKETS_CloseInvalidParams) PASS
　TEST(Full_TCP, AFQP_SECURE_SOCKETS_CloseWithoutReceiving) PASS
　TEST(Full_TCP, AFQP_SECURE_SOCKETS_ShutdownInvalidParams) PAS
　TEST(Full_TCP, AFQP_SECURE_SOCKETS_ShutdownWithoutReceiving) PASS
　TEST(Full_TCP, AFQP_SECURE_SOCKETS_Recv_On_Unconnected_Socket) PASS
　
　NGが出ているのは以下。
　TEST(Full_TCP, AFQP_SECURE_SOCKETS_Threadsafe_SameSocketDifferentTasks)
　
　ひとまずここまでコミットしてみる。

2018/11/23
　しばらくGitHub上の公式のアップデートを行っていなかったが開発自体は順調に推移。
　主にボードの量産手配やテストの段取りを進めている。
　テストはAmazon FreeRTOSのかなり前のバージョンを土台にして6エラーを残して通った状況。
　最新版でPKCS周りの実装及びテストが変更になったため、GitHub上で関係者間でコード共有し
　最新版でテスト環境の最終整備を行う方向で進める。
　とはいえ11/23-25の3連休は休みであるためシェルティの私物のGR-SAKURAとE2 Liteで
　開発を進める。26日以降に受験対象の環境であるRX65N RSK (Ether)の環境で
　テスト環境の最終整備を継続していく。
　
　まずはRX63N GR-SAKURAの環境の再確認。
　e2 studioのCC-RX環境から。
　特に問題なくビルドは通るがRAMが128KBギリギリになっている。
　NoMaY氏と相談し、256KBのRAM搭載のGR-SAKURA限定にすることで調整済み。
　各種デバイス設定を「R5F563NYDDFP」に変更。これでRAMが厳しい状況が改善した。
　合わせてFreeRTOSのヒープ量も86KBから128KBに変更。動作確認OK。
　
　CS+のCC-RX環境の動作確認。
　各種デバイス設定を「R5F563NYDDFP」に変更。
　セクション設定でROMの開始位置が0xffc00000から0xfff00000に変更。
　特に問題なし。動作確認OK。
　
　続いてe2 studioのGCC環境。
　各種デバイス設定を「R5F563NYDDFP」に変更。
　特に問題なし。動作確認OK。
　ただし1個課題発見。現状SCI経由UARTでPCにシステムログを送っているが、
　printf()経由で仮想コンソール（E1_DBG_PORT.RX_DATA/E1_DBG_PORT.TX_DATA)と通信する場合に
　パスが分からない。CC-RXの場合は、printf()->標準関数ライブラリ->charput() (r_bspのlowlvl.c)->E1_DBG_PORT.TX_DATAの
　経路であるがGCCの場合はどうか。GCCのマニュアルを確認してBSPに実装する必要がある。
　⇒BSPの開発者にインプット。
　
2018/09/24
　NoMaY氏がBSPを共通化してくれている。
　この状態でひとまず全環境の動作を確認し、リリースビルドを作成する。

　RX65N RSK CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N RSK CC-RX CS+ with E2 Emulator Lite...OK
　RX65N RSK GCC e2 studio with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite...NG
　RX65N Target Board + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite (on board)
　RX65N Target Board + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite (on board)
　RX65N Target Board + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite (on board)
　RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)...OK
　RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite
　RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite
　RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite
　RX64M GR-KAEDE CC-RX e2 studio with E2 Emulator Lite
　RX64M GR-KAEDE CC-RX CS+ with E2 Emulator Lite
　RX64M GR-KAEDE GCC e2 studio with E2 Emulator Lite
　
　RX65N RSK + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite の環境でNGとなった。
　追いかけていったところ、sx_ulpgn_driver.c の sx_ulpgn_uart_sci_handle が
　main.c の198行目のconfigPRINTF( ( "WiFi module initialized. Connecting to AP...\r\n" ) );、
　奥底のsnprintf()を実行したところで破壊される。
　
　FreeRTOSのスタックを増やしたら直った。
　#define configMINIMAL_STACK_SIZE                   ( ( unsigned short ) 140 )
　#define configMINIMAL_STACK_SIZE                   ( ( unsigned short ) 180 )
　
　他のプロジェクトもスタックを増やしておく。
　もう一度テストやりなおし。
　
　RX65N RSK CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N RSK CC-RX CS+ with E2 Emulator Lite...OK
　RX65N RSK GCC e2 studio with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite...OK
　RX65N Target Board + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite (on board)...OK デバッガのデバイス設定を修正
　RX65N Target Board + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite (on board)...OK デバッガの電源設定を修正
　RX65N Target Board + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite (on board)
　RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)...OK
　RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite...OK
　RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite...OK
　RX64M GR-KAEDE CC-RX e2 studio with E2 Emulator Lite...OK
　RX64M GR-KAEDE CC-RX CS+ with E2 Emulator Lite...OK
　RX64M GR-KAEDE GCC e2 studio with E2 Emulator Lite...OK
　
2018/09/08
　NoMaY氏が本家最新版v141に追従してくれている。
　RX65N Target Board用の拡張ボードが仕上がってきた。
　最終的にはRX65N Target Board + 拡張ボード + Silex SX-ULPGN のセット品で
　販売する計画。現状は拡張ボードの仕様がまだウェブ公開されてないため、
　RX65N Target Boardに直接Silex SX-ULPGNを接続した体でGitHubにはコード登録を
　行っていく。あと、BSPをマージする話が持ち上がっているがまだ作戦が完璧に
　仕上がってないため、BSPは現状維持（GNU、CC-RX別個）のままとする。
　BSP周りは別途調整する。
　この状態でひとまず全環境の動作を確認し、リリースビルドを作成する。

　RX65N RSK CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N RSK CC-RX CS+ with E2 Emulator Lite...OK
　RX65N RSK GCC e2 studio with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite...OK
　RX65N Target Board + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite (on board)...OK
　RX65N Target Board + Silex SX-ULPGN PMOD CC-RX CS+with E2 Emulator Lite (on board)...OK
　RX65N Target Board + Silex SX-ULPGN PMOD GCC e2 studio with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)...OK
　RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite...OK
　RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite...OK
　RX64M GR-KAEDE CC-RX e2 studio with E2 Emulator Lite...OK
　RX64M GR-KAEDE CC-RX CS+ with E2 Emulator Lite...OK
　RX64M GR-KAEDE GCC e2 studio with E2 Emulator Lite...OK

　RX65N GR-ROSEのEtherのLINKSTA設定がなぜか有効になっていたので無効化した。
　
2018/08/26
　NoMaY氏が本家最新版v132に追従してくれている。
　その他、細かい調整を実施したり、モッチー氏の協力もありGR-KAEDE用の
　プロジェクトも追加できた。それぞれのプロジェクトの動作確認を実施し
　リリースビルドを作成する。
　
　RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Liteの
　プロジェクトのスマートコンフィグレータ設定に不要なEtherドライバが含まれているので
　スマートコンフィグレータの設定を変更する必要がある。NoMaY氏に相談。
　
　また、性能評価を実施した。性能評価の過程で、Etherドライバのハンドリングに
　不備があることが分かった。
　\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.c
　ソフトウェアが追い付かずに、FIFOにデータが溜まる状況において、
　FIFOに溜まったデータを適切に解放できないケースがあり、それが続くと
　やがてFIFOが溢れて通信が止まるというもの。パケットロスがあったら
　FIFOから処理しきれなかったパケットを取り出し捨てる機構を追加。
　
　以下は性能評価レポート。
　
　FreeRTOS+TCPは非常によくできている。
　性能を引き出すためにはネットワーク関連パラメータの調整が必要。

　まず、\demos\renesas\rx65n-rsk\common\config_files\FreeRTOSIPConfig.h
　デフォルトOFFになっているが、以下TCPウィンドウウィングメカニズムをONに
　することでハードウェアの性能を最大限に引き出すことができる。
　/* USE_WIN: Let TCP use windowing mechanism. */
　#define ipconfigUSE_TCP_WIN                            ( 0 )
　
　また、性能を出すためにはTCPウィンドウウィングのために多くのRAMが
　必要となる。以下設定変更を施すことでハードウェア性能が引き出せる。
　/* Define the size of Tx buffer for TCP sockets. */
　#define ipconfigTCP_TX_BUFFER_LENGTH                   ( 1460*8 )

　さらにEtherドライバの受信ディスクリプタも複数用意する必要がある。
　\demos\renesas\rx65n-rsk\ccrx-e2studio\src\smc_gen\r_config\r_ether_rx_config.h
　/* The number of Rx descriptors. */
　#define ETHER_CFG_EMAC_RX_DESCRIPTORS               (12)
　/* The number of Tx descriptors. */
　#define ETHER_CFG_EMAC_TX_DESCRIPTORS               (4)

　RX65N@120MHzでTCP/IP通信の中で最も性能を出すことが難しいTCP送信において、
　45Mbpsを安定的に出せることを確認。
　改善点がまだありそうな予感がするので tracealyzer を導入して詳細評価する予定。
　https://percepio.com/tracealyzer/
　
　この状態でリリースビルドを作成。以下環境を順次実機動作確認していく。
　
　RX65N RSK CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N RSK CC-RX CS+ with E2 Emulator Lite...OK
　RX65N RSK GCC e2 studio with E2 Emulator Lite...OK
　RX65N RSK + Silex SX-ULPGN PMOD CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N Envision Kit CC-RX CS+ with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit CC-RX e2 studio with E2 Emulator Lite (on board)...OK
　RX65N Envision Kit GCC e2 studio with E2 Emulator Lite (on board)...OK
　RX65N GR-ROSE CC-RX e2 studio with E2 Emulator Lite...OK
　RX65N GR-ROSE CC-RX CS+ with E2 Emulator Lite...OK
　RX65N GR-ROSE GCC e2 studio with E2 Emulator Lite...OK
　RX64M GR-KAEDE CC-RX e2 studio with E2 Emulator Lite...OK エミュレータ設定変更：電源供給OFF
　RX64M GR-KAEDE CC-RX CS+ with E2 Emulator Lite...OK エミュレータ設定変更：E1->E2 Lite、電源供給OFF
　RX64M GR-KAEDE GCC e2 studio with E2 Emulator Lite...OK エミュレータ設定変更：E1->E2 Lite、電源供給OFF

　全部OK。問題なし。
　
2018/08/10
　夏休みだ！
　
　初手GitHubを確認。NoMaY氏の作業が完了し、ファイル構成が絞れている。
　更新漏れのファイルなども補完いただいている様子。毎度感謝です。
　担当者がWIFI関連のコードを修正完了したとのことでこのマージを行う。
　綺麗にマージできていれば、FreeRTOS+TCP以下のコードは使用しなくて
　済むはずだ。プロジェクト登録から外す。
　ビルドしてみるとFreeRTOS+TCP関連のヘッダのインクルードで引っかかる。
　このインクルードを全部削除していく。
　FreeRTOS+TCPから呼ばれる関数も削除。
　void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
　get_random_number()は依然として必要なようだ。リンクエラーで残る。
　これは、/demos/renesas/rx65n-rsk-uart-wifi/common/application_code/entropy_hardware_poll.c に
　移植しておく。
　ビルド。正常動作確認。OK。相変わらずAWSに繋がるまでが遅い。
　なぜだ。とりあえず保留。
　　⇒ちょっとアクセスポイントとボードを離すと通信がうまくいかない。電波強度の問題？
　
　最適化設定を2に戻しビルド。正常動作確認。OK。
　ひとまずここまででコミット。v0.1.1-pre5。
　
　全プロジェクトで動作確認を行う。
　スマートコンフィグレータによるコード生成も行い、
　合わせてNoMaY氏のスクリプトの動作チェックも行う。
　\demos\renesas\rx65n-rsk\ccrx-csplus ... OK
　\demos\renesas\rx65n-rsk\ccrx-e2studio ... OK
　\demos\renesas\rx65n-rsk\gnurx-e2studio ... OK
　\demos\renesas\rx65n-envision-kit\ccrx-csplus ... OK
　\demos\renesas\rx65n-envision-kit\ccrx-e2studio ... OK
　\demos\renesas\rx65n-envision-kit\gnurx-e2studio ... OK
　\demos\renesas\rx65n-gr-rose\ccrx-csplus ... NG -> OK (エミュレータから電源供給する設定に変更、通信方式をFINEに変更)
　\demos\renesas\rx65n-gr-rose\ccrx-e2studio ... OK
　
　どうやらNoMaY氏の保存の改行コードは LF で、CS+が出力するコード、プロジェクトファイルの
　改行コードは CR+LF のようだ。GitHub上で大量に差分が出るように見えるのはこれが原因か。
　
　ざっくり調べてみた。
　
　CS+のコード生成系: r_cg_hardware_setup.c : CR+LF # 現状何らかの要因でLFになっている
　CS+のピンコンフィグ系: Pin.c : CR+LF # 現状N何らかの要因でLFになっている
　CS+のピンコンフィグ系ヘッダ: r_pinset.h : LF
　CS+のFITモジュールのピンコンフィグ系: r_ether_rx_pinset.c: CR+LF # 現状何らかの要因でLFになっている
　CS+のFITモジュールのコンフィグ系: r_bsp_config.h: CR+LF # 現状何らかの要因でLFになっている
　CS+のプロジェクトファイル: aws_demos.mtpj: CR+LF # 現状何らかの要因でLFになっている
　e2 studioのコード生成系: r_cg_hardware_setup.c : CR+LF # 現状何らかの要因でLFになっている
　e2 studioのピンコンフィグ系: Pin.c : CR+LF # 現状何らかの要因でLFになっている
　e2 studioのピンコンフィグ系ヘッダ: r_pinset.h : LF
　e2 studioのFITモジュールのピンコンフィグ系: r_ether_rx_pinset.c: CR+LF # 現状何らかの要因でLFになっている
　e2 studioのプロジェクトファイル: .cproject: LF
　
　改行コードの件はNoMaY氏に相談。
　コミットはe2 studioのプロジェクトファイル、本Readme等を除きCR+LFにて行う。
　
　ここまででコミット。
　全プロジェクト無事に動作確認OKとなったので、v0.1.1のリリースビルドとする。

2018/08/05
　引き続き、NoMaY氏にフォルダ構成の調整行っていただいている。
　大きくは、①/lib/third_party/mcu_vendor/renesas にFIT関連を引越ししたこと、
　スマートコンフィグレータ周りのフォルダ調整(ccrx-e2studioNとか
　gnurx-e2studioNとして従来プロジェクトと分離して実験)、②本家V130への追従。
　②はcommonについて本家新規のファイルを登録だけしてあって、
　ビルドから除外の状態になっているとのこと。
　
　まずはざっと変更点の確認を行って大きな問題が無いか確認していく。
　⇒問題なし。
　
　RX65N RSKで動作確認を行う。他の環境の①対応はRSKでの確認が終わってから。
　⇒CC-RX環境、GCC環境、e2/CS+環境、①②共に問題なし。GCCで割り込みベクタが未生成になる問題も解消。
　　\demos\renesas\rx65n-rsk\ccrx-csplusN
　　\demos\renesas\rx65n-rsk\ccrx-e2studio6N
　　\demos\renesas\rx65n-rsk\gnurx-e2studio6N
　　\demos\renesas\rx65n-rsk\ccrx-csplus
　　\demos\renesas\rx65n-rsk\ccrx-e2studio6
　　\demos\renesas\rx65n-rsk\gnurx-e2studio6
　
　問題なかったので本Readme更新だけだが、ここまでで一旦コミットを行う。
　pre版としてコミット。v0.1.1-pre1。
　
　続いて、②本家V130を行う。(1)と(3)はNoMaY氏、(2)はシェルティの分担。
　(1)は完了している様子。
　
　\demos\renesas\rx65n-rsk\ccrx-e2studio6N の環境で作業開始。
　
　(1) 新規フォルダと新規ファイルの追加(ただしビルドから除外)、共通部分のCソース以外のファイル変更のマージ
　(2) 共通部分のCソースファイル変更のマージとRXマイコン用のプロジェクトのソースへの反映(ビルドから除外の解除を含む)
　(3) RXマイコン用のテストプロジェクトのe2 studioのプロジェクト作成(STの環境などを参考にしてトライしてみる)

　(2)を進めていく。差分を確認。
　かふぇルネに差分のスクリーンショットが既に貼ってある。
　NoMaYさん、ありがとうございます！
　
　testsフォルダはtests用のプロジェクトを別途格納する形式なので、
　ひとまず保留しておく。libフォルダの差分を適用していくことにする。

　libフォルダは以下3個が増えている。
　
　①cbor
　②defender
　③FreeRTOS-Plus-POSIX
　
　一番簡単そうな③を登録する。
　
　インクルードパスを登録する。
　\lib\FreeRTOS-Plus-POSIX\include\portableフォルダがある。
　ヘッダのテンプレは、FreeRTOS_POSIX_portable_default.h。
　_defaultを外して対応ボードのフォルダを作ってそこにFreeRTOS_POSIX_portable.h として設置。
　
　エラー3個出る。
　E0562310:Undefined external symbol "_xTaskGetApplicationTaskTag" referenced in ".\lib\aws\FreeRTOS-Plus-POSIX\source\FreeRTOS_POSIX_pthread.obj"
　E0562310:Undefined external symbol "_vTaskSetApplicationTaskTag" referenced in ".\lib\aws\FreeRTOS-Plus-POSIX\source\FreeRTOS_POSIX_pthread.obj"
　E0562310:Undefined external symbol "_xTaskGetIdleTickCount" referenced in ".\lib\aws\FreeRTOS-Plus-POSIX\source\FreeRTOS_POSIX_clock.obj"

　xTaskGetApplicationTaskTag を全文検索。
　\lib\FreeRTOS\portable\Common\mpu_wrappers.c にある。
　
　Commonがプロジェクト登録から外れているようだ。
　portableフォルダのリソースフィルタに Common を追加する。

　"mpu_prototypes.h"が見つからないらしい。
　
　グーグルで検索したところ、FreeRTOS本家のSVNにあった。
　https://sourceforge.net/p/freertos/code/HEAD/tree/trunk/FreeRTOS/Source/include/
　
　GitHub上のAmazon FreeRTOS内のFreeRTOSカーネルと、
　Sourceforge上のFreeRTOSカーネルが微妙に整合取れてない感じか。
　
　mpu_prototypes.hをダウンロードして、\lib\include に入れておく。
　
　E0562310:Undefined external symbol "_vPortResetPrivilege" referenced in ".\lib\aws\FreeRTOS\portable\Common\mpu_wrappers.obj"
　E0562310:Undefined external symbol "_xTaskCreateRestricted" referenced in ".\lib\aws\FreeRTOS\portable\Common\mpu_wrappers.obj"
　E0562310:Undefined external symbol "_xTaskGetApplicationTaskTag" referenced in ".\lib\aws\FreeRTOS-Plus-POSIX\source\FreeRTOS_POSIX_pthread.obj"
　E0562310:Undefined external symbol "_vTaskAllocateMPURegions" referenced in ".\lib\aws\FreeRTOS\portable\Common\mpu_wrappers.obj"
　E0562310:Undefined external symbol "_vTaskSetApplicationTaskTag" referenced in ".\lib\aws\FreeRTOS-Plus-POSIX\source\FreeRTOS_POSIX_pthread.obj"
　E0562310:Undefined external symbol "_xPortRaisePrivilege" referenced in ".\lib\aws\FreeRTOS\portable\Common\mpu_wrappers.obj"
　E0562310:Undefined external symbol "_xTaskGetIdleTickCount" referenced in ".\lib\aws\FreeRTOS-Plus-POSIX\source\FreeRTOS_POSIX_clock.obj"
　E0562310:Undefined external symbol "_xTaskCreateRestrictedStatic" referenced in ".\lib\aws\FreeRTOS\portable\Common\mpu_wrappers.obj"
　
　色々足りないらしい。
　SourceforgeからFreeRTOSカーネルのSnapshotをゲットして、Amazon FreeRTOS内包のFreeRTOSカーネルとDIFFを取ってみる。
　色々構成が違うようだ。
　
　アプローチを変えて、Windowsのプロジェクトがどのような構造になっているか調べる。
　FreeRTOS-Plus-POSIXがプロジェクト登録されていない。
　v130で追加されたコードを使うのは、ちょっと保留したほうがよさそう。
　
　その他、\lib\secure_sockets\portable\renesas の aws_secure_sockets.c が古いので、
　\lib\secure_sockets\portable\freertos_plus_tcp にある最新テンプレートの内容を反映。

　uRand()の実装体が、以下関数ポインタ経由で呼び出されるようだ。
　今は未だuRand()を直接デバイス依存側で実装する方式で繋いでおこう。
　xResult = pxPkcs11FunctionList->C_Initialize( NULL );
　aws_secure_sockets.c 内の uRand()の実装は #if 0で無効化しておく。
　
　動作確認、特に問題なし。
　アップデートしたのは結局のところ、aws_secure_sockets.c だけとなった。
　pre版としてコミット。v0.1.1-pre2。
　　
　本家v1.3.0追従の作業はひとまずこれで終了として、
　Silex WIFIモジュールの環境を新規追加する。
　
　\demos\renesas\rx65n-rsk-uart-wifi を新設。
　\demos\renesas\rx65n-rsk\ccrx-e2studio6N を丸コピーで持ってくる。
　
　Silex WIFIモジュールとはボード上のPMOD1で、RX65NのSCI-ch6 でUARTで繋がっている。
　スマートコンフィグレータでSCI-ch6の設定を施す。
　⇒なんか、RX65N RSK 2MBボードの設定をしているとSCI6でP01、P00を使おうとすると
　　警告がでる。なぜなのか。ツール部門に問い合わせてみる。★
　　(これは担当者が動作確認していたときの暫定プロジェクトでも出てた模様)
　
　ひとまず無視して先に進む。
　
　コード生成してビルド。NoMaY氏の自動コピーがうまく働いているようだ。
　各環境にsmc_genが生成され、各々にPDFがあったので容量が大きくなって気になっていたところ。
　これでうまく因数分解できて容量も食わなくなったので万々歳だ。
　しかも、いちいち、BSP_CFG_RTOS_USEDを直す必要もない。
　一発でビルド通った・・・すごいぞこれは。
　理屈は分かっていたけれど、目の前で実際に動くとさらに感動する。
　
　ひとまずここまでで、既存動作(Ether)がOKかどうかを確認する。
　バッチリ動く。もう更新漏れで動かなくなってイライラすることもないであろう。
　
　あとは、担当者が動作確認してた動作確認済みのWIFI用のコードを適用していく。
　DIFFをとってマージしていけばよいだろう。
　
　まずキーポイントになっている、aws_secure_sockets.c。
　
　aws_secure_sockets.c 補足：
　　これはMQTT層が呼び出すSSL/TLS層の実装体。デフォルトは空っぽで、
　　テンプレートでFreeRTOS+TCPとmbed TLSを使ったものが用意されている。
　　で、TCP/IPとTLSの実装をWIFIモジュール側にオフロードする場合は、
　　このコードの中身を全部WIFIモジュール側に繋いでやる必要があるが、
　　AWS接続には、SSLクライアント認証が必要であり、Silex WIFIモジュールはこれを
　　サポートしていない。従って、TCP/IPはWIFIモジュール側にオフロードするが、
　　TLSの実装はmbed TLSに頼らなければならないという良くない構成となる。
　　まあWIFIモジュールによってはこういうケースもあるであろうから、
　　これをpattenXとして、妥協案とした。
　　ESP32であればこの問題はなさそうなので、あとでちゃんとpattern1も作り込みたい。
　　WIFIでもTCP/IPとTLSを完全にMCU側に持ってこれて、かつ通信路がSDIOになった
　　高速タイプのモジュールも存在する、というよりこれが本命。pattern3の
　　村田製作所Type 1FXである。スマホに載っている無線LANモジュールの
　　多くは村田製作所製（中身チップセットはブロードコム製）。
　　通常はLinuxが制御するようなチップだが、リアルタイムOSでも十分に動く。
　　性能もUART接続のWIFIモジュールが数Mbpsが限界なのに対し、SDIO接続の
　　モジュールはRX65NでもTCPで30Mbps程度でる。(EtherだとTCPで60Mbpsくらい出る)
　　さらにRX65Nだと暗号器搭載モデルもあるので、この通信レートを落とさず
　　CPU負荷も上げずにSSL通信が可能である。mbed TLSをRX65Nの暗号器を
　　呼ぶように改造しなければならないが、これは2018年中に検討完了する計画。
　
　さて閑話休題。
　EtherではFreeRTOS+TCPとmbed TLSのものを使っていた。
　Silex WIFIモジュールの場合は、patternXでいく。patternXの場合MQTTが呼び出すのは
　mbed TLSであるが、TCP/IPはSilex WIFIモジュールのAPIを呼ばなければならない。
　mbed TLSは接続が確立したあとのデータ転送のR/WのAPIを関数ポインタ登録し、
　デフォルトだと以下2関数でFreeRTOS+TCPのAPIを呼び出してくる。
　・prvNetworkSend() からの FreeRTOS_send()
　・prvNetworkRecv() からの FreeRTOS_recv()
　また、TCP接続時には通常はFreeRTOS_connect()を用いてTCP接続を開始している。
　
　これらFreeRTOS+TCP系のAPIの代わりに、Silex WIFIモジュールが持つ等価なAPIで
　実装していけばmbed TLSからSilex WIFIモジュールを呼び出す変な構成も作れるであろう。
　
　という考えで、作り込みを実施。
　aws_secure_sockets.c は v0.1.1-pre2 で更新する前のものを使って
　WIFIモジュールとの合わせ込みをおこなったため、古い状態である。
　来週担当者に依頼して最新の aws_secure_sockets.c で更新、動作確認してもらう。
　今はひとまず古い状態で進める。
　
　特に問題なく結合完了。
　pre版としてコミット。v0.1.1-pre3。
　
　v0.1.1-pre3 のRenesas Starter Kit+ for RX65N-2MB + Silex SX-ULPGN PMOD environmentは
　プロジェクトファイルがおかしい。リソースフィルタにおいて必要な設定がなぜか消えて
　しまっている。修正する。
　また、この環境、Renesas Debug Virtual ConsolにWIFI関連のログを出力するが
　窓を開いてないと先に進まない。またこの機能をOFFにしたら動かなくなる。
　また、まだ実験中のため通信が遅い。あと最適化をONにすると動かない。
　このあたりをReadmeに注意事項を追加する。
　
　
　本日の作業はこれまで。
　
　
2018/07/22
　スマートコンフィグレータ出力周りをNoMaY氏が整備してくれている。
　だいぶすっきりしてきた。RX65N RSK のGCC用プロジェクトも追加された。
　こちらでは、e2 studio v700 と CS+ v700と RX Driver Package v115への
　更新を行っていく。ひとまず代表パタンのRX65N RSK e2 studio CC-RXの
　組をトップバッターにして更新完了。特に問題なし。
　一旦pre版としてコミット。v0.1.0-pre1。
　
　NoMaY氏の作業と1ファイル衝突。シェルティ側を優先した。
　# Conflicts:
　#	demos/renesas/rx65n-rsk/ccrx-e2studio6/src/smc_gen/r_config/r_bsp_config.h
　
　スマートコンフィグレータは、出力先にすでにコードがあると上書きしない設定が
　デフォルトなので、RX Driver Package をスマートコンフィグレータ上で最新に交換した
　としてそれがコードに反映されるとは限らない。
　一方で、コンパイラ差分を吸収するマクロ(R_PRAGMA_など)はすでにコードに反映されており
　これがスマートコンフィグレータによって消えられても困る。
　
　少々面倒ではあるが、代表パタン(RX65N RSK e2 studio CC-RX)を決めて、そこで
　手動でRX Driver Packageの最新版コードとコンパイラ差分を吸収するマクロ(R_PRAGMA_など)を
　含む旧版とのマージを行っていく。他の環境では、スマートコンフィグレータで
　最新のRX Driver PackageのFITモジュールを使用する設定にして、代表パタンのsmc_genフォルダを
　丸ごとコピーすればよいであろう。これでBSPを含む全FITモジュールを、
　コンパイラ差分を吸収するマクロ(R_PRAGMA_など)を伴い更新することができる。
　
　あと、RX65N RSKの環境で暗号有品デバイスをスマートコンフィグレータ設定していたが、
　暗号無し品に変更する。スマートコンフィグレータのボード設定が追加されたが、まだ
　暗号有品のボードの設定が無いようだ。（まあデバイス以外に違いはないのだけど）
　プロジェクトのデバイス設定も暗号無し品に変更。DUALを使おうとしていたのもやめる。
　プロジェクトのデバイス設定：R5F565NEHxFC_DUAL->R5F565NEDxFC
　スマートコンフィグレータのデバイス設定：R5F565NEHxFC->R5F565NEDxFC
　
　この状態で以下フォルダをバックアップを取り、いったん削除し、コード生成。
　\demos\renesas\rx65n-rsk\ccrx-e2studio6\src\smc_gen
　
　バックアップデータとコード生成されたデータをマージしていく。
　これで最新のRX Driver Packageのコードにコンパイラ差分を吸収するマクロ(R_PRAGMA_など)を
　反映できるはずだ。
　
　スマートコンフィグレータのボード設定で RX65N RSK-2MB を選んでいるのに、
　BSPで選択されて出力されるボード毎のフォルダが generic_rx65n になっている。
　どういうことだ？ 新規プロジェクトで試してみる。やっぱりだめ。期待動作ではない。
　開発元に報告する。★
　
　ひとまず generic で我慢しておく。端子設定周りは読み込めている様子。
　
　マージで具合の悪いところを洗い出す。
　
　r_bsp.h 以下ヘッダはAmazon FreeRTOSに存在しない。BSP開発者に修正依頼。
　ローカルではコメントアウトしておく。
　/*#include "croutine.h" Amazon FreeRTOS does not have this header file. */
　/*#include "freertos_start.h" Amazon FreeRTOS does not have this header file. */
　resetprg.c にも、#include "freertos_start.h" がある。上記と同様にコメントアウトしておく。
　
　コード生成すると、r_bsp_config.h の以下項目が必ず (0)に戻るようだ。
　コード生成するたびにこの値を確認しなければならない。
　まだ未対応ならスマートコンフィグレータ側はこの定義を無視するようにした方が良い。
　#define BSP_CFG_RTOS_USED               (1) // <-- Updated by GUI. Do not edit this value manually
　
　他は特に問題なし。ビルド問題なし。実機確認問題なし。
　
　なんか、改行コードが LF のものと、CRLF のものが混在している。
　FITのコーディングルールがどうであったか確認する。
　ひとまずローカルでは Amazon FreeRTOSに倣い、LF でなるべく統一するものとする。
　スマートコンフィグレータ出力のものは触らない方がよいであろう。
　混在しているように見えるが、おおむねCRLFが多いようだ。
　
　一旦pre版としてコミット。v0.1.0-pre2。
　
　Etherドライバがv114からv115に更新されていないことに気付く。
　\demos\renesas\rx65n-rsk\ccrx-e2studio6\src\smc_gen\ をZIP化してバックアップ。
　\demos\renesas\rx65n-rsk\ccrx-e2studio6\src\smc_gen\r_ether_rx と
　\demos\renesas\rx65n-rsk\ccrx-e2studio6\src\smc_gen\r_config\r_ether_rx_config.h を削除。
　スマートコンフィグレータ上でEtherドライバをv114からv115に更新。
　Etherドライバの設定を行う。
　r_bsp_config.h の BSP_CFG_RTOS_USEDが(0)に戻っているであろうから、(1)に設定する。
　ZIP化したバックアップを解き、スマートコンフィグレータ出力と比較、コンパイラ差分を
　吸収するマクロ(R_PRAGMA_など)を反映。
　
　ビルド、動作確認、OK。
　pre版としてコミット。v0.1.0-pre3。

　次はRX65N RSK CS+ CC-RXの組み合わせのメンテ。
　まずはデバイス設定の変更。
　プロジェクトのデバイス設定：R5F565NEHxFC_DUAL->R5F565NEDxFC
　スマートコンフィグレータのデバイス設定：R5F565NEHxFC->R5F565NEDxFC
　
　RX Driver Package v114->v115においては、Amazon FreeRTOSで使用している
　FITモジュールはBSPとEtherのみ更新がかかっている。
　BSPとEtherのみスマートコンフィグレータで入れ替えを行う。
　
　コード生成を実行、
　\demos\renesas\rx65n-rsk\ccrx-csplus\src\smc_gen を削除
　\demos\renesas\rx65n-rsk\ccrx-e2studio6\src\smc_gen\ を代わりにコピーペースト。

　ビルド、動作確認、OK。
　pre版としてコミット。v0.1.0-pre4。

　さて、次は少し壁があるかもしれない。
　RX65N RSK e2 studio GCCの組み合わせのメンテ。
　NoMaY氏にコミットいただいた状態のものをそのままe2 studio v700で動作だけさせてみる。
　動かない。暴走はしないが、UART出力がされない。
　UART送信の割り込みが入ってないようだ。
　マップファイルを確認して割り込みベクタ周りを見てみる。
　割り込みベクタが生成されていない様子。
　
　リリースビルドのv0.0.9と比較してみる。割り込みベクタ周りに差が出ている。
　\demos\renesas\rx65n-rsk\gnurx-e2studio6\src\smc_gen\r_sci_rx\src\targets\rx65n\r_sci_rx65n.c
　v0.0.9の記述に戻したら正常動作となった。
　
　　NG: 現状
　　　#if SCI_CFG_CH0_INCLUDED
　　　R_PRAGMA_STATIC_INTERRUPT(sci0_txi0_isr, VECT(SCI0,TXI0))
　　　R_PRAGMA_STATIC_INTERRUPT(sci0_rxi0_isr, VECT(SCI0,RXI0))
　　　#endif
　　　__INTERRUPT void sci0_tei0_isr(void *cb_args)
　　　R_ATTRIB_STATIC_INTERRUPT void sci0_rxi0_isr(void)
　
　　v0.0.9
　　　#if SCI_CFG_CH0_INCLUDED
　　　R_PRAGMA_STATIC_INTERRUPT(sci0_txi0_isr, VECT(SCI0,TXI0))
　　　static void sci0_txi0_isr(void);
　　　R_PRAGMA_STATIC_INTERRUPT(sci0_rxi0_isr, VECT(SCI0,RXI0))
　　　static void sci0_rxi0_isr(void);
　　　#endif
　　　void sci0_tei0_isr(void *cb_args)
　　　static void sci0_rxi0_isr(void)
　　　
　　⇒SCIに関しては一旦v0.0.9の記述法に戻す。
　　　CC-RXでは動作するがGCCだと動作しないようだ。
　　　
　　正常動作OK。逆にCC-RX環境で動作するか試してみる。問題なし。
　
　CS+ CC-RX環境のスマートコンフィグレータのスタック設定ができてない。
　修正してコード生成しなおす。
　\demos\renesas\rx65n-rsk\ccrx-csplus\src\smc_gen\
　
　スマートコンフィグレータ出力をCC-RXのものをGCC環境に上書き。
　BSPはまだ1本化できてないので除く。
　\demos\renesas\rx65n-rsk\ccrx-csplus\src\smc_gen\
　↓
　\demos\renesas\rx65n-rsk\gnurx-e2studio6\src\smc_gen\
　
　ここまででRX65N RSKの3種のプロジェクトが動作確認OKとなった。
　コミットする。pre版としてコミット。v0.1.0-pre5。
　
　種類が増えてきてだいぶ大変になってきた。
　続いてRX65N Envision Kitのプロジェクトの調整。IARはひとまず保留。
　
　RX65N Envision Kit CC-RX CS+ の環境からメンテ。
　スマートコンフィグレータでBSPとEtherを最新に交換。
　
　出力済みのコードを削除する。
　\test\demos\renesas\rx65n-envision-kit\ccrx-csplus\src\smc_gen
　
　以下のようにコピー。ボード依存の端子設定コードr_pincfgとgeneralとr_configを除く。
　\demos\renesas\rx65n-rsk\ccrx-csplus\src\smc_gen\
　↓
　\demos\renesas\rx65n-envision-kit\ccrx-csplus\src\smc_gen\
　r_bsp_config.h に追加したボードリビジョン設定を変更。(1)->(2)
　r_sci_config.h で、SCI 2chを使用するよう変更。
　
　ビルド、動作確認、OK。
　pre版としてコミット。v0.1.0-pre6。
　
　RX65N Envision Kit CC-RX e2 studio の環境メンテ。
　スマートコンフィグレータでBSPとEtherを最新に交換。
　
　出力済みのコードを削除する。
　\demos\renesas\rx65n-envision-kit\ccrx-e2studio6\src\smc_gen
　
　以下のようにコピー。ボード依存の端子設定コードr_pincfgとgeneralとr_configを除く。
　\demos\renesas\rx65n-rsk\ccrx-e2studio6\src\smc_gen\
　↓
　\demos\renesas\rx65n-envision-kit\ccrx-e2studio6\src\smc_gen\
　r_bsp_config.h に追加したボードリビジョン設定を変更。(1)->(2)
　r_sci_config.h で、SCI 2chを使用するよう変更。
　
　ビルド、動作確認、OK。
　pre版としてコミット。v0.1.0-pre7。
　
　RX65N Envision Kit GCC e2 studio の環境メンテ。
　出力済みのコードを削除する。
　\demos\renesas\rx65n-envision-kit\gnurx-e2studio6\src\smc_gen

　スマートコンフィグレータが使えないので、以下のようにコピー。
　\demos\renesas\rx65n-rsk\gnurx-e2studio6\src\smc_gen
　↓
　\demos\renesas\rx65n-envision-kit\gnurx-e2studio6\src\smc_gen

　r_pincfgとgeneralとr_configは、RX65N Envision Kitの別プロジェクトからもってくる。
　\demos\renesas\rx65n-envision-kit\ccrx-csplus\src\smc_gen
　↓
　\demos\renesas\rx65n-envision-kit\gnurx-e2studio6\src\smc_gen
　
　BSP_CFG_IO_LIB_ENABLE を (0)にしておく。しないと標準I/Oのライブラリ辺りでエラーがでる。

　ビルド、動作確認、OK。
　pre版としてコミット。v0.1.0-pre8。

　RX65N GR-ROSE CC-RX e2 studio の環境メンテ。
　スマートコンフィグレータでBSPとEtherを最新に交換。
　
　コード生成し、出力済みのコードを削除する。ボード依存の端子設定コードr_pincfgとgeneralを除く。
　\demos\renesas\rx65n-gr-rose\ccrx-e2studio6\src\smc_gen
　
　以下のようにコピー。ボード依存の端子設定コードr_pincfgとgeneralとbspを除く。
　\demos\renesas\rx65n-rsk\ccrx-csplus\src\smc_gen\
　↓
　\demos\renesas\rx65n-gr-rose\ccrx-e2studio6\src\smc_gen
　r_bsp_config.h に追加したボードリビジョン設定を変更。(1)->(3)
　r_sci_config.h で、SCI 12chを使用するよう変更。
　
　ビルド、動作確認、OK。
　pre版としてコミット。v0.1.0-pre9。

　RX65N GR-ROSE GCC e2 studio の環境メンテ。
　出力済みのコードを削除する。
　\demos\renesas\rx65n-gr-rose\gnurx-e2studio6\src\smc_gen

　スマートコンフィグレータが使えないので、以下のようにコピー。
　\demos\renesas\rx65n-rsk\gnurx-e2studio6\src\smc_gen
　↓
　\demos\renesas\rx65n-gr-rose\gnurx-e2studio6\src\smc_gen

　r_pincfgとgeneralとr_configは、RX65N GR-ROSEの別プロジェクトからもってくる。
　\demos\renesas\rx65n-gr-rose\ccrx-e2studio6\src\smc_gen
　↓
　\demos\renesas\rx65n-gr-rose\gnurx-e2studio6\src\smc_gen
　
　BSP_CFG_IO_LIB_ENABLE を (0)にしておく。しないと標準I/Oのライブラリ辺りでエラーがでる。

　ビルド、動作確認、OK。
　pre版としてコミット。v0.1.0-pre10。

　
2018/07/16
　GitHub上のデータ調整。一気にファイル整理したのでいろいろボロがありそう。
　v0.0.7-pre1では、GR-ROSE用のプロジェクト(CC-RXのe2 studio)が動作しない。
　SCIの設定ファイルがおかしいようだ。修正してコミット。v0.0.7-pre2。
　
　GR-ROSE用のプロジェクト(GCCのe2 studio)を追加する。
　Envision Kit用のプロジェクト(GCCのe2 studio)をコピーしてボード周りの
　コンフィグを修正していくだけでいけるはず。
　
　いけない。なぜなのか。main()にたどり着く前に暴走する。
　
　コピー元のEnvision Kit用のプロジェクト(GCCのe2 studio)がいつの間にか
　壊れたか？ Envision Kit用のプロジェクト(GCCのe2 studio)が動作した環境は
　どれだろう。細かくGitHubにコミットしておけばよかった。
　
　ゴミ箱から動いていたプロジェクトを引っ張り出してきて差分確認。
　どうやらセクション設定がおかしくなっていたようだ。元に戻す。
　割り込みベクタ周りの記述もおかしくなっていた。元に戻す。
　
　ここで一旦コミット。v0.0.7-pre3。
　(readme保存し忘れでメモ書きが残ってたので再コミット)
　
　今度こそ、GR-ROSE用のプロジェクト(GCCのe2 studio)を追加する。
　Envision Kit用のプロジェクト(GCCのe2 studio)をコピーしてボード周りの
　コンフィグを修正していくだけでいけるはず。
　
　いけた。
　
　ここでGitHubにコミット。今週の作業は終了。v0.0.7-pre4。
　
2018/07/15
　道が拓けてきた。あとはIAR対応。
　NoMaY氏から案をいただいた。
　
　■NoMaY氏案
　(1) とりあえずキーワード置換無しでCC-RX＋e2 studio v6のプロジェクトを変換してみる → 失敗したら別の手を考える
　(2) FITのR_BSPモジュールはIAR社が提供しているものに置き換える
　(3) FITの他のモジュールはGNURXで試したものに置き換える(SCFGcompiler.hにIARコンパイラ対応の記述を試しに入れてあります)
　(4) FreeRTOSのPortレイヤーをlib\FreeRTOS\portable\IAR\RXv2へ置き換える
　(5) FreeRTOS-Plus-TCPのPortレイヤーをlib\FreeRTOS-Plus-TCP\source\portable\Compiler\IARへ置き換える
　(6) 割り込みベクタの登録、セクションの確保、など
　(7) iodefine.hはIAR社が提供しているヘッダファイルをインクルードするようにしておく
　(8) machine.hはとりあえず中身が空のファイルにしておく

　(1)を試す。
　Project migration tools
　　www.iar.com/jp/iar-embedded-workbench/project-migration/
　
　エラーがでた。バージョンが整合しないのかな。(1)は諦める。
　幸いコードはUnified済であるので、プロジェクト新規作成してコードを追加していけばよいであろう。
　
　新規プロジェクトを作成してデバイスを選んだら、RX65Nがない。
　どうやら手元のEWRXのバージョンが古い(v2.9)ようだ。最新版はv4.1。
　更新はマイページに入らないといけないようだけれど、ID/パスが分からない。
　コンパイラのライセンスIDは有るので、無償評価版をインストールして
　そこにコンパイラのライセンスIDを登録する方法を試みる。
　
　最新版v4.1をインストールして起動してみる。ちょっと表示がおしゃれになっている。
　起動後にライセンスマネージャが起動し、自動でライセンスIDの確認を行ってくれて
　無事製品版に昇格した。
　
　(1)がエラーになったのはEWRXのバージョンが古かったからなのでは。もう一度試す。
　予想通りうまくいった。けど、プロジェクトディレクトリ外のファイル(libフォルダ)が登録から外れている。
　手動で登録していく必要がある。ビルドしてみた感じインクルードパスの設定も施してやる必要がありそう。
　⇒新規プロジェクト作成で自力で組み立てたほうが不明点がなくよさげだ。
　
　新規プロジェクト作成で、main()のみのプロジェクトがデバッグできるか試す。
　ビルドはOK。デバッガやデバイスの設定を施してEnvision Kit(E2Lite)に接続。
　デバッガの設定にFreeRTOSのプラグイン追加があるので追加してみる。
　FreeRTOS関連の便利なデバッグ機能が追加されるのかな。
　無事ターゲット接続ができた。
　
　(2)に進む。
　まずはNoMaY氏のZIPパッケージ(\src\smc_genに入れる)の取り込みを行う。

　ディレクトリをまとめてツリーに登録することが出来ない(?)ようだ。
　面倒なのでやっぱりプロジェクトコンバータを使ってFITモジュールの取り込みを行う。
　Amazon FreeRTOS関連は手動で登録するかな。これはなかなか果てしない作業になりそう。
　
　まずは、Amazon FreeRTOS抜きにして、NoMaY氏のZIPパッケージとリアルタイムOS用FITパッケージを取り込む。
　CC-RX、e2 studioの環境をコンバートして取り込む。そこにNoMaY氏のZIPパッケージを上書き。
　リアルタイムOS用FITパッケージ(\src\realtime_OS_pkgに入れる)が付いてきたので削除。
　
　まとめてビルドしてみるも、エラー項目が多く厳しい。ミニマムスタートをしてみる。
　完全新規プロジェクトに、BSPだけを組み込み、NoMaY氏の「SCFGcompiler.h」だけを組み込み
　1個ずつエラーを潰していこう。
　
　新規プロジェクトにEWRXのFIT Module Importerを使用しBSPを出力する。
　ソースコードを通し、インクルードパスを通したところでビルド。
　r_bsp_config.h でエラーがでる。
　どうやらスタックサイズはツールで入力する必要があるようだ。
　ツールでスタック(ユーザ=0x3000、割り込み=0x3000、ヒープ0x2000)を設定し
　r_bsp_config.hのエラー箇所はコメントアウト。
　ビルドが通ったので注意点はここだけであろう。
　r_bsp_config.hは、CC-RX+e2 studio+Envision Kitのプロジェクトから持ってくる。
　
　ビルドが通ったので実行してみる。無事main()に到達。
　main()前の動きを把握するため、デバッガの設定でmain()まで自動実行の設定をやめる。
　リセット後のプログラムはIARの標準のものが自動で組み込まれるようだ。
　途中でBSPの__low_level_init() (公式BSPだとResetPRG())が呼ばれる。
　続いてmain()が呼ばれる仕組みのようだ。
　
　(3)に進む。
　次にGCC側のコード群から、compiler_supportフォルダのSCFGcompiler.hを持ってくる。
　インクルードパスを通す。ビルド、特に問題なし。
　BSP共通ヘッダのplatform.h にSCFGcompiler.hのインクルードを追加。
　
　コンパイルしてみると、SCFGcompiler.h の #define __LIT1 が怒られる。
　IARではすでに存在するマクロのようだ。
　BIGも合わせて/* nothing to do */のコメントに変えておく。
　エラーゼロになった。
　
　NoMaY氏のZIPファイルからCMTタイマだけを組み込んでみる。
　
　static volatile struct st_cmt0 __evenaccess * const g_cmt_channels[CMT_RX_NUM_CHANNELS] =
　
　この行でエラーが起きた。__evenaccessが引っかかっている。
　早速、SCFGcompiler.hのマクロを活用しようとするが、使えるマクロはまだない。
　IARが出力するCMTのFITモジュールでは、__sfr に置き換わっていた。
　ひとまず、#define __evenaccess    __sfr とする。
　
　次のエラーは割り込みベクタ。
　
　R_PRAGMA_STATIC_INTERRUPT(cmt0_isr, VECT(CMT0, CMI0))
　static void cmt0_isr (void)
　
　これもIARが出力するCMTのFITモジュールを参照。
　
　#pragma vector=VECT(CMT0, CMI0)
　__interrupt static void cmt0_isr (void)

　関数の先頭に __interrupt が無いとダメなようだ。
　これ用の共通マクロ追加必要か。
　
　#if defined(__CCRX__)
　#define R_ATTRIB_INTERRUPT_FUNCTION /* none */
　#elif defined(__GNUC__)
　#define R_ATTRIB_INTERRUPT_FUNCTION /* none */
　#elif defined(__ICCRX__)
　#define __INTERRUPT    __interrupt
　#endif

　以上追加し、CMTの割り込み関数の先頭に__INTERRUPTを追加し、
　コンパイルが通るようになった。
　
　main()にCMTの関数(R_CMT_CreatePeriodic())を呼び出し実験。
　100Hz周期割り込みを発生させる。
　割り込みが実行され、正しく時間を刻んでいるようだ。
　
　この調子でSCIとBYTEQとPINCFGを組み込む。
　こちらも特に問題なし。型の暗黙変換で細かいワーニングが出ているがまあ良し。
　
　次はEther。以下構造体の__evenaccess で怒られる。
　ポインタじゃないのに__evenaccessだからか？ これは元のコードがおかしい気がする。
　__evenaccess を削除した。
　
　typedef struct DescriptorS
　{
    __evenaccess uint32_t           status;

　最後はフラッシュ。
　ワーニングが出たがエラーは出ず。ひとまず先に進む。
　　・"unrecognized #pragma" が R_ATTRIB_BITFIELD_BIT_ORDER_LEFT_6で出ている。
　　・引数の型ミスマッチが  xchg(&is_locked, plock);で出ている。
　　・他にもFITのコードが元からIARだとワーニングを出す要素がいくつかあるようだ
　
　FITモジュールの組み込みが終わった。
　続いてAmazon FreeRTOSのコードを登録していく。
　
　ひとまずビルドが通るところまで辿り着いた。以下課題。
　・そもそもまだ動かない
　・group_al1_handler_isr()の扱い。今コメントアウトしてある。
　・FreeRTOSIPConfig.hの扱い。
　・/lib/FreeRTOS/portable/IAR/RXv2/port.c の vApplicationSetupTimerInterrupt()の実装。
　　⇒_IEN() とかをコメントアウトしてある
　・/lib/FreeRTOS/portable/IAR/RXv2/port.c の vTickISR()の実装。
　　⇒割り込み関数でなくしてる
　・/lib/FreeRTOS/portable/IAR/RXv2/port_asm.s の
　　インクルード"PriorityDefinitions.h"が見つからない
　　⇒コメントアウトして、ヘッダに書いてあることを期待しているコンフィグ値を直打ち
　・/lib/pkcs11/mbedtls/aws_pkcs11_mbedtls.c
　　prvSetupPkcs11SigningForMbedTls関数
　　⇒IARだとビルドエラーになる。
　
　ここまでで、一旦ExperimentalとしてGitHubに登録。
　
2018/07/14-(2)
　NoMaY氏が作ってくれたGCC用のFITモジュールZIPパックを導入。
　https://japan.renesasrulz.com/cafe_rene/f/forum21/4772/amazon-freertos-rx/28951#28951
　⇒2251.smc_fit_modify_20180714.zip
　
　RX65N Amazon FreeRTOS v0.0.6の以下フォルダに解凍したデータを全部入れる。
　\demos\renesas\rx65n-envision-kit\gnurx-e2studio6\src\smc_gen
　
　RX65N Amazon FreeRTOS v0.0.6の他のフォルダはRX Driver Package v114環境に移行したが、
　GCC対応版はRX Driver Package v113環境を維持。
　
　プロジェクトエクスプローラの aws_demos.scfg をダブルクリックし、コード生成。
　
　ビルド実行。
　
　/demos/renesas/rx65n-envision-kit/common/application_code/rskrx65n_uart.c
　⇒ r_pinset.h が無いとのことでコンパイルエラーが発生する。
　
　e2 studio v620からv630にアップしたことによる影響か。
　ZIPパックからr_pinset.hを含むフォルダ「\smc_gen\r_pincfg」を
　RX65N Amazon FreeRTOS v0.0.6の以下フォルダに解凍したデータを全部入れる。
　\demos\renesas\rx65n-envision-kit\gnurx-e2studio6\src\smc_gen\r_pincfg
　
　ここでビルドOKとなる。(一度クリーンビルドが必要）
　
　ダウンロードして実行してみる。
　暴走。トレースしてみる。
　
　reset_program.asm にて、FreeRTOSを初期化せずにmain()にジャンプし
　その後処理が進んでいきOS内部の処理でNULLポインタアクセスとなり暴走している。
　
　reset_program.asm の main()呼び出しをしているところを代わりに
　RTOSパッケージの r_bsp_rtos の resetprg.c に倣い、以下2関数を呼び出すことにする。
　これはNoMaY氏のメモ書きの通りである。
　
　_Processing_Before_Start_Kernel    
　_vTaskStartScheduler

　ダウンロードして実行してみる。
　暴走。トレースしてみる。

　prvGetNextExpireTime()にて暴走するようだ。
　どうやらスタックが足りないらしい。
　リンカスクリプト(linker_script.ld)をe2 studio上から編集してみる。
　他の環境では、USTACK、ISTACKをそれぞれ0x3000確保している。
　ヒープは0x2000確保している。GCC環境もこれらを合わせ込む。
　ヒープはBSSに配置されるであろう。たぶん。
　　.ustack の仮想アドレス/ロードアドレスを 0x200 から 0x6000
　　.istack の仮想アドレス/ロードアドレスを 0x100 から 0x3000
　　.data   の仮想アドレス/ロードアドレスを 0x204 から 0x6004

　ダウンロードして実行してみる。
　暴走。トレースしてみる。

　prvGetNextExpireTime()にて暴走するようだ。
　どうやら割り込み周りでおかしくなっている。
　NoMaY氏の割り込みベクタの記述差吸収マクロは働いていて
　正しくCMT0割り込みに対応するベクタが生成され、割り込み自体も発生する様子。
　試しにcmt0_isr()にブレークしてみる。ブレークするが、何度か割り込みが発生して
　デバッガの挙動がおかしくなりデバッグ継続できなくなる。e2 studioがエラーを吐いて
　デバッガ接続が切れる。
　
　フリーラン実行+ブレーク+トレースで、どこにブレークを貼った時に正常/異常動作が
　分かれるかを検証。prvProcessTimerOrBlockTask()がreturnするところがフリーランで正常動作を
　維持できる限界のようだ。returnする際の戻り番地が格納されたスタックが破壊されたか？
　
　reset_program.asm の先頭で istack と ustack を設定しているところを見てみる。
　istack に 0x100、ustack に0x200が設定された。さっきのリンカスクリプトの設定が反映されてない様子。
　クリーンビルドしてみる。
　
　ダウンロードして実行してみる。暴走しなくなった。
　
　■NoMaY氏の見解
　①FreeRTOSのスケジューラを呼び出す処理がない(スタートアップルーチンもしくはmain()から呼び出す)
　②FreeRTOSのタイマ割り込みをベクタ登録してない(lib\FreeRTOS\portable\GCC\RX600v2\port.cのvTickISR()を登録する)
　③FreeRTOSのソフトウェア割り込みをベクタ登録していない(同port.cのvSoftwareInterruptISR()を登録する

　④GNURXプロジェクトのR_BSPのR_BSP_RegisterProtectEnable()とR_BSP_RegisterProtectDisable()に割り込み禁止を追加
　⑤GNURXプロジェクトのR_BSPのR_BSP_SoftwareLock()でXCHG命令を使用するように変更
　⑥GNURXプロジェクトのR_CMT_RXがRTOS対応になっていないがaws_demosプロジェクトで未使用なので一旦削除
　(・aws_demos.scfgに小細工をして本来はGNURXプロジェクトで生成出来ないFITモジュールを生成させる裏技は早めにやめる)

　⑦今回のaws_demosプロジェクトをインポートするとe2 studioがlinker_script.ldの記述でエラーを表示するので調査/対処
　⑧リンク時のあのワーニングにはどう対処すべきだろう、、、(今夜は無理かな、、、)

　⑨フラッシュセルフプログラミングでコードをROMからRAMへコピーする処理がどうなっているか気掛かり(でも当面先延ばし)
　⑩フラッシュセルフプログラミングのコードにインラインアセンブラのコードがあったので気掛かり(これも当面先延ばし)

　上記NoMaY氏の見解のうち、①は完了、②③を実現してみる。
　あとできれいに呼び出す方法は考えるとして、vTickISR()をCMT0タイマ割り込みから直接呼び出す。
　cmt0_isr()で検索するとvTickISR()を呼んでるところが出てくる。
　port.c でvTickISR()は割り込み関数指定されているがこれを解除。
　vSoftwareInterruptISR()はNoMaY氏の準備してくれた共通pragmaで以下のようにport.cで定義。
　R_PRAGMA_STATIC_INTERRUPT(vSoftwareInterruptISR, VECT(ICU, SWINT));
　これを使用するため、port.cで#include "platform.h"を呼び出して、SCFGcompiler.hを読み込ませる必要がある。
　port.c はFreeRTOSカーネルの奥底なので変更はなかなか大変そう。本家に問い合わせないといけない。
　外側でどうにかする手を別途考える必要がある。課題★
　
　あと、Ether関連のセクションが未定義のためEtherコントローラの制限を回避できていない。
　(制限=送信・受信バッファが32バイト境界、送信・受信ディスクリプタが16バイト境界)
　そこで、リンカスクリプトを修正し、_ETHERNET_BUFFERSを0番地から、_RX_DESC、_TX_DESCをその続きに
　配置して制限回避した。
　
　この状態で無事Amazon Web Serviceに接続するサンプルが動作した。
　続いてIAR環境の動作確認を進めることにする。
　
2018/07/14
　しばらくぶりの更新。この1か月は内部調整に奔走。この開発は、とても楽しい。
　出張ラッシュがひと段落しての3連休。NoMaY氏がGCC対応のプロジェクトファイルを
　コミットしてくれている。これを試したい。
　その前に、GR-ROSEへのAmazon FreeRTOS移植を実施する。
　
　GR-ROSEとは、がじぇっとるねさすプロジェクトの最新ボード。RX65N搭載。
　ロボットの制御基板をターゲットとした小型ボードであり、通信制御、
　リアルタイム制御用にAmazon FreeRTOSを標準搭載することにした。
　http://gadget.renesas.com/ja/event/2018/RenesasNight13.html
　
　OS/ドライバ周りのインテグレートをシェルティが担当。
　ネットワーク層の上位にROSと呼ばれるロボット制御プロトコルを搭載。
　ROSはAIBOにも載っているソフトウェアとのこと。
　
　RX65N Envision Kitのe2 studio用プロジェクトのファイルをコピーして作業開始。
　リンクフォルダの設定等を調整していく。
　あとはスマートコンフィグレータでEther周り、UART周りの端子を調整。

　Ether周りは特に問題なし。
　あっさりとGR-ROSEでAmazon Web Serviceに繋ぐことができた。
　
　UARTはPMODコネクタに出ているが、PMODのpin7とpin8にSCI-ch1のRxD1、TxD1、
　PMODのpin9とpin10にSCI-ch12のRxD12、TxD12が配線されている。
　PMODのUART仕様はpin3がRxD(PMOD側が入力)、pin4がTxD(PMOD側が出力)であり、
　回路構成と合致しない。
　このPMODはSPI用途なので間違ってはいないが、Amazon FreeRTOSにはUART入出力も必要である。
　(システムログ出力や、アカウントデータのインストールのため等)
　
　RX65N側のUSBのCommunication Device ClassでPC側と通信する手も有るが、
　ファームウェアにUSBドライバが必須になり得策ではない。
　他のプロモーションボードでも同様に基本ファームウェア構成は限りなくシンプルに抑える方針であり、
　システムログ出力はUSBではなく、SCIのUARTで行う。
　
　したがって、GR-ROSE proto1のPMODの配線を修正しUARTを使えるようにする。
　RX65Nのピンマルチをうまくつかえば、ソフトウェアの切り替えでSPIとUARTが同一配線で実現可能。
　現在RSPI1がPMODに配線されているが、残念ながらRSPIの送受信端子はSCIとピン互換ではない。
　PMODに配線するSPIはSCIのSPIモードで妥協し、SCI-ch12のSPI端子4本(UART端子込)を
　PMODのpin1-6に配線することにする。ついでにI2CバスのPMODモジュールも使えるようになる。
　
　まとめると以下。
　
　現状PMOD配線：
　　RSPI-ch1(SPI)  ---> PMOD pin1-4 (fit to PMOD spec)
　　SCI-ch1(UART)  ---> PMOD pin7-8 (not fit to PMOD spec)
　　SCI-ch12(I2C)  ---> PMOD pin9-10 (not fit to PMOD spec)
　次版PMOD配線：
　　SCI-ch12(SPI(SS/MOSI/MISO/SCK))  ---> PMOD pin1-4 (fit to PMOD spec)
　　SCI-ch12(UART(RxD/TxD))          ---> PMOD pin3-4 (fit to PMOD spec)
　　SCI-ch12(I2C(SCL/SDA))           ---> PMOD pin3-4 (not fit to PMOD spec)
　
　ひとまずここでGitHubへのコミット作業。
　
2018/06/10
　特に進捗はしなかった。
　NoMaY氏がコードのメンテを進め、本家v127へ追従してくれている。
　動作確認してv005としてコミットする。
　そろそろSilexモジュールの動作確認も進めなくてはいけない。
　6/8-6/10の3連休もメール処理で終わってしまいそうである。
　何とかしなくては。

2018/06/03
　特に進捗はしなかった。
　Readmeを少し書き換えてv004としてコミットする。
　
2018/05/27
　特に進捗はしなかった。
　NoMaY氏がコードのメンテを進め、本家v126へ追従してくれている。
　動作確認してv003としてコミットする。
　
　開発進捗としては、以下課題が挙がり、対応方法を考えた。
　
　[課題]
　Silex側のTCP/IP機能を使わず、マイコン側でTCP/IPを実装した方式にしたかったが
　Silex側のTCP/IP機能を使わないオプションが無い様子。
　作戦は考えたが、出張週間がまだ続くため検討の時間がない。
　出張が続くとメールが溜まりコードの整備が進まない。
　出張がひと段落した後、6/8-6/10の間が3連休なのでここでどうにかしたい。
　
　[対応方法]
　以下構成とする。これはこれでRX130、RX231、RX65Nで無線LAN実現方法として
　全部共通構造になるので良い。これをpattern1とする。
　有線LANを使うpattern2、SDIO無線LANを使うpattern3もまとめた。
　ボード発売に合わせるのは<first step>と書いたものとする。
　
　pattern1:
　osi	protocol	implemented into
　7	aws		Amazon FreeRTOS
　7	mqtt		Amazon FreeRTOS
　6	ssl/tls		wifi module
　5	socket		wifi module
　4	tcp		wifi module
　3	ip		wifi module
　2	ether		wifi module
　1	phy		wifi module
　
　RX65N Target Board + Cloud Option Board (with Silex SX-ULPGN)	<first step>
　RX231 Target Board + Cloud Option Board (with Silex SX-ULPGN)	<first step>
　RX130 Target Board + Cloud Option Board (with Silex SX-ULPGN)	<first step>
　RX65N Target Board + Cloud Option Board (with Espressif ESP8266)
　RX231 Target Board + Cloud Option Board (with Espressif ESP8266)
　RX130 Target Board + Cloud Option Board (with Espressif ESP8266)

　pattern2:
　osi	protocol	implemented into
　7	aws		Amazon FreeRTOS
　7	mqtt		Amazon FreeRTOS
　6	ssl/tls		Amazon FreeRTOS
　5	socket		Amazon FreeRTOS
　4	tcp		Amazon FreeRTOS
　3	ip		Amazon FreeRTOS
　2	ether		Amazon FreeRTOS(RX Ether specific)
　1	phy		ether-phy

　RX65N RSK <first step>
　RX65N Envision Kit <first step>
　
　pattern3:
　osi	protocol	implemented into
　7	aws		Amazon FreeRTOS
　7	mqtt		Amazon FreeRTOS
　6	ssl/tls		Amazon FreeRTOS
　5	socket		Amazon FreeRTOS
　4	tcp		Amazon FreeRTOS
　3	ip		Amazon FreeRTOS
　2	ether		Amazon FreeRTOS(RX SDIO-wifi specific)
　1	phy		wifi radio

　RX65N RSK + SDIO wifi SDK (with Murata Type 1FX)

2018/05/20
　この1週間で収集したフィードバックは以下3点。
　　ルネサスアメリカ：
　　　①readme.txt の step6-7 でプロジェクトをどのようにインポートするのか
　　　　明確ではない。import->existing project into workspace -> select a root directory
　　　　のように書いたほうがいい。
　　　　 ⇒(シェルティ): OK
　　　②generate!.txt がない
　　　　 ⇒(シェルティ): ある。smc_gen内はコード生成すると関係ないファイルが消える仕様。
　　　　 　　　　　　　　GitHub上には存在するから問題なし。
　　　③一度に一個の環境しかインポートできない旨を書いたほうがいい
　　　　(RSKとEnvision Kitのプロジェクト名が同じだから、
　　　　　同時にインポートしようとするとインポート時にエラーが出る）
　　　　 ⇒(シェルティ): OK
　　ルネサスシンガポール：
　　　./realtime_OS_pkg/r_bsp_rtos/mcu/rx65n/mcu_interrupts.c 
　　　Line851 from #if 0 to #if 1. にあるグループ割り込みの定義は
　　　ユーザアプリ側に移動。Getting startの説明としては不要として良い。
　　　　 ⇒(シェルティ): OK。ただし移動した真意を確かめる必要あり。
　　NoMaY氏：
　　　公式がV1.2.5にアップデート。（2018/05/20 16:00 時点でV1.2.6になったようだ）
　　　RX65N用のフォークをV1.2.5にアップした。その他調整は以下URL参照。
　　　http://japan.renesasrulz.com/cafe_rene/f/forum21/4772/amazon-freertos-rx/28287#28287
　　　　 ⇒(シェルティ): OK
　　　
　作業開始。
　　まず何も考えずリモートフェッチでNoMaY氏の状態を取得。
　　RX65N RSKのe2 studioのプロジェクトで動作確認開始。
　　デバッガの設定で読み込むバイナリ名称とプロジェクト名がおかしかったので修正。
　　　rx65n_rsk_aws -> aws_demos
　　もともとシェルティが作ったときは rx65n_rsk_aws というプロジェクト名だったが
　　NoMaY氏の環境ではaws_demos となっている。落ち着いたらプロジェクト名を調整したい。
　　その後、ルネサスシンガポールのフィードバックを反映。mcu_interrupts.cの
　　グループ割り込みのコードは freertos_usr_func.c に移動。この対処方法はやっぱり
　　何か気になる。元のままでいいのではという気がする。確認する。★
　　step7 のifdefに関する指示を削除した。
　　動作確認。エコー動作OK。
　　なんかAWS接続時にかかる時間が短くなった気がする。

　　RX65N RSKのCS+のプロジェクトで動作確認開始。
　　こちらも特に問題なし。
　　RX65N Envision Kitのe2 studioのプロジェクトで動作確認開始。
　　こちらも特に問題なし。
　　RX65N Envision KitのCS+のプロジェクトで動作確認開始。
　　こちらも特に問題なし。

　ひとまずここまででコミット。v0.02とする。
　
2018/05/12
　NoMaY氏にアドバイスをいただき、5/1課題に対して以下方針とした。
　・以下ボード依存が無いコード(全く同じになった)なので1階層UPしてマージする。
　　⇒現状維持。
　　　NoMaY氏：フラッシュのブロックサイズが機種毎に異なるため分けておいたほうが後々楽。
　　　　⇒シェルティ：YES。ただ、ブロックサイズはフラッシュモジュールの共通マクロ名で判別できるので
　　　　　将来的にはやはりマージできる可能性もありそう。
　・BSPやドライバは、以下フォルダに入れるのがお作法のようだ。そのうち引っ越しする。
　　⇒現状維持。
　　　NoMaY氏：スマートコンフィグレータの仕様と合わせられない。
　　　　⇒シェルティ：把握。現状維持としたい。
　　　　　他ベンダと似た構成にすべく、継続議論は必要。
　　　　　（スマートコンフィグレータで対応できる方法を考えるなど）
　
　本日の活動は、GitHubへのコード登録。NoMaY氏と相談しひとまずはv1.2.3の現状で登録するのが
　良いであろうとのこと。さしあたり、以下3点を対処すれば現状のものをアップロードできる。
　　(1)AWS接続用アカウント情報(デバイス証明書、秘密鍵)のデータを削除
　　(2)AWS接続用アカウント情報の設定方法、デバイスドライバのインストール方法、
　　　 デモの動作確認方法、改訂履歴などスタートアップガイドを本書の先頭に記す。
　　　 スタートアップガイドは英語、日本語併記とし
　　　 ポーティング記録自体は日本語で良いであろう。
　　(3)ルネサスのデバイスドライバのコードを削除しておく。(NoMaY氏の成果を使わせていただく)
　　
2018/05/01
　種々調整を経て以下4種類のプロジェクトの動作確認が完了。
　Amazon FreeRTOS 1.2.3 と同じ構造に出来た。
　(1)RX65N RSK/CS+
　(2)RX65N RSK/e2 studio
　(3)RX65N Envision Kit/CS+
　(4)RX65N Envision Kit/e2 studio
　
　以下が課題。★
　・AWS IoTのアカウントに紐づいた"モノ"のデバイス証明書、秘密鍵情報を
　　RX65N側に持たせておく必要がある。
　　現在は、以下ソースコードに埋め込んである。
　　　\demos\common\include\aws_clientcredential.h
　　　　・clientcredentialMQTT_BROKER_ENDPOINT[]
　　　　・clientcredentialIOT_THING_NAME
　　　\demos\common\include\aws_clientcredential_keys.h
　　　　・clientcredentialCLIENT_CERTIFICATE_PEM
　　　　・clientcredentialCLIENT_PRIVATE_KEY_PEM
　　　　
　　　ログインするときに必要なデータであるが、実際はこの値はユーザ毎に異なる。
　　　STM32の環境では、システム初回起動時に上記データをコンソールから入力する
　　　方式になっているようだ。正式リリースまでには、真似してシステムを作る必要がある。
　　　（いまのコードはシェルティのアカウントに繋がるのでいたずらしないでくださいね）
　・NIST FIPS140-2では秘密鍵のようにシステムセキュリティの要になるデータを
　　Critical Security Parameter(CSP)と呼称し、適切に定めた暗号境界の中でのみ扱うように
　　定めている。通常のマイコンだと暗号境界が無いためこの要件を満たせないが、
　　RX65N搭載のTrusted Secure IPではこの秘密鍵はもとより、SSL/TLSのセッション鍵に至るまで
　　すべてのCSPをTrusted Secure IPの中(暗号境界の中)で扱えるように設計してある。
　　現状のmbed TLSのコードのままだと、セッション鍵や秘密鍵がデバッガやコード上丸見えであるが、
　　将来これをTrusted Secure IPに秘匿したまま暗号通信が行えるよう改良していく予定。
　・まだデモ「\demos\common」のうち、エコーのデモしか動いてない
　　その他のデモも動作確認していく。特にOTAのデモに着目。
　・e2 studioの環境のフォルダ構成が実物とまだ合ってない。修正する。(CS+はできた)
　・以下はボード依存が無いコード(全く同じになった)なので1階層UPしてマージする。
　　　\lib\ota\portable\renesas\rx65n-envision-kit\aws_ota_pal.c
　　　\lib\ota\portable\renesas\rx65n-rsk\aws_ota_pal.c
　　　\lib\pkcs11\portable\renesas\rx65n-envision-kit\pkcs11.c
　　　\lib\pkcs11\portable\renesas\rx65n-rsk\pkcs11.c
　　　\lib\secure_sockets\portable\renesas\rx65n-envision-kit\aws_secure_sockets.c
　　　\lib\secure_sockets\portable\renesas\rx65n-rsk\aws_secure_sockets.c
　・BSPやドライバは、以下フォルダに入れるのがお作法のようだ。そのうち引っ越しする。
　　　\lib\third_party\mcu_vendor\renesas\
　　　　⇒DriverLibNameとあるので、ベンダが好きな名前を付けられるようだ。
　　　　　何にしようかな？
　　　　　⇒素直に、「rx_driver_package」にしよう。

2018/04/30
　ここ1か月は社内調整に奔走。コードの調整はアメリカの開発部隊に任せた。
　アメリカの開発部隊の調整によりAmazon FreeRTOS Qualification Programの合格に目途が立った。
　また、協力会社との契約は4月は切れており進捗無し。
　5月以降再契約を結びFreeRTOS+TCPへのIGMP機能追加とGCC対応を進めてもらうよう調整した。
　シェルティは、RX65N Amazon FreeRTOS用のボード仕様をW/Wメンバー間で検討。
　ここでは詳細は伏せるがゴールが見えてきた状況。
　デバイスドライバの再配布についてリーガルチームと協議中、
　無線LANモジュール(Silex SX-ULPGN)の日本国内技適取得が大きな課題。
　
　また、セキュリティIP連携やSDIO連携を一旦停止し、
　ボード販売を優先させるよう全体方針を更新。以下進捗表も更新する。

    ------------------------------------
    RX65N Envision Kit、RX65N RSK(2MB版/暗号器あり品)をターゲットにコードメンテを維持します。
    下記 Amazon FreeRTOS 1.2.x は適宜最新コードに更新していきます。
    2018/04/30時点での適用コードは 1.2.3 です。

    ①ルネサスTCP/IPをターゲットで動作させる（Etherの動作確認）
    ②SDIO無線LANを動作確認した時期のFreeRTOS 8.2.2をターゲットで動作させる
    ③ルネサスのFreeRTOSパッケージ、9.0.0をターゲットで動作させる
    ④Amazon FreeRTOS 1.2.xのFreeRTOS部分をターゲットで動作させる
    ⑤Amazon FreeRTOS 1.2.xのFreeRTOS+TCP部分をターゲットで動作させる
    ⑥Amazon FreeRTOS 1.2.xのmbed TLS部分をターゲットで動作させる
    ⑦Amazon FreeRTOS 1.2.xのMQTT部分をターゲットで動作させる（AWSへの接続実験）
    ⑧Amazon FreeRTOS 1.2.xのFreeRTOS+TCP部分のネットワーク層の結合部分を工夫して、
    　(1)Ether、(2)SPI接続無線LANモジュール、(3)SDIO無線LANモジュールの3種類を
    　切り替えられるようにする　★一旦停止、後ほど(2)に絞って再開
    ⑨Amazon FreeRTOS 1.2.xのmbed TLS部分の暗号処理プリミティブの
    　ソフトウェア実装（AESとかRSAとか）をRX65N内蔵暗号器を使った
    　ハードウェア実装に置き換える　★一旦停止
    ⑩Ether層のゼロコピーに対応する　★一旦停止
    ⑪Amazon FreeRTOS本家環境にマージし、Amazon FreeRTOS本家コードへの追従を簡単にできるようにする　★いまここ＠シェルティ
    ⑫Amazon FreeRTOS のGitのforkに登録する
    ⑬Amazon FreeRTOS のCertificationを受験し合格しGitの本家に登録する
    　https://docs.aws.amazon.com/freertos/latest/userguide/freertos-qualification-program.html
    ------------------------------------

　GW期間中は、以下調整を行う。
　(1)RX65N Amazon FreeRTOSコードのマージ(NoMaY氏版、ルネサスアメリカ版、シェルティ版)⇒⑪～⑫
　(2)Amazon Web Serviceの勉強
　(3)Silex SX-ULPGNのドライバ作り⇒⑧の(2)対応 <実際は地ならしだけして5月から弊課に来る研修生に作ってもらう>

　それでは作業開始。まずは(1)から。
　(1)RX65N Amazon FreeRTOSコードのマージ(NoMaY氏版、ルネサスアメリカ版、シェルティ版)⇒⑪～⑫

　作戦を考える。まずは動作確認状況をマトリクス化。
　			NoMaY氏版	アメリカ版	シェルティ版
　ビルド		〇		〇		〇
　正常動作		×		〇		〇
　フォルダ位置非依存	〇		×		〇
　GitHubベース		〇		×		×
　検定合格		×		〇		×
　ベースバージョン	1.2.3		1.2.0?		1.2.3
　
　表を見てみると、NoMaY氏版のプロジェクトにアメリカ版のコードで上書きすれば良さそうだ。
　正常動作したら改めて本家の1.2.3とマージすればよい。
　マージツールでひたすらアメリカ版⇒NoMaY氏版のコードに上書きしていく。
　NoMaY氏のコードは、GitHubから4/29 (Amazon FreeRTOS 1.2.3)にCloneしたもの。
　アメリカ版のコードは、AmazonFreeRTOS_04052018.zipを用いた。
　
2018/03/17
　協力会社に依頼した進捗⑥⑦が戻ってきた。
　3/3分の成果に対し、AWS接続(MQTT実装とSSL/TLS実装)が出来たようだ。
　これで進捗⑥⑦がOKとなった。実機確認し、確かにMQTTでAWSに繋がって
　HELLO WORLDが動いていることを確認。協力会社は現在進捗⑨の調査を進めている。
　
　シェルティはそろそろ⑧をメンテする必要がありそう。
　以下に直近で検討を行った本件の無線LAN対応方針を記す。
　
　SPI接続の無線LANモジュールは主要国で使用/サポート可能なものとしてSilexのSX-ULPGN、
　新興国向けにRealtekのRTL8710を選定。ホビーユース向けにEspressifのESP8266。
　Realtekというとシェルティを組み込みネットワークの世界に誘い込んだ
　あのカニマークのチップ、RTL8019ASを思い出す。
　　https://www.google.co.jp/search?q=rtl8019as&source=lnms&tbm=isch&sa=X&ved=0ahUKEwjDvoyzsvLZAhVDGpQKHVjECycQ_AUICygC&biw=2021&bih=1014
　
　
　SDIO接続の無線LANモジュールは計画通り村田製作所のType 1FX。
　それぞれSDKを入手して実験できるよう手配を進めておく。
　インタフェースはPMODになっていればどのルネサスボードにも
　無改造で接続可能。PMODタイプがあれば尚良し。
　PMODタイプはSPI接続最優先。（UARTだと遅いから）
　SDKがあればSPIでもUSBでもUARTでもなんでもできるジェネリックな感じが良い。
　
　<<SPI接続無線LANモジュール>>
　SilexのSX-ULPGN
　(SDK)：未入手
　　http://www.silexamerica.com/products/connectivity-solutions/embedded-wireless/IoT-Modules/sx-ulpgn-evk/
　(モジュール)：未入手
　　http://www.silexamerica.com/products/connectivity-solutions/embedded-wireless/IoT-Modules/sx-ulpgn-iot-qca4010-radio-module/
　　⇒PMODコンバータを作るか？
　　
　RealtekのRTL8710
　(SDK)：未入手
　　良いのが無い。
　(モジュール)：未入手
　　https://www.seeedstudio.com/RTL8710-WiFi-Module-p-2793.html
　　⇒PMODコンバータを作るか？
　
　EspressifのESP8266
　(SDK)：未入手
　　https://www.mouser.jp/ProductDetail/Olimex-Ltd/MOD-WIFI-ESP8266?qs=QGk6feVlqMKKopUWzCH4tA%3D%3D
　(モジュール)：
　　⇒SDKで十分安いか？

　<<SDIO接続無線LANモジュール>>
　村田製作所のType 1FX
　(SDK)：入手済み（ただし1台のみ。協力会社用にもう1台追加必要）
　　https://www.digikey.jp/product-detail/ja/murata-electronics-north-america/LBWA1KL1FX-TEMP-DS-SD/490-14857-ND/6612483
　(モジュール）：未入手
　　https://wireless.murata.com/eng/products/rf-modules-1/embedded-wi-fi-1/type-1fx.html
　　
　<<無線LANドライバ開発の作戦>>
　SPI接続の無線LANモジュールのドライバ作りは若手の教育素材に良さそう。
　あまり難易度は高くないので、データシート見ながらとにかく作ればOK。

　SDIO接続の無線LANモジュールのドライバ作りは協力会社に依頼する予定。
　Type 1FXはBroadcomのチップセットが搭載されている。Broadcomの無線事業はCypressに
　売却されており、現在無線LANドライバ(WICED)はCypressからオープンソース提供されている。
　Type 1FXの前身であるType ZX用のWICEDは過去FreeRTOS上での動作確認ができたので、
　まずType ZX用のWICEDソフトを用いてType ZXで動作確認を進めてみる。Type ZXのSDKは入手済み。
　Type 1FX対応はWICEDのオープンソースを頑張って自力移植するか
　村田製作所にサポート依頼するか検討。
　
　翻って手元での進捗⑥⑦のマージを行う。
　その後Amazon FreeRTOS本家とのズレ具合を確認する。

　rx65n_rsk_awsで実験開始。

　<<進捗⑥⑦のマージ>>
　まず変化点を分解して構造を把握していき、リポジトリ登録していく。
　まずは定番のgrep調査。
　以降、シェルティ版を本流、協力会社版を支流と表現する。
　
　rx65n_rsk_aws.c
　　メインタスク部分。支流にはAWS接続を行うタスク起動等の処理が追加されている。
　　本流にマージ。
　　以下2関数が追加されている。あとで理解する。★
　　　vApplicationGetIdleTaskMemory()
　　　vApplicationGetTimerTaskMemory()
　　以下関数呼び出しが増えている。あとで理解する。★
　　　SYSTEM_Init()
　　　
　\rx65n_rsk_aws_\src\amazon-freertos-master
　 \demos\common
　　以下ファイル/フォルダが増えている。デモ用コードの様子。本流にマージ。
　　あとで理解する。★
　　　demo_runner
　　　include　　　←インクルードパスを追加しておく
　　　mqtt
　　　tcp
　
　\rx65n_rsk_aws\src\amazon-freertos-master
　 \demos\renesas\rx65n_envision_kit\common\application_code\renesas_code
　  \entropy_hardware_poll.c
　  Amazon FreeRTOSが要求するエントロピーソース(乱数性)の実装。
　  現在は固定値が返るように実装されているが、
　  NetworkInterface.cにget_random_number()関数があるのであとでそこに繋ぐよう変更。
　  最終的にget_random_number()は暗号IPがあるチップのときは暗号IPの乱数器から
　  乱数を取得するように変更する。暗号IPがないチップのときは疑似乱数にする。★
　  
　  ついでに、NetworkInterface.cの uRand()の実装がバグっていたので修正。
　  uint32_t ulRand(void)
    {
	/* 後でハードウェア実装(暗号器使用)に変更します (シェルティ) */
	uint32_t tmp;
	get_random_number(tmp, 4);
	return tmp;
    }
    ↓
　  uint32_t ulRand(void)
    {
	/* 後でハードウェア実装(暗号器使用)に変更します (シェルティ) */
	uint32_t tmp;
	get_random_number(&tmp, 4);
	return tmp;
    }

　\rx65n_rsk_aws\src\amazon-freertos-master
　 \demos\renesas\rx65n_envision_kit\common\config_files
　 　以下コンフィグファイルが増えている。本流にマージ。
　 　aws_bufferpool_config.h
　 　aws_demo_config.h
　 　aws_mqtt_agent_config.h
　 　aws_mqtt_config.h
　 　aws_secure_sockets_config.h
　 　
　 　以下コンフィグファイルが変更されている。本流にマージ。あとで理解する。★
　 　FreeRTOSConfig.h
　 　
　　 　追加：
　　 　#define configSUPPORT_STATIC_ALLOCATION              1

　　　/*********************************************
　　　 * FreeRTOS specific demos
　　　 ********************************************/

　　　/* The address of an echo server that will be used by the two demo echo client
　　　 * tasks.
　　　 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html
　　　 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/UDP_Echo_Clients.html */
　　　#define configECHO_SERVER_ADDR0       192
　　　#define configECHO_SERVER_ADDR1       168
　　　#define configECHO_SERVER_ADDR2       0
　　　#define configECHO_SERVER_ADDR3       1
　　　#define configTCP_ECHO_CLIENT_PORT    8888

　\rx65n_rsk_aws_\src\amazon-freertos-master
　 \lib
　 　以下フォルダが増えている。本流にマージ。
　 　　bufferpool
　 　　crypto
　 　　mqtt
　 　　pkcs11
　 　　secure_sockets
　 　　third_party
　 　　　\mbedtls\include　←インクルードパスを追加しておく
　 　　　 \mbedtls
　 　　　\pkcs11　←インクルードパスを追加しておく
　 　　tls
　 　　utils
　
　\rx65n_rsk_aws_\src\smc_gen
　　以下フォルダが増えている。これはルネサスのフラッシュAPIのFITモジュールだ。
　　スマートコンフィグレータの機能でフラッシュAPIのモジュールを追加する。
　　r_flash_rx
　 
　ここまでで支流を本流にマージ完了、シェルティの開発サーバにコミットする。(r5128)
　
　<<Amazon FreeRTOS本家とのズレ具合を確認>>
　最新版v1.2.2をダウンロードする。本家と呼称する。
　　https://github.com/aws/amazon-freertos

　amazon-freertos-masterフォルダ全体をgrepして本家と本流の差分確認する。
　\rx65n_rsk_aws\src\amazon-freertos-master\demos
　 \common
　  本流には以下フォルダが足りていない。最終的にはどうするか検討。★
　  　devmode_key_provisioning
　  　greengrass_connectivity
　  　logging
　  　ota
　  　shadow
　
　\rx65n_rsk_aws\src\amazon-freertos-master\demos
　 \renesas\rx65n_envision_kit\common\application_code\renesas_code
　  本家は各マイコン用のBSPや標準デバイスドライバ、main関数、
　  プロジェクトファイル等はここに登録されている。最終的にはルネサスも引っ越す。★
　
　\rx65n_rsk_aws\src\amazon-freertos-master
　　ライセンス関係の以下ファイルが抜けている。登録した。
　　　change_log.txt
　　　directories.txt
　　　LICENSE
　　　README.md
　　　
　\rx65n_rsk_aws\src\amazon-freertos-master\demos
　 \common\include
　 　Amazon FreeRTOS v1.2.2の最新コードに置き換え。
　 　(RXマイコン用テストアカウントデータは残す)
　
　\rx65n_rsk_aws\src\amazon-freertos-master
　 \demos\common\demo_runner
　  aws_demo_runner.c
　 　Amazon FreeRTOS v1.2.2の最新コードに置き換え。
　 　★マークのところ（協力会社による調整コード）は残す。★
　
　\rx65n_rsk_aws\src\amazon-freertos-master\lib
　 \FreeRTOS\portable\Renesas\RX600v2
　  port.c
　   インクルードファイルが、iodefine.h(本家)とplatform.h(本流)とで違う。
　   いずれはAmazon本家にかけあい、本家コード修正が必要。★
　
　\rx65n_rsk_aws\src\amazon-freertos-master\lib
　 \FreeRTOS-Plus-TCP\include
　 　Amazon FreeRTOS v1.2.2の最新コードに置き換え。
　
　\rx65n_rsk_aws\src\amazon-freertos-master\lib
　 \FreeRTOS-Plus-TCP\source
　 　Amazon FreeRTOS v1.2.2の最新コードに置き換え。
　
　\rx65n_rsk_aws\src\amazon-freertos-master\lib
　 \FreeRTOS-Plus-TCP\source\portable\BufferManagement
　 　Amazon FreeRTOS v1.2.2の最新コードに置き換え。
　 　　BufferAllocation_2.c が大幅に変わっている。要注意。あとで調べる。★
　 　　
　\rx65n_rsk_aws\src\amazon-freertos-master\lib
　 \FreeRTOS-Plus-TCP\source\portable\Compiler\Renesas
　 　Amazon FreeRTOS v1.2.2の最新コードに置き換え。
　 　構造体 pack の調整コード。
　   いずれはAmazon本家にかけあい、本家コード修正が必要。★
　
　\rx65n_rsk_aws\src\amazon-freertos-master\lib
　 \FreeRTOS-Plus-TCP\source\protocols
　 　Amazon FreeRTOS v1.2.2の最新コードに置き換え。
　
　ここまでで本家 v1.2.2を本流にマージ完了。
　シェルティの開発サーバにコミットする。(r5129)
　
　RX65N Envision Kitの環境もメンテする。
　　RX65N Envision KitのほうはDHCPの最初の1回目のパケットが相変わらずコケている。
　　DHCPの再送を待ってDHCP完了してからでないとデモアプリのMQTT通信に
　　失敗するので、rx65n_envision_kit_aws.c においてDEMO_RUNNER_RunDemos()を
　　呼び出すまでのウェイトを2秒から10秒に変更。
　シェルティの開発サーバにコミットする。(r5130)
　

--------------------------------------------------------------------------
2018/03/03
　rx65n_rsk_awsで実験開始。
　開発環境のコンパイラバージョンの表記が間違っていた。修正。V2.07→V2.08。
　
　ルネサスTCP/IP(T4)の関数群を使用している箇所を整備して、
　FreeRTOS+TCPからEtherドライバを直接呼び出すように改良する。

　スマートコンフィグレータの設定から、r_sys_time_rx、r_t4_driver_rx、
　r_t4_rxを削除し、その状態でコード生成を行う。

　ビルドする。ルネサスTCP/IP(T4)に関連する関数呼び出しでエラーになるはず。

　(コンパイルエラー1)
　../src/rx65n_rsk_aws.c(15):E0520005:Could not open source file "r_sys_time_rx_if.h"
　../src/rx65n_rsk_aws.c(17):E0520005:Could not open source file "r_t4_itcpip.h"

　main()付近で呼び出しているT4関連のヘッダ、関数呼び出し、変数等を削除していく。

　<<ここまででコンパイル通った>>

　(リンクエラー1)
　E0562310:Undefined external symbol "_get_random_number" referenced in ".\src\amazon-freertos-master\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.obj"
　E0562310:Undefined external symbol "_lan_read" referenced in ".\src\amazon-freertos-master\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.obj"
　E0562310:Undefined external symbol "_rcv_buff_release" referenced in ".\src\amazon-freertos-master\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.obj"
　E0562310:Undefined external symbol "_lan_open" referenced in ".\src\amazon-freertos-master\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.obj"
　E0562310:Undefined external symbol "_lan_write" referenced in ".\src\amazon-freertos-master\lib\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.obj"

　ここまでで、NetworkInterface.cの実装のみに絞られた。
　狙い通りT4関連のAPI呼び出しだけになっている。
　→t4_driver.c から関数の中身を持ってくる。
　　UBとかUHとかのITRONのデータ型定義は、uint8_tとかuint16_tとかの標準型に置き換える。
　　
　FreeRTOS_IPInit()で設定されるMACアドレスは、TCP/IP内部RAMに保持される分と
　Etherドライバ経由(R_ETHER_Open_ZC2())でMACのレジスタに保持される分と同じ値に
　する必要がある。FreeRTOS_IPInit()で設定したあとは、どこかの変数に保持されているから
　それをNetworkInterface.cに引き渡す必要があるが、xNetworkInterfaceInitialise()には
　引き渡されてこない。
　STのサンプルを見ると、ucMACAddress[]というグローバル変数で受け渡しをしている。いまいち。
　せめて、FreeRTOSConfig.hにあるコンフィグ値を大元とし、FreeRTOS_IPInit()と
　MAC層に分配してあげるべきであろう。(FreeRTOSIPConfig.hではないのか？これもいまいち）
　
　ここまでの改造で、r_sys_time_rx、r_t4_driver_rx、r_t4_rxを削除し、動作確認もOK。
　rx65n_envision_kit_awsも同じ改造を施し動作確認OK。

　ここまででコミット。(r5091)
　
　NetworkInterface.c に再度着目。課題の整理。過去の課題★のうちNetworkInterface.cのものをサルベージ。
　(1)なぜか lan_write()が-5を返す。あとで確認する。★
　(2)ゼロコピーの概念を無視するが...★
　(3)★後でEther割り込み駆動に必ず直す。
　(4)(送信側のゼロコピーの仕組みが不明。これもまた後で調べる★)
　(5)LANケーブル挿抜のイベントをTCP/IP側に通知する仕組みがあるが...★

　(1)なぜか lan_write()が-5を返す。あとで確認する。★
　　Etherドライバのリンク状態を表す変数「transfer_enable_flag」がONになっておらず
　　最初の1回だけlan_write()が-5を返しているようだ。
　　→xNetworkInterfaceInitialise()が完了時にEtherのリンクアップが完了しているとも限らないので、
　　　何らかFreeRTOS+TCPにリンクアップ/ダウンを通知する仕掛けがあると思うが、見当たらず。
　　　xNetworkInterfaceInitialise()が完了したらリンクアップは完了している前提とする。
　　　
　　　R_ETHER_CheckLink_ZC()でリンクチェックをするのは自分側のPHYの状態であり、
　　　MAC(Etherドライバ)が準備完了しているとは限らない。
　　　なのでEtherドライバからの準備完了のコールバック(callback_link_on())が来てから
　　　xNetworkInterfaceInitialise()を先に進めるようにする。
　　　callback_link_on()がくると、link_detectがETHER_FLAG_ON_LINK_ONになるのでこれで待つ。
　　　
　　　以上から、xNetworkInterfaceInitialise()の流れを整理する。
　　　
　　　整理前：
　　　　(1)lan_open() (MAC/PHYの初期化、オートネゴシエーション開始)
　　　　(2)R_ETHER_CheckLink_ZC()　PHYリンクアップ完了待ち
　　　　(3)ソフトウェアループ　＜適当＞
　　　　(4)受信タスク開始
　　　　(5)MAC層リンクチェック開始
　　　　(6)xNetworkInterfaceInitialise()終了→DHCP送信→xNetworkInterfaceOutput()失敗（まだMAC層準備完了してない）
　　　　　　→DHCPタイムアウト→DHCP再送→xNetworkInterfaceOutput()成功

　　　整理後：
　　　　(1)lan_open() (MAC/PHYの初期化、オートネゴシエーション開始)
　　　　(2)MAC層リンクチェック開始
　　　　(3)MAC層リンク完了待ち
　　　　(4)受信タスク開始
　　　　(5)xNetworkInterfaceInitialise()終了→DHCP送信→xNetworkInterfaceOutput()成功
　　　　　　まだ完ぺきではなくて、動作中にLANケーブルが
　　　　　　抜けたりささったりする動作に対応できてない。(活線挿抜）
　　　　　　現時点で、LANケーブルの挿抜に応じてEtherドライバはコールバックできるが、
　　　　　　これをFreeRTOS+TCPに通知する手段がない。
　　　　　　xSendEventStructToIPTask()で通知できるような気がするので後で調べる。
　　　　　　(5)にあとは任せて、(1)としては完了。
　　　　　　
　(2)ゼロコピーの概念を無視するが...★
　　→後回し。
　(3)★後でEther割り込み駆動に必ず直す。
　　receive_check()タスクでvTaskDelay()で横着しているところをサンプル同様に、ulTaskNotifyTake();を使用する。
　　Ether割り込みはlan_inthdr()に到着するので、ここでvTaskNotifyGiveFromISR()を使って
　　receive_check()タスクにイベント通知する。
　　→ここで実行してみるが、task.cのvTaskNotifyGiveFromISR()の実行時にAssertが発行されてフリーズしてしまう。
　　　割り込みからタスクに通知するのはやはりセンシティブなようだ。OSの機能について調べる必要がある。
　　　→task.cのvTaskNotifyGiveFromISR()の4799行目のAssertに止まっている。
　　　　→ether_receive_check_task_handleが初期化される前に、vTaskNotifyGiveFromISR()を呼んでしまっていた。
　　　　　→ether_receive_check_task_handleに値が入っていたらvTaskNotifyGiveFromISRを呼ぶif文を追加。
　　　　　　→task.cのvTaskNotifyGiveFromISR()の4817行目のportASSERT_IF_INTERRUPT_PRIORITY_INVALID()でフリーズしてしまう。
　　　　　　　→portASSERT_IF_INTERRUPT_PRIORITY_INVALID()を調べてみると以下の通り。
　　　　　　　　現在のIPLがconfigMAX_SYSCALL_INTERRUPT_PRIORITYより小さいとASSERTとなるようだ。
　　　　　　　	#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID() configASSERT( ( get_ipl() <= configMAX_SYSCALL_INTERRUPT_PRIORITY ) )
　　　　　　　	→lan_inthdr()実行中にget_ipl()を呼び出してIPL値を見ると、7となっている。
　　　　　　　	　configMAX_SYSCALL_INTERRUPT_PRIORITYの値は6だった。configMAX_SYSCALL_INTERRUPT_PRIORITYを7に変更。
　　　　　　　	　→ここまでの設定で、動作OKとなった。ping応答もこれまで最大10msのジッターがあったが、
　　　　　　　	　　Ether割り込みとFreeRTOS+TCPを繋いだおかげで、常に1ms以内に応答するようになった。完了。
　(4)(送信側のゼロコピーの仕組みが不明。これもまた後で調べる★)
　　→後回し。
　(5)LANケーブル挿抜のイベントをTCP/IP側に通知する仕組みがあるが...★
　　→課題としては(1)と同じ。

　残った課題は以下。ゼロコピーと活線挿抜である。
　　(2)ゼロコピーの概念を無視するが...★
　　(4)(送信側のゼロコピーの仕組みが不明。これもまた後で調べる★)
　　(5)LANケーブル挿抜のイベントをTCP/IP側に通知する仕組みがあるが...★

　ここまででEnvision KitとRSKのコードを同期させ動作確認し、一旦コミットする。(r5092)

　続いて、ゼロコピーにトライしてみる。
　　BufferAllocation_2.cをビルドから除外し、BufferAllocation_1.c を有効にする。
　　BufferAllocation_1.cを使用するときの vNetworkInterfaceAllocateRAMToBuffers() は何となく実装してあるので
　　ビルドは通る。が、予想通り、エラーメッセージで"INVALID BUFFER"とでる。
　　このときのvNetworkInterfaceAllocateRAMToBuffers()の後を辿っていってみる。

　　2/18に調査した時にたどり着いた、FreeRTOS_IP.cの917行目の分岐が犯人。
　　アライメントが取れてない。
　　
　　ここをクリアするためには、
　　ゼロコピー対応するためのsecond sampleというのを熟読する必要がある。
　　https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Porting.html#creating_a_zero_copy_network_port_layer

　　何となく分かったが、少し時間がかかりそうだ。次週に持ち込み。
　　
--------------------------------------------------------------------------
2018/02/24
　rx65n_rsk_awsで実験開始。
　ポーティングガイドを見てもう一度方向確認。
　BufferAllocation_2.cで簡易的に実装することにしたので、
　first exampleというのを実装してみる。
　https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Porting.html#creating_a_simple_network_interface_port_layer

　xNetworkInterfaceInitialise()の構造は特に問題なさそう。
　xNetworkInterfaceOutput()の実装は、送信ディスクリプタの開放が抜けていた。
　　→送信完了後に、vReleaseNetworkBufferAndDescriptor( pxDescriptor );を追加
　
　問題の受信側であるが、NOTE1にBufferAllocation_2.cを使用する場合は
　割り込みの最中にdiscriptorとEthernet buffersの割り当てが行えない、
　と書いてあった。このため、タスクを1個立ち上げてそこで受信割り込みから
　通知を受けた後バッファ割り当てを行い、IPタスクに通知を行うとのこと。
　これはDeferrd Interrupt Handlingといって以下に解説がある。
　https://www.freertos.org/deferred_interrupt_processing.html
　
　これはハードウェアから直接呼び出される割り込み関数では
　割り込み要因の記録とOS層への通知のみを実施し、速やかに割り込み関数を
　終了させるためのメカニズム。
　#OSが存在しない場合は、割り込みコントローラに優先度を設定して
　 多重割り込みを許可する手法で乗り切るが、
　 この仕組みはマイコンの仕組みに依存する。
　 
　割り込み関数からの通知は、deferred interrupt handler taskというので
　処理される。このタスクは通常のアプリケーションタスクより優先度が高い。
　
　When to use deferred interrupt handling：
　このメカニズムは以下3点のようなケースに威力を発揮する。
　・割り込み信号に応じて長い処理が必要
　・割り込み処理内でクリティカルな処理を自力で完全に実装するのが難しいと感じる場合
　　(RTOSが提供するAPIのみで構築したほうが簡単・安全であろう)
　・決まりきった動作をしない処理をさせる場合
　　（何らかの条件により処理時間が伸び縮みする場合）
　　
　→Etherの受信処理はまさに上記ケースに該当する。
　
　Techniques for deferring interrupt processing to a task：
　実装方法は以下2通りとのこと。
　　①1個のタスクで集中管理する（たとえばタイマータスク）
　　「割り込み発生→フラグON→タイマータスク起動→deferred interruptで処理」
　　②アプリがコントロールする（Task Notificationを使用）
　　「割り込み発生→Noticication→deferred interruptで処理」
　　
　　②の利点は割り込み発生から処理実行までのレイテンシが減る、とのこと。
　
　ポーティングガイドのprvEMACDeferredInterruptHandlerTask()のサンプルは
　②を使っているようだ。②を使って実装してみる。
　
　Etherを監視して無限ループするタスクを作る。receive_check()をこのタスクとする。
　
　Ether関連の初期化を行うxNetworkInterfaceInitialise()でタスクを作ればよいであろう。
　xTaskCreate()でタスクハンドルを作り、receive_check()を登録する。
	return_code = xTaskCreate(receive_check, "ETHER_RECEIVE_CHECK_TASK", 100/* stack size (word) */, 0,  configMAX_PRIORITIES, &ether_receive_check_task_handle);
	if(return_code == xTaskCreate)
	{
        	/* The task was created.  Use the task's handle to delete the task. */
        	vTaskDelete( ether_receive_check_task_handle );
	}


　これに伴い、check_ether_link()でreceive_check()を実行していたのをやめる。
　それで、Notifyで受信イベントを受けるのではなくて、
　vTaskDelay()で10msに1回ポーリングする簡易実装にする。
　
　ついでに、xNetworkInterfaceInitialise()でタイマータスクを作ってみる。
　今は、R_CMT_CreatePeriodic()で割り込み関数を定期的に呼び出しているところを
　タイマータスクに置き換える。
　https://www.freertos.org/FreeRTOS-timers-xTimerCreate.html
　
　xTimerCreate()でタイマーハンドルを作って、それをxTimerStart()に入力する感じか。
　
　timer_handle = xTimerCreate("ETHER_TIMER", 10, pdTRUE, &timer_id, check_ether_link);
　xTimerStart(timer_handle, 0);
　
　といったところか。未定義の変数はグローバルで定義しておく。
　これで、receive_check()がFreeRTOS配下のタスクとして呼び出された。
　
　ポーティングガイドのprvEMACDeferredInterruptHandlerTask()のサンプルに従って
　API呼び出し順序を確かめていく。
　
　・  ulTaskNotifyTake()
　　→これは上記「②アプリがコントロールする」場合に必要なイベント通知受付口。
　　　今回はvTaskDelay()を使って横着しているので不要。
　・ pxGetNetworkBufferWithDescriptor( xBytesReceived, 0 );
　　→受信データ長に応じたバッファを確保して、ディスクリプタを生成して戻り値で
　　　返してくれるAPI。
　　　RXマイコンのEtherコントローラドライバは、lan_read()の戻り値で受信データ長を
　　　戻してくれる。received_data_sizeに入れておく。
　　　また、引数のポインタに、バッファポインタを書いてくれる。
　　　memcpy()でコピーすればFreeRTOS+TCP側のバッファにコピーすればよいであろう。
　　　（あとでゼロコピー対応に進化させる）
　・ディスクリプタへのデータ設定、イベント通知用の構造体の設定、特に問題なさそう
　				pxBufferDescriptor->xDataLength = received_data_size;
				xRxEvent.eEventType = eNetworkRxEvent;
				xRxEvent.pvData = pxBufferDescriptor;

　実行して確認してみるも、やはりpxGetNetworkBufferWithDescriptor()が正常終了しない。
　pxGetNetworkBufferWithDescriptor()に潜って再確認していく。
　
　なんかセマフォが解放されてない様子？
　送信側xNetworkInterfaceOutput()でディスクリプタを確保して送信完了したときに
　リリースバッファのAPIを呼んでいるはずだが・・・？
　
　なぜかlan_write()が送信成功しているのに-5を返していて、if文が偽になっていた。
　暫定で偽のときもリリースバッファを呼ぶようにした。
　
　	return_code = lan_write(0, tmp_buf, 14, tmp_buf + 14, pxNetworkBuffer->xDataLength - 14);
	if(return_code > 0)
	{
		/* Call the standard trace macro to log the send event. */
		iptraceNETWORK_INTERFACE_TRANSMIT();

		vReleaseNetworkBufferAndDescriptor(pxNetworkBuffer);
	}
	else
	{
		/* なぜか lan_write()が-5を返す。あとで確認する。★ */
		vReleaseNetworkBufferAndDescriptor(pxNetworkBuffer);
	}
	
　ここまでの実装でDHCPシーケンスが完了するようになった。
　ping応答もできている。無事FreeRTOS+TCPの移植完了！！！！
　（★課題山盛りですが）
　
　RSKで動作確認していたので、Envision Kit側にもネットワークインタフェースのコードを
　登録してコミット。(r5066)
　
--------------------------------------------------------------------------
2018/02/18
　rx65n_envision_kitで作業開始。
　
　どうにもデバッガが重たい。FreeRTOS用のスタック(FreeRTOSConfig.hで設定)が
　足りていないような気がする。FreeRTOSがシステムスタック側に影響しているか？

　現在のRX65NのRAM使用状況を変更してみる。
　後でGUIを載せたときにチラつき防止のためダブルバッファにしたかったが致し方なし。

　0x00000000番地(256KB)：液晶バッファ1
　0x00800000番地(256KB)：液晶バッファ2
　0x00840000番地(128KB)：システム領域
　↓
　0x00000000番地(256KB)：システム領域(Bセクション(FreeRTOSスタック、ヒープ、その他変数等))
　0x00800000番地(256KB)：液晶バッファ2
　0x00840000番地(128KB)：システム領域(Bセクション以外(システムスタックやEtherバッファ等))

　これで、デバッガ重たい現象が改善された。
　main()ループにvTaskDelay(10000);を入れて無限ループさせ、
　main()ループが生きてるか確認するメッセージを出力するようにした。
　
　安定稼働しているようだ。ただしまだDHCP Discoverパケットを
　出力するための送信関数xNetworkInterfaceOutput()は呼ばれないようだ。

　ただし定常的にFreeRTOSかFreeRTOS+TCPからは謎のログ(文字化けしている？)は出力されている。(★調べる)→OK
　何やらステートマシンは動き続けている様子だ。

　ひとまず安定したので、いったんコミットする。(r5008)

　メッセージの文字化けの原因はきっと、#include <stdio.h> が無いからだ。
　FreeRTOS本体のコードでインクルードするわけにもいかないが、幸いコンフィグファイルは
　ユーザ定義部分に吐き出されている格好なのでそちらに#include <stdio.h>を追加。
　\rx65n_envision_kit_aws\src\amazon-freertos-master\demos\renesas\rx65n_envision_kit\common\config_files
　　FreeRTOSConfig.h
　　FreeRTOSIPConfig.h

　→無事、FreeRTOSとFreeRTOS+TCPのシステムログが見れるようになった。
　　エラー時のトレースログもだせるみたいなので、あとでこれも出力に繋いでみよう。
　　液晶表示が賑やかになってきた。
　　バックログが見えるように、UARTでも出力したほうがよい。
　　RSKは標準でUARTがUSBシリアル変換されてUSBから出てくるが、
　　Envision Kitはその仕掛けがない。（E2OBの機能はデバッガ使用時はUART出力できないので却下）
　　Envision KitはPMODにSCI2が配線されていて、PMODモジュールでUSBシリアル変換があるから
　　それを付ければUARTをUSBから出せる。（前に試してちゃんとできた）
　　　→https://store.digilentinc.com/pmod-usbuart-usb-to-uart-interface/
　　　　→これをEnvision KitのCH14の1-6に挿せばよい。
　　　　　SCI2をUARTモードで使用すればPCにシステムログを送れる。
　　　　　コードでいうと、lcd_conf.hで my_sw_charput_function()がprintf()経由BSPから呼ばれるが、
　　　　　今はここに液晶に1文字出力するようにしているところがあり、ここに並行してSCIから出力する
　　　　　ようにコード追加すればよい。(★後でメンテ)

　さて液晶表示のシステムログだけでもある程度何が起きているか分かるようになった。
　システムログ内容を紐解いてみる。
　　→案の定、DHCP discoverプロセスが、応答がなくてタイムアウトして、リトライ制御をし続けている。
　
　ディスクリプタを4個設定にしたのに、(now59)とか表示されてるので、ディスクリプタ周りの調整が
　何らか足りてないようだ。そもそも送信関数(xNetworkInterfaceOutput())すら呼ばれてないので、
　送信処理の最初の方でこけてそう。
　確か昨日、FreeRTOSIPConfig.hで60をEtherドライバ側の定義ETHER_CFG_EMAC_TX_DESCRIPTORS（中身は4)を
　読み込むようにしたはずだが・・・？
　
　FreeRTOSIPConfig.hを確認したらいつの間にかデフォルトの60に戻ってた。
　ガチャガチャやっててデフォルトで上書きしてしまったようだ。もう一度修正。
　
　期待通り(now59)の表示は(now3)となった。依然としてxNetworkInterfaceOutput()は呼ばれない。
　何かを見落としているか。
　
　FreeRTOS+TCPのIP層の実装(FreeRTOS_TCP_IP.c)を確認していく。
　xNetworkInterfaceOutput()を呼び出しているのは950行目1か所のみ。
　コメントに /* Send! */ と書いて有って気合が入っているので間違いないだろう。
　
　この呼び出し口の実装は、prvTCPReturnPacket()。この関数の先頭にブレークを
　しかけるも、反応なし。上位に上っていく。
　
　FreeRTOS_TCP_IP.c の実装に関数呼び出し順が記載されている。ありがたい。
　prvTCPReturnPacket()を呼び出しているところを片っ端からブレーク貼って網をはる。
　prvTCPSendRepeated()、prvTCPPrepareSend()、prvTCPHandleState()の先頭にブレークはる。
　
 * IP-Task:
 *		xTCPTimerCheck()				// Check all sockets ( declared in FreeRTOS_Sockets.c )
 *		xTCPSocketCheck()				// Either send a delayed ACK or call prvTCPSendPacket()
 *		prvTCPSendPacket()				// Either send a SYN or call prvTCPSendRepeated ( regular messages )
 *		prvTCPSendRepeated()			// Send at most 8 messages on a row
 *			prvTCPReturnPacket()		// Prepare for returning
 *			xNetworkInterfaceOutput()	// Sends data to the NIC ( declared in portable/NetworkInterface/xxx )
 
  *	xProcessReceivedTCPPacket()
 *		prvTCPHandleState()
 *			prvTCPPrepareSend()
 *				prvTCPReturnPacket()
 *				xNetworkInterfaceOutput()	// Sends data to the NIC
 *		prvTCPSendRepeated()
 *			prvTCPReturnPacket()		// Prepare for returning
 *			xNetworkInterfaceOutput()	// Sends data to the NIC
 
 
　結果、どこにもブレークしない。
　
　そもそもIPタスクに送信要求が届いてないのではないか。
　DHCPプロセスからIPタスクに送信要求を出す方向から見直してみる。
　
　DHCPプロセスからIPタスクに対する送信要求は、FreeRTOS_DHCP.cの
　926行目のFreeRTOS_sendto()と思われる。この中を確認していく。
　
　進んでいくと、FreeRTOS_Socket.cの855行目の
　pxUDPPayloadBuffer_to_NetworkBuffer()が呼ばれる。
　ここまでの処理にエラーなどは発生していない様子。
　
　FreeRTOS_IP.cの917行目の分岐にたどり着いた。
　if( ( ( ( uint32_t ) pucBuffer ) & ( sizeof( pucBuffer ) - 1 ) ) == 0 )が
　偽になるため、バッファポインタが登録されずpxResultにNULLがセットされ
　抜けてしまう。これが原因か。
　
　2/17に適当に実装したvNetworkInterfaceAllocateRAMToBuffers()が原因か。

　上記if()文の条件を確認すると、何やら送信バッファのアライメントが
　取れているかどうかを確認するためのものの様子。
　
　少し巻き戻って、FreeRTOS_Socket.cの829行目の条件分岐で以下ある。
　if( ( xFlags & FREERTOS_ZERO_COPY ) == 0 )
　
　これでFREERTOS_ZERO_COPYがONのほうに流れていっているのが原因のようだ。
　ゼロコピー実装は後回しにすると決めたので、なんでゼロコピーが有効に
　なっているのか調べる。
　
　FreeRTOS_DHCP.cの926行目をよくみると、FREERTOS_ZERO_COPYを直接指定していた。
　ということは、DHCPを使用するときはゼロコピー必須ということ。
　if( FreeRTOS_sendto( xDHCPData.xDHCPSocket, pucUDPPayloadBuffer, ( sizeof( DHCPMessage_t ) + xOptionsLength ), FREERTOS_ZERO_COPY, &xAddress, sizeof( xAddress ) ) == 0 )

　DHCP機能はさすがに切ることはできないので、
　先にゼロコピーの実装を済ませたほうが良さそうだ。
　
　Amazon FreeRTOS全文でZERO_COPYでGrep。PICのサンプルに以下2行を発見。
　これをRX側のFreeRTOSIPConfig.hに入れる。
　#define ipconfigZERO_COPY_TX_DRIVER              ( 1 )
　#define ipconfigZERO_COPY_RX_DRIVER              ( 1 )
　
　あと、#include "pack_struct_start.h"と#include "pack_struct_end.h"に挟まれた構造体の
　typedefに他コードからのシンボル参照(Ctrl+左クリック)ができず、エラー表示になっている。
　コンパイルは通るのでコンパイル前処理の構文解析でエラーになっている感じ。
　→★ツール開発部門に問い合わせてみる。
　
　翻って手元の実装。
　FreeRTOS_IP.cの917行目の分岐をもう一度見てみる。これが通らないと先に進まない。
　　if( ( ( ( uint32_t ) pucBuffer ) & ( sizeof( pucBuffer ) - 1 ) ) == 0 )

　ここに最初に到達した時のpucBufferの値は、0x84f716。
　この値がポインタサイズ(4)から1を引いた値でマスクを取り
　何かが残ったら（＝4バイトアライメント取れてなければ）偽。
　0x84f716が4の倍数になっていなければ通らないということ。
　このケースだと4の倍数になってないので通らない。
　
　この値がどこから来ているかというと、
　FreeRTOS_DHCP.cの926行目のpucUDPPayloadBufferである。
　このポインタは、921行目のprvCreatePartDHCPMessage()で出力される。この中身を探る。
　バッファ領域は、FreeRTOS_GetUDPPayloadBuffer()で取得するようだ。この中身を探る。

　このバッファ領域は、BufferAllocation_1.cから割り当てられている様子なので、
　BufferAllocation_1.c をビルドから除外、BufferAllocation_2.cをビルド対象とする。
　BufferAllocation_2.c はゼロコピー用のメモリ割り当て機構だ。
　
　ここまでの実装で、xNetworkInterfaceOutput()に送信データが引き渡されるようになった。

　なお、送受信バッファの取り扱いは要注意だ。
　FreeRTOSIPConfig.h に、 ipconfigPACKET_FILLER_SIZEというマクロが2で設定されている。
　これは、Etherパケットが以下構成になっていて、IPヘッダ以降が32ビットアライメント取れていない
　問題に起因する。これは世界共通の問題であり、通常MAC層でEtherヘッダとIPヘッダの間に2バイト
　パディングを挟み込んで解決している。（あるいはデータをコピーしなおしたり何らかのペナルティを背負って実装）
　RXマイコンのEtherMACは幸いこのパディング機能を持っているのでソフト次第で上手に解決できるはず。
　
　Etherパケット構成
　[ETHER_HEADER(14 byte)][IP_HEADER(20 byte)][TCP_HEADER(20 byte)][PAYLOAD(0-1460 byte)]
　↓実際にハードウェアが受信データをメモリ展開する際
　[ETHER_HEADER(14 byte)][IP_HEADER(20 byte)][TCP_HEADER(20 byte)][PAYLOAD(0-1460 byte)]・・・工夫無し
　[ETHER_HEADER(14 byte)][pad(2 byte)][IP_HEADER(20 byte)][TCP_HEADER(20 byte)][PAYLOAD(0-1460 byte)]・・・工夫有り
　
　ひとまず現状の構成でどのようにこの辺りが解釈されているか確認しつつ、送信側の実装を進めていく。
　
　単純にxNetworkInterfaceOutput()に渡ってきたデータを
　t4_driver.cが持っている lan_write() 関数に引き渡してみる。
　
　ゼロコピーの概念を無視するが、たぶん、xNetworkInterfaceOutput()で渡されるデータは
　Ether送信バッファのポインタであるため、いったんテンポラリに格納してから
　Etherドライバに渡すことにする。歪だがまだ全貌が分からないので仕方ない。あとで必ず直す。★
　
　ここまでで、RX65Nから、DHCP discoverパケットが出力できるようになった。Wiresharkで確認済。
　DHCPサーバからDHCP offerパケットが返ってきたので、正しいDHCP discoverとして扱われたようだ。
　RX65N側はこれをまだ受ける口を付けてないので、これ以上はDHCPシーケンスは進んでいない。
　
　MAC層で受信からのEDMAC転送は完了して割り込みは発生しているので、
　この割り込みをキャッチしてFreeRTOS+TCP側に受信割り込みイベントを伝える口を
　つけてやればいいはずだ。
　
　ポーティングガイドを見てみる。
　xSendEventStructToIPTask()で受信割り込みイベントを通知すればいいようだ。

　割り込みイベントはt4_driver.c の lan_inthdr()まで上がってきているはずだが
　t4_driver.c は最終的には不要。r_ether_rxのAPIを直接NetworkInterface.cから
　呼び出してあげればよい。今はとりあえずでEther割り込みではなく、
　リンク状態を定期的に調べる check_ether_link() で受信データがあった場合に
　FreeRTOS+TCPのxSendEventStructToIPTask()を呼び出すコードを追加する。
　★後でEther割り込み駆動に必ず直す。
　
　汚い実装だが、ひとまず受信ディスクリプタは正常に回転し始めた。
　受信リクエストを受け取ったFreeRTOS+TCPは、「CANNOT ADD 1」とメッセージを吐いている。
　
　受信したらpxGetNetworkBufferWithDescriptor()でバッファ/ディスクリプタを確保して
　ディスクリプタにバッファポインタと受信データ長を書き込んでxSendEventStructToIPTask()で
　渡せば良いようだ。
　
　今一度、ポーティングガイドのバッファ管理の項をよく読んでみる。
　https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Buffer_Management.html
　
　BufferAllocation_1.c はEtherバッファが静的にEtherドライバ内部にある場合、
　BufferAllocation_2.c はEtherバッファが動的にヒープから確保される場合、とある。

　ゼロコピーを使うか使わないかと勘違いしていたが、そうではなかった。
　BufferAllocation_1.c を使うようにする。
　
　vNetworkInterfaceAllocateRAMToBuffers()の実装体は送信バッファポインタを
　渡すようにしていたが、受信バッファのポインタを渡すように変更した。
　(送信側のゼロコピーの仕組みが不明。これもまた後で調べる★)
　
　BufferAllocation_1.cに変更したことで、ディスクリプタの初期値に不足がある様子。
　何やら受信リクエストをxSendEventStructToIPTask()で飛ばす前に
　pxGetNetworkBufferWithDescriptor()を実行するとおかしくなる。
　vNetworkInterfaceAllocateRAMToBuffers()の実装を見直す必要がありそう。
　
　動作としては安定しているのでひとまずコミットする。(r5009)
　→ただしDHCP送信はできなくなっている。おそらくディスクリプタの不調。
　　xNetworkInterfaceOutput()も呼ばれていない状態。
　
　rx65n_rsk 側にコードを反映した。(r5010)
　
　ひとまず、BufferAllocation_2.cを使うことにして、
　全体動作を確認することを優先することにする。
　DHCP送信まで出来ることを確認。
　受信についてもイベント通知はOS層まで届いているが
　ディスクリプタ内の受信バッファポインタが異常値なので
　データの受け渡し方法に問題があるようだ。(r5011)

--------------------------------------------------------------------------
2018/02/17
　rx65n_envision_kitで作業開始。

　NoMaY氏からの情報でAmazon FreeRTOSが 1.2.0 にバージョンアップされているとのこと。
　1.2.0のコードをダウンロードし、差分確認。
　NoMaY氏の情報通り、MQTT周りのセキュリティ実装が追加されている様子。
　RX65Nに移植済みのFreeRTOS本体部分は特に影響なし。
　リポジトリ内のコードを1.2.0のものに置き換えていく。

　そういえばバージョン表記がきになる。"version"でAmazon FreeRTOS全体をgrepしてみた。
　Amazon FreeRTOS全体を包含するバージョン定義はまだないようだ。
　Amazon FreeRTOS 1.2.0 に内包するソフトの主要なバージョンは以下の通り。
　・アプリ：v0.9.0
　　\amazon-freertos-master\demos\common\include\aws_application_version.h(29,13)  [SJIS]: #define APP_VERSION_MAJOR  0
　　\amazon-freertos-master\demos\common\include\aws_application_version.h(30,13)  [SJIS]: #define APP_VERSION_MINOR  9
　　\amazon-freertos-master\demos\common\include\aws_application_version.h(31,13)  [SJIS]: #define APP_VERSION_BUILD  0
　・mbed TLS：v2.6.0
　　\amazon-freertos-master\lib\third_party\mbedtls\include\mbedtls\version.h(40,17)  [SJIS]: #define MBEDTLS_VERSION_MAJOR  2
　　\amazon-freertos-master\lib\third_party\mbedtls\include\mbedtls\version.h(41,17)  [SJIS]: #define MBEDTLS_VERSION_MINOR  6
　　\amazon-freertos-master\lib\third_party\mbedtls\include\mbedtls\version.h(42,17)  [SJIS]: #define MBEDTLS_VERSION_PATCH  0
　・PKCS11：v2.40.0
　　\amazon-freertos-master\lib\third_party\pkcs11\pkcs11t.h(14,18)  [SJIS]: #define CRYPTOKI_VERSION_MAJOR 2
　　\amazon-freertos-master\lib\third_party\pkcs11\pkcs11t.h(15,18)  [SJIS]: #define CRYPTOKI_VERSION_MINOR 40
　　\amazon-freertos-master\lib\third_party\pkcs11\pkcs11t.h(16,18)  [SJIS]: #define CRYPTOKI_VERSION_AMENDMENT 0
　・FreeRTOSカーネル：v10.0.1
　　\amazon-freertos-master\lib\include\task.h(46,19)  [SJIS]: #define tskKERNEL_VERSION_NUMBER "V10.0.1"
　　\amazon-freertos-master\lib\include\task.h(47,19)  [SJIS]: #define tskKERNEL_VERSION_MAJOR 10
　　\amazon-freertos-master\lib\include\task.h(48,19)  [SJIS]: #define tskKERNEL_VERSION_MINOR 0
　　\amazon-freertos-master\lib\include\task.h(49,19)  [SJIS]: #define tskKERNEL_VERSION_BUILD 1
　・FreeRTOS+TCP：無し？

　作業に戻る。

　とにかく「FreeRTOS-Plus-TCP」をプロジェクト登録してみる。
　「\FreeRTOS-Plus-TCP\source\portable」にデバイス毎の独自部分を
　独自作成する必要がある。バッファ管理「BufferManagement」コンパイラ毎定義「Compiler」
　ネットワーク層「NetworkInterface」の3種類を独自作成しなければならない。

　http://japan.renesasrulz.com/cafe_rene/f/forum21/4772/amazon-freertos-rx/26876#26876
　上記URLのNoMaY氏の先行調査を頭に入れながら、コンパイル開始。

　(コンパイルエラー1)FreeRTOS+TCP関連のヘッダファイル見つからない
　　→「\amazon-freertos-master\lib\FreeRTOS-Plus-TCP\include」にインクルードパスを通す
　(コンパイルエラー2)"FreeRTOSIPConfig.h"が見つからない
　　→「\amazon-freertos-master\demos\vendor\board\common\」にあるようだ。
　　　ベンダ毎のBSPのコンフィグに入れると良いようだ。
　　　「\rx65n_envision_kit_aws\src\smc_gen\r_config」に入れておく。]
　　　　★あとで \amazon-freertos-master\demos\renesas\rx65n_envision_kit\common\config_files 
　　　　　にFreeRTOSのコンフィグを引越し。→完了
　(コンパイルエラー3)"pack_struct_start.h"が見つからない
　　→これは、TCP/IPのヘッダを表す構造体に pack オプションを付けるための
　　　マルチコード対応用のヘッダ。
　　　「\rx65n_envision_kit_aws\src\FreeRTOS-Plus-TCP\source\portable\Compiler\Renesas」にインクルードパスを通す
　(SH用コードが実装されているので、RX用コードに変更/NoMaY氏の先行調査参照)
　　→"pack_struct_start.h", "pack_struct_end.h"
　(コンパイルエラー4)"FreeRTOS_HTTP_commands.h"が見つからない
　　→「\rx65n_envision_kit_aws\src\FreeRTOS-Plus-TCP\source\protocols\include」ににインクルードパスを通す
　(コンパイルエラー5)"ff_stdio.h"が見つからない
　　→"\rx65n_envision_kit_aws\src\FreeRTOS-Plus-TCP\source\protocols\HTTP\FreeRTOS_HTTP_server.c"
　　　エラー元はこのファイル。
　　　FTPサーバのコードからファイルシステム系のソフトが呼び出されている様子。
　　　今はまだFTPサーバ機能は要らないのでコード自体をプロジェクトから除外しておく。
　　　TCP/IPの上位にあたるプロトコル群（HTTPとFTPとNTPとサーバ設置用コード）に関する以下ファイルをプロジェクトから除外。
　　　・FreeRTOS_HTTP_server.c
　　　・FreeRTOS_HTTP_commands.c
　　　・FreeRTOS_FTP_server.c
　　　・FreeRTOS_FTP_commands.c
　　　・FreeRTOS_TCP_server.c
　　　・NTPDemo.c
　　(コンパイルワーニング1)
　　　→packオプション周りのワーニングがいっぱいでるので、NoMaY氏の先行調査を参考に修正。
　　
　　<<ここまででコンパイル通った>>
　　
　　(リンクエラー1)
　　　E0562300:Duplicate symbol "_uxGetMinimumFreeNetworkBuffers"
　　　　→バッファ管理「BufferManagement」に2個同じようなファイルが入っているのが原因であろう。
　　　　　中身はあとで確認するとして、BufferAllocation_2.cをビルドから除外しとく。
　　　　　
　　(リンクエラー2)
　　　E0562310:Undefined external symbol "_ulRand" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_DHCP.obj"
　　　E0562310:Undefined external symbol "_ulRand" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_Sockets.obj"
　　　E0562310:Undefined external symbol "_pcApplicationHostnameHook" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_DHCP.obj"
　　　E0562310:Undefined external symbol "_vNetworkInterfaceAllocateRAMToBuffers" referenced in ".\src\FreeRTOS-Plus-TCP\source\portable\BufferManagement\BufferAllocation_1.obj"
　　　E0562310:Undefined external symbol "_xApplicationDNSQueryHook" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_DNS.obj"
　　　E0562310:Undefined external symbol "_vApplicationIPNetworkEventHook" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_IP.obj"
　　　E0562310:Undefined external symbol "_xNetworkInterfaceOutput" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_ARP.obj"
　　　E0562310:Undefined external symbol "_xNetworkInterfaceOutput" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_IP.obj"
　　　E0562310:Undefined external symbol "_xNetworkInterfaceOutput" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_TCP_IP.obj"
　　　E0562310:Undefined external symbol "_xNetworkInterfaceOutput" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_UDP_IP.obj"
　　　E0562310:Undefined external symbol "_xNetworkInterfaceInitialise" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_IP.obj"

　　　→ネットワーク層(IP層)とデータリンク層(Ether層)のインタフェースの実装体が無い感じ。
　　　　これはFreeRTOS+TCPのポーティングガイドに従って実装すればよさげ。
　　　　ラッキーなことに、SH2A用のNetworkInterface.cが用意されているではないか。（AmazonがSH2Aでトライアルしたのかな？）
　　　　SH2AとRXのEtherコントローラは同じで、Etherドライバも似た感じなので、SH2A用のコードを拝借して、
　　　　以下ファイルを作成。
　　　　\rx65n_envision_kit_aws\src\FreeRTOS-Plus-TCP\source\portable\NetworkInterface\RX\NetworkInterface.c
　　
　　<<新たなコードを追加したのでコンパイルエラー発生>>

　　(コンパイルエラー6)
　　　E0520005:Could not open source file "hwEthernet.h"
　　　　→RXマイコンのEtherドライバのヘッダファイルは、r_ether_rx_if.hである。変更。
　　
　　(コンパイルエラー7)
　　　E0520020:Identifier "NetworkBufferDescriptor_t" is undefined
　　　　→Amazon FreeRTOS全体をgrepすると、FreeRTOS_IP.h に定義があった。
　　　　　→NetworkInterface.cに #include "FreeRTOS_IP.h" を足してあげる。
　　　　
　　(コンパイルエラー8)
　　　E0520136:Struct "xNETWORK_BUFFER" has no field "pucBuffer"
　　　　→xNETWORK_BUFFERにはpucBufferは無いようだ。
　　　　　このエラーはvEMACCopyWrite()関数内のものであるので、きっと送信ができなくなる
　　　　　だろうけれど、ひとまずコメントアウトしておく。
　　　　
　　<<ここまででコンパイル通った>>

　　(リンクエラー3)
　　　(リンクエラー2)と比較して、_xNetworkInterfaceOutput()、_xNetworkInterfaceInitialise()のリンクエラーが消えた。
　　　だけれど、まだ大半のインタフェース関数が未実装。SH2A版のコードはトライアル早々に諦められたようだ。
　　　今は空関数を作っておいて、あとで動かしつつポーティングガイドを見ながら実装してみる。
　　　関数の定義はポーティングガイドにあるはずだ。NoMaY氏のメモを見る。以下参考になりそう。
　　
　　　Porting FreeRTOS+TCP to a Different Microcontroller
　　　http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/Embedded_Ethernet_Porting.html
　　　
　　　ざっと見ると、性能を出すためには、このポーティングガイドを本気で読む必要がある。
　　　単に動かすだけならとにかく関数同士をつないであげれば良さそう。
　　　必要なAPIはすでにRX用のEtherドライバに有る感じ。
　　　無線LAN対応時もここの実装がキモになる。
　　　
　　　_xNetworkInterfaceOutput()と、_xNetworkInterfaceInitialise()が真にFreeRTOS+TCPから
　　　呼ばれるインタフェースのようだ。他のはどうだろうか。
　　　コードのくっつき具合から判定していく。
　　　
　　E0562310:Undefined external symbol "_ulRand" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_DHCP.obj"
　　E0562310:Undefined external symbol "_ulRand" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_Sockets.obj"
　　　→関数名から、乱数を取得するための関数を期待しているようだ。
　　　　NetworkInterface.cに空関数を用意しておく。あとで中身をつくる。
　　　　ルネサスTCP/IPのt4_driver.cに疑似乱数を生成する似たような関数が有るのでそれを持ってくる。
　　　　最終的にはRX65N内蔵の暗号器にある乱数発生器のAPIを使うとよいであろう。
　　　　
　　　　関数プロトタイプは uint32_t ulRand(void); FreeRTOSIPConfig.h にexternで宣言がある。
　　　　NetworkInterface.cに実体、NetworkInterface.hにプロトタイプ宣言を置けばよいかな。
　　　　
　　E0562310:Undefined external symbol "_pcApplicationHostnameHook" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_DHCP.obj"
　　　→ポーティングガイドに載ってないので、Amazon FreeRTOS全文grepから予想。
　　　　DHCPでホスト名を登録する機能があるが、ホスト名をユーザ定義関数で引き渡せる仕組みのようだ。
　　　　DHCP動作中にこの関数が呼ばれてユーザが指定した名前がDHCPにより取得される。
　　　　FreeRTOS+TCPコンフィグ(FreeRTOSIPConfig.h)でON/OFFできる。
　　　　#define ipconfigDHCP_REGISTER_HOSTNAME           1
　　　
　　　　いくつかのサンプル(PIC用、Win用）でmain.cに実装体があった。
　　　　コメントを読むと、DHCPでユーザの名前をネットワークに登録する際に
　　　　DHCPのコードがそのユーザの名前を読み込むときに呼び出すユーザ定義関数である。
　　　　まあmain関数付近に実装しておくのが妥当。
　　　　
　　　　以下PICのサンプル。LLMNR、NBNSがOFF、DHCP_REGISTER_HOSTNAMEがONのときに必要な関数。
　　　　以下PICのコードをそのままmain付近(rx65n_envision_kit_aws.c)に持ってくる。
　　　　IPコンフィグでLLMNR、NBNSはOFFにしておく。
　　　　#include "FreeRTOSIPConfig.h"のインクルードも追加しておく。
　　　　
　　　　#if ( ipconfigUSE_LLMNR != 0 ) || ( ipconfigUSE_NBNS != 0 ) || ( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
　　　　const char * pcApplicationHostnameHook( void )
　　　　{
　　　　    /* Assign the name "FreeRTOS" to this network node.  This function will
　　　　     * be called during the DHCP: the machine will be registered with an IP
　　　　     * address plus this name. */
　　　　    return "RX65N_FREERTOS_TCP_TEST_BY_SHELTY";
　　　　}
　　　　#endif

　　E0562310:Undefined external symbol "_vNetworkInterfaceAllocateRAMToBuffers" referenced in ".\src\FreeRTOS-Plus-TCP\source\portable\BufferManagement\BufferAllocation_1.obj"
　　　→ポーティングガイドに載ってる。
　　　　BufferAllocation_1.cのみを使用する場合に必要になる関数のようだ。
　　　　先ほど、BufferAllocation_2.cはビルド対象外にしたが、このバッファ確保をうまく使いこなせば、
　　　　性能向上が出来る感じ。たぶん、Ether層からデータを吸い上げる時にTCP/IP層に対して
　　　　データコピーで渡すか、ポインタで渡すか（ゼロコピー）切り替えるものだ。
　　　　ただしゼロコピーは実装が面倒。RXマイコンのEtherドライバはゼロコピー対応なので、
　　　　あとでじっくり性能評価しながら作り込みをしてみる。
　　　　性能はあとで改善するとして、今はvNetworkInterfaceAllocateRAMToBuffers()を実装しておく。
　　　　実装体は後で考えるとして、今はvNetworkInterfaceAllocateRAMToBuffersを空にしておく。
　　　
　　E0562310:Undefined external symbol "_xApplicationDNSQueryHook" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_DNS.obj"
　　　→いつの間にか消えてた。たぶん"pcApplicationHostnameHook"の対処あたりと関係しているようだ。
　　　　後で調べる。
　　　　
　　E0562310:Undefined external symbol "_vApplicationIPNetworkEventHook" referenced in ".\src\FreeRTOS-Plus-TCP\source\FreeRTOS_IP.obj"
　　　→最後の1個だ。
　　　　ポーティングガイドに載ってないので、Amazon FreeRTOS全文grepから予想。
　　　　PICのサンプルがヒット。IP層からのイベント通知用の様子。
　　　　コードを見ると、DHCP処理が完了した時のコールバックが書かれてる感じ。
　　　　他にもいろいろなイベントが飛んできそう。PICのコードをmain付近(rx65n_envision_kit_aws.c)に持ってくる。
　　　　ネットワークインタフェースを作り込んだときの動作確認に役に立つであろう。
　　　　
　　　　PIC特有の以下コードは除外しておく。
　　　  /* The network is up so we can run. */
        if( ( SYSTEM_Init() == pdPASS ) && ( xTasksAlreadyCreated == pdFALSE ) )
        {
            /* Run all demos. */
            DEMO_RUNNER_RunDemos();
            xTasksAlreadyCreated = pdTRUE;
        }
　　　
　　<<ここまででリンク通った>>
　　
　　　ダウンロードして実行してみる。
　　　従来通り、ルネサスTCP/IPが起動し、無事DHCPでIPアドレス取得できている。
　　　FreeRTOS+TCPを入れたことで、従来の動作(主にEtherドライバ部分)に影響ないことを確認。
　　　これから、main付近(rx65n_envision_kit_aws.c)を改造し、ルネサスTCP/IPの初期化部分を
　　　除外していき、代わりにFreeRTOS+TCPno初期化部分を入れていく。
　　　
　　　configPRINTF()を有効にするため、FreeRTOS.hの以下を変更
　　　	#define configPRINTF( X )
　　　	↓
　　　	#define configPRINTF( X ) printf X
　　　	
　　　FreeRTOS_debug_printf()を有効にするため、FreeRTOSIPConfig.hの以下を変更
　　　	#define ipconfigHAS_DEBUG_PRINTF    0
　　　	↓
　　　	#define ipconfigHAS_DEBUG_PRINTF    1
　　　	
　　　RX65Nの今回のシステムはprintf()出力を液晶表示に繋いであるので、
　　　これでFreeRTOS関連のシステムログが液晶表示に出てくるはず。
　　　
　　　PICのコードを見ると、FreeRTOS_IPInit()を呼び出せばTCP/IPの初期化が走るようだ。
　　　FreeRTOS_IPInit()の実装体を確認する。
　　　第1～第4引数はIP関連のパラメータ。DHCP有効であれば自動取得するはずなので設定不要。
　　　第5引数のMACアドレスは入力必要であろう。適当にMACアドレスの配列を作って入力する。
      FreeRTOS_IPInit(0, 0, 0, 0, renesas_mac_address);
      このときのmain()は以下のように実装。

void main(void)
{
	char buff[256];

	R_Pins_Create();
	R_SYS_TIME_Open();
	lcd_open();
	R_SIMPLE_GRAPHIC_Open();

	/* hello message */
	sprintf(buff, "hello, this is RX65N Envision Kit system log.\r\n");
	printf("%s", buff);
	sprintf(buff, "built in %s, %s\r\n", __DATE__, __TIME__);
	printf("%s", buff);
	sprintf(buff, "\r\n");
	printf("%s", buff);

    	FreeRTOS_IPInit(0, 0, 0, 0, renesas_mac_address);
}

　　　FreeRTOS_IPInit()終了後、IPタスクが起動して、prvIPTask()は呼ばれたようだ。
　　　eNetworkDownEventのprvProcessNetworkDownEvent()が呼ばれたところで暴走。
　　　詳細を追いかけていく。
　　　
　　　xNetworkInterfaceInitialise()が呼ばれるが初期化関数が無い状態。
　　　ひとまずlan_open()を呼んでおく。xNetworkInterfaceInitialise()としては
　　　戻り値は 1(pdTRUE) が成功のようだが、lan_open()は成功時 0 が戻るので調整しておく。
　　　RXマイコンのEtherは、LANケーブル挿抜の状態を確認するため、R_ETHER_LinkProcess()を定期的に
　　　呼び出す必要がある。FreeRTOSでタスクを生成して監視させれば良いが、
　　　まだFreeRTOSの使い方がよくわからないため、CMTを1チャネル消費して監視させる。(後で調べる)★→OK
　　　
　　　たぶん、LANケーブル挿抜のイベントをTCP/IP側に通知する仕組みがあるが、ひとまず、
　　　xNetworkInterfaceInitialise()が完了したときにリンクアップ完了しているように
　　　xNetworkInterfaceInitialise()内部で待つようにしておく。(後でちゃんと実装する)★
　　　
　　　暴走はなくなった。でもxNetworkInterfaceInitialise()が失敗したら常に暴走するのでは？(後で調べる)★→現状特に問題なし
　　　IPタスクのループが動き続けていることを確認。
　　　
　　　TCP/IP起動時はきっとDHCPが動くであろうから、DHCPの処理を追いかけていく。
　　　どこかでDHCPの初手である、DHCP Discover Packetの生成を行っていて、
　　　その後、ネットワーク層に送信するはずである。
　　　
　　　DHCPの送信の前に、そもそも送信受信用のバッファが適切に割り当てられていないはず。
　　　なぜなら、vNetworkInterfaceAllocateRAMToBuffers()が未実装だから。
　　　まずはこれを調整する。
　　　
　　　ポーティングガイドを見ると、ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS で定義される分だけ、
　　　バッファを割り当てよ、と書いてある。検索すると、FreeRTOSIPConfig.hに60と定義されている。
　　　おそらくここで割り当てるべきは送信バッファのみと思われる。(本当か？）★→送信バッファではなく受信バッファ。
　　　RXマイコンのEtherの実際の送信バッファ数は、r_ether_rx_config.hのETHER_CFG_EMAC_TX_DESCRIPTORSで定義される。
　　　60の代わりに、これを読み込んでおく。
　　　
　　　ETHER_CFG_EMAC_TX_DESCRIPTORSがデフォルト1だけれど、1のままだと暴走する。
　　　4に増やす。ETHER_CFG_EMAC_RX_DESCRIPTORSも4に増やす。(なぜか？）★→1でも大丈夫になった。
　　　
　　　vNetworkInterfaceAllocateRAMToBuffers()の実装体は、Etherドライバにおける
　　　送信バッファのポインタをFreeRTOS+TCPに渡すように実装する。
　　　
　　　vDHCPProcess()を内部に進む。
　　　　中身は伝統的なswitch-caseのステートマシンである。
　　　　予想通り、最初の状態にdiscover requestの送信があった。
　　　　/* Send the first discover request. */
　　　　prvSendDHCPDiscover()に潜っていく。
　　　　　正常に送信リクエストが出ているようだが、vApplicationStackOverflowHook()で停止する。
　　　
　　　r_bsp_config.hでスタックを増やしてみる。FreeRTOSはユーザスタックも使用するようだ。
　　　　#pragma stacksize si=0x400
　　　　↓
　　　　#pragma stacksize si=0x2000

　　　　#pragma stacksize su=0x1000
　　　　↓
　　　　#pragma stacksize su=0x2000

　　　スタック周りを触っていたら、何度かIPタスクのループを
　　　実行したところで暴走するようになってしまった。元にも戻せなくなった。
　　　(動いている状態をバックアップしておけばよかった）
　　　
　　　今日はここで終わり！

--------------------------------------------------------------------------
2018/02/14
rx65n_rsk_awsで作業開始。
FreeRTOSの 8.2.2のコードを結合していく。

FreeRTOS内のport.cに #include "iodefine.h"があったので、 
#include "platform.h" に変更

\rx65n_rsk_aws\src\amazon-freertos-master\lib\FreeRTOS\Source\portable\MemMangの
heap_3.c と heap_4.c が両方ビルド対象になっているが、heap_4.c のみが適切。
何でそうなのかは知らない。　（なんでか後で調べる）

インクルードファイルパスの調整などを経て 8.2.2の環境で動作確認完了。
FreeRTOSのコードを 9.0.0に変更し動作確認完了。
FreeRTOSのコードを 11.0.0に変更し動作確認完了。

rx65n_envision_kitにボード非依存部分のコードを移植し動作確認。問題なし。

--------------------------------------------------------------------------
2018/02/12
コード調整再開。

先ず作戦を考える。NoMaY氏は上位レイヤーからの調整を実施しているので、
私は下位レイヤーから積み上げていき、良いタイミングでマージするのが良い。
NoMaY氏の活動をみると大きく4点ステップがありそう。
①FreeRTOSで動作させる
②FreeRTOS+TCPを動作させる
③mbed TLSを動作させる
④実際にAWSに繋ぐ

先ずは①を対応するため、RX65N用に用意されたOSパッケージを参考にする。
https://www.renesas.com/en-us/software/D6001925.html

この中には、特殊なFITモジュールが入っていることはNoMaY氏の調査により確認済み。
r_bsp_rtos
r_cmt_rtos

これを(2017/12/29)のものに含まれる以下2点と置き換えてやれば
FreeRTOSと結合できるのではないか。
r_bsp
r_cmt_rx

diffをとって、クリティカルなところを(2017/12/29)に反映していく。

(1)r_bsp の resetprg.c でOS用のインクルードファイルを呼んでるところ
#if BSP_CFG_RTOS_USED == 0      // Non-OS
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
    #include    "freertos_usr_func.h"    // FreeRTOS's user configuration
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif

(2) r_bsp の resetprg.c と r_cmt_rx の r_cmt_rx.c において、
　上記マクロに伴いFreeRTOS用のコード各所
　（カーネルのスケジューラを呼び出しているところなど）も反映。

(3) r_bsp の r_bsp.h にFreeRTOSのヘッダファイル群のインクルードを追加。
　ユーザからはRXマイコンのFIT群のヘッダファイルplatform.h をインクルード
　すればよいようになる。

#if BSP_CFG_RTOS_USED == 0      // Non-OS
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "croutine.h"
#include "timers.h"
#include "event_groups.h"
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#endif

(3)BSPのコンフィグ(r_bsp_config.h)にあるリアルタイムOS切り替えの
　マクロ(BSP_CFG_RTOS_USED)でFreeRTOSを選択する。
　すると、(2)で反映した部分のFreeRTOS関連のコードが有効になる。
　FreeRTOSに対応するマクロ番号は(1)。
　デフォルトの(0)だとRTOSを使わない設定となる。

(4)BSPのコンフィグ(r_bsp_config.h)にあるタイマチャネルの指定について以下追加。
/* This macro is used to select which CMT channel used for system timer of RTOS.
 * The setting of this macro is only valid if the macro BSP_CFG_RTOS_USED is set to a value other than 0. */
#if (BSP_CFG_RTOS_USED != 0)
/* Setting value.
 * 0      = CMT channel 0 used for system timer of RTOS (recommended to be used for RTOS).
 * 1      = CMT channel 1 used for system timer of RTOS.
 * 2      = CMT channel 2 used for system timer of RTOS.
 * 3      = CMT channel 3 used for system timer of RTOS.
 * Others = Invalid. */
#define BSP_CFG_RTOS_SYSTEM_TIMER		(0)
#endif

さて、次に使用するFreeRTOSのバージョン。
私が動作確認したことがあるのは8.2.2で、上記RX65N用に用意された
OSパッケージは9.0.0で、Amazon FreeRTOSは11.0.0である。
ひとまず、8.2.2を持ってきて、差分を埋めていく感じで進める。

協力会社と連携するため、ボードをRX65N RSKに乗り換え。
以降、rx65n_rsk_aws フォルダに移動。

--------------------------------------------------------------------------
2018/02/09
諸事情により開発一時停止。

--------------------------------------------------------------------------
2017/12/29 
RX65N Envision Kitを素材にする。
購入時はLANコネクタとSDスロットなどが付いてないので増設が必要。
先ずはe2 studio のスマートコンフィグレータ機能で
以下モジュールをインポートする。
- r_bsp
- r_cmt_rx
- r_ether_rx
- r_sys_time_rx
- r_t4_rx
- r_t4_driver_rx

⇒これでEther関連の動作確認ができる。
　端子設定はRX65N Envision Kitのコードと等しいかどうか調べながら調整。
　同じにすればEther関連は動くはず。
- r_pincfg

FITモジュール以外の以下ソースコードはRX65N Envision Kitのコードから流用する。
- lcd_conf.c
- r_ascii.c
- r_ascii.h
- r_simple_graphic.c
- r_simple_graphic_if.h

⇒これでprintf()でRX65N Envision Kitの液晶に文字表示が可能。
　システムログを画面に出す。
　ポーティングが進めば、SeggerのGUIも載せてウィンドウ形式にしたい。

ここまでの動作確認でPCからpingが通ることを確認した。

