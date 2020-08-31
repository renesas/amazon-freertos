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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name : r_intc_lld_rza2m.h
* Description :
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "driver.h"

#ifndef R_INTC_LLD_RZA2M_H_
#define R_INTC_LLD_RZA2M_H_

/******************************************************************************
Macro definitions
******************************************************************************/

/* Version Number of API */

#define STDIO_INTC_RZ_LLD_DRV_NAME ("LLD EBK_RZA2M INTC")

/** Major Version Number of API */
#define STDIO_INTC_RZ_LLD_VERSION_MAJOR      (1)
/** Minor Version Number of API */
#define STDIO_INTC_RZ_LLD_VERSION_MINOR      (4)
/** Minor Version Number of API */
#define STDIO_INTC_RZ_LLD_BUILD_NUM          (0)
/** Unique ID */
#define STDIO_INTC_RZ_LLD_UID                (66)

/* ==== Interrupt ID ==== */
#define INTC_GIC_ID_TOTAL               (512)   /* The number of interrupt sources */
#define INTC_ID_GIC_SW0                 ((e_r_drv_intc_intid_t)0)
#define INTC_ID_GIC_SW1                 ((e_r_drv_intc_intid_t)1)
#define INTC_ID_GIC_SW2                 ((e_r_drv_intc_intid_t)2)
#define INTC_ID_GIC_SW3                 ((e_r_drv_intc_intid_t)3)
#define INTC_ID_GIC_SW4                 ((e_r_drv_intc_intid_t)4)
#define INTC_ID_GIC_SW5                 ((e_r_drv_intc_intid_t)5)
#define INTC_ID_GIC_SW6                 ((e_r_drv_intc_intid_t)6)
#define INTC_ID_GIC_SW7                 ((e_r_drv_intc_intid_t)7)
#define INTC_ID_GIC_SW8                 ((e_r_drv_intc_intid_t)8)
#define INTC_ID_GIC_SW9                 ((e_r_drv_intc_intid_t)9)
#define INTC_ID_GIC_SW10                ((e_r_drv_intc_intid_t)10)
#define INTC_ID_GIC_SW11                ((e_r_drv_intc_intid_t)11)
#define INTC_ID_GIC_SW12                ((e_r_drv_intc_intid_t)12)
#define INTC_ID_GIC_SW13                ((e_r_drv_intc_intid_t)13)
#define INTC_ID_GIC_SW14                ((e_r_drv_intc_intid_t)14)
#define INTC_ID_GIC_SW15                ((e_r_drv_intc_intid_t)15)
#define INTC_ID_INT16                   ((e_r_drv_intc_intid_t)16)
#define INTC_ID_INT17                   ((e_r_drv_intc_intid_t)17)
#define INTC_ID_INT18                   ((e_r_drv_intc_intid_t)18)
#define INTC_ID_INT19                   ((e_r_drv_intc_intid_t)19)
#define INTC_ID_INT20                   ((e_r_drv_intc_intid_t)20)
#define INTC_ID_INT21                   ((e_r_drv_intc_intid_t)21)
#define INTC_ID_INT22                   ((e_r_drv_intc_intid_t)22)
#define INTC_ID_INT23                   ((e_r_drv_intc_intid_t)23)
#define INTC_ID_INT24                   ((e_r_drv_intc_intid_t)24)
#define INTC_ID_INT25                   ((e_r_drv_intc_intid_t)25)
#define INTC_ID_INT26                   ((e_r_drv_intc_intid_t)26)
#define INTC_ID_INT27                   ((e_r_drv_intc_intid_t)27)
#define INTC_ID_INT28                   ((e_r_drv_intc_intid_t)28)
#define INTC_ID_INT29                   ((e_r_drv_intc_intid_t)29)
#define INTC_ID_INT30                   ((e_r_drv_intc_intid_t)30)
#define INTC_ID_INT31                   ((e_r_drv_intc_intid_t)31)
#define INTC_ID_CA9_MAIN_PMUIRQ0        ((e_r_drv_intc_intid_t)32)
#define INTC_ID_CA9_MAIN_COMMRX         ((e_r_drv_intc_intid_t)33)
#define INTC_ID_CA9_MAIN_COMMTX         ((e_r_drv_intc_intid_t)34)
#define INTC_ID_CA9_MAIN_CTIIRQ         ((e_r_drv_intc_intid_t)35)
#define INTC_ID_IRQ_IRQ0                ((e_r_drv_intc_intid_t)36)
#define INTC_ID_IRQ_IRQ1                ((e_r_drv_intc_intid_t)37)
#define INTC_ID_IRQ_IRQ2                ((e_r_drv_intc_intid_t)38)
#define INTC_ID_IRQ_IRQ3                ((e_r_drv_intc_intid_t)39)
#define INTC_ID_IRQ_IRQ4                ((e_r_drv_intc_intid_t)40)
#define INTC_ID_IRQ_IRQ5                ((e_r_drv_intc_intid_t)41)
#define INTC_ID_IRQ_IRQ6                ((e_r_drv_intc_intid_t)42)
#define INTC_ID_IRQ_IRQ7                ((e_r_drv_intc_intid_t)43)
#define INTC_ID_PL310_PL310ERR          ((e_r_drv_intc_intid_t)44)
#define INTC_ID_DMAC30_DMAINT0          ((e_r_drv_intc_intid_t)45)
#define INTC_ID_DMAC30_DMAINT1          ((e_r_drv_intc_intid_t)46)
#define INTC_ID_DMAC30_DMAINT2          ((e_r_drv_intc_intid_t)47)
#define INTC_ID_DMAC30_DMAINT3          ((e_r_drv_intc_intid_t)48)
#define INTC_ID_DMAC30_DMAINT4          ((e_r_drv_intc_intid_t)49)
#define INTC_ID_DMAC30_DMAINT5          ((e_r_drv_intc_intid_t)50)
#define INTC_ID_DMAC30_DMAINT6          ((e_r_drv_intc_intid_t)51)
#define INTC_ID_DMAC30_DMAINT7          ((e_r_drv_intc_intid_t)52)
#define INTC_ID_DMAC30_DMAINT8          ((e_r_drv_intc_intid_t)53)
#define INTC_ID_DMAC30_DMAINT9          ((e_r_drv_intc_intid_t)54)
#define INTC_ID_DMAC30_DMAINT10         ((e_r_drv_intc_intid_t)55)
#define INTC_ID_DMAC30_DMAINT11         ((e_r_drv_intc_intid_t)56)
#define INTC_ID_DMAC30_DMAINT12         ((e_r_drv_intc_intid_t)57)
#define INTC_ID_DMAC30_DMAINT13         ((e_r_drv_intc_intid_t)58)
#define INTC_ID_DMAC30_DMAINT14         ((e_r_drv_intc_intid_t)59)
#define INTC_ID_DMAC30_DMAINT15         ((e_r_drv_intc_intid_t)60)
#define INTC_ID_DMAC30_DMAERR0          ((e_r_drv_intc_intid_t)61)
#define INTC_ID_DMAC30_DMAERR1          ((e_r_drv_intc_intid_t)62)
#define INTC_ID_USB_USBHI0              ((e_r_drv_intc_intid_t)63)
#define INTC_ID_USB_USBFI0              ((e_r_drv_intc_intid_t)64)
#define INTC_ID_USB_USBFDMA00           ((e_r_drv_intc_intid_t)65)
#define INTC_ID_USB_USBFDMA01           ((e_r_drv_intc_intid_t)66)
#define INTC_ID_USB_USBFDMAERR0         ((e_r_drv_intc_intid_t)67)
#define INTC_ID_USB_USBHI1              ((e_r_drv_intc_intid_t)68)
#define INTC_ID_USB_USBFI1              ((e_r_drv_intc_intid_t)69)
#define INTC_ID_USB_USBFDMA10           ((e_r_drv_intc_intid_t)70)
#define INTC_ID_USB_USBFDMA11           ((e_r_drv_intc_intid_t)71)
#define INTC_ID_USB_USBFDMAERR1         ((e_r_drv_intc_intid_t)72)
#define INTC_ID_VDC6_S0_VI_VSYNC0       ((e_r_drv_intc_intid_t)73)
#define INTC_ID_VDC6_S0_LO_VSYNC0       ((e_r_drv_intc_intid_t)74)
#define INTC_ID_VDC6_S0_VSYNCERR0       ((e_r_drv_intc_intid_t)75)
#define INTC_ID_VDC6_GR3_VLINE0         ((e_r_drv_intc_intid_t)76)
#define INTC_ID_VDC6_S0_VFIELD0         ((e_r_drv_intc_intid_t)77)
#define INTC_ID_VDC6_IV1_VBUFERR0       ((e_r_drv_intc_intid_t)78)
#define INTC_ID_VDC6_IV3_VBUFERR0       ((e_r_drv_intc_intid_t)79)
#define INTC_ID_VDC6_IV5_VBUFERR0       ((e_r_drv_intc_intid_t)80)
#define INTC_ID_VDC6_IV6_VBUFERR0       ((e_r_drv_intc_intid_t)81)
#define INTC_ID_VDC6_S0_WLINE0          ((e_r_drv_intc_intid_t)82)
#define INTC_ID_IMR_IMR2I0              ((e_r_drv_intc_intid_t)83)
#define INTC_ID_JPGC_JEDI               ((e_r_drv_intc_intid_t)84)
#define INTC_ID_JPGC_JDTI               ((e_r_drv_intc_intid_t)85)
#define INTC_ID_DAVE_DRWI               ((e_r_drv_intc_intid_t)86)
#define INTC_ID_MIPI_CSII               ((e_r_drv_intc_intid_t)87)
#define INTC_ID_OSTM_OSTMI0             ((e_r_drv_intc_intid_t)88)
#define INTC_ID_OSTM_OSTMI1             ((e_r_drv_intc_intid_t)89)
#define INTC_ID_OSTM_OSTMI2             ((e_r_drv_intc_intid_t)90)
#define INTC_ID_BSC_CMI                 ((e_r_drv_intc_intid_t)91)
#define INTC_ID_BSC_WTOUT               ((e_r_drv_intc_intid_t)92)
#define INTC_ID_CPG_ITI                 ((e_r_drv_intc_intid_t)93)
#define INTC_ID_CPG_CA9PEI              ((e_r_drv_intc_intid_t)94)
#define INTC_ID_MTU3_TGIA0              ((e_r_drv_intc_intid_t)95)
#define INTC_ID_MTU3_TGIB0              ((e_r_drv_intc_intid_t)96)
#define INTC_ID_MTU3_TGIC0              ((e_r_drv_intc_intid_t)97)
#define INTC_ID_MTU3_TGID0              ((e_r_drv_intc_intid_t)98)
#define INTC_ID_MTU3_TCIV0              ((e_r_drv_intc_intid_t)99)
#define INTC_ID_MTU3_TGIE0              ((e_r_drv_intc_intid_t)100)
#define INTC_ID_MTU3_TGIF0              ((e_r_drv_intc_intid_t)101)
#define INTC_ID_MTU3_TGIA1              ((e_r_drv_intc_intid_t)102)
#define INTC_ID_MTU3_TGIB1              ((e_r_drv_intc_intid_t)103)
#define INTC_ID_MTU3_TCIV1              ((e_r_drv_intc_intid_t)104)
#define INTC_ID_MTU3_TCIU1              ((e_r_drv_intc_intid_t)105)
#define INTC_ID_MTU3_TGIA2              ((e_r_drv_intc_intid_t)106)
#define INTC_ID_MTU3_TGIB2              ((e_r_drv_intc_intid_t)107)
#define INTC_ID_MTU3_TCIV2              ((e_r_drv_intc_intid_t)108)
#define INTC_ID_MTU3_TCIU2              ((e_r_drv_intc_intid_t)109)
#define INTC_ID_MTU3_TGIA3              ((e_r_drv_intc_intid_t)110)
#define INTC_ID_MTU3_TGIB3              ((e_r_drv_intc_intid_t)111)
#define INTC_ID_MTU3_TGIC3              ((e_r_drv_intc_intid_t)112)
#define INTC_ID_MTU3_TGID3              ((e_r_drv_intc_intid_t)113)
#define INTC_ID_MTU3_TCIV3              ((e_r_drv_intc_intid_t)114)
#define INTC_ID_MTU3_TGIA4              ((e_r_drv_intc_intid_t)115)
#define INTC_ID_MTU3_TGIB4              ((e_r_drv_intc_intid_t)116)
#define INTC_ID_MTU3_TGIC4              ((e_r_drv_intc_intid_t)117)
#define INTC_ID_MTU3_TGID4              ((e_r_drv_intc_intid_t)118)
#define INTC_ID_MTU3_TCIV4              ((e_r_drv_intc_intid_t)119)
#define INTC_ID_MTU3_TGIU5              ((e_r_drv_intc_intid_t)120)
#define INTC_ID_MTU3_TGIV5              ((e_r_drv_intc_intid_t)121)
#define INTC_ID_MTU3_TGIW5              ((e_r_drv_intc_intid_t)122)
#define INTC_ID_MTU3_TGIA6              ((e_r_drv_intc_intid_t)123)
#define INTC_ID_MTU3_TGIB6              ((e_r_drv_intc_intid_t)124)
#define INTC_ID_MTU3_TGIC6              ((e_r_drv_intc_intid_t)125)
#define INTC_ID_MTU3_TGID6              ((e_r_drv_intc_intid_t)126)
#define INTC_ID_MTU3_TCIV6              ((e_r_drv_intc_intid_t)127)
#define INTC_ID_MTU3_TGIA7              ((e_r_drv_intc_intid_t)128)
#define INTC_ID_MTU3_TGIB7              ((e_r_drv_intc_intid_t)129)
#define INTC_ID_MTU3_TGIC7              ((e_r_drv_intc_intid_t)130)
#define INTC_ID_MTU3_TGID7              ((e_r_drv_intc_intid_t)131)
#define INTC_ID_MTU3_TCIV7              ((e_r_drv_intc_intid_t)132)
#define INTC_ID_MTU3_TGIA8              ((e_r_drv_intc_intid_t)133)
#define INTC_ID_MTU3_TGIB8              ((e_r_drv_intc_intid_t)134)
#define INTC_ID_MTU3_TGIC8              ((e_r_drv_intc_intid_t)135)
#define INTC_ID_MTU3_TGID8              ((e_r_drv_intc_intid_t)136)
#define INTC_ID_MTU3_TCIV8              ((e_r_drv_intc_intid_t)137)
#define INTC_ID_INT138                  ((e_r_drv_intc_intid_t)138)
#define INTC_ID_GPT_CCMPA0              ((e_r_drv_intc_intid_t)139)
#define INTC_ID_GPT_CCMPB0              ((e_r_drv_intc_intid_t)140)
#define INTC_ID_GPT_CMPC0               ((e_r_drv_intc_intid_t)141)
#define INTC_ID_GPT_CMPD0               ((e_r_drv_intc_intid_t)142)
#define INTC_ID_INT143                  ((e_r_drv_intc_intid_t)143)
#define INTC_ID_INT144                  ((e_r_drv_intc_intid_t)144)
#define INTC_ID_INT145                  ((e_r_drv_intc_intid_t)145)
#define INTC_ID_GPT_CMPE0               ((e_r_drv_intc_intid_t)146)
#define INTC_ID_GPT_CMPF0               ((e_r_drv_intc_intid_t)147)
#define INTC_ID_GPT_ADTRGA0             ((e_r_drv_intc_intid_t)148)
#define INTC_ID_GPT_ADTRGB0             ((e_r_drv_intc_intid_t)149)
#define INTC_ID_GPT_OVF0                ((e_r_drv_intc_intid_t)150)
#define INTC_ID_GPT_UNF0                ((e_r_drv_intc_intid_t)151)
#define INTC_ID_GPT_CCMPA1              ((e_r_drv_intc_intid_t)152)
#define INTC_ID_GPT_CCMPB1              ((e_r_drv_intc_intid_t)153)
#define INTC_ID_GPT_CMPC1               ((e_r_drv_intc_intid_t)154)
#define INTC_ID_GPT_CMPD1               ((e_r_drv_intc_intid_t)155)
#define INTC_ID_INT156                  ((e_r_drv_intc_intid_t)156)
#define INTC_ID_INT157                  ((e_r_drv_intc_intid_t)157)
#define INTC_ID_INT158                  ((e_r_drv_intc_intid_t)158)
#define INTC_ID_GPT_CMPE1               ((e_r_drv_intc_intid_t)159)
#define INTC_ID_GPT_CMPF1               ((e_r_drv_intc_intid_t)160)
#define INTC_ID_GPT_ADTRGA1             ((e_r_drv_intc_intid_t)161)
#define INTC_ID_GPT_ADTRGB1             ((e_r_drv_intc_intid_t)162)
#define INTC_ID_GPT_OVF1                ((e_r_drv_intc_intid_t)163)
#define INTC_ID_GPT_UNF1                ((e_r_drv_intc_intid_t)164)
#define INTC_ID_GPT_CCMPA2              ((e_r_drv_intc_intid_t)165)
#define INTC_ID_GPT_CCMPB2              ((e_r_drv_intc_intid_t)166)
#define INTC_ID_GPT_CMPC2               ((e_r_drv_intc_intid_t)167)
#define INTC_ID_GPT_CMPD2               ((e_r_drv_intc_intid_t)168)
#define INTC_ID_INT169                  ((e_r_drv_intc_intid_t)169)
#define INTC_ID_INT170                  ((e_r_drv_intc_intid_t)170)
#define INTC_ID_INT171                  ((e_r_drv_intc_intid_t)171)
#define INTC_ID_GPT_CMPE2               ((e_r_drv_intc_intid_t)172)
#define INTC_ID_GPT_CMPF2               ((e_r_drv_intc_intid_t)173)
#define INTC_ID_GPT_ADTRGA2             ((e_r_drv_intc_intid_t)174)
#define INTC_ID_GPT_ADTRGB2             ((e_r_drv_intc_intid_t)175)
#define INTC_ID_GPT_OVF2                ((e_r_drv_intc_intid_t)176)
#define INTC_ID_GPT_UNF2                ((e_r_drv_intc_intid_t)177)
#define INTC_ID_GPT_CCMPA3              ((e_r_drv_intc_intid_t)178)
#define INTC_ID_GPT_CCMPB3              ((e_r_drv_intc_intid_t)179)
#define INTC_ID_GPT_CMPC3               ((e_r_drv_intc_intid_t)180)
#define INTC_ID_GPT_CMPD3               ((e_r_drv_intc_intid_t)181)
#define INTC_ID_INT182                  ((e_r_drv_intc_intid_t)182)
#define INTC_ID_INT183                  ((e_r_drv_intc_intid_t)183)
#define INTC_ID_INT184                  ((e_r_drv_intc_intid_t)184)
#define INTC_ID_GPT_CMPE3               ((e_r_drv_intc_intid_t)185)
#define INTC_ID_GPT_CMPF3               ((e_r_drv_intc_intid_t)186)
#define INTC_ID_GPT_ADTRGA3             ((e_r_drv_intc_intid_t)187)
#define INTC_ID_GPT_ADTRGB3             ((e_r_drv_intc_intid_t)188)
#define INTC_ID_GPT_OVF3                ((e_r_drv_intc_intid_t)189)
#define INTC_ID_GPT_UNF3                ((e_r_drv_intc_intid_t)190)
#define INTC_ID_GPT_CCMPA4              ((e_r_drv_intc_intid_t)191)
#define INTC_ID_GPT_CCMPB4              ((e_r_drv_intc_intid_t)192)
#define INTC_ID_GPT_CMPC4               ((e_r_drv_intc_intid_t)193)
#define INTC_ID_GPT_CMPD4               ((e_r_drv_intc_intid_t)194)
#define INTC_ID_INT195                  ((e_r_drv_intc_intid_t)195)
#define INTC_ID_INT196                  ((e_r_drv_intc_intid_t)196)
#define INTC_ID_INT197                  ((e_r_drv_intc_intid_t)197)
#define INTC_ID_GPT_CMPE4               ((e_r_drv_intc_intid_t)198)
#define INTC_ID_GPT_CMPF4               ((e_r_drv_intc_intid_t)199)
#define INTC_ID_GPT_ADTRGA4             ((e_r_drv_intc_intid_t)200)
#define INTC_ID_GPT_ADTRGB4             ((e_r_drv_intc_intid_t)201)
#define INTC_ID_GPT_OVF4                ((e_r_drv_intc_intid_t)202)
#define INTC_ID_GPT_UNF4                ((e_r_drv_intc_intid_t)203)
#define INTC_ID_GPT_CCMPA5              ((e_r_drv_intc_intid_t)204)
#define INTC_ID_GPT_CCMPB5              ((e_r_drv_intc_intid_t)205)
#define INTC_ID_GPT_CMPC5               ((e_r_drv_intc_intid_t)206)
#define INTC_ID_GPT_CMPD5               ((e_r_drv_intc_intid_t)207)
#define INTC_ID_INT208                  ((e_r_drv_intc_intid_t)208)
#define INTC_ID_INT209                  ((e_r_drv_intc_intid_t)209)
#define INTC_ID_INT210                  ((e_r_drv_intc_intid_t)210)
#define INTC_ID_GPT_CMPE5               ((e_r_drv_intc_intid_t)211)
#define INTC_ID_GPT_CMPF5               ((e_r_drv_intc_intid_t)212)
#define INTC_ID_GPT_ADTRGA5             ((e_r_drv_intc_intid_t)213)
#define INTC_ID_GPT_ADTRGB5             ((e_r_drv_intc_intid_t)214)
#define INTC_ID_GPT_OVF5                ((e_r_drv_intc_intid_t)215)
#define INTC_ID_GPT_UNF5                ((e_r_drv_intc_intid_t)216)
#define INTC_ID_GPT_CCMPA6              ((e_r_drv_intc_intid_t)217)
#define INTC_ID_GPT_CCMPB6              ((e_r_drv_intc_intid_t)218)
#define INTC_ID_GPT_CMPC6               ((e_r_drv_intc_intid_t)219)
#define INTC_ID_GPT_CMPD6               ((e_r_drv_intc_intid_t)220)
#define INTC_ID_INT221                  ((e_r_drv_intc_intid_t)221)
#define INTC_ID_INT222                  ((e_r_drv_intc_intid_t)222)
#define INTC_ID_INT223                  ((e_r_drv_intc_intid_t)223)
#define INTC_ID_GPT_CMPE6               ((e_r_drv_intc_intid_t)224)
#define INTC_ID_GPT_CMPF6               ((e_r_drv_intc_intid_t)225)
#define INTC_ID_GPT_ADTRGA6             ((e_r_drv_intc_intid_t)226)
#define INTC_ID_GPT_ADTRGB6             ((e_r_drv_intc_intid_t)227)
#define INTC_ID_GPT_OVF6                ((e_r_drv_intc_intid_t)228)
#define INTC_ID_GPT_UNF6                ((e_r_drv_intc_intid_t)229)
#define INTC_ID_GPT_CCMPA7              ((e_r_drv_intc_intid_t)230)
#define INTC_ID_GPT_CCMPB7              ((e_r_drv_intc_intid_t)231)
#define INTC_ID_GPT_CMPC7               ((e_r_drv_intc_intid_t)232)
#define INTC_ID_GPT_CMPD7               ((e_r_drv_intc_intid_t)233)
#define INTC_ID_INT234                  ((e_r_drv_intc_intid_t)234)
#define INTC_ID_INT235                  ((e_r_drv_intc_intid_t)235)
#define INTC_ID_INT236                  ((e_r_drv_intc_intid_t)236)
#define INTC_ID_GPT_CMPE7               ((e_r_drv_intc_intid_t)237)
#define INTC_ID_GPT_CMPF7               ((e_r_drv_intc_intid_t)238)
#define INTC_ID_GPT_ADTRGA7             ((e_r_drv_intc_intid_t)239)
#define INTC_ID_GPT_ADTRGB7             ((e_r_drv_intc_intid_t)240)
#define INTC_ID_GPT_OVF7                ((e_r_drv_intc_intid_t)241)
#define INTC_ID_GPT_UNF7                ((e_r_drv_intc_intid_t)242)
#define INTC_ID_POE_OEI1                ((e_r_drv_intc_intid_t)243)
#define INTC_ID_POE_OEI2                ((e_r_drv_intc_intid_t)244)
#define INTC_ID_POE_OEI3                ((e_r_drv_intc_intid_t)245)
#define INTC_ID_POE_OEI4                ((e_r_drv_intc_intid_t)246)
#define INTC_ID_ADC_S12ADI0             ((e_r_drv_intc_intid_t)247)
#define INTC_ID_ADC_S12GBADI0           ((e_r_drv_intc_intid_t)248)
#define INTC_ID_ADC_S12GCADI0           ((e_r_drv_intc_intid_t)249)
#define INTC_ID_ADC_S12ADCMPAI0         ((e_r_drv_intc_intid_t)250)
#define INTC_ID_ADC_S12ADCMPBI0         ((e_r_drv_intc_intid_t)251)
#define INTC_ID_SSIF_INT_SSIF_INT_REQ_0 ((e_r_drv_intc_intid_t)252)
#define INTC_ID_SSIF_INT_SSIF_DMA_TX_0  ((e_r_drv_intc_intid_t)253)
#define INTC_ID_SSIF_INT_SSIF_DMA_RX_0  ((e_r_drv_intc_intid_t)254)
#define INTC_ID_SSIF_INT_SSIF_INT_REQ_1 ((e_r_drv_intc_intid_t)255)
#define INTC_ID_SSIF_INT_SSIF_DMA_TX_1  ((e_r_drv_intc_intid_t)256)
#define INTC_ID_SSIF_INT_SSIF_DMA_RX_1  ((e_r_drv_intc_intid_t)257)
#define INTC_ID_SSIF_INT_SSIF_INT_REQ_2 ((e_r_drv_intc_intid_t)258)
#define INTC_ID_SSIF_INT_SSIF_DMA_RT_2  ((e_r_drv_intc_intid_t)259)
#define INTC_ID_SSIF_INT_SSIF_INT_REQ_3 ((e_r_drv_intc_intid_t)260)
#define INTC_ID_SSIF_INT_SSIF_DMA_TX_3  ((e_r_drv_intc_intid_t)261)
#define INTC_ID_SSIF_INT_SSIF_DMA_RX_3  ((e_r_drv_intc_intid_t)262)
#define INTC_ID_SPDIF_SPDIFI            ((e_r_drv_intc_intid_t)263)
#define INTC_ID_IIC_INTRIICTEI0         ((e_r_drv_intc_intid_t)264)
#define INTC_ID_IIC_INTRIICRI0          ((e_r_drv_intc_intid_t)265)
#define INTC_ID_IIC_INTRIICTI0          ((e_r_drv_intc_intid_t)266)
#define INTC_ID_IIC_INTRIICSPI0         ((e_r_drv_intc_intid_t)267)
#define INTC_ID_IIC_INTRIICSTI0         ((e_r_drv_intc_intid_t)268)
#define INTC_ID_IIC_INTRIICNAKI0        ((e_r_drv_intc_intid_t)269)
#define INTC_ID_IIC_INTRIICALI0         ((e_r_drv_intc_intid_t)270)
#define INTC_ID_IIC_INTRIICTMOI0        ((e_r_drv_intc_intid_t)271)
#define INTC_ID_IIC_INTRIICTEI1         ((e_r_drv_intc_intid_t)272)
#define INTC_ID_IIC_INTRIICRI1          ((e_r_drv_intc_intid_t)273)
#define INTC_ID_IIC_INTRIICTI1          ((e_r_drv_intc_intid_t)274)
#define INTC_ID_IIC_INTRIICSPI1         ((e_r_drv_intc_intid_t)275)
#define INTC_ID_IIC_INTRIICSTI1         ((e_r_drv_intc_intid_t)276)
#define INTC_ID_IIC_INTRIICNAKI1        ((e_r_drv_intc_intid_t)277)
#define INTC_ID_IIC_INTRIICALI1         ((e_r_drv_intc_intid_t)278)
#define INTC_ID_IIC_INTRIICTMOI1        ((e_r_drv_intc_intid_t)279)
#define INTC_ID_IIC_INTRIICTEI2         ((e_r_drv_intc_intid_t)280)
#define INTC_ID_IIC_INTRIICRI2          ((e_r_drv_intc_intid_t)281)
#define INTC_ID_IIC_INTRIICTI2          ((e_r_drv_intc_intid_t)282)
#define INTC_ID_IIC_INTRIICSPI2         ((e_r_drv_intc_intid_t)283)
#define INTC_ID_IIC_INTRIICSTI2         ((e_r_drv_intc_intid_t)284)
#define INTC_ID_IIC_INTRIICNAKI2        ((e_r_drv_intc_intid_t)285)
#define INTC_ID_IIC_INTRIICALI2         ((e_r_drv_intc_intid_t)286)
#define INTC_ID_IIC_INTRIICTMOI2        ((e_r_drv_intc_intid_t)287)
#define INTC_ID_IIC_INTRIICTEI3         ((e_r_drv_intc_intid_t)288)
#define INTC_ID_IIC_INTRIICRI3          ((e_r_drv_intc_intid_t)289)
#define INTC_ID_IIC_INTRIICTI3          ((e_r_drv_intc_intid_t)290)
#define INTC_ID_IIC_INTRIICSPI3         ((e_r_drv_intc_intid_t)291)
#define INTC_ID_IIC_INTRIICSTI3         ((e_r_drv_intc_intid_t)292)
#define INTC_ID_IIC_INTRIICNAKI3        ((e_r_drv_intc_intid_t)293)
#define INTC_ID_IIC_INTRIICALI3         ((e_r_drv_intc_intid_t)294)
#define INTC_ID_IIC_INTRIICTMOI3        ((e_r_drv_intc_intid_t)295)
#define INTC_ID_INT296                  ((e_r_drv_intc_intid_t)296)
#define INTC_ID_SCIFA_ERI0BRI0          ((e_r_drv_intc_intid_t)297)
#define INTC_ID_SCIFA_RXI0              ((e_r_drv_intc_intid_t)298)
#define INTC_ID_SCIFA_TXI0              ((e_r_drv_intc_intid_t)299)
#define INTC_ID_SCIFA_TEI0DRI0          ((e_r_drv_intc_intid_t)300)
#define INTC_ID_INT301                  ((e_r_drv_intc_intid_t)301)
#define INTC_ID_INT302                  ((e_r_drv_intc_intid_t)302)
#define INTC_ID_SCIFA_ERI1BRI1          ((e_r_drv_intc_intid_t)303)
#define INTC_ID_SCIFA_RXI1              ((e_r_drv_intc_intid_t)304)
#define INTC_ID_SCIFA_TXI1              ((e_r_drv_intc_intid_t)305)
#define INTC_ID_SCIFA_TEI1DRI1          ((e_r_drv_intc_intid_t)306)
#define INTC_ID_INT307                  ((e_r_drv_intc_intid_t)307)
#define INTC_ID_INT308                  ((e_r_drv_intc_intid_t)308)
#define INTC_ID_SCIFA_ERI2BRI2          ((e_r_drv_intc_intid_t)309)
#define INTC_ID_SCIFA_RXI2              ((e_r_drv_intc_intid_t)310)
#define INTC_ID_SCIFA_TXI2              ((e_r_drv_intc_intid_t)311)
#define INTC_ID_SCIFA_TEI2DRI2          ((e_r_drv_intc_intid_t)312)
#define INTC_ID_INT313                  ((e_r_drv_intc_intid_t)313)
#define INTC_ID_INT314                  ((e_r_drv_intc_intid_t)314)
#define INTC_ID_SCIFA_ERI3BRI3          ((e_r_drv_intc_intid_t)315)
#define INTC_ID_SCIFA_RXI3              ((e_r_drv_intc_intid_t)316)
#define INTC_ID_SCIFA_TXI3              ((e_r_drv_intc_intid_t)317)
#define INTC_ID_SCIFA_TEI3DRI3          ((e_r_drv_intc_intid_t)318)
#define INTC_ID_INT319                  ((e_r_drv_intc_intid_t)319)
#define INTC_ID_INT320                  ((e_r_drv_intc_intid_t)320)
#define INTC_ID_SCIFA_ERI4BRI4          ((e_r_drv_intc_intid_t)321)
#define INTC_ID_SCIFA_RXI4              ((e_r_drv_intc_intid_t)322)
#define INTC_ID_SCIFA_TXI4              ((e_r_drv_intc_intid_t)323)
#define INTC_ID_SCIFA_TEI4DRI4          ((e_r_drv_intc_intid_t)324)
#define INTC_ID_INT325                  ((e_r_drv_intc_intid_t)325)
#define INTC_ID_CAN_GERI                ((e_r_drv_intc_intid_t)326)
#define INTC_ID_CAN_RFI                 ((e_r_drv_intc_intid_t)327)
#define INTC_ID_CAN_CFRXI0              ((e_r_drv_intc_intid_t)328)
#define INTC_ID_CAN_CERI0               ((e_r_drv_intc_intid_t)329)
#define INTC_ID_CAN_CTXI0               ((e_r_drv_intc_intid_t)330)
#define INTC_ID_CAN_CFRXI1              ((e_r_drv_intc_intid_t)331)
#define INTC_ID_CAN_CERI1               ((e_r_drv_intc_intid_t)332)
#define INTC_ID_CAN_CTXI1               ((e_r_drv_intc_intid_t)333)
#define INTC_ID_DMAC31_DMAINT0          ((e_r_drv_intc_intid_t)334)
#define INTC_ID_DMAC31_DMAINT1          ((e_r_drv_intc_intid_t)335)
#define INTC_ID_DMAC31_DMAINT2          ((e_r_drv_intc_intid_t)336)
#define INTC_ID_DMAC31_DMAINT3          ((e_r_drv_intc_intid_t)337)
#define INTC_ID_DMAC31_DMAINT4          ((e_r_drv_intc_intid_t)338)
#define INTC_ID_DMAC31_DMAINT5          ((e_r_drv_intc_intid_t)339)
#define INTC_ID_DMAC31_DMAINT6          ((e_r_drv_intc_intid_t)340)
#define INTC_ID_DMAC31_DMAINT7          ((e_r_drv_intc_intid_t)341)
#define INTC_ID_DMAC31_DMAINT8          ((e_r_drv_intc_intid_t)342)
#define INTC_ID_DMAC31_DMAINT9          ((e_r_drv_intc_intid_t)343)
#define INTC_ID_RSPI_SPEI0              ((e_r_drv_intc_intid_t)344)
#define INTC_ID_RSPI_SPRI0              ((e_r_drv_intc_intid_t)345)
#define INTC_ID_RSPI_SPTI0              ((e_r_drv_intc_intid_t)346)
#define INTC_ID_RSPI_SPEI1              ((e_r_drv_intc_intid_t)347)
#define INTC_ID_RSPI_SPRI1              ((e_r_drv_intc_intid_t)348)
#define INTC_ID_RSPI_SPTI1              ((e_r_drv_intc_intid_t)349)
#define INTC_ID_RSPI_SPEI2              ((e_r_drv_intc_intid_t)350)
#define INTC_ID_RSPI_SPRI2              ((e_r_drv_intc_intid_t)351)
#define INTC_ID_RSPI_SPTI2              ((e_r_drv_intc_intid_t)352)
#define INTC_ID_NDF_NAND                ((e_r_drv_intc_intid_t)353)
#define INTC_ID_SDMMC_SDHI0_0           ((e_r_drv_intc_intid_t)354)
#define INTC_ID_INT355                  ((e_r_drv_intc_intid_t)355)
#define INTC_ID_SDMMC_SDHI1_0           ((e_r_drv_intc_intid_t)356)
#define INTC_ID_INT357                  ((e_r_drv_intc_intid_t)357)
#define INTC_ID_HYPER_HYPER             ((e_r_drv_intc_intid_t)358)
#define INTC_ID_RTC_ARM                 ((e_r_drv_intc_intid_t)359)
#define INTC_ID_RTC_PRD                 ((e_r_drv_intc_intid_t)360)
#define INTC_ID_RTC_CUP                 ((e_r_drv_intc_intid_t)361)
#define INTC_ID_RTC_ARM_S               ((e_r_drv_intc_intid_t)362)
#define INTC_ID_RTC_PRD_S               ((e_r_drv_intc_intid_t)363)
#define INTC_ID_RTC_CUP_S               ((e_r_drv_intc_intid_t)364)
#define INTC_ID_SCIM_ERI0               ((e_r_drv_intc_intid_t)365)
#define INTC_ID_SCIM_RXI0               ((e_r_drv_intc_intid_t)366)
#define INTC_ID_SCIM_TXI0               ((e_r_drv_intc_intid_t)367)
#define INTC_ID_SCIM_TEI0               ((e_r_drv_intc_intid_t)368)
#define INTC_ID_SCIM_ERI1               ((e_r_drv_intc_intid_t)369)
#define INTC_ID_SCIM_RXI1               ((e_r_drv_intc_intid_t)370)
#define INTC_ID_SCIM_TXI1               ((e_r_drv_intc_intid_t)371)
#define INTC_ID_SCIM_TEI1               ((e_r_drv_intc_intid_t)372)
#define INTC_ID_ETHER_EINT0             ((e_r_drv_intc_intid_t)373)
#define INTC_ID_ETHER_EINT1             ((e_r_drv_intc_intid_t)374)
#define INTC_ID_ETHER_PINT              ((e_r_drv_intc_intid_t)375)
#define INTC_ID_ETHER_MINT              ((e_r_drv_intc_intid_t)376)
#define INTC_ID_ETHER_IPLS              ((e_r_drv_intc_intid_t)377)
#define INTC_ID_CEU_CEUI                ((e_r_drv_intc_intid_t)378)
#define INTC_ID_INB_H2XUSB00_ERRINT     ((e_r_drv_intc_intid_t)379)
#define INTC_ID_INB_H2XUSB01_ERRINT     ((e_r_drv_intc_intid_t)380)
#define INTC_ID_INB_H2XUSB10_ERRINT     ((e_r_drv_intc_intid_t)381)
#define INTC_ID_INB_H2XUSB11_ERRINT     ((e_r_drv_intc_intid_t)382)
#define INTC_ID_INB_H2XETH_ERRINT       ((e_r_drv_intc_intid_t)383)
#define INTC_ID_INB_X2HPERI12_ERRINT    ((e_r_drv_intc_intid_t)384)
#define INTC_ID_INB_X2HPERI34_ERRINT    ((e_r_drv_intc_intid_t)385)
#define INTC_ID_INB_X2HPERI5_ERRINT     ((e_r_drv_intc_intid_t)386)
#define INTC_ID_INB_X2HPERI67_ERRINT    ((e_r_drv_intc_intid_t)387)
#define INTC_ID_INB_H2XDBG_ERRINT       ((e_r_drv_intc_intid_t)388)
#define INTC_ID_INB_X2HDBGR_ERRINT      ((e_r_drv_intc_intid_t)389)
#define INTC_ID_DMAC31_DMAINT10         ((e_r_drv_intc_intid_t)390)
#define INTC_ID_DMAC31_DMAINT11         ((e_r_drv_intc_intid_t)391)
#define INTC_ID_DMAC31_DMAINT12         ((e_r_drv_intc_intid_t)392)
#define INTC_ID_DMAC31_DMAINT13         ((e_r_drv_intc_intid_t)393)
#define INTC_ID_DMAC31_DMAINT14         ((e_r_drv_intc_intid_t)394)
#define INTC_ID_DMAC31_DMAINT15         ((e_r_drv_intc_intid_t)395)
#define INTC_ID_INB_H2XDAV0_ERRINT      ((e_r_drv_intc_intid_t)396)
#define INTC_ID_INB_H2XDAV1_ERRINT      ((e_r_drv_intc_intid_t)397)
#define INTC_ID_INT398                  ((e_r_drv_intc_intid_t)398)
#define INTC_ID_INT399                  ((e_r_drv_intc_intid_t)399)
#define INTC_ID_INT400                  ((e_r_drv_intc_intid_t)400)
#define INTC_ID_INT401                  ((e_r_drv_intc_intid_t)401)
#define INTC_ID_INT402                  ((e_r_drv_intc_intid_t)402)
#define INTC_ID_INT403                  ((e_r_drv_intc_intid_t)403)
#define INTC_ID_INT404                  ((e_r_drv_intc_intid_t)404)
#define INTC_ID_INT405                  ((e_r_drv_intc_intid_t)405)
#define INTC_ID_INT406                  ((e_r_drv_intc_intid_t)406)
#define INTC_ID_INT407                  ((e_r_drv_intc_intid_t)407)
#define INTC_ID_INT408                  ((e_r_drv_intc_intid_t)408)
#define INTC_ID_INT409                  ((e_r_drv_intc_intid_t)409)
#define INTC_ID_INT410                  ((e_r_drv_intc_intid_t)410)
#define INTC_ID_INT411                  ((e_r_drv_intc_intid_t)411)
#define INTC_ID_INT412                  ((e_r_drv_intc_intid_t)412)
#define INTC_ID_INT413                  ((e_r_drv_intc_intid_t)413)
#define INTC_ID_INT414                  ((e_r_drv_intc_intid_t)414)
#define INTC_ID_INT415                  ((e_r_drv_intc_intid_t)415)
#define INTC_ID_INT416                  ((e_r_drv_intc_intid_t)416)
#define INTC_ID_INT417                  ((e_r_drv_intc_intid_t)417)
#define INTC_ID_INT418                  ((e_r_drv_intc_intid_t)418)
#define INTC_ID_INT419                  ((e_r_drv_intc_intid_t)419)
#define INTC_ID_INT420                  ((e_r_drv_intc_intid_t)420)
#define INTC_ID_INT421                  ((e_r_drv_intc_intid_t)421)
#define INTC_ID_INT422                  ((e_r_drv_intc_intid_t)422)
#define INTC_ID_INT423                  ((e_r_drv_intc_intid_t)423)
#define INTC_ID_INT424                  ((e_r_drv_intc_intid_t)424)
#define INTC_ID_INT425                  ((e_r_drv_intc_intid_t)425)
#define INTC_ID_INT426                  ((e_r_drv_intc_intid_t)426)
#define INTC_ID_INT427                  ((e_r_drv_intc_intid_t)427)
#define INTC_ID_INT428                  ((e_r_drv_intc_intid_t)428)
#define INTC_ID_INT429                  ((e_r_drv_intc_intid_t)429)
#define INTC_ID_INT430                  ((e_r_drv_intc_intid_t)430)
#define INTC_ID_INT431                  ((e_r_drv_intc_intid_t)431)
#define INTC_ID_INT432                  ((e_r_drv_intc_intid_t)432)
#define INTC_ID_INT433                  ((e_r_drv_intc_intid_t)433)
#define INTC_ID_INT434                  ((e_r_drv_intc_intid_t)434)
#define INTC_ID_TSIP_ROMOK              ((e_r_drv_intc_intid_t)435)
#define INTC_ID_TSIP_LONG_PLG           ((e_r_drv_intc_intid_t)436)
#define INTC_ID_TSIP_PROC_BUSY          ((e_r_drv_intc_intid_t)437)
#define INTC_ID_TSIP_RDRDY1             ((e_r_drv_intc_intid_t)438)
#define INTC_ID_TSIP_RDRDY0             ((e_r_drv_intc_intid_t)439)
#define INTC_ID_TSIP_WRRDY4             ((e_r_drv_intc_intid_t)440)
#define INTC_ID_INT441                  ((e_r_drv_intc_intid_t)441)
#define INTC_ID_TSIP_WRRDY1             ((e_r_drv_intc_intid_t)442)
#define INTC_ID_TSIP_WRRDY0             ((e_r_drv_intc_intid_t)443)
#define INTC_ID_TSIP_IRDRDY             ((e_r_drv_intc_intid_t)444)
#define INTC_ID_TSIP_IWRRDY             ((e_r_drv_intc_intid_t)445)
#define INTC_ID_DRP_ERRINT              ((e_r_drv_intc_intid_t)446)
#define INTC_ID_DRP_NMLINT              ((e_r_drv_intc_intid_t)447)
#define INTC_ID_DRP_PAE5                ((e_r_drv_intc_intid_t)448)
#define INTC_ID_DRP_PAF5                ((e_r_drv_intc_intid_t)449)
#define INTC_ID_DRP_INTB5               ((e_r_drv_intc_intid_t)450)
#define INTC_ID_DRP_INTA5               ((e_r_drv_intc_intid_t)451)
#define INTC_ID_DRP_PAE4                ((e_r_drv_intc_intid_t)452)
#define INTC_ID_DRP_PAF4                ((e_r_drv_intc_intid_t)453)
#define INTC_ID_DRP_INTB4               ((e_r_drv_intc_intid_t)454)
#define INTC_ID_DRP_INTA4               ((e_r_drv_intc_intid_t)455)
#define INTC_ID_DRP_PAE3                ((e_r_drv_intc_intid_t)456)
#define INTC_ID_DRP_PAF3                ((e_r_drv_intc_intid_t)457)
#define INTC_ID_DRP_INTB3               ((e_r_drv_intc_intid_t)458)
#define INTC_ID_DRP_INTA3               ((e_r_drv_intc_intid_t)459)
#define INTC_ID_DRP_PAE2                ((e_r_drv_intc_intid_t)460)
#define INTC_ID_DRP_PAF2                ((e_r_drv_intc_intid_t)461)
#define INTC_ID_DRP_INTB2               ((e_r_drv_intc_intid_t)462)
#define INTC_ID_DRP_INTA2               ((e_r_drv_intc_intid_t)463)
#define INTC_ID_DRP_PAE1                ((e_r_drv_intc_intid_t)464)
#define INTC_ID_DRP_PAF1                ((e_r_drv_intc_intid_t)465)
#define INTC_ID_DRP_INTB1               ((e_r_drv_intc_intid_t)466)
#define INTC_ID_DRP_INTA1               ((e_r_drv_intc_intid_t)467)
#define INTC_ID_DRP_PAE0                ((e_r_drv_intc_intid_t)468)
#define INTC_ID_DRP_PAF0                ((e_r_drv_intc_intid_t)469)
#define INTC_ID_DRP_INTB0               ((e_r_drv_intc_intid_t)470)
#define INTC_ID_DRP_INTA0               ((e_r_drv_intc_intid_t)471)
#define INTC_ID_VIN_VINI                ((e_r_drv_intc_intid_t)472)
#define INTC_ID_POEG_GROUP0             ((e_r_drv_intc_intid_t)473)
#define INTC_ID_POEG_GROUP1             ((e_r_drv_intc_intid_t)474)
#define INTC_ID_POEG_GROUP2             ((e_r_drv_intc_intid_t)475)
#define INTC_ID_POEG_GROUP3             ((e_r_drv_intc_intid_t)476)
#define INTC_ID_SPIBSC_SPIHF            ((e_r_drv_intc_intid_t)477)
#define INTC_ID_INT478                  ((e_r_drv_intc_intid_t)478)
#define INTC_ID_INT479                  ((e_r_drv_intc_intid_t)479)
#define INTC_ID_TINT_TINT0              ((e_r_drv_intc_intid_t)480)
#define INTC_ID_TINT_TINT1              ((e_r_drv_intc_intid_t)481)
#define INTC_ID_TINT_TINT2              ((e_r_drv_intc_intid_t)482)
#define INTC_ID_TINT_TINT3              ((e_r_drv_intc_intid_t)483)
#define INTC_ID_TINT_TINT4              ((e_r_drv_intc_intid_t)484)
#define INTC_ID_TINT_TINT5              ((e_r_drv_intc_intid_t)485)
#define INTC_ID_TINT_TINT6              ((e_r_drv_intc_intid_t)486)
#define INTC_ID_TINT_TINT7              ((e_r_drv_intc_intid_t)487)
#define INTC_ID_TINT_TINT8              ((e_r_drv_intc_intid_t)488)
#define INTC_ID_TINT_TINT9              ((e_r_drv_intc_intid_t)489)
#define INTC_ID_TINT_TINT10             ((e_r_drv_intc_intid_t)490)
#define INTC_ID_TINT_TINT11             ((e_r_drv_intc_intid_t)491)
#define INTC_ID_TINT_TINT12             ((e_r_drv_intc_intid_t)492)
#define INTC_ID_TINT_TINT13             ((e_r_drv_intc_intid_t)493)
#define INTC_ID_TINT_TINT14             ((e_r_drv_intc_intid_t)494)
#define INTC_ID_TINT_TINT15             ((e_r_drv_intc_intid_t)495)
#define INTC_ID_TINT_TINT16             ((e_r_drv_intc_intid_t)496)
#define INTC_ID_TINT_TINT17             ((e_r_drv_intc_intid_t)497)
#define INTC_ID_TINT_TINT18             ((e_r_drv_intc_intid_t)498)
#define INTC_ID_TINT_TINT19             ((e_r_drv_intc_intid_t)499)
#define INTC_ID_TINT_TINT20             ((e_r_drv_intc_intid_t)500)
#define INTC_ID_TINT_TINT21             ((e_r_drv_intc_intid_t)501)
#define INTC_ID_TINT_TINT22             ((e_r_drv_intc_intid_t)502)
#define INTC_ID_TINT_TINT23             ((e_r_drv_intc_intid_t)503)
#define INTC_ID_TINT_TINT24             ((e_r_drv_intc_intid_t)504)
#define INTC_ID_TINT_TINT25             ((e_r_drv_intc_intid_t)505)
#define INTC_ID_TINT_TINT26             ((e_r_drv_intc_intid_t)506)
#define INTC_ID_TINT_TINT27             ((e_r_drv_intc_intid_t)507)
#define INTC_ID_TINT_TINT28             ((e_r_drv_intc_intid_t)508)
#define INTC_ID_TINT_TINT29             ((e_r_drv_intc_intid_t)509)
#define INTC_ID_TINT_TINT30             ((e_r_drv_intc_intid_t)510)
#define INTC_ID_TINT_TINT31             ((e_r_drv_intc_intid_t)511)

/* ==== Virtual Interrupt ID ==== */
#define INTC_VID_TOTAL                  (513)                         /* The number of interrupt sources */
#define INTC_VID_MIN                    (512)                         /* The minimum number of virtual interrupt */
#define INTC_VID_NMI                    ((e_r_drv_intc_intid_t)512)

/* ==== Interrupt Priority ==== */
#define INTC_PRIORITY_HIGHEST  ((e_r_drv_intc_priority_t)0)
#define INTC_PRIORITY_LOWEST   ((e_r_drv_intc_priority_t)31)
#define INTC_PRIORITY_0        ((e_r_drv_intc_priority_t)0)     /*!<  Interrupt priority level 0 (Max) */
#define INTC_PRIORITY_1        ((e_r_drv_intc_priority_t)1)     /*!<  Interrupt priority level 1       */
#define INTC_PRIORITY_2        ((e_r_drv_intc_priority_t)2)     /*!<  Interrupt priority level 2       */
#define INTC_PRIORITY_3        ((e_r_drv_intc_priority_t)3)     /*!<  Interrupt priority level 3       */
#define INTC_PRIORITY_4        ((e_r_drv_intc_priority_t)4)     /*!<  Interrupt priority level 4       */
#define INTC_PRIORITY_5        ((e_r_drv_intc_priority_t)5)     /*!<  Interrupt priority level 5       */
#define INTC_PRIORITY_6        ((e_r_drv_intc_priority_t)6)     /*!<  Interrupt priority level 6       */
#define INTC_PRIORITY_7        ((e_r_drv_intc_priority_t)7)     /*!<  Interrupt priority level 7       */
#define INTC_PRIORITY_8        ((e_r_drv_intc_priority_t)8)     /*!<  Interrupt priority level 8       */
#define INTC_PRIORITY_9        ((e_r_drv_intc_priority_t)9)     /*!<  Interrupt priority level 9       */
#define INTC_PRIORITY_10       ((e_r_drv_intc_priority_t)10)    /*!<  Interrupt priority level 10      */
#define INTC_PRIORITY_11       ((e_r_drv_intc_priority_t)11)    /*!<  Interrupt priority level 11      */
#define INTC_PRIORITY_12       ((e_r_drv_intc_priority_t)12)    /*!<  Interrupt priority level 12      */
#define INTC_PRIORITY_13       ((e_r_drv_intc_priority_t)13)    /*!<  Interrupt priority level 13      */
#define INTC_PRIORITY_14       ((e_r_drv_intc_priority_t)14)    /*!<  Interrupt priority level 14      */
#define INTC_PRIORITY_15       ((e_r_drv_intc_priority_t)15)    /*!<  Interrupt priority level 15      */
#define INTC_PRIORITY_16       ((e_r_drv_intc_priority_t)16)    /*!<  Interrupt priority level 16      */
#define INTC_PRIORITY_17       ((e_r_drv_intc_priority_t)17)    /*!<  Interrupt priority level 17      */
#define INTC_PRIORITY_18       ((e_r_drv_intc_priority_t)18)    /*!<  Interrupt priority level 18      */
#define INTC_PRIORITY_19       ((e_r_drv_intc_priority_t)19)    /*!<  Interrupt priority level 19      */
#define INTC_PRIORITY_20       ((e_r_drv_intc_priority_t)20)    /*!<  Interrupt priority level 20      */
#define INTC_PRIORITY_21       ((e_r_drv_intc_priority_t)21)    /*!<  Interrupt priority level 21      */
#define INTC_PRIORITY_22       ((e_r_drv_intc_priority_t)22)    /*!<  Interrupt priority level 22      */
#define INTC_PRIORITY_23       ((e_r_drv_intc_priority_t)23)    /*!<  Interrupt priority level 23      */
#define INTC_PRIORITY_24       ((e_r_drv_intc_priority_t)24)    /*!<  Interrupt priority level 24      */
#define INTC_PRIORITY_25       ((e_r_drv_intc_priority_t)25)    /*!<  Interrupt priority level 25      */
#define INTC_PRIORITY_26       ((e_r_drv_intc_priority_t)26)    /*!<  Interrupt priority level 26      */
#define INTC_PRIORITY_27       ((e_r_drv_intc_priority_t)27)    /*!<  Interrupt priority level 27      */
#define INTC_PRIORITY_28       ((e_r_drv_intc_priority_t)28)    /*!<  Interrupt priority level 28      */
#define INTC_PRIORITY_29       ((e_r_drv_intc_priority_t)29)    /*!<  Interrupt priority level 29      */
#define INTC_PRIORITY_30       ((e_r_drv_intc_priority_t)30)    /*!<  Interrupt priority level 30      */
#define INTC_PRIORITY_31       ((e_r_drv_intc_priority_t)31)    /*!<  Interrupt priority level 31 (Mix)*/

/* IRQ */
#define INTC_IRQ0               ((e_r_drv_irq_num_t)0)    /*!< Select IRQ0 pin */
#define INTC_IRQ1               ((e_r_drv_irq_num_t)1)    /*!< Select IRQ1 pin */
#define INTC_IRQ2               ((e_r_drv_irq_num_t)2)    /*!< Select IRQ2 pin */
#define INTC_IRQ3               ((e_r_drv_irq_num_t)3)    /*!< Select IRQ3 pin */
#define INTC_IRQ4               ((e_r_drv_irq_num_t)4)    /*!< Select IRQ4 pin */
#define INTC_IRQ5               ((e_r_drv_irq_num_t)5)    /*!< Select IRQ5 pin */
#define INTC_IRQ6               ((e_r_drv_irq_num_t)6)    /*!< Select IRQ6 pin */
#define INTC_IRQ7               ((e_r_drv_irq_num_t)7)    /*!< Select IRQ7 pin */


/* TINT */
#define INTC_TINT0              ((e_r_drv_tint_num_t)0)    /*!<  Group0  (TINT0)  : ID:480 : PL_0,PL_1,PL_2,PL_3,PL4,PM_0 */
#define INTC_TINT1              ((e_r_drv_tint_num_t)1)    /*!<  Group1  (TINT1)  : ID:481 : PK_0,PK_1,PK_2,PK_3,PK_4,PK_5 */
#define INTC_TINT2              ((e_r_drv_tint_num_t)2)    /*!<  Group2  (TINT2)  : ID:482 : PJ_4,PJ_5,PJ_6,PJ_7 */
#define INTC_TINT3              ((e_r_drv_tint_num_t)3)    /*!<  Group3  (TINT3)  : ID:483 : PJ_0,PJ_1,PJ_2,PJ_3 */
#define INTC_TINT4              ((e_r_drv_tint_num_t)4)    /*!<  Group4  (TINT4)  : ID:484 : PH_0,PH_1,PH_2,PH_3,PH_4,PH_5,PH_6 */
#define INTC_TINT5              ((e_r_drv_tint_num_t)5)    /*!<  Group5  (TINT5)  : ID:485 : PG_4,PG_5,PG_6,PG_7 */
#define INTC_TINT6              ((e_r_drv_tint_num_t)6)    /*!<  Group6  (TINT6)  : ID:486 : PG_0,PG_1,PG_2,PG_3 */
#define INTC_TINT7              ((e_r_drv_tint_num_t)7)    /*!<  Group7  (TINT7)  : ID:487 : PF_4,PF_5,PF_6,PF_7 */
#define INTC_TINT8              ((e_r_drv_tint_num_t)8)    /*!<  Group8  (TINT8)  : ID:488 : PF_0,PF_1,PF_2,PF_3 */
#define INTC_TINT9              ((e_r_drv_tint_num_t)9)    /*!<  Group9  (TINT9)  : ID:489 : PE_0,PE_1,PE_2,PE_3,PE_4,PE_5,PE_6 */
#define INTC_TINT10             ((e_r_drv_tint_num_t)10)   /*!<  Group10 (TINT10) : ID:490 : PD_4,PD_5,PD_6,PD_7 */
#define INTC_TINT11             ((e_r_drv_tint_num_t)11)   /*!<  Group11 (TINT11) : ID:491 : PD_0,PD_1,PD_2,PD_3 */
#define INTC_TINT12             ((e_r_drv_tint_num_t)12)   /*!<  Group12 (TINT12) : ID:492 : PC_4,PC_5,PC_6,PC_7 */
#define INTC_TINT13             ((e_r_drv_tint_num_t)13)   /*!<  Group13 (TINT13) : ID:493 : PC_0,PC_1,PC_2,PC_3 */
#define INTC_TINT14             ((e_r_drv_tint_num_t)14)   /*!<  Group14 (TINT14) : ID:494 : PB_0,PB_1,PB_2,PB_3,PB_4,PB_5 */
#define INTC_TINT15             ((e_r_drv_tint_num_t)15)   /*!<  Group15 (TINT15) : ID:495 : PA_4,PA_5,PA_6,PA_7 */
#define INTC_TINT16             ((e_r_drv_tint_num_t)16)   /*!<  Group16 (TINT16) : ID:496 : PA_0,PA_1,PA_2,PA_3 */
#define INTC_TINT17             ((e_r_drv_tint_num_t)17)   /*!<  Group17 (TINT17) : ID:497 : P9_4,P9_5,P9_6,P9_7 */
#define INTC_TINT18             ((e_r_drv_tint_num_t)18)   /*!<  Group18 (TINT18) : ID:498 : P9_0,P9_1,P9_2,P9_3 */
#define INTC_TINT19             ((e_r_drv_tint_num_t)19)   /*!<  Group19 (TINT19) : ID:499 : P8_4,P8_5,P8_6,P8_7 */
#define INTC_TINT20             ((e_r_drv_tint_num_t)20)   /*!<  Group20 (TINT20) : ID:500 : P8_0,P8_1,P8_2,P8_3 */
#define INTC_TINT21             ((e_r_drv_tint_num_t)21)   /*!<  Group21 (TINT21) : ID:501 : P7_4,P7_5,P7_6,P7_7 */
#define INTC_TINT22             ((e_r_drv_tint_num_t)22)   /*!<  Group22 (TINT22) : ID:502 : P7_0,P7_1,P7_2,P7_3 */
#define INTC_TINT23             ((e_r_drv_tint_num_t)23)   /*!<  Group23 (TINT23) : ID:503 : P6_4,P6_5,P6_6,P6_7 */
#define INTC_TINT24             ((e_r_drv_tint_num_t)24)   /*!<  Group24 (TINT24) : ID:504 : P6_0,P6_1,P6_2,P6_3 */
#define INTC_TINT25             ((e_r_drv_tint_num_t)25)   /*!<  Group25 (TINT25) : ID:505 : P5_4,P5_5,P5_6,P5_7 */
#define INTC_TINT26             ((e_r_drv_tint_num_t)26)   /*!<  Group26 (TINT26) : ID:506 : P5_0,P5_1,P5_2,P5_3 */
#define INTC_TINT27             ((e_r_drv_tint_num_t)27)   /*!<  Group27 (TINT27) : ID:507 : P4_1,P4_2,P4_3,P4_4,P4_5,P4_6,P4_7*/
#define INTC_TINT28             ((e_r_drv_tint_num_t)28)   /*!<  Group28 (TINT28) : ID:508 : P3_1,P3_2,P3_3,P3_4,P3_5,P4_0*/
#define INTC_TINT29             ((e_r_drv_tint_num_t)29)   /*!<  Group29 (TINT29) : ID:509 : P2_0,P2_1,P2_2,P2_3,P3_0 */
#define INTC_TINT30             ((e_r_drv_tint_num_t)30)   /*!<  Group30 (TINT30) : ID:510 : P1_0,P1_1,P1_2,P1_3,P1_4 */
#define INTC_TINT31             ((e_r_drv_tint_num_t)31)   /*!<  Group31 (TINT31) : ID:511 : P0_0,P0_1,P0_2,P0_3,P0_4,P0_5,P0_6 */

/* ==== Interrupt Sense Selector ==== */
#define INT_SENSE_LEVEL         (0)
#define INT_SENSE_EDGE          (1)

/* ==== Interrupt Mask for keeping resume state as unsignaled during transitioning to standby mode ==== */
#define IRQ_MASK_ON             ((e_r_drv_irq_mask_t)0)
#define IRQ_MASK_OFF            ((e_r_drv_irq_mask_t)1)

/* Interrupt registers definition */
#define INTC_REG_ICR0                   ((e_r_drv_intc_reg_t)0x8000u)
#define INTC_REG_ICR1                   ((e_r_drv_intc_reg_t)0x8002u)
#define INTC_REG_IRQRR                  ((e_r_drv_intc_reg_t)0x8004u)
#define INTC_REG_GICD_CTLR              ((e_r_drv_intc_reg_t)0x1000u)
#define INTC_REG_GICD_TYPER             ((e_r_drv_intc_reg_t)0x1004u)
#define INTC_REG_GICD_IIDR              ((e_r_drv_intc_reg_t)0x1008u)
#define INTC_REG_GICD_IGROUPR0          ((e_r_drv_intc_reg_t)0x1080u)
#define INTC_REG_GICD_IGROUPR1          ((e_r_drv_intc_reg_t)0x1084u)
#define INTC_REG_GICD_IGROUPR2          ((e_r_drv_intc_reg_t)0x1088u)
#define INTC_REG_GICD_IGROUPR3          ((e_r_drv_intc_reg_t)0x108Cu)
#define INTC_REG_GICD_IGROUPR4          ((e_r_drv_intc_reg_t)0x1090u)
#define INTC_REG_GICD_IGROUPR5          ((e_r_drv_intc_reg_t)0x1094u)
#define INTC_REG_GICD_IGROUPR6          ((e_r_drv_intc_reg_t)0x1098u)
#define INTC_REG_GICD_IGROUPR7          ((e_r_drv_intc_reg_t)0x109Cu)
#define INTC_REG_GICD_IGROUPR8          ((e_r_drv_intc_reg_t)0x10A0u)
#define INTC_REG_GICD_IGROUPR9          ((e_r_drv_intc_reg_t)0x10A4u)
#define INTC_REG_GICD_IGROUPR10         ((e_r_drv_intc_reg_t)0x10A8u)
#define INTC_REG_GICD_IGROUPR11         ((e_r_drv_intc_reg_t)0x10ACu)
#define INTC_REG_GICD_IGROUPR12         ((e_r_drv_intc_reg_t)0x10B0u)
#define INTC_REG_GICD_IGROUPR13         ((e_r_drv_intc_reg_t)0x10B4u)
#define INTC_REG_GICD_IGROUPR14         ((e_r_drv_intc_reg_t)0x10B8u)
#define INTC_REG_GICD_IGROUPR15         ((e_r_drv_intc_reg_t)0x10BCu)
#define INTC_REG_GICD_ISENABLER0        ((e_r_drv_intc_reg_t)0x1100u)
#define INTC_REG_GICD_ISENABLER1        ((e_r_drv_intc_reg_t)0x1104u)
#define INTC_REG_GICD_ISENABLER2        ((e_r_drv_intc_reg_t)0x1108u)
#define INTC_REG_GICD_ISENABLER3        ((e_r_drv_intc_reg_t)0x110Cu)
#define INTC_REG_GICD_ISENABLER4        ((e_r_drv_intc_reg_t)0x1110u)
#define INTC_REG_GICD_ISENABLER5        ((e_r_drv_intc_reg_t)0x1114u)
#define INTC_REG_GICD_ISENABLER6        ((e_r_drv_intc_reg_t)0x1118u)
#define INTC_REG_GICD_ISENABLER7        ((e_r_drv_intc_reg_t)0x111Cu)
#define INTC_REG_GICD_ISENABLER8        ((e_r_drv_intc_reg_t)0x1120u)
#define INTC_REG_GICD_ISENABLER9        ((e_r_drv_intc_reg_t)0x1124u)
#define INTC_REG_GICD_ISENABLER10       ((e_r_drv_intc_reg_t)0x1128u)
#define INTC_REG_GICD_ISENABLER11       ((e_r_drv_intc_reg_t)0x112Cu)
#define INTC_REG_GICD_ISENABLER12       ((e_r_drv_intc_reg_t)0x1130u)
#define INTC_REG_GICD_ISENABLER13       ((e_r_drv_intc_reg_t)0x1134u)
#define INTC_REG_GICD_ISENABLER14       ((e_r_drv_intc_reg_t)0x1138u)
#define INTC_REG_GICD_ISENABLER15       ((e_r_drv_intc_reg_t)0x113Cu)
#define INTC_REG_GICD_ICENABLER0        ((e_r_drv_intc_reg_t)0x1180u)
#define INTC_REG_GICD_ICENABLER1        ((e_r_drv_intc_reg_t)0x1184u)
#define INTC_REG_GICD_ICENABLER2        ((e_r_drv_intc_reg_t)0x1188u)
#define INTC_REG_GICD_ICENABLER3        ((e_r_drv_intc_reg_t)0x118Cu)
#define INTC_REG_GICD_ICENABLER4        ((e_r_drv_intc_reg_t)0x1190u)
#define INTC_REG_GICD_ICENABLER5        ((e_r_drv_intc_reg_t)0x1194u)
#define INTC_REG_GICD_ICENABLER6        ((e_r_drv_intc_reg_t)0x1198u)
#define INTC_REG_GICD_ICENABLER7        ((e_r_drv_intc_reg_t)0x119Cu)
#define INTC_REG_GICD_ICENABLER8        ((e_r_drv_intc_reg_t)0x11A0u)
#define INTC_REG_GICD_ICENABLER9        ((e_r_drv_intc_reg_t)0x11A4u)
#define INTC_REG_GICD_ICENABLER10       ((e_r_drv_intc_reg_t)0x11A8u)
#define INTC_REG_GICD_ICENABLER11       ((e_r_drv_intc_reg_t)0x11ACu)
#define INTC_REG_GICD_ICENABLER12       ((e_r_drv_intc_reg_t)0x11B0u)
#define INTC_REG_GICD_ICENABLER13       ((e_r_drv_intc_reg_t)0x11B4u)
#define INTC_REG_GICD_ICENABLER14       ((e_r_drv_intc_reg_t)0x11B8u)
#define INTC_REG_GICD_ICENABLER15       ((e_r_drv_intc_reg_t)0x11BCu)
#define INTC_REG_GICD_ISPENDR0          ((e_r_drv_intc_reg_t)0x1200u)
#define INTC_REG_GICD_ISPENDR1          ((e_r_drv_intc_reg_t)0x1204u)
#define INTC_REG_GICD_ISPENDR2          ((e_r_drv_intc_reg_t)0x1208u)
#define INTC_REG_GICD_ISPENDR3          ((e_r_drv_intc_reg_t)0x120Cu)
#define INTC_REG_GICD_ISPENDR4          ((e_r_drv_intc_reg_t)0x1210u)
#define INTC_REG_GICD_ISPENDR5          ((e_r_drv_intc_reg_t)0x1214u)
#define INTC_REG_GICD_ISPENDR6          ((e_r_drv_intc_reg_t)0x1218u)
#define INTC_REG_GICD_ISPENDR7          ((e_r_drv_intc_reg_t)0x121Cu)
#define INTC_REG_GICD_ISPENDR8          ((e_r_drv_intc_reg_t)0x1220u)
#define INTC_REG_GICD_ISPENDR9          ((e_r_drv_intc_reg_t)0x1224u)
#define INTC_REG_GICD_ISPENDR10         ((e_r_drv_intc_reg_t)0x1228u)
#define INTC_REG_GICD_ISPENDR11         ((e_r_drv_intc_reg_t)0x122Cu)
#define INTC_REG_GICD_ISPENDR12         ((e_r_drv_intc_reg_t)0x1230u)
#define INTC_REG_GICD_ISPENDR13         ((e_r_drv_intc_reg_t)0x1234u)
#define INTC_REG_GICD_ISPENDR14         ((e_r_drv_intc_reg_t)0x1238u)
#define INTC_REG_GICD_ISPENDR15         ((e_r_drv_intc_reg_t)0x123Cu)
#define INTC_REG_GICD_ICPENDR0          ((e_r_drv_intc_reg_t)0x1280u)
#define INTC_REG_GICD_ICPENDR1          ((e_r_drv_intc_reg_t)0x1284u)
#define INTC_REG_GICD_ICPENDR2          ((e_r_drv_intc_reg_t)0x1288u)
#define INTC_REG_GICD_ICPENDR3          ((e_r_drv_intc_reg_t)0x128Cu)
#define INTC_REG_GICD_ICPENDR4          ((e_r_drv_intc_reg_t)0x1290u)
#define INTC_REG_GICD_ICPENDR5          ((e_r_drv_intc_reg_t)0x1294u)
#define INTC_REG_GICD_ICPENDR6          ((e_r_drv_intc_reg_t)0x1298u)
#define INTC_REG_GICD_ICPENDR7          ((e_r_drv_intc_reg_t)0x129Cu)
#define INTC_REG_GICD_ICPENDR8          ((e_r_drv_intc_reg_t)0x12A0u)
#define INTC_REG_GICD_ICPENDR9          ((e_r_drv_intc_reg_t)0x12A4u)
#define INTC_REG_GICD_ICPENDR10         ((e_r_drv_intc_reg_t)0x12A8u)
#define INTC_REG_GICD_ICPENDR11         ((e_r_drv_intc_reg_t)0x12ACu)
#define INTC_REG_GICD_ICPENDR12         ((e_r_drv_intc_reg_t)0x12B0u)
#define INTC_REG_GICD_ICPENDR13         ((e_r_drv_intc_reg_t)0x12B4u)
#define INTC_REG_GICD_ICPENDR14         ((e_r_drv_intc_reg_t)0x12B8u)
#define INTC_REG_GICD_ICPENDR15         ((e_r_drv_intc_reg_t)0x12BCu)
#define INTC_REG_GICD_ISACTIVER0        ((e_r_drv_intc_reg_t)0x1300u)
#define INTC_REG_GICD_ISACTIVER1        ((e_r_drv_intc_reg_t)0x1304u)
#define INTC_REG_GICD_ISACTIVER2        ((e_r_drv_intc_reg_t)0x1308u)
#define INTC_REG_GICD_ISACTIVER3        ((e_r_drv_intc_reg_t)0x130Cu)
#define INTC_REG_GICD_ISACTIVER4        ((e_r_drv_intc_reg_t)0x1310u)
#define INTC_REG_GICD_ISACTIVER5        ((e_r_drv_intc_reg_t)0x1314u)
#define INTC_REG_GICD_ISACTIVER6        ((e_r_drv_intc_reg_t)0x1318u)
#define INTC_REG_GICD_ISACTIVER7        ((e_r_drv_intc_reg_t)0x131Cu)
#define INTC_REG_GICD_ISACTIVER8        ((e_r_drv_intc_reg_t)0x1320u)
#define INTC_REG_GICD_ISACTIVER9        ((e_r_drv_intc_reg_t)0x1324u)
#define INTC_REG_GICD_ISACTIVER10       ((e_r_drv_intc_reg_t)0x1328u)
#define INTC_REG_GICD_ISACTIVER11       ((e_r_drv_intc_reg_t)0x132Cu)
#define INTC_REG_GICD_ISACTIVER12       ((e_r_drv_intc_reg_t)0x1330u)
#define INTC_REG_GICD_ISACTIVER13       ((e_r_drv_intc_reg_t)0x1334u)
#define INTC_REG_GICD_ISACTIVER14       ((e_r_drv_intc_reg_t)0x1338u)
#define INTC_REG_GICD_ISACTIVER15       ((e_r_drv_intc_reg_t)0x133Cu)
#define INTC_REG_GICD_ICACTIVER0        ((e_r_drv_intc_reg_t)0x1380u)
#define INTC_REG_GICD_ICACTIVER1        ((e_r_drv_intc_reg_t)0x1384u)
#define INTC_REG_GICD_ICACTIVER2        ((e_r_drv_intc_reg_t)0x1388u)
#define INTC_REG_GICD_ICACTIVER3        ((e_r_drv_intc_reg_t)0x138Cu)
#define INTC_REG_GICD_ICACTIVER4        ((e_r_drv_intc_reg_t)0x1390u)
#define INTC_REG_GICD_ICACTIVER5        ((e_r_drv_intc_reg_t)0x1394u)
#define INTC_REG_GICD_ICACTIVER6        ((e_r_drv_intc_reg_t)0x1398u)
#define INTC_REG_GICD_ICACTIVER7        ((e_r_drv_intc_reg_t)0x139Cu)
#define INTC_REG_GICD_ICACTIVER8        ((e_r_drv_intc_reg_t)0x13A0u)
#define INTC_REG_GICD_ICACTIVER9        ((e_r_drv_intc_reg_t)0x13A4u)
#define INTC_REG_GICD_ICACTIVER10       ((e_r_drv_intc_reg_t)0x13A8u)
#define INTC_REG_GICD_ICACTIVER11       ((e_r_drv_intc_reg_t)0x13ACu)
#define INTC_REG_GICD_ICACTIVER12       ((e_r_drv_intc_reg_t)0x13B0u)
#define INTC_REG_GICD_ICACTIVER13       ((e_r_drv_intc_reg_t)0x13B4u)
#define INTC_REG_GICD_ICACTIVER14       ((e_r_drv_intc_reg_t)0x13B8u)
#define INTC_REG_GICD_ICACTIVER15       ((e_r_drv_intc_reg_t)0x13BCu)
#define INTC_REG_GICD_IPRIORITYR0       ((e_r_drv_intc_reg_t)0x1400u)
#define INTC_REG_GICD_IPRIORITYR1       ((e_r_drv_intc_reg_t)0x1404u)
#define INTC_REG_GICD_IPRIORITYR2       ((e_r_drv_intc_reg_t)0x1408u)
#define INTC_REG_GICD_IPRIORITYR3       ((e_r_drv_intc_reg_t)0x140Cu)
#define INTC_REG_GICD_IPRIORITYR4       ((e_r_drv_intc_reg_t)0x1410u)
#define INTC_REG_GICD_IPRIORITYR5       ((e_r_drv_intc_reg_t)0x1414u)
#define INTC_REG_GICD_IPRIORITYR6       ((e_r_drv_intc_reg_t)0x1418u)
#define INTC_REG_GICD_IPRIORITYR7       ((e_r_drv_intc_reg_t)0x141Cu)
#define INTC_REG_GICD_IPRIORITYR8       ((e_r_drv_intc_reg_t)0x1420u)
#define INTC_REG_GICD_IPRIORITYR9       ((e_r_drv_intc_reg_t)0x1424u)
#define INTC_REG_GICD_IPRIORITYR10      ((e_r_drv_intc_reg_t)0x1428u)
#define INTC_REG_GICD_IPRIORITYR11      ((e_r_drv_intc_reg_t)0x142Cu)
#define INTC_REG_GICD_IPRIORITYR12      ((e_r_drv_intc_reg_t)0x1430u)
#define INTC_REG_GICD_IPRIORITYR13      ((e_r_drv_intc_reg_t)0x1434u)
#define INTC_REG_GICD_IPRIORITYR14      ((e_r_drv_intc_reg_t)0x1438u)
#define INTC_REG_GICD_IPRIORITYR15      ((e_r_drv_intc_reg_t)0x143Cu)
#define INTC_REG_GICD_IPRIORITYR16      ((e_r_drv_intc_reg_t)0x1440u)
#define INTC_REG_GICD_IPRIORITYR17      ((e_r_drv_intc_reg_t)0x1444u)
#define INTC_REG_GICD_IPRIORITYR18      ((e_r_drv_intc_reg_t)0x1448u)
#define INTC_REG_GICD_IPRIORITYR19      ((e_r_drv_intc_reg_t)0x144Cu)
#define INTC_REG_GICD_IPRIORITYR20      ((e_r_drv_intc_reg_t)0x1450u)
#define INTC_REG_GICD_IPRIORITYR21      ((e_r_drv_intc_reg_t)0x1454u)
#define INTC_REG_GICD_IPRIORITYR22      ((e_r_drv_intc_reg_t)0x1458u)
#define INTC_REG_GICD_IPRIORITYR23      ((e_r_drv_intc_reg_t)0x145Cu)
#define INTC_REG_GICD_IPRIORITYR24      ((e_r_drv_intc_reg_t)0x1460u)
#define INTC_REG_GICD_IPRIORITYR25      ((e_r_drv_intc_reg_t)0x1464u)
#define INTC_REG_GICD_IPRIORITYR26      ((e_r_drv_intc_reg_t)0x1468u)
#define INTC_REG_GICD_IPRIORITYR27      ((e_r_drv_intc_reg_t)0x146Cu)
#define INTC_REG_GICD_IPRIORITYR28      ((e_r_drv_intc_reg_t)0x1470u)
#define INTC_REG_GICD_IPRIORITYR29      ((e_r_drv_intc_reg_t)0x1474u)
#define INTC_REG_GICD_IPRIORITYR30      ((e_r_drv_intc_reg_t)0x1478u)
#define INTC_REG_GICD_IPRIORITYR31      ((e_r_drv_intc_reg_t)0x147Cu)
#define INTC_REG_GICD_IPRIORITYR32      ((e_r_drv_intc_reg_t)0x1480u)
#define INTC_REG_GICD_IPRIORITYR33      ((e_r_drv_intc_reg_t)0x1484u)
#define INTC_REG_GICD_IPRIORITYR34      ((e_r_drv_intc_reg_t)0x1488u)
#define INTC_REG_GICD_IPRIORITYR35      ((e_r_drv_intc_reg_t)0x148Cu)
#define INTC_REG_GICD_IPRIORITYR36      ((e_r_drv_intc_reg_t)0x1490u)
#define INTC_REG_GICD_IPRIORITYR37      ((e_r_drv_intc_reg_t)0x1494u)
#define INTC_REG_GICD_IPRIORITYR38      ((e_r_drv_intc_reg_t)0x1498u)
#define INTC_REG_GICD_IPRIORITYR39      ((e_r_drv_intc_reg_t)0x149Cu)
#define INTC_REG_GICD_IPRIORITYR40      ((e_r_drv_intc_reg_t)0x14A0u)
#define INTC_REG_GICD_IPRIORITYR41      ((e_r_drv_intc_reg_t)0x14A4u)
#define INTC_REG_GICD_IPRIORITYR42      ((e_r_drv_intc_reg_t)0x14A8u)
#define INTC_REG_GICD_IPRIORITYR43      ((e_r_drv_intc_reg_t)0x14ACu)
#define INTC_REG_GICD_IPRIORITYR44      ((e_r_drv_intc_reg_t)0x14B0u)
#define INTC_REG_GICD_IPRIORITYR45      ((e_r_drv_intc_reg_t)0x14B4u)
#define INTC_REG_GICD_IPRIORITYR46      ((e_r_drv_intc_reg_t)0x14B8u)
#define INTC_REG_GICD_IPRIORITYR47      ((e_r_drv_intc_reg_t)0x14BCu)
#define INTC_REG_GICD_IPRIORITYR48      ((e_r_drv_intc_reg_t)0x14C0u)
#define INTC_REG_GICD_IPRIORITYR49      ((e_r_drv_intc_reg_t)0x14C4u)
#define INTC_REG_GICD_IPRIORITYR50      ((e_r_drv_intc_reg_t)0x14C8u)
#define INTC_REG_GICD_IPRIORITYR51      ((e_r_drv_intc_reg_t)0x14CCu)
#define INTC_REG_GICD_IPRIORITYR52      ((e_r_drv_intc_reg_t)0x14D0u)
#define INTC_REG_GICD_IPRIORITYR53      ((e_r_drv_intc_reg_t)0x14D4u)
#define INTC_REG_GICD_IPRIORITYR54      ((e_r_drv_intc_reg_t)0x14D8u)
#define INTC_REG_GICD_IPRIORITYR55      ((e_r_drv_intc_reg_t)0x14DCu)
#define INTC_REG_GICD_IPRIORITYR56      ((e_r_drv_intc_reg_t)0x14E0u)
#define INTC_REG_GICD_IPRIORITYR57      ((e_r_drv_intc_reg_t)0x14E4u)
#define INTC_REG_GICD_IPRIORITYR58      ((e_r_drv_intc_reg_t)0x14E8u)
#define INTC_REG_GICD_IPRIORITYR59      ((e_r_drv_intc_reg_t)0x14ECu)
#define INTC_REG_GICD_IPRIORITYR60      ((e_r_drv_intc_reg_t)0x14F0u)
#define INTC_REG_GICD_IPRIORITYR61      ((e_r_drv_intc_reg_t)0x14F4u)
#define INTC_REG_GICD_IPRIORITYR62      ((e_r_drv_intc_reg_t)0x14F8u)
#define INTC_REG_GICD_IPRIORITYR63      ((e_r_drv_intc_reg_t)0x14FCu)
#define INTC_REG_GICD_IPRIORITYR64      ((e_r_drv_intc_reg_t)0x1500u)
#define INTC_REG_GICD_IPRIORITYR65      ((e_r_drv_intc_reg_t)0x1504u)
#define INTC_REG_GICD_IPRIORITYR66      ((e_r_drv_intc_reg_t)0x1508u)
#define INTC_REG_GICD_IPRIORITYR67      ((e_r_drv_intc_reg_t)0x150Cu)
#define INTC_REG_GICD_IPRIORITYR68      ((e_r_drv_intc_reg_t)0x1510u)
#define INTC_REG_GICD_IPRIORITYR69      ((e_r_drv_intc_reg_t)0x1514u)
#define INTC_REG_GICD_IPRIORITYR70      ((e_r_drv_intc_reg_t)0x1518u)
#define INTC_REG_GICD_IPRIORITYR71      ((e_r_drv_intc_reg_t)0x151Cu)
#define INTC_REG_GICD_IPRIORITYR72      ((e_r_drv_intc_reg_t)0x1520u)
#define INTC_REG_GICD_IPRIORITYR73      ((e_r_drv_intc_reg_t)0x1524u)
#define INTC_REG_GICD_IPRIORITYR74      ((e_r_drv_intc_reg_t)0x1528u)
#define INTC_REG_GICD_IPRIORITYR75      ((e_r_drv_intc_reg_t)0x152Cu)
#define INTC_REG_GICD_IPRIORITYR76      ((e_r_drv_intc_reg_t)0x1530u)
#define INTC_REG_GICD_IPRIORITYR77      ((e_r_drv_intc_reg_t)0x1534u)
#define INTC_REG_GICD_IPRIORITYR78      ((e_r_drv_intc_reg_t)0x1538u)
#define INTC_REG_GICD_IPRIORITYR79      ((e_r_drv_intc_reg_t)0x153Cu)
#define INTC_REG_GICD_IPRIORITYR80      ((e_r_drv_intc_reg_t)0x1540u)
#define INTC_REG_GICD_IPRIORITYR81      ((e_r_drv_intc_reg_t)0x1544u)
#define INTC_REG_GICD_IPRIORITYR82      ((e_r_drv_intc_reg_t)0x1548u)
#define INTC_REG_GICD_IPRIORITYR83      ((e_r_drv_intc_reg_t)0x154Cu)
#define INTC_REG_GICD_IPRIORITYR84      ((e_r_drv_intc_reg_t)0x1550u)
#define INTC_REG_GICD_IPRIORITYR85      ((e_r_drv_intc_reg_t)0x1554u)
#define INTC_REG_GICD_IPRIORITYR86      ((e_r_drv_intc_reg_t)0x1558u)
#define INTC_REG_GICD_IPRIORITYR87      ((e_r_drv_intc_reg_t)0x155Cu)
#define INTC_REG_GICD_IPRIORITYR88      ((e_r_drv_intc_reg_t)0x1560u)
#define INTC_REG_GICD_IPRIORITYR89      ((e_r_drv_intc_reg_t)0x1564u)
#define INTC_REG_GICD_IPRIORITYR90      ((e_r_drv_intc_reg_t)0x1568u)
#define INTC_REG_GICD_IPRIORITYR91      ((e_r_drv_intc_reg_t)0x156Cu)
#define INTC_REG_GICD_IPRIORITYR92      ((e_r_drv_intc_reg_t)0x1570u)
#define INTC_REG_GICD_IPRIORITYR93      ((e_r_drv_intc_reg_t)0x1574u)
#define INTC_REG_GICD_IPRIORITYR94      ((e_r_drv_intc_reg_t)0x1578u)
#define INTC_REG_GICD_IPRIORITYR95      ((e_r_drv_intc_reg_t)0x157Cu)
#define INTC_REG_GICD_IPRIORITYR96      ((e_r_drv_intc_reg_t)0x1580u)
#define INTC_REG_GICD_IPRIORITYR97      ((e_r_drv_intc_reg_t)0x1584u)
#define INTC_REG_GICD_IPRIORITYR98      ((e_r_drv_intc_reg_t)0x1588u)
#define INTC_REG_GICD_IPRIORITYR99      ((e_r_drv_intc_reg_t)0x158Cu)
#define INTC_REG_GICD_IPRIORITYR100     ((e_r_drv_intc_reg_t)0x1590u)
#define INTC_REG_GICD_IPRIORITYR101     ((e_r_drv_intc_reg_t)0x1594u)
#define INTC_REG_GICD_IPRIORITYR102     ((e_r_drv_intc_reg_t)0x1598u)
#define INTC_REG_GICD_IPRIORITYR103     ((e_r_drv_intc_reg_t)0x159Cu)
#define INTC_REG_GICD_IPRIORITYR104     ((e_r_drv_intc_reg_t)0x15A0u)
#define INTC_REG_GICD_IPRIORITYR105     ((e_r_drv_intc_reg_t)0x15A4u)
#define INTC_REG_GICD_IPRIORITYR106     ((e_r_drv_intc_reg_t)0x15A8u)
#define INTC_REG_GICD_IPRIORITYR107     ((e_r_drv_intc_reg_t)0x15ACu)
#define INTC_REG_GICD_IPRIORITYR108     ((e_r_drv_intc_reg_t)0x15B0u)
#define INTC_REG_GICD_IPRIORITYR109     ((e_r_drv_intc_reg_t)0x15B4u)
#define INTC_REG_GICD_IPRIORITYR110     ((e_r_drv_intc_reg_t)0x15B8u)
#define INTC_REG_GICD_IPRIORITYR111     ((e_r_drv_intc_reg_t)0x15BCu)
#define INTC_REG_GICD_IPRIORITYR112     ((e_r_drv_intc_reg_t)0x15C0u)
#define INTC_REG_GICD_IPRIORITYR113     ((e_r_drv_intc_reg_t)0x15C4u)
#define INTC_REG_GICD_IPRIORITYR114     ((e_r_drv_intc_reg_t)0x15C8u)
#define INTC_REG_GICD_IPRIORITYR115     ((e_r_drv_intc_reg_t)0x15CCu)
#define INTC_REG_GICD_IPRIORITYR116     ((e_r_drv_intc_reg_t)0x15D0u)
#define INTC_REG_GICD_IPRIORITYR117     ((e_r_drv_intc_reg_t)0x15D4u)
#define INTC_REG_GICD_IPRIORITYR118     ((e_r_drv_intc_reg_t)0x15D8u)
#define INTC_REG_GICD_IPRIORITYR119     ((e_r_drv_intc_reg_t)0x15DCu)
#define INTC_REG_GICD_IPRIORITYR120     ((e_r_drv_intc_reg_t)0x15E0u)
#define INTC_REG_GICD_IPRIORITYR121     ((e_r_drv_intc_reg_t)0x15E4u)
#define INTC_REG_GICD_IPRIORITYR122     ((e_r_drv_intc_reg_t)0x15E8u)
#define INTC_REG_GICD_IPRIORITYR123     ((e_r_drv_intc_reg_t)0x15ECu)
#define INTC_REG_GICD_IPRIORITYR124     ((e_r_drv_intc_reg_t)0x15F0u)
#define INTC_REG_GICD_IPRIORITYR125     ((e_r_drv_intc_reg_t)0x15F4u)
#define INTC_REG_GICD_IPRIORITYR126     ((e_r_drv_intc_reg_t)0x15F8u)
#define INTC_REG_GICD_IPRIORITYR127     ((e_r_drv_intc_reg_t)0x15FCu)
#define INTC_REG_GICD_ITARGETSR0        ((e_r_drv_intc_reg_t)0x1800u)
#define INTC_REG_GICD_ITARGETSR1        ((e_r_drv_intc_reg_t)0x1804u)
#define INTC_REG_GICD_ITARGETSR2        ((e_r_drv_intc_reg_t)0x1808u)
#define INTC_REG_GICD_ITARGETSR3        ((e_r_drv_intc_reg_t)0x180Cu)
#define INTC_REG_GICD_ITARGETSR4        ((e_r_drv_intc_reg_t)0x1810u)
#define INTC_REG_GICD_ITARGETSR5        ((e_r_drv_intc_reg_t)0x1814u)
#define INTC_REG_GICD_ITARGETSR6        ((e_r_drv_intc_reg_t)0x1818u)
#define INTC_REG_GICD_ITARGETSR7        ((e_r_drv_intc_reg_t)0x181Cu)
#define INTC_REG_GICD_ITARGETSR8        ((e_r_drv_intc_reg_t)0x1820u)
#define INTC_REG_GICD_ITARGETSR9        ((e_r_drv_intc_reg_t)0x1824u)
#define INTC_REG_GICD_ITARGETSR10       ((e_r_drv_intc_reg_t)0x1828u)
#define INTC_REG_GICD_ITARGETSR11       ((e_r_drv_intc_reg_t)0x182Cu)
#define INTC_REG_GICD_ITARGETSR12       ((e_r_drv_intc_reg_t)0x1830u)
#define INTC_REG_GICD_ITARGETSR13       ((e_r_drv_intc_reg_t)0x1834u)
#define INTC_REG_GICD_ITARGETSR14       ((e_r_drv_intc_reg_t)0x1838u)
#define INTC_REG_GICD_ITARGETSR15       ((e_r_drv_intc_reg_t)0x183Cu)
#define INTC_REG_GICD_ITARGETSR16       ((e_r_drv_intc_reg_t)0x1840u)
#define INTC_REG_GICD_ITARGETSR17       ((e_r_drv_intc_reg_t)0x1844u)
#define INTC_REG_GICD_ITARGETSR18       ((e_r_drv_intc_reg_t)0x1848u)
#define INTC_REG_GICD_ITARGETSR19       ((e_r_drv_intc_reg_t)0x184Cu)
#define INTC_REG_GICD_ITARGETSR20       ((e_r_drv_intc_reg_t)0x1850u)
#define INTC_REG_GICD_ITARGETSR21       ((e_r_drv_intc_reg_t)0x1854u)
#define INTC_REG_GICD_ITARGETSR22       ((e_r_drv_intc_reg_t)0x1858u)
#define INTC_REG_GICD_ITARGETSR23       ((e_r_drv_intc_reg_t)0x185Cu)
#define INTC_REG_GICD_ITARGETSR24       ((e_r_drv_intc_reg_t)0x1860u)
#define INTC_REG_GICD_ITARGETSR25       ((e_r_drv_intc_reg_t)0x1864u)
#define INTC_REG_GICD_ITARGETSR26       ((e_r_drv_intc_reg_t)0x1868u)
#define INTC_REG_GICD_ITARGETSR27       ((e_r_drv_intc_reg_t)0x186Cu)
#define INTC_REG_GICD_ITARGETSR28       ((e_r_drv_intc_reg_t)0x1870u)
#define INTC_REG_GICD_ITARGETSR29       ((e_r_drv_intc_reg_t)0x1874u)
#define INTC_REG_GICD_ITARGETSR30       ((e_r_drv_intc_reg_t)0x1878u)
#define INTC_REG_GICD_ITARGETSR31       ((e_r_drv_intc_reg_t)0x187Cu)
#define INTC_REG_GICD_ITARGETSR32       ((e_r_drv_intc_reg_t)0x1880u)
#define INTC_REG_GICD_ITARGETSR33       ((e_r_drv_intc_reg_t)0x1884u)
#define INTC_REG_GICD_ITARGETSR34       ((e_r_drv_intc_reg_t)0x1888u)
#define INTC_REG_GICD_ITARGETSR35       ((e_r_drv_intc_reg_t)0x188Cu)
#define INTC_REG_GICD_ITARGETSR36       ((e_r_drv_intc_reg_t)0x1890u)
#define INTC_REG_GICD_ITARGETSR37       ((e_r_drv_intc_reg_t)0x1894u)
#define INTC_REG_GICD_ITARGETSR38       ((e_r_drv_intc_reg_t)0x1898u)
#define INTC_REG_GICD_ITARGETSR39       ((e_r_drv_intc_reg_t)0x189Cu)
#define INTC_REG_GICD_ITARGETSR40       ((e_r_drv_intc_reg_t)0x18A0u)
#define INTC_REG_GICD_ITARGETSR41       ((e_r_drv_intc_reg_t)0x18A4u)
#define INTC_REG_GICD_ITARGETSR42       ((e_r_drv_intc_reg_t)0x18A8u)
#define INTC_REG_GICD_ITARGETSR43       ((e_r_drv_intc_reg_t)0x18ACu)
#define INTC_REG_GICD_ITARGETSR44       ((e_r_drv_intc_reg_t)0x18B0u)
#define INTC_REG_GICD_ITARGETSR45       ((e_r_drv_intc_reg_t)0x18B4u)
#define INTC_REG_GICD_ITARGETSR46       ((e_r_drv_intc_reg_t)0x18B8u)
#define INTC_REG_GICD_ITARGETSR47       ((e_r_drv_intc_reg_t)0x18BCu)
#define INTC_REG_GICD_ITARGETSR48       ((e_r_drv_intc_reg_t)0x18C0u)
#define INTC_REG_GICD_ITARGETSR49       ((e_r_drv_intc_reg_t)0x18C4u)
#define INTC_REG_GICD_ITARGETSR50       ((e_r_drv_intc_reg_t)0x18C8u)
#define INTC_REG_GICD_ITARGETSR51       ((e_r_drv_intc_reg_t)0x18CCu)
#define INTC_REG_GICD_ITARGETSR52       ((e_r_drv_intc_reg_t)0x18D0u)
#define INTC_REG_GICD_ITARGETSR53       ((e_r_drv_intc_reg_t)0x18D4u)
#define INTC_REG_GICD_ITARGETSR54       ((e_r_drv_intc_reg_t)0x18D8u)
#define INTC_REG_GICD_ITARGETSR55       ((e_r_drv_intc_reg_t)0x18DCu)
#define INTC_REG_GICD_ITARGETSR56       ((e_r_drv_intc_reg_t)0x18E0u)
#define INTC_REG_GICD_ITARGETSR57       ((e_r_drv_intc_reg_t)0x18E4u)
#define INTC_REG_GICD_ITARGETSR58       ((e_r_drv_intc_reg_t)0x18E8u)
#define INTC_REG_GICD_ITARGETSR59       ((e_r_drv_intc_reg_t)0x18ECu)
#define INTC_REG_GICD_ITARGETSR60       ((e_r_drv_intc_reg_t)0x18F0u)
#define INTC_REG_GICD_ITARGETSR61       ((e_r_drv_intc_reg_t)0x18F4u)
#define INTC_REG_GICD_ITARGETSR62       ((e_r_drv_intc_reg_t)0x18F8u)
#define INTC_REG_GICD_ITARGETSR63       ((e_r_drv_intc_reg_t)0x18FCu)
#define INTC_REG_GICD_ITARGETSR64       ((e_r_drv_intc_reg_t)0x1900u)
#define INTC_REG_GICD_ITARGETSR65       ((e_r_drv_intc_reg_t)0x1904u)
#define INTC_REG_GICD_ITARGETSR66       ((e_r_drv_intc_reg_t)0x1908u)
#define INTC_REG_GICD_ITARGETSR67       ((e_r_drv_intc_reg_t)0x190Cu)
#define INTC_REG_GICD_ITARGETSR68       ((e_r_drv_intc_reg_t)0x1910u)
#define INTC_REG_GICD_ITARGETSR69       ((e_r_drv_intc_reg_t)0x1914u)
#define INTC_REG_GICD_ITARGETSR70       ((e_r_drv_intc_reg_t)0x1918u)
#define INTC_REG_GICD_ITARGETSR71       ((e_r_drv_intc_reg_t)0x191Cu)
#define INTC_REG_GICD_ITARGETSR72       ((e_r_drv_intc_reg_t)0x1920u)
#define INTC_REG_GICD_ITARGETSR73       ((e_r_drv_intc_reg_t)0x1924u)
#define INTC_REG_GICD_ITARGETSR74       ((e_r_drv_intc_reg_t)0x1928u)
#define INTC_REG_GICD_ITARGETSR75       ((e_r_drv_intc_reg_t)0x192Cu)
#define INTC_REG_GICD_ITARGETSR76       ((e_r_drv_intc_reg_t)0x1930u)
#define INTC_REG_GICD_ITARGETSR77       ((e_r_drv_intc_reg_t)0x1934u)
#define INTC_REG_GICD_ITARGETSR78       ((e_r_drv_intc_reg_t)0x1938u)
#define INTC_REG_GICD_ITARGETSR79       ((e_r_drv_intc_reg_t)0x193Cu)
#define INTC_REG_GICD_ITARGETSR80       ((e_r_drv_intc_reg_t)0x1940u)
#define INTC_REG_GICD_ITARGETSR81       ((e_r_drv_intc_reg_t)0x1944u)
#define INTC_REG_GICD_ITARGETSR82       ((e_r_drv_intc_reg_t)0x1948u)
#define INTC_REG_GICD_ITARGETSR83       ((e_r_drv_intc_reg_t)0x194Cu)
#define INTC_REG_GICD_ITARGETSR84       ((e_r_drv_intc_reg_t)0x1950u)
#define INTC_REG_GICD_ITARGETSR85       ((e_r_drv_intc_reg_t)0x1954u)
#define INTC_REG_GICD_ITARGETSR86       ((e_r_drv_intc_reg_t)0x1958u)
#define INTC_REG_GICD_ITARGETSR87       ((e_r_drv_intc_reg_t)0x195Cu)
#define INTC_REG_GICD_ITARGETSR88       ((e_r_drv_intc_reg_t)0x1960u)
#define INTC_REG_GICD_ITARGETSR89       ((e_r_drv_intc_reg_t)0x1964u)
#define INTC_REG_GICD_ITARGETSR90       ((e_r_drv_intc_reg_t)0x1968u)
#define INTC_REG_GICD_ITARGETSR91       ((e_r_drv_intc_reg_t)0x196Cu)
#define INTC_REG_GICD_ITARGETSR92       ((e_r_drv_intc_reg_t)0x1970u)
#define INTC_REG_GICD_ITARGETSR93       ((e_r_drv_intc_reg_t)0x1974u)
#define INTC_REG_GICD_ITARGETSR94       ((e_r_drv_intc_reg_t)0x1978u)
#define INTC_REG_GICD_ITARGETSR95       ((e_r_drv_intc_reg_t)0x197Cu)
#define INTC_REG_GICD_ITARGETSR96       ((e_r_drv_intc_reg_t)0x1980u)
#define INTC_REG_GICD_ITARGETSR97       ((e_r_drv_intc_reg_t)0x1984u)
#define INTC_REG_GICD_ITARGETSR98       ((e_r_drv_intc_reg_t)0x1988u)
#define INTC_REG_GICD_ITARGETSR99       ((e_r_drv_intc_reg_t)0x198Cu)
#define INTC_REG_GICD_ITARGETSR100      ((e_r_drv_intc_reg_t)0x1990u)
#define INTC_REG_GICD_ITARGETSR101      ((e_r_drv_intc_reg_t)0x1994u)
#define INTC_REG_GICD_ITARGETSR102      ((e_r_drv_intc_reg_t)0x1998u)
#define INTC_REG_GICD_ITARGETSR103      ((e_r_drv_intc_reg_t)0x199Cu)
#define INTC_REG_GICD_ITARGETSR104      ((e_r_drv_intc_reg_t)0x19A0u)
#define INTC_REG_GICD_ITARGETSR105      ((e_r_drv_intc_reg_t)0x19A4u)
#define INTC_REG_GICD_ITARGETSR106      ((e_r_drv_intc_reg_t)0x19A8u)
#define INTC_REG_GICD_ITARGETSR107      ((e_r_drv_intc_reg_t)0x19ACu)
#define INTC_REG_GICD_ITARGETSR108      ((e_r_drv_intc_reg_t)0x19B0u)
#define INTC_REG_GICD_ITARGETSR109      ((e_r_drv_intc_reg_t)0x19B4u)
#define INTC_REG_GICD_ITARGETSR110      ((e_r_drv_intc_reg_t)0x19B8u)
#define INTC_REG_GICD_ITARGETSR111      ((e_r_drv_intc_reg_t)0x19BCu)
#define INTC_REG_GICD_ITARGETSR112      ((e_r_drv_intc_reg_t)0x19C0u)
#define INTC_REG_GICD_ITARGETSR113      ((e_r_drv_intc_reg_t)0x19C4u)
#define INTC_REG_GICD_ITARGETSR114      ((e_r_drv_intc_reg_t)0x19C8u)
#define INTC_REG_GICD_ITARGETSR115      ((e_r_drv_intc_reg_t)0x19CCu)
#define INTC_REG_GICD_ITARGETSR116      ((e_r_drv_intc_reg_t)0x19D0u)
#define INTC_REG_GICD_ITARGETSR117      ((e_r_drv_intc_reg_t)0x19D4u)
#define INTC_REG_GICD_ITARGETSR118      ((e_r_drv_intc_reg_t)0x19D8u)
#define INTC_REG_GICD_ITARGETSR119      ((e_r_drv_intc_reg_t)0x19DCu)
#define INTC_REG_GICD_ITARGETSR120      ((e_r_drv_intc_reg_t)0x19E0u)
#define INTC_REG_GICD_ITARGETSR121      ((e_r_drv_intc_reg_t)0x19E4u)
#define INTC_REG_GICD_ITARGETSR122      ((e_r_drv_intc_reg_t)0x19E8u)
#define INTC_REG_GICD_ITARGETSR123      ((e_r_drv_intc_reg_t)0x19ECu)
#define INTC_REG_GICD_ITARGETSR124      ((e_r_drv_intc_reg_t)0x19F0u)
#define INTC_REG_GICD_ITARGETSR125      ((e_r_drv_intc_reg_t)0x19F4u)
#define INTC_REG_GICD_ITARGETSR126      ((e_r_drv_intc_reg_t)0x19F8u)
#define INTC_REG_GICD_ITARGETSR127      ((e_r_drv_intc_reg_t)0x19FCu)
#define INTC_REG_GICD_ICFGR0            ((e_r_drv_intc_reg_t)0x1C00u)
#define INTC_REG_GICD_ICFGR1            ((e_r_drv_intc_reg_t)0x1C04u)
#define INTC_REG_GICD_ICFGR2            ((e_r_drv_intc_reg_t)0x1C08u)
#define INTC_REG_GICD_ICFGR3            ((e_r_drv_intc_reg_t)0x1C0Cu)
#define INTC_REG_GICD_ICFGR4            ((e_r_drv_intc_reg_t)0x1C10u)
#define INTC_REG_GICD_ICFGR5            ((e_r_drv_intc_reg_t)0x1C14u)
#define INTC_REG_GICD_ICFGR6            ((e_r_drv_intc_reg_t)0x1C18u)
#define INTC_REG_GICD_ICFGR7            ((e_r_drv_intc_reg_t)0x1C1Cu)
#define INTC_REG_GICD_ICFGR8            ((e_r_drv_intc_reg_t)0x1C20u)
#define INTC_REG_GICD_ICFGR9            ((e_r_drv_intc_reg_t)0x1C24u)
#define INTC_REG_GICD_ICFGR10           ((e_r_drv_intc_reg_t)0x1C28u)
#define INTC_REG_GICD_ICFGR11           ((e_r_drv_intc_reg_t)0x1C2Cu)
#define INTC_REG_GICD_ICFGR12           ((e_r_drv_intc_reg_t)0x1C30u)
#define INTC_REG_GICD_ICFGR13           ((e_r_drv_intc_reg_t)0x1C34u)
#define INTC_REG_GICD_ICFGR14           ((e_r_drv_intc_reg_t)0x1C38u)
#define INTC_REG_GICD_ICFGR15           ((e_r_drv_intc_reg_t)0x1C3Cu)
#define INTC_REG_GICD_ICFGR16           ((e_r_drv_intc_reg_t)0x1C40u)
#define INTC_REG_GICD_ICFGR17           ((e_r_drv_intc_reg_t)0x1C44u)
#define INTC_REG_GICD_ICFGR18           ((e_r_drv_intc_reg_t)0x1C48u)
#define INTC_REG_GICD_ICFGR19           ((e_r_drv_intc_reg_t)0x1C4Cu)
#define INTC_REG_GICD_ICFGR20           ((e_r_drv_intc_reg_t)0x1C50u)
#define INTC_REG_GICD_ICFGR21           ((e_r_drv_intc_reg_t)0x1C54u)
#define INTC_REG_GICD_ICFGR22           ((e_r_drv_intc_reg_t)0x1C58u)
#define INTC_REG_GICD_ICFGR23           ((e_r_drv_intc_reg_t)0x1C5Cu)
#define INTC_REG_GICD_ICFGR24           ((e_r_drv_intc_reg_t)0x1C60u)
#define INTC_REG_GICD_ICFGR25           ((e_r_drv_intc_reg_t)0x1C64u)
#define INTC_REG_GICD_ICFGR26           ((e_r_drv_intc_reg_t)0x1C68u)
#define INTC_REG_GICD_ICFGR27           ((e_r_drv_intc_reg_t)0x1C6Cu)
#define INTC_REG_GICD_ICFGR28           ((e_r_drv_intc_reg_t)0x1C70u)
#define INTC_REG_GICD_ICFGR29           ((e_r_drv_intc_reg_t)0x1C74u)
#define INTC_REG_GICD_ICFGR30           ((e_r_drv_intc_reg_t)0x1C78u)
#define INTC_REG_GICD_ICFGR31           ((e_r_drv_intc_reg_t)0x1C7Cu)
#define INTC_REG_GICD_PPISR             ((e_r_drv_intc_reg_t)0x1D00u)
#define INTC_REG_GICD_SPISR0            ((e_r_drv_intc_reg_t)0x1D04u)
#define INTC_REG_GICD_SPISR1            ((e_r_drv_intc_reg_t)0x1D08u)
#define INTC_REG_GICD_SPISR2            ((e_r_drv_intc_reg_t)0x1D0Cu)
#define INTC_REG_GICD_SPISR3            ((e_r_drv_intc_reg_t)0x1D10u)
#define INTC_REG_GICD_SPISR4            ((e_r_drv_intc_reg_t)0x1D14u)
#define INTC_REG_GICD_SPISR5            ((e_r_drv_intc_reg_t)0x1D18u)
#define INTC_REG_GICD_SPISR6            ((e_r_drv_intc_reg_t)0x1D1Cu)
#define INTC_REG_GICD_SPISR7            ((e_r_drv_intc_reg_t)0x1D20u)
#define INTC_REG_GICD_SPISR8            ((e_r_drv_intc_reg_t)0x1D24u)
#define INTC_REG_GICD_SPISR9            ((e_r_drv_intc_reg_t)0x1D28u)
#define INTC_REG_GICD_SPISR10           ((e_r_drv_intc_reg_t)0x1D2Cu)
#define INTC_REG_GICD_SPISR11           ((e_r_drv_intc_reg_t)0x1D30u)
#define INTC_REG_GICD_SPISR12           ((e_r_drv_intc_reg_t)0x1D34u)
#define INTC_REG_GICD_SPISR13           ((e_r_drv_intc_reg_t)0x1D38u)
#define INTC_REG_GICD_SPISR14           ((e_r_drv_intc_reg_t)0x1D3Cu)
#define INTC_REG_GICD_SGIR              ((e_r_drv_intc_reg_t)0x1F00u)
#define INTC_REG_GICD_CPENDSGIR0        ((e_r_drv_intc_reg_t)0x1F10u)
#define INTC_REG_GICD_CPENDSGIR1        ((e_r_drv_intc_reg_t)0x1F14u)
#define INTC_REG_GICD_CPENDSGIR2        ((e_r_drv_intc_reg_t)0x1F18u)
#define INTC_REG_GICD_CPENDSGIR3        ((e_r_drv_intc_reg_t)0x1F1Cu)
#define INTC_REG_GICD_SPENDSGIR0        ((e_r_drv_intc_reg_t)0x1F20u)
#define INTC_REG_GICD_SPENDSGIR1        ((e_r_drv_intc_reg_t)0x1F24u)
#define INTC_REG_GICD_SPENDSGIR2        ((e_r_drv_intc_reg_t)0x1F28u)
#define INTC_REG_GICD_SPENDSGIR3        ((e_r_drv_intc_reg_t)0x1F2Cu)
#define INTC_REG_GICD_PIDR4             ((e_r_drv_intc_reg_t)0x1FD0u)
#define INTC_REG_GICD_PIDR5             ((e_r_drv_intc_reg_t)0x1FD4u)
#define INTC_REG_GICD_PIDR6             ((e_r_drv_intc_reg_t)0x1FD8u)
#define INTC_REG_GICD_PIDR7             ((e_r_drv_intc_reg_t)0x1FDCu)
#define INTC_REG_GICD_PIDR0             ((e_r_drv_intc_reg_t)0x1FE0u)
#define INTC_REG_GICD_PIDR1             ((e_r_drv_intc_reg_t)0x1FE4u)
#define INTC_REG_GICD_PIDR2             ((e_r_drv_intc_reg_t)0x1FE8u)
#define INTC_REG_GICD_PIDR3             ((e_r_drv_intc_reg_t)0x1FECu)
#define INTC_REG_GICD_CIDR0             ((e_r_drv_intc_reg_t)0x1FF0u)
#define INTC_REG_GICD_CIDR1             ((e_r_drv_intc_reg_t)0x1FF4u)
#define INTC_REG_GICD_CIDR2             ((e_r_drv_intc_reg_t)0x1FF8u)
#define INTC_REG_GICD_CIDR3             ((e_r_drv_intc_reg_t)0x1FFCu)
#define INTC_REG_GICC_CTLR              ((e_r_drv_intc_reg_t)0x2000u)
#define INTC_REG_GICC_PMR               ((e_r_drv_intc_reg_t)0x2004u)
#define INTC_REG_GICC_BPR               ((e_r_drv_intc_reg_t)0x2008u)
#define INTC_REG_GICC_IAR               ((e_r_drv_intc_reg_t)0x200Cu)
#define INTC_REG_GICC_EOIR              ((e_r_drv_intc_reg_t)0x2010u)
#define INTC_REG_GICC_RPR               ((e_r_drv_intc_reg_t)0x2014u)
#define INTC_REG_GICC_HPPIR             ((e_r_drv_intc_reg_t)0x2018u)
#define INTC_REG_GICC_ABPR              ((e_r_drv_intc_reg_t)0x201Cu)
#define INTC_REG_GICC_AIAR              ((e_r_drv_intc_reg_t)0x2020u)
#define INTC_REG_GICC_AEOIR             ((e_r_drv_intc_reg_t)0x2024u)
#define INTC_REG_GICC_AHPPIR            ((e_r_drv_intc_reg_t)0x2028u)
#define INTC_REG_GICC_APR0              ((e_r_drv_intc_reg_t)0x20D0u)
#define INTC_REG_GICC_NSAPR0            ((e_r_drv_intc_reg_t)0x20E0u)
#define INTC_REG_GICC_IIDR              ((e_r_drv_intc_reg_t)0x20FCu)
#define INTC_REG_GICC_DIR               ((e_r_drv_intc_reg_t)0x3000u)




/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum {
    INTC_SUCCESS = 0,                   /*!<  SUCCESS */
    INTC_ERR_INVALID = (-1),              /*!<  Error End  */
    INTC_ERR_INVALID_PRIORITY = (-2),     /*!<  Invalid interrupt priority level specified by argument */
    INTC_ERR_INVALID_ID = (-3),           /*!<  Invalid interrupt ID specified by argument */
    INTC_ERR_INVALID_NUM = (-4),          /*!<  Invalid TINT pin group number/IRQ pin number specified by argument */
    INTC_ERR_INVALID_SENSE = (-5),        /*!<  Invalid detection method specified by argument */
} e_r_drv_intc_err_t;


typedef enum {
    INTC_ENABLE,                   /*!<   Interrupt enabled */
    INTC_DISABLE,                  /*!<   Interrupt disabled */
} e_r_drv_intc_enable_t;

typedef enum {
    INTC_PENDING_NO,           /*!<  Not pending state */
    INTC_PENDING_YES,          /*!<  Pending state */
} e_r_drv_intc_pending_t;



typedef enum {
    IRQ_SENSE_LEVEL         = 0,   /*!< IRQ pin : Level detection  */
    IRQ_SENSE_FALLINGEDGE   = 1,   /*!< IRQ pin : Falling Edge detection  */
    IRQ_SENSE_RISINGEDGE    = 2,   /*!< IRQ pin : Rising Edge detection  */
    IRQ_SENSE_BOTHEDGE      = 3,   /*!< IRQ pin : Falling and Rising Edge detection */
}e_r_drv_irq_sense_t;



typedef enum {
    TINT_SENSE_LEVEL        = 0,  /*!< TINT pin : Level detection  */
    TINT_SENSE_RISINGEDGE   = 1,  /*!< TINT pin : Rising Edge detection  */
}e_r_drv_tint_sense_t;

typedef enum
{
    NMI_SENSE_FALLINGEDGE = 0,
    NMI_SENSE_RISINGEDGE = 1
} e_r_drv_nmi_edge_t;



typedef uint16_t    e_r_drv_intc_intid_t;
typedef uint8_t     e_r_drv_intc_priority_t;
typedef uint8_t     e_r_drv_irq_num_t;
typedef uint8_t     e_r_drv_tint_num_t;

typedef uint8_t     e_r_drv_irq_mask_t;
typedef uint16_t    e_r_drv_intc_reg_t;

typedef struct {
    e_r_drv_intc_priority_t mask;
} st_r_drv_gic_cfg_t;

typedef struct {
    e_r_drv_nmi_edge_t edge;
} st_r_drv_nmi_cfg_t;

typedef struct {
    uint16_t sense0:2;
    uint16_t sense1:2;
    uint16_t sense2:2;
    uint16_t sense3:2;
    uint16_t sense4:2;
    uint16_t sense5:2;
    uint16_t sense6:2;
    uint16_t sense7:2;
} st_r_drv_irq_cfg_t;

typedef struct {
    uint32_t sense0 :1;
    uint32_t sense1 :1;
    uint32_t sense2 :1;
    uint32_t sense3 :1;
    uint32_t sense4 :1;
    uint32_t sense5 :1;
    uint32_t sense6 :1;
    uint32_t sense7 :1;
    uint32_t sense8 :1;
    uint32_t sense9 :1;
    uint32_t sense10:1;
    uint32_t sense11:1;
    uint32_t sense12:1;
    uint32_t sense13:1;
    uint32_t sense14:1;
    uint32_t sense15:1;
    uint32_t sense16:1;
    uint32_t sense17:1;
    uint32_t sense18:1;
    uint32_t sense19:1;
    uint32_t sense20:1;
    uint32_t sense21:1;
    uint32_t sense22:1;
    uint32_t sense23:1;
    uint32_t sense24:1;
    uint32_t sense25:1;
    uint32_t sense26:1;
    uint32_t sense27:1;
    uint32_t sense28:1;
    uint32_t sense29:1;
    uint32_t sense30:1;
    uint32_t sense31:1;
} st_r_drv_tint_cfg_t;


/* interrupt config format */
typedef struct {
    e_r_drv_intc_enable_t    enable;       /**< INTC_ENABLE or INTC_DISABLE */
    e_r_drv_intc_priority_t  priority;      /**< 0(highest) - 31(lowest) */
    void *           function;      /**< User ISR */
} st_r_drv_interrupt_config_t;



/******************************************************************************
Variable Externs
******************************************************************************/
extern void (* g_intc_func_table[INTC_VID_TOTAL])(uint32_t int_sense);


/******************************************************************************
Functions Prototypes
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @fn         R_INTC_Init
 * @brief      Initialization of INTC (Arm GIC)
 *
 * @param      none
 *
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
extern e_r_drv_intc_err_t R_INTC_Init(void);

/**
 * @fn         R_INTC_SetNMIConfig
 * @brief      set NMI interrupt configuration
 *
 * @param[in]  p_nmi_config : NMI configuration parameter
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
extern e_r_drv_intc_err_t R_INTC_SetNMIConfig( const st_r_drv_nmi_cfg_t * p_nmi_config );

/**
 * @fn         R_INTC_GetNMIConfig
 * @brief      get NMI interrupt configuration
 *
 * @param[out] p_nmi_config : NMI configuration parameter
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
extern e_r_drv_intc_err_t R_INTC_GetNMIConfig( st_r_drv_nmi_cfg_t * p_nmi_config );

/**
 * @fn         R_IRQ_Init
 * @brief      Initialization of IRQ
 *
 * @param[in]  cfg : interrupt detection method of IRQ7-IRQ0 pin\n
 *                   cfg.sense7 - cfg.sense0\n
 *                     IRQ_SENSE_LEVEL(0)\n
 *                     IRQ_SENSE_FALLINGEDGE(1)\n
 *                     IRQ_SENSE_RISINGEDGE(2)\n
 *                     IRQ_SENSE_BOTHEDGE(3)
 * @retval     INTC_SUCCESS : SUCCESS
 */
extern e_r_drv_intc_err_t R_IRQ_Init( const st_r_drv_irq_cfg_t * p_irq_config );

/**
 * @fn         R_TINT_Init
 * @brief      Initialization of TINT
 *
 * @param[in]  cfg : interrupt detection method of TINT31-TINT0 pin\n
 *                   cfg.sense31 - cfg.sense0\n
 *                     TINT_SENSE_LEVEL        (0)\n
 *                     TINT_SENSE_RISINGEDGE   (1)
 * @retval     INTC_SUCCESS : SUCCESS
 */
extern e_r_drv_intc_err_t R_TINT_Init( const st_r_drv_tint_cfg_t * p_tint_config );

/**
 * @fn         R_INTC_RegistIntFunc
 * @brief      Registers the function specified by the func\n
 *             to the element specified by the int_id \n
 *             in the INTC interrupt handler function
 *
 * @param[in]  int_id : Interrupt ID (0 - 512)
 * @param[in]  void (*func)(uint32_t) : Function to be registered to INTC interrupt hander table
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID : Error End (Illegal parameters)
 */
extern e_r_drv_intc_err_t R_INTC_RegistIntFunc(e_r_drv_intc_intid_t int_id, void (* func)(uint32_t int_sense));

/**
 * @fn         R_INTC_GetIntFunc
 * @brief      Get address of interrupt function
 *
 * @param[in]  int_id : Interrupt ID (0 - 512)
 * @param[out] void (*func)(uint32_t) : Function to be registered to INTC interrupt hander table
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID : Error End (Illegal parameters)
 */
extern e_r_drv_intc_err_t R_INTC_GetIntFunc(e_r_drv_intc_intid_t int_id, void (* func)(uint32_t int_sense));

/**
 * @fn         R_INTC_Enable
 * @brief      Enables interrupt
 *
 * @param[in]  int_id : Interrupt ID (0 - 511)
 *
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_ID : Invalid interrupt ID specified by argument
 */
extern e_r_drv_intc_err_t R_INTC_Enable( e_r_drv_intc_intid_t int_id );

/**
 * @fn         R_INTC_Disable
 * @brief      Disables interrupt
 *
 * @param[in]  int_id : Interrupt ID (0 - 511)
 *
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_ID : Invalid interrupt ID specified by argument
 */
extern e_r_drv_intc_err_t R_INTC_Disable( e_r_drv_intc_intid_t int_id );

/**
 * @fn         R_INTC_SetPriority
 * @brief      Sets the interrupt priority level
 *
 * @param[in]  int_id : Interrupt ID (0 - 511)
 * @param[in]  priority : Interrupt priority level (0 - 31)
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_ID : Invalid interrupt ID specified by argument
 * @retval     INTC_ERR_INVALID_PRIORITY : Invalid interrupt priority level specified by argument
 */
extern e_r_drv_intc_err_t R_INTC_SetPriority( e_r_drv_intc_intid_t int_id, e_r_drv_intc_priority_t priority );

/**
 * @fn         R_INTC_GetPriority
 * @brief      Get the interrupt priority level
 *
 * @param[in]  int_id : Interrupt ID (0 - 511)
 * @param[out] *priority : Interrupt priority level (0 - 31)
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID : Error End (The pointer is NULL.)
 * @retval     INTC_ERR_INVALID_ID : Invalid interrupt ID specified by argument
 */
extern e_r_drv_intc_err_t R_INTC_GetPriority( e_r_drv_intc_intid_t int_id, e_r_drv_intc_priority_t *priority );

/**
 * @fn         R_INTC_SetMaskLevel
 * @brief      Sets the interrupt mask level
 *
 * @param[in]  mask_level : Interrupt mask level (0 - 31)
 *
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_PRIORITY : Invalid interrupt ID specified by argument
 */
extern e_r_drv_intc_err_t R_INTC_SetMaskLevel( e_r_drv_intc_priority_t mask_level );

/**
 * @fn         R_INTC_GetMaskLevel
 * @brief      Get the Interrupt mask level
 *
 * @param[out] *mask_level : Interrupt mask level (0 - 31)
 *
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
extern e_r_drv_intc_err_t R_INTC_GetMaskLevel( e_r_drv_intc_priority_t *mask_level );

/**
 * @fn         R_IRQ_SetSense
 * @brief      Set IRQ pin interrupt detection method (Edge / level)
 *
 * @param[in]  num : IRQ pin Number  (INTC_IRQ0 - INTC_IRQ7)
 * @param[in]  sense : interrupt detection method of IRQ pin (IRQ0 - IRQ7)\n
 *                     IRQ_SENSE_LEVEL(0)\n
 *                     IRQ_SENSE_FALLINGEDGE(1) \n
 *                     IRQ_SENSE_RISINGEDGE(2)\n
 *                     IRQ_SENSE_BOTHEDGE(3)
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_SENSE : Invalid detection method specified by argument
 * @retval     INTC_ERR_INVALID_NUM : Invalid IRQ pin number specified by argument
 */
extern e_r_drv_intc_err_t R_IRQ_SetSense( e_r_drv_irq_num_t num, e_r_drv_irq_sense_t sense);

/**
 * @fn         R_IRQ_GetSense
 * @brief      Get IRQ pin interrupt detection method (Edge / level)
 *
 * @param[in]  num : IRQ pin Number  (INTC_IRQ0 - INTC_IRQ7)
 * @param[out] *sense : interrupt detection method of IRQ pin (IRQ0 - IRQ7)\n
 *                      IRQ_SENSE_LEVEL(0)\n
 *                      IRQ_SENSE_FALLINGEDGE(1) \n
 *                      IRQ_SENSE_RISINGEDGE(2)\n
 *                      IRQ_SENSE_BOTHEDGE(3)
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_NUM : Invalid IRQ pin number specified by argument
 */
extern e_r_drv_intc_err_t R_IRQ_GetSense( e_r_drv_irq_num_t num, e_r_drv_irq_sense_t *sense);

/**
 * @fn         R_TINT_SetSense
 * @brief      Set detection method of pin interrupt (TINT 31 - TINT0)
 *
 * @param[in]  num : TINT pin  Group Number (INTC_TINT0 - INTC_TINT31)
 * @param[in]  sense : interrupt detection method of TINT pin\n
 *                     TINT_SENSE_LEVEL(0)\n
 *                     TINT_SENSE_RISINGEDGE(1)
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_SENSE : Invalid detection method specified by argument
 * @retval     INTC_ERR_INVALID_NUM : Invalid TINT pin number specified by argument
 */
extern e_r_drv_intc_err_t R_TINT_SetSense( e_r_drv_tint_num_t num, e_r_drv_tint_sense_t sense);

/**
 * @fn         R_TINT_GetSense
 * @brief      Get detection method of pin interrupt (TINT 31 - TINT0)
 *
 * @param[in]  num : TINT pin  Group Number (INTC_TINT0 - INTC_TINT31)
 * @param[out] *sense : interrupt detection method of TINT pin\n
 *                      TINT_SENSE_LEVEL(0)\n
 *                      TINT_SENSE_RISINGEDGE(1)
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_NUM : Invalid TINT pin number specified by argument
 */
extern e_r_drv_intc_err_t R_TINT_GetSense( e_r_drv_tint_num_t num, e_r_drv_tint_sense_t *sense);

/**
 * @fn         R_INTC_GetPendingStatus
 * @brief      Get pending state of interrupt
 *
 * @param[in]  int_id : Interrupt ID (0 - 511)
 * @param[out] *pending_status : Pending status\n
 *                               INTC_PENDING_YES (1)\n
 *                               INTC_PENDING_NO (0)
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_ID : Invalid interrupt ID specified by argument
 */
extern e_r_drv_intc_err_t R_INTC_GetPendingStatus( e_r_drv_intc_intid_t int_id, e_r_drv_intc_pending_t *pending_status);

/**
 * @fn         R_INTC_SetIrqMask
 * @brief      Set IRQRR.IRQMSK
 *
 * @param[in]  mask :  Setting value for IRQRR.IRQMSK\n
 *                     1 :  INTC.IRQRR.IRQMSK = 1\n
 *                     0 :  INTC.IRQRR.IRQMSK = 0\n
 *                     1 :  IRQ signal can be used as software standby cancle source\n
 *                     0 :  IRQ signal can not be used as  software  standby cancle source
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID : Error End  (Illegal parameters : Invalid IRQ mask setting value of argument)
 */
extern e_r_drv_intc_err_t R_INTC_SetIrqMask(e_r_drv_irq_mask_t mask);

/**
 * @fn         R_INTC_GetIrqMask
 * @brief      Get IRQRR.IRQMSK
 *
 * @param[out] *mask : Value of IRQRR.IRQMSK
 *
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
extern e_r_drv_intc_err_t R_INTC_GetIrqMask(e_r_drv_irq_mask_t *mask);

/**
 * @fn         R_INTC_GetVersion
 * @brief      Get driver version number of INTC Driver (LLD)
 *
 * @param      none
 *
 * @retval     DRV_SUCCESS : Succeed (always)
 */
extern int_t R_INTC_GetVersion(st_ver_info_t *pVerInfo);

/* ==== User function ==== */
/**
 * @fn         Userdef_INTC_Pre_Interrupt
 * @brief      This function is the user-defined function called by the
 *             INTC driver. Write the processing to be executed
 *             before registered interrupt function is called
 *
 * @param      int_id: interrupt ID
 *
 * @retval     INTC_SUCCESS : must return this value in current version
 */
extern e_r_drv_intc_err_t Userdef_INTC_Pre_Interrupt(e_r_drv_intc_intid_t int_id);

/**
 * @fn         Userdef_INTC_Post_Interrupt
 * @brief      This function is the user-defined function called by the
 *             INTC driver. Write the processing to be executed
 *             after registered interrupt function is called
 *
 * @param      int_id: interrupt ID
 *
 * @retval     INTC_SUCCESS : must return this value in current version
 */
extern e_r_drv_intc_err_t Userdef_INTC_Post_Interrupt(e_r_drv_intc_intid_t int_id);

/**
 * @fn         Userdef_INTC_UndefId
 * @brief      This function is the user-defined function called by the
 *             INTC driver. Write the processing to be executed
 *             when an unsupported interrupt ID is received.
 *
 * @param      int_id: interrupt ID
 *
 * @retval     INTC_SUCCESS : must return this value in current version
 */

extern e_r_drv_intc_err_t Userdef_INTC_UndefId(e_r_drv_intc_intid_t int_id);

/**
 * @fn         Userdef_INTC_UnregisteredID
 * @brief      This function is the user-defined function called by the
 *             INTC driver. Write the processing to be executed
 *             when interrupt ID who never registered by R_INTC_RegistIntFunc
 *             is received
 *
 * @param      int_id: interrupt ID
 *
 * @retval     INTC_SUCCESS : must return this value in current version
 */
extern e_r_drv_intc_err_t Userdef_INTC_UnregisteredID(e_r_drv_intc_intid_t int_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* R_INTC_LLD_RZA2M_H_ */

/* End of File */
