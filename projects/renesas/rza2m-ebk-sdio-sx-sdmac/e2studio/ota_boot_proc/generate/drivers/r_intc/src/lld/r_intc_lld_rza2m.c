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
* File Name   : r_intc_lld_rza2m_reg.c
* Description : INTC Low layer driver
*******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_intc_drv_link.h"
#include "compiler_settings.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "rza_io_regrw.h"
#include "r_intc_rza2_config.h"
#include "r_intc_lld_rza2m.h"


/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/
/* ==== Total number of registers ==== */
#define INTC_PRV_GICD_IGROUPR_REG_TOTAL    ((uint16_t)(INTC_GIC_ID_TOTAL + 31) / 32) /* GICD_IGROUPR    */
#define INTC_PRV_GICD_ICFGR_REG_TOTAL      ((uint16_t)(INTC_GIC_ID_TOTAL + 15) / 16) /* GICD_ICFGR      */
#define INTC_PRV_GICD_IPRIORITYR_REG_TOTAL ((uint16_t)(INTC_GIC_ID_TOTAL +  3) /  4) /* GICD_IPRIORITYR */
#define INTC_PRV_GICD_ITARGETSR_REG_TOTAL  ((uint16_t)(INTC_GIC_ID_TOTAL +  3) /  4) /* GICD_ITARGETSR  */
#define INTC_PRV_GICD_ISENABLER_REG_TOTAL  ((uint16_t)(INTC_GIC_ID_TOTAL + 31) / 32) /* GICD_ISENABLER  */
#define INTC_PRV_GICD_ICENABLER_REG_TOTAL  ((uint16_t)(INTC_GIC_ID_TOTAL + 31) / 32) /* GICD_ICENABLER  */

/* register base address */
#define INTC_PRV_GIC_BASE_ADDR   (0xE8220000uL)
#define INTC_PRV_INTC_BASE_ADDR  (0xFCFEF800uL)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/


/******************************************************************************
Private global variables and functions
******************************************************************************/
/*! Version Information */
static const st_drv_info_t gs_lld_info =
{
    {
        ((STDIO_INTC_RZ_LLD_VERSION_MAJOR << 16) + STDIO_INTC_RZ_LLD_VERSION_MINOR)
    },
    STDIO_INTC_RZ_LLD_BUILD_NUM,
    STDIO_INTC_RZ_LLD_DRV_NAME
};

/* ==== Global variable ==== */
static const uint32_t gs_gicd_icfgrn_table[] =     /* Initial value table of Interrupt Configuration Registers */
{
		                   /*           Interrupt ID */
    0xAAAAAAAAuL,          /* GICD_ICFGR0  :  15 to   0 */
    0x55540000uL,          /* GICD_ICFGR1  :  19 to  16 */
    0xFD555555uL,          /* GICD_ICFGR2  :  47 to  32 */
    0x7FFFFFFFuL,          /* GICD_ICFGR3  :  63 to  48 */
    0x55555555uL,          /* GICD_ICFGR4  :  79 to  64 */
    0xD57F5555uL,          /* GICD_ICFGR5  :  95 to  80 */
    0xFFFFFFFFuL,          /* GICD_ICFGR6  : 111 to  96 */
    0xFFFFFFFFuL,          /* GICD_ICFGR7  : 127 to 112 */
    0xFFFFFFFFuL,          /* GICD_ICFGR8  : 143 to 128 */
    0xFFFFFFFFuL,          /* GICD_ICFGR9  : 159 to 144 */
    0xFFFFFFFFuL,          /* GICD_ICFGR10 : 175 to 160 */
    0xFFFFFFFFuL,          /* GICD_ICFGR11 : 191 to 176 */
    0xFFFFFFFFuL,          /* GICD_ICFGR12 : 207 to 192 */
    0xFFFFFFFFuL,          /* GICD_ICFGR13 : 223 to 208 */
    0xFFFFFFFFuL,          /* GICD_ICFGR14 : 239 to 224 */
    0x7D5FD57FuL,          /* GICD_ICFGR15 : 255 to 240 */
    0x557D7DDFuL,          /* GICD_ICFGR16 : 271 to 256 */
    0x557D557DuL,          /* GICD_ICFGR17 : 287 to 272 */
    0x5555557DuL,          /* GICD_ICFGR18 : 303 to 288 */
    0x55555555uL,          /* GICD_ICFGR19 : 319 to 304 */
    0xF5555555uL,          /* GICD_ICFGR20 : 335 to 320 */
    0x5555FFFFuL,          /* GICD_ICFGR21 : 351 to 336 */
    0xF5555555uL,          /* GICD_ICFGR22 : 367 to 352 */
    0xFFDD55F5uL,          /* GICD_ICFGR23 : 383 to 368 */
    0xFFFFFFFFuL,          /* GICD_ICFGR24 : 399 to 384 */
    0xFFFFFFFFuL,          /* GICD_ICFGR25 : 415 to 400 */
    0xFFFFFFFFuL,          /* GICD_ICFGR26 : 431 to 416 */
    0x5FFFFFDFuL,          /* GICD_ICFGR27 : 447 to 432 */
    0x55555555uL,          /* GICD_ICFGR28 : 463 to 448 */
    0x55555555uL,          /* GICD_ICFGR29 : 479 to 464 */
    0x55555555uL,          /* GICD_ICFGR30 : 495 to 480 */
    0x55555555uL,          /* GICD_ICFGR31 : 511 to 496 */
};

static e_r_drv_intc_err_t intc_id_check( e_r_drv_intc_intid_t int_id );
static e_r_drv_intc_err_t intc_priority_check( e_r_drv_intc_priority_t priority );
static uint8_t intc_priority_to_reg( e_r_drv_intc_priority_t priority );
static e_r_drv_intc_priority_t intc_reg_to_priority( uint8_t reg_value );

/* Interrupt Vector Table (Virtual:includes NMI) */
void (* g_intc_func_table[INTC_VID_TOTAL])(uint32_t int_sense) =
{
    NULL,   /* 0   : SW0               */
    NULL,   /* 1   : SW1               */
    NULL,   /* 2   : SW2               */
    NULL,   /* 3   : SW3               */
    NULL,   /* 4   : SW4               */
    NULL,   /* 5   : SW5               */
    NULL,   /* 6   : SW6               */
    NULL,   /* 7   : SW7               */
    NULL,   /* 8   : SW8               */
    NULL,   /* 9   : SW9               */
    NULL,   /* 10  : SW10              */
    NULL,   /* 11  : SW11              */
    NULL,   /* 12  : SW12              */
    NULL,   /* 13  : SW13              */
    NULL,   /* 14  : SW14              */
    NULL,   /* 15  : SW15              */
    NULL,   /* 16  : INT16             */
    NULL,   /* 17  : INT17             */
    NULL,   /* 18  : INT18             */
    NULL,   /* 19  : INT19             */
    NULL,   /* 20  : INT20             */
    NULL,   /* 21  : INT21             */
    NULL,   /* 22  : INT22             */
    NULL,   /* 23  : INT23             */
    NULL,   /* 24  : INT24             */
    NULL,   /* 25  : INT25             */
    NULL,   /* 26  : INT26             */
    NULL,   /* 27  : INT27             */
    NULL,   /* 28  : INT28             */
    NULL,   /* 29  : INT29             */
    NULL,   /* 30  : INT30             */
    NULL,   /* 31  : INT31             */
    NULL,   /* 32  : PMUIRQ0           */
    NULL,   /* 33  : COMMRX            */
    NULL,   /* 34  : COMMTX            */
    NULL,   /* 35  : CTIIRQ            */
    NULL,   /* 36  : IRQ0              */
    NULL,   /* 37  : IRQ1              */
    NULL,   /* 38  : IRQ2              */
    NULL,   /* 39  : IRQ3              */
    NULL,   /* 40  : IRQ4              */
    NULL,   /* 41  : IRQ5              */
    NULL,   /* 42  : IRQ6              */
    NULL,   /* 43  : IRQ7              */
    NULL,   /* 44  : PL310ERR          */
    NULL,   /* 45  : DMAINT0           */
    NULL,   /* 46  : DMAINT1           */
    NULL,   /* 47  : DMAINT2           */
    NULL,   /* 48  : DMAINT3           */
    NULL,   /* 49  : DMAINT4           */
    NULL,   /* 50  : DMAINT5           */
    NULL,   /* 51  : DMAINT6           */
    NULL,   /* 52  : DMAINT7           */
    NULL,   /* 53  : DMAINT8           */
    NULL,   /* 54  : DMAINT9           */
    NULL,   /* 55  : DMAINT10          */
    NULL,   /* 56  : DMAINT11          */
    NULL,   /* 57  : DMAINT12          */
    NULL,   /* 58  : DMAINT13          */
    NULL,   /* 59  : DMAINT14          */
    NULL,   /* 60  : DMAINT15          */
    NULL,   /* 61  : DMAERR0           */
    NULL,   /* 62  : DMAERR1           */
    NULL,   /* 63  : USB0HI0           */
    NULL,   /* 64  : USB0FI0           */
    NULL,   /* 65  : USB0FDMA00        */
    NULL,   /* 66  : USB0FDMA10        */
    NULL,   /* 67  : USB0FDMAERR0      */
    NULL,   /* 68  : USB1HI0           */
    NULL,   /* 69  : USB1FI0           */
    NULL,   /* 70  : USB1FDMA00        */
    NULL,   /* 71  : USB1FDMA10        */
    NULL,   /* 72  : USB1FDMAERR0      */
    NULL,   /* 73  : S0_VI_VSYNC0      */
    NULL,   /* 74  : S0_LO_VSYNC0      */
    NULL,   /* 75  : S0_VSYNCERR0      */
    NULL,   /* 76  : GR3_VLINE0        */
    NULL,   /* 77  : SO_VFIELD0        */
    NULL,   /* 78  : IV1_VBUFERR0      */
    NULL,   /* 79  : IV3_VBUFERR0      */
    NULL,   /* 80  : IV5_VBUFERR0      */
    NULL,   /* 81  : IV6_VBUFERR0      */
    NULL,   /* 82  : S0_WLINE0         */
    NULL,   /* 83  : IMR2I0            */
    NULL,   /* 84  : JEDI              */
    NULL,   /* 85  : JDTI              */
    NULL,   /* 86  : DRWI              */
    NULL,   /* 87  : CSII              */
    NULL,   /* 88  : OSTMI0            */
    NULL,   /* 89  : OSTMI1            */
    NULL,   /* 90  : OSTMI2            */
    NULL,   /* 91  : CMI               */
    NULL,   /* 92  : WTOUT             */
    NULL,   /* 93  : ITI               */
    NULL,   /* 94  : CA9PEI            */
    NULL,   /* 95  : TGIA0             */
    NULL,   /* 96  : TGIB0             */
    NULL,   /* 97  : TGIC0             */
    NULL,   /* 98  : TGID0             */
    NULL,   /* 99  : TCIV0             */
    NULL,   /* 100 : TGIE0             */
    NULL,   /* 101 : TGIF0             */
    NULL,   /* 102 : TGIA1             */
    NULL,   /* 103 : TGIB1             */
    NULL,   /* 104 : TCIV1             */
    NULL,   /* 105 : TCIU1             */
    NULL,   /* 106 : TGIA2             */
    NULL,   /* 107 : TGIB2             */
    NULL,   /* 108 : TCIV2             */
    NULL,   /* 109 : TCIU2             */
    NULL,   /* 110 : TGIA3             */
    NULL,   /* 111 : TGIB3             */
    NULL,   /* 112 : TGIC3             */
    NULL,   /* 113 : TGID3             */
    NULL,   /* 114 : TCIV3             */
    NULL,   /* 115 : TGIA4             */
    NULL,   /* 116 : TGIB4             */
    NULL,   /* 117 : TGIC4             */
    NULL,   /* 118 : TGID4             */
    NULL,   /* 119 : TCIV4             */
    NULL,   /* 120 : TGIU5             */
    NULL,   /* 121 : TGIV5             */
    NULL,   /* 122 : TGIW5             */
    NULL,   /* 123 : TGIA6             */
    NULL,   /* 124 : TGIB6             */
    NULL,   /* 125 : TGIC6             */
    NULL,   /* 126 : TGID6             */
    NULL,   /* 127 : TCIV6             */
    NULL,   /* 128 : TGIA7             */
    NULL,   /* 129 : TGIB7             */
    NULL,   /* 130 : TGIC7             */
    NULL,   /* 131 : TGID7             */
    NULL,   /* 132 : TCIV7             */
    NULL,   /* 133 : TGIA8             */
    NULL,   /* 134 : TGIB8             */
    NULL,   /* 135 : TGIC8             */
    NULL,   /* 136 : TGID8             */
    NULL,   /* 137 : TCIV8             */
    NULL,   /* 138 : INT138            */
    NULL,   /* 139 : CCMPA0            */
    NULL,   /* 140 : CCMPB0            */
    NULL,   /* 141 : CMPC0             */
    NULL,   /* 142 : CMPD0             */
    NULL,   /* 143 : INT143            */
    NULL,   /* 144 : INT144            */
    NULL,   /* 145 : INT145            */
    NULL,   /* 146 : CMPE0             */
    NULL,   /* 147 : CMPF0             */
    NULL,   /* 148 : ADTRGA0           */
    NULL,   /* 149 : ADTRGB0           */
    NULL,   /* 150 : OVF0              */
    NULL,   /* 151 : UNF0              */
    NULL,   /* 152 : CCMPA1            */
    NULL,   /* 153 : CCMPB1            */
    NULL,   /* 154 : CMPC1             */
    NULL,   /* 155 : CMPD1             */
    NULL,   /* 156 : INT156            */
    NULL,   /* 157 : INT157            */
    NULL,   /* 158 : INT158            */
    NULL,   /* 159 : CMPE1             */
    NULL,   /* 160 : CMPF1             */
    NULL,   /* 161 : ADTRGA1           */
    NULL,   /* 162 : ADTRGB1           */
    NULL,   /* 163 : OVF1              */
    NULL,   /* 164 : UNF1              */
    NULL,   /* 165 : CCMPA2            */
    NULL,   /* 166 : CCMPB2            */
    NULL,   /* 167 : CMPC2             */
    NULL,   /* 168 : CMPD2             */
    NULL,   /* 169 : INT169            */
    NULL,   /* 170 : INT170            */
    NULL,   /* 171 : INT171            */
    NULL,   /* 172 : CMPE2             */
    NULL,   /* 173 : CMPF2             */
    NULL,   /* 174 : ADTRGA2           */
    NULL,   /* 175 : ADTRGB2           */
    NULL,   /* 176 : OVF2              */
    NULL,   /* 177 : UNF2              */
    NULL,   /* 178 : CCMPA3            */
    NULL,   /* 179 : CCMPB3            */
    NULL,   /* 180 : CMPC3             */
    NULL,   /* 181 : CMPD3             */
    NULL,   /* 182 : INT182            */
    NULL,   /* 183 : INT183            */
    NULL,   /* 184 : INT184            */
    NULL,   /* 185 : CMPE3             */
    NULL,   /* 186 : CMPF3             */
    NULL,   /* 187 : ADTRGA3           */
    NULL,   /* 188 : ADTRGB3           */
    NULL,   /* 189 : OVF3              */
    NULL,   /* 190 : UNF3              */
    NULL,   /* 191 : CCMPA4            */
    NULL,   /* 192 : CCMPB4            */
    NULL,   /* 193 : CMPC4             */
    NULL,   /* 194 : CMPD4             */
    NULL,   /* 195 : INT195            */
    NULL,   /* 196 : INT196            */
    NULL,   /* 197 : INT197            */
    NULL,   /* 198 : CMPE4             */
    NULL,   /* 199 : CMPF4             */
    NULL,   /* 200 : ADTRGA4           */
    NULL,   /* 201 : ADTRGB4           */
    NULL,   /* 202 : OVF4              */
    NULL,   /* 203 : UNF4              */
    NULL,   /* 204 : CCMPA5            */
    NULL,   /* 205 : CCMPB5            */
    NULL,   /* 206 : CMPC5             */
    NULL,   /* 207 : CMPD5             */
    NULL,   /* 208 : INT208            */
    NULL,   /* 209 : INT209            */
    NULL,   /* 210 : INT210            */
    NULL,   /* 211 : CMPE5             */
    NULL,   /* 212 : CMPF5             */
    NULL,   /* 213 : ADTRGA5           */
    NULL,   /* 214 : ADTRGB5           */
    NULL,   /* 215 : OVF5              */
    NULL,   /* 216 : UNF5              */
    NULL,   /* 217 : CCMPA6            */
    NULL,   /* 218 : CCMPB6            */
    NULL,   /* 219 : CMPC6             */
    NULL,   /* 220 : CMPD6             */
    NULL,   /* 221 : INT221            */
    NULL,   /* 222 : INT222            */
    NULL,   /* 223 : INT223            */
    NULL,   /* 224 : CMPE6             */
    NULL,   /* 225 : CMPF6             */
    NULL,   /* 226 : ADTRGA6           */
    NULL,   /* 227 : ADTRGB6           */
    NULL,   /* 228 : OVF6              */
    NULL,   /* 229 : UNF6              */
    NULL,   /* 230 : CCMPA7            */
    NULL,   /* 231 : CCMPB7            */
    NULL,   /* 232 : CMPC7             */
    NULL,   /* 233 : CMPD7             */
    NULL,   /* 234 : INT234            */
    NULL,   /* 235 : INT235            */
    NULL,   /* 236 : INT236            */
    NULL,   /* 237 : CMPE7             */
    NULL,   /* 238 : CMPF7             */
    NULL,   /* 239 : ADTRGA7           */
    NULL,   /* 240 : ADTRGB7           */
    NULL,   /* 241 : OVF7              */
    NULL,   /* 242 : UNF7              */
    NULL,   /* 243 : OEI1              */
    NULL,   /* 244 : OEI2              */
    NULL,   /* 245 : OEI3              */
    NULL,   /* 246 : OEI4              */
    NULL,   /* 247 : S12ADI0           */
    NULL,   /* 248 : S12GBADI0         */
    NULL,   /* 249 : S12GCADI0         */
    NULL,   /* 250 : S12ADCMPAI0       */
    NULL,   /* 251 : S12ADCMPBI0       */
    NULL,   /* 252 : SSIF_INT_REQ_0    */
    NULL,   /* 253 : SSIF_DMA_RX_0     */
    NULL,   /* 254 : SSIF_DMA_TX_0     */
    NULL,   /* 255 : SSIF_INT_REQ_1    */
    NULL,   /* 256 : SSIF_DMA_RX_1     */
    NULL,   /* 257 : SSIF_DMA_TX_1     */
    NULL,   /* 258 : SSIF_INT_REQ_2    */
    NULL,   /* 259 : SSIF_DMA_RT_2     */
    NULL,   /* 260 : SSIF_INT_REQ_3    */
    NULL,   /* 261 : SSIF_DMA_RX_3     */
    NULL,   /* 262 : SSIF_DMA_TX_3     */
    NULL,   /* 263 : SPDIDI            */
    NULL,   /* 264 : RIICTEI0          */
    NULL,   /* 265 : RIICRI0           */
    NULL,   /* 266 : RIICTI0           */
    NULL,   /* 267 : RIICSPI0          */
    NULL,   /* 268 : RIICSTI0          */
    NULL,   /* 269 : RIICNAKI0         */
    NULL,   /* 270 : RIICALI0          */
    NULL,   /* 271 : RIICTMOI0         */
    NULL,   /* 272 : RIICTEI1          */
    NULL,   /* 273 : RIICRI1           */
    NULL,   /* 274 : RIICTI1           */
    NULL,   /* 275 : RIICSPI1          */
    NULL,   /* 276 : RIICSTI1          */
    NULL,   /* 277 : RIICNAKI1         */
    NULL,   /* 278 : RIICALI1          */
    NULL,   /* 279 : RIICTMOI1         */
    NULL,   /* 280 : RIICTEI2          */
    NULL,   /* 281 : RIICRI2           */
    NULL,   /* 282 : RIICTI2           */
    NULL,   /* 283 : RIICSPI2          */
    NULL,   /* 284 : RIICSTI2          */
    NULL,   /* 285 : RIICNAKI2         */
    NULL,   /* 286 : RIICALI2          */
    NULL,   /* 287 : RIICTMOI2         */
    NULL,   /* 288 : RIICTEI3          */
    NULL,   /* 289 : RIICRI3           */
    NULL,   /* 290 : RIICTI3           */
    NULL,   /* 291 : RIICSPI3          */
    NULL,   /* 292 : RIICSTI3          */
    NULL,   /* 293 : RIICNAKI3         */
    NULL,   /* 294 : RIICALI3          */
    NULL,   /* 295 : RIICTMOI3         */
    NULL,   /* 296 : INT296            */
    NULL,   /* 297 : ERI0              */
    NULL,   /* 298 : RXI0              */
    NULL,   /* 299 : TXI0              */
    NULL,   /* 300 : TEI0              */
    NULL,   /* 301 : INT301            */
    NULL,   /* 302 : INT302            */
    NULL,   /* 303 : ERI1              */
    NULL,   /* 304 : RXI1              */
    NULL,   /* 305 : TXI1              */
    NULL,   /* 306 : TEI1              */
    NULL,   /* 307 : INT307            */
    NULL,   /* 308 : INT308            */
    NULL,   /* 309 : ERI2              */
    NULL,   /* 310 : RXI2              */
    NULL,   /* 311 : TXI2              */
    NULL,   /* 312 : TEI2              */
    NULL,   /* 313 : INT313            */
    NULL,   /* 314 : INT314            */
    NULL,   /* 315 : ERI3              */
    NULL,   /* 316 : RXI3              */
    NULL,   /* 317 : TXI3              */
    NULL,   /* 318 : TEI3              */
    NULL,   /* 319 : INT319            */
    NULL,   /* 320 : INT320            */
    NULL,   /* 321 : ERI4              */
    NULL,   /* 322 : RXI4              */
    NULL,   /* 323 : TXI4              */
    NULL,   /* 324 : TEI4              */
    NULL,   /* 325 : INT325            */
    NULL,   /* 326 : GERI              */
    NULL,   /* 327 : RFI               */
    NULL,   /* 328 : CFRXI0            */
    NULL,   /* 329 : CERI0             */
    NULL,   /* 330 : CTXI0             */
    NULL,   /* 331 : CFRXI1            */
    NULL,   /* 332 : CERI1             */
    NULL,   /* 333 : CTXI1             */
    NULL,   /* 334 : DMA31INT0         */
    NULL,   /* 335 : DMA31INT1         */
    NULL,   /* 336 : DMA31INT2         */
    NULL,   /* 337 : DMA31INT3         */
    NULL,   /* 338 : DMA31INT4         */
    NULL,   /* 339 : DMA31INT5         */
    NULL,   /* 340 : DMA31INT6         */
    NULL,   /* 341 : DMA31INT7         */
    NULL,   /* 342 : DMA31INT8         */
    NULL,   /* 343 : DMA31INT9         */
    NULL,   /* 344 : SPEI0             */
    NULL,   /* 345 : SPRI0             */
    NULL,   /* 346 : SPTI0             */
    NULL,   /* 347 : SPEI1             */
    NULL,   /* 348 : SPRI1             */
    NULL,   /* 349 : SPTI1             */
    NULL,   /* 350 : SPEI2             */
    NULL,   /* 351 : SPRI2             */
    NULL,   /* 352 : SPTI2             */
    NULL,   /* 353 : NAND              */
    NULL,   /* 354 : SDHI0_0           */
    NULL,   /* 355 : INT355            */
    NULL,   /* 356 : SDHI1_0           */
    NULL,   /* 357 : INT357            */
    NULL,   /* 358 : NET_HYPER_IENON   */
    NULL,   /* 359 : ARM               */
    NULL,   /* 360 : PRD               */
    NULL,   /* 361 : CUP               */
    NULL,   /* 362 : ARM_S             */
    NULL,   /* 363 : PRD_S             */
    NULL,   /* 364 : CUP_S             */
    NULL,   /* 365 : SCIM_ERI0         */
    NULL,   /* 366 : SCIM_RXI0         */
    NULL,   /* 367 : SCIM_TXI0         */
    NULL,   /* 368 : SCIM_TEI0         */
    NULL,   /* 369 : SCIM_ERI1         */
    NULL,   /* 370 : SCIM_RXI1         */
    NULL,   /* 371 : SCIM_TXI1         */
    NULL,   /* 372 : SCIM_TEI1         */
    NULL,   /* 373 : EINT0             */
    NULL,   /* 374 : EINT1             */
    NULL,   /* 375 : PINT              */
    NULL,   /* 376 : MINT              */
    NULL,   /* 377 : IPLS              */
    NULL,   /* 378 : CEUI              */
    NULL,   /* 379 : H2USB00_ERRINT    */
    NULL,   /* 380 : H2SUB01_ERRINT    */
    NULL,   /* 381 : H2SUB10_ERRINT    */
    NULL,   /* 382 : H2SUB11_ERRINT    */
    NULL,   /* 383 : H2ETH_ERRINT      */
    NULL,   /* 384 : X2PERI12_ERRINT   */
    NULL,   /* 385 : X2PERI34_ERRINT   */
    NULL,   /* 386 : X2PERI5_ERRINT    */
    NULL,   /* 387 : X2PERI67_ERRINT   */
    NULL,   /* 388 : H2IC4_ERRINT      */
    NULL,   /* 389 : X2DBGR_ERRINT     */
    NULL,   /* 390 : DMA32INT10        */
    NULL,   /* 391 : DMA32INT11        */
    NULL,   /* 392 : DMA32INT12        */
    NULL,   /* 393 : DMA32INT13        */
    NULL,   /* 394 : DMA32INT14        */
    NULL,   /* 395 : DMA32INT15        */
    NULL,   /* 396 : H2XDAV0_ERRINT    */
    NULL,   /* 397 : H2XDAV1_ERRINT    */
    NULL,   /* 398 : INT398            */
    NULL,   /* 399 : INT399            */
    NULL,   /* 400 : INT400            */
    NULL,   /* 401 : INT401            */
    NULL,   /* 402 : INT402            */
    NULL,   /* 403 : INT403            */
    NULL,   /* 404 : INT404            */
    NULL,   /* 405 : INT405            */
    NULL,   /* 406 : INT406            */
    NULL,   /* 407 : INT407            */
    NULL,   /* 408 : INT408            */
    NULL,   /* 409 : INT409            */
    NULL,   /* 410 : INT410            */
    NULL,   /* 411 : INT411            */
    NULL,   /* 412 : INT412            */
    NULL,   /* 413 : INT413            */
    NULL,   /* 414 : INT414            */
    NULL,   /* 415 : INT415            */
    NULL,   /* 416 : INT416            */
    NULL,   /* 417 : INT417            */
    NULL,   /* 418 : INT418            */
    NULL,   /* 419 : INT419            */
    NULL,   /* 420 : INT420            */
    NULL,   /* 421 : INT421            */
    NULL,   /* 422 : INT422            */
    NULL,   /* 423 : INT423            */
    NULL,   /* 424 : INT424            */
    NULL,   /* 425 : INT425            */
    NULL,   /* 426 : INT426            */
    NULL,   /* 427 : INT427            */
    NULL,   /* 428 : INT428            */
    NULL,   /* 429 : INT429            */
    NULL,   /* 430 : INT430            */
    NULL,   /* 431 : INT431            */
    NULL,   /* 432 : INT432            */
    NULL,   /* 433 : INT433            */
    NULL,   /* 434 : INT434            */
    NULL,   /* 435 : TSIP_ROMOK        */
    NULL,   /* 436 : TSIP_LONG_PLG     */
    NULL,   /* 437 : TSIP_PROC_BUSY    */
    NULL,   /* 438 : TSIP_RDRDY1       */
    NULL,   /* 439 : TSIP_RDRDY0       */
    NULL,   /* 440 : TSIP_WRRDY4       */
    NULL,   /* 441 : INT441            */
    NULL,   /* 442 : TSIP_WRRDY1       */
    NULL,   /* 443 : TSIP_WRRDY0       */
    NULL,   /* 444 : TSIP_IRDRDY       */
    NULL,   /* 445 : TSIP_IWRRDY       */
    NULL,   /* 446 : ERRINT            */
    NULL,   /* 447 : NMLINT            */
    NULL,   /* 448 : PAE5              */
    NULL,   /* 449 : PAF5              */
    NULL,   /* 450 : INTB5             */
    NULL,   /* 451 : INTA5             */
    NULL,   /* 452 : PAE4              */
    NULL,   /* 453 : PAF4              */
    NULL,   /* 454 : INTB4             */
    NULL,   /* 455 : INTA4             */
    NULL,   /* 456 : PAE3              */
    NULL,   /* 457 : PAF3              */
    NULL,   /* 458 : INTB3             */
    NULL,   /* 459 : INTA3             */
    NULL,   /* 460 : PAE2              */
    NULL,   /* 461 : PAF2              */
    NULL,   /* 462 : INTB2             */
    NULL,   /* 463 : INTA1             */
    NULL,   /* 464 : PAE1              */
    NULL,   /* 465 : PAF1              */
    NULL,   /* 466 : INTB1             */
    NULL,   /* 467 : INTA1             */
    NULL,   /* 468 : PAE0              */
    NULL,   /* 469 : PAF0              */
    NULL,   /* 470 : INTB0             */
    NULL,   /* 471 : INTA1             */
    NULL,   /* 472 : VINI              */
    NULL,   /* 473 : GROUP0            */
    NULL,   /* 474 : GROUP1            */
    NULL,   /* 475 : GROUP2            */
    NULL,   /* 476 : GROUP3            */
    NULL,   /* 477 : SPIHF             */
    NULL,   /* 478 : INT478            */
    NULL,   /* 479 : INT479            */
    NULL,   /* 480 : TINT0             */
    NULL,   /* 481 : TINT1             */
    NULL,   /* 482 : TINT2             */
    NULL,   /* 483 : TINT3             */
    NULL,   /* 484 : TINT4             */
    NULL,   /* 485 : TINT5             */
    NULL,   /* 486 : TINT6             */
    NULL,   /* 487 : TINT7             */
    NULL,   /* 488 : TINT8             */
    NULL,   /* 489 : TINT9             */
    NULL,   /* 490 : TINT10            */
    NULL,   /* 491 : TINT11            */
    NULL,   /* 492 : TINT12            */
    NULL,   /* 493 : TINT13            */
    NULL,   /* 494 : TINT14            */
    NULL,   /* 495 : TINT15            */
    NULL,   /* 496 : TINT16            */
    NULL,   /* 497 : TINT17            */
    NULL,   /* 498 : TINT18            */
    NULL,   /* 499 : TINT19            */
    NULL,   /* 500 : TINT20            */
    NULL,   /* 501 : TINT21            */
    NULL,   /* 502 : TINT22            */
    NULL,   /* 503 : TINT23            */
    NULL,   /* 504 : TINT24            */
    NULL,   /* 505 : TINT25            */
    NULL,   /* 506 : TINT26            */
    NULL,   /* 507 : TINT27            */
    NULL,   /* 508 : TINT28            */
    NULL,   /* 509 : TINT29            */
    NULL,   /* 510 : TINT30            */
    NULL,   /* 511 : TINT31            */
    NULL,   /* 512 : NMI               */
};

/**
 * @fn         R_INTC_Init
 * @brief      Initialization of INTC (Arm GIC)
 *
 * @param      none
 *
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
e_r_drv_intc_err_t R_INTC_Init(void)
{
    uint16_t offset;
    volatile uint32_t * p_addr;

    /* ==== Initial setting 1 to receive GIC interrupt request ==== */
    /* Interrupt Security Registers setting */
    p_addr = (volatile uint32_t *)&INTC.GICD_IGROUPR0.LONG;
    for (offset = 0; offset < INTC_PRV_GICD_IGROUPR_REG_TOTAL; offset++)
    {
        RZA_IO_RegWrite_32((p_addr + offset), 0x00000000uL, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);    /* Set all interrupts to be secured */
    }

    /* Interrupt Configuration Registers setting */
    p_addr = (volatile uint32_t *)&INTC.GICD_ICFGR0.LONG;
    for (offset = 0; offset < INTC_PRV_GICD_ICFGR_REG_TOTAL; offset++)
    {
        RZA_IO_RegWrite_32((p_addr + offset), gs_gicd_icfgrn_table[offset], IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    }

    /* Interrupt Priority Registers setting */
    p_addr = (volatile uint32_t *)&INTC.GICD_IPRIORITYR0.LONG;
    for (offset = 0; offset < INTC_PRV_GICD_IPRIORITYR_REG_TOTAL; offset++)
    {
        RZA_IO_RegWrite_32((p_addr + offset), 0xF8F8F8F8uL, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);    /* Set the priority for all interrupts to 31 */
    }

    /* Interrupt Clear-Enable Registers setting */
    p_addr = (volatile uint32_t *)&INTC.GICD_ICENABLER0.LONG;
    for (offset = 0; offset < INTC_PRV_GICD_ICENABLER_REG_TOTAL; offset++)
    {
        RZA_IO_RegWrite_32((p_addr + offset), 0xFFFFFFFFuL, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);    /* Set all interrupts to be disabled */
    }

    /* ==== Initial setting for CPU interface ==== */
    /* Interrupt Priority Mask Register setting */
    R_INTC_SetMaskLevel(g_gic_cfg.mask);

    /* Binary Point Register setting */
    RZA_IO_RegWrite_32(&INTC.GICC_BPR.LONG, 0x00000002uL, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);             /* Group priority field [7:3], Subpriority field [2:0](Do not use) */

    /* CPU Interface Control Register setting */
    RZA_IO_RegWrite_32(&INTC.GICC_CTLR.LONG, 0x00000001uL, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ==== Initial setting 2 to receive GIC interrupt request ==== */
    /* Distributor Control Register setting */
    RZA_IO_RegWrite_32(&INTC.GICD_CTLR.LONG, 0x00000001uL, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_Init
 ******************************************************************************/

/**
 * @fn         R_INTC_SetNMIConfig
 * @brief      set NMI interrupt configuration
 *
 * @param[in]  p_nmi_config : NMI configuration parameter
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
e_r_drv_intc_err_t R_INTC_SetNMIConfig( const st_r_drv_nmi_cfg_t * p_nmi_config )
{
    /* enumeration value of e_r_drv_nmi_edge_t is treated as bit value.
     * this enumeration value can use safely as bit value.
     */
    RZA_IO_RegWrite_16(&INTC.ICR0.WORD, p_nmi_config->edge, INTC_ICR0_NMIE_SHIFT, INTC_ICR0_NMIE);
    RZA_IO_RegRead_16(&INTC.ICR0.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_SetNMIConfig
 ******************************************************************************/

/**
 * @fn         R_INTC_GetNMIConfig
 * @brief      get NMI interrupt configuration
 *
 * @param[in]  p_nmi_config : NMI configuration parameter
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
e_r_drv_intc_err_t R_INTC_GetNMIConfig( st_r_drv_nmi_cfg_t * p_nmi_config )
{
    /* enumeration value of e_r_drv_nmi_edge_t is treated as bit value.
     * this enumeration value can use safely as bit value.
     */
    p_nmi_config->edge = (e_r_drv_nmi_edge_t)RZA_IO_RegRead_16(&INTC.ICR0.WORD, INTC_ICR0_NMIE_SHIFT, INTC_ICR0_NMIE);
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_GetNMIConfig
 ******************************************************************************/

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
e_r_drv_intc_err_t R_IRQ_Init( const st_r_drv_irq_cfg_t * p_irq_config )
{
    uint16_t dummy16;
    dummy16 = 0;
    /* make IRQn pin interrupt sense fields on ICR1 register */
    dummy16  = (uint16_t)(dummy16 | (p_irq_config->sense0 << INTC_ICR1_IRQ00S_SHIFT));  /* 2bits per unit */
    dummy16 |= (uint16_t)(p_irq_config->sense1 << INTC_ICR1_IRQ10S_SHIFT);  /* 2bits per unit */
    dummy16 |= (uint16_t)(p_irq_config->sense2 << INTC_ICR1_IRQ20S_SHIFT);  /* 2bits per unit */
    dummy16 |= (uint16_t)(p_irq_config->sense3 << INTC_ICR1_IRQ30S_SHIFT);  /* 2bits per unit */
    dummy16 |= (uint16_t)(p_irq_config->sense4 << INTC_ICR1_IRQ40S_SHIFT);  /* 2bits per unit */
    dummy16 |= (uint16_t)(p_irq_config->sense5 << INTC_ICR1_IRQ50S_SHIFT);  /* 2bits per unit */
    dummy16 |= (uint16_t)(p_irq_config->sense6 << INTC_ICR1_IRQ60S_SHIFT);  /* 2bits per unit */
    dummy16 |= (uint16_t)(p_irq_config->sense7 << INTC_ICR1_IRQ70S_SHIFT);  /* 2bits per unit */
    RZA_IO_RegWrite_16(&INTC.ICR1.WORD, dummy16, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegRead_16(&INTC.ICR1.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_IRQ_Init
 ******************************************************************************/

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
e_r_drv_intc_err_t R_TINT_Init( const st_r_drv_tint_cfg_t * p_tint_config )
{
    uint32_t dummy32;
    /* make TINTn pin group interrupt sense fields on GIC400 ICFGR30 register */
    dummy32 = 0x55555555;   /* set reserved bits */
    dummy32 |= (uint32_t)(p_tint_config->sense0 << 1);   /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense1 << 3);   /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense2 << 5);   /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense3 << 7);   /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense4 << 9);   /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense5 << 11);  /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense6 << 13);  /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense7 << 15);  /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense8 << 17);  /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense9 << 19);  /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense10 << 21); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense11 << 23); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense12 << 25); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense13 << 27); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense14 << 29); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense15 << 31); /* 1 of 2 bits per unit */
    RZA_IO_RegWrite_32(&INTC.GICD_ICFGR30.LONG, dummy32, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegRead_32(&INTC.GICD_ICFGR30.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    /* make TINTn pin group interrupt sense fields on GIC400 ICFGR31 register */
    dummy32 = 0x55555555;   /* set reserved bits */
    dummy32 |= (uint32_t)(p_tint_config->sense16 << 1); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense17 << 3); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense18 << 5); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense19 << 7); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense20 << 9); /* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense21 << 11);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense22 << 13);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense23 << 15);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense24 << 17);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense25 << 19);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense26 << 21);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense27 << 23);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense28 << 25);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense29 << 27);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense30 << 29);/* 1 of 2 bits per unit */
    dummy32 |= (uint32_t)(p_tint_config->sense31 << 31);/* 1 of 2 bits per unit */
    RZA_IO_RegWrite_32(&INTC.GICD_ICFGR31.LONG, dummy32, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegRead_32(&INTC.GICD_ICFGR31.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_TINT_Init
 ******************************************************************************/

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
e_r_drv_intc_err_t R_INTC_RegistIntFunc( e_r_drv_intc_intid_t int_id, void (* func)(uint32_t int_sense) )
{
    /* ==== Argument check ==== */
    if (int_id >= INTC_VID_TOTAL)
    {
        return INTC_ERR_INVALID;
    }

    g_intc_func_table[int_id] = func;
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_RegistIntFunc
 ******************************************************************************/

/**
 * @fn         R_INTC_GetIntFunc
 * @brief      Get address of interrupt function
 *
 * @param[in]  int_id : Interrupt ID (0 - 512)
 * @param[out] void (*func)(uint32_t) : Function to be registered to INTC interrupt hander table
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID : Error End (Illegal parameters)
 */
e_r_drv_intc_err_t R_INTC_GetIntFunc( e_r_drv_intc_intid_t int_id, void (* func)(uint32_t int_sense) )
{
    void **p_tmp;

    /* ==== Argument check ==== */
    if (int_id >= INTC_VID_TOTAL)
    {
        return INTC_ERR_INVALID;
    }

    /* store interrupt handler address to user work */
    p_tmp = (void**)func;
    *p_tmp = g_intc_func_table[int_id];
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_GetIntFunc
 ******************************************************************************/

/**
 * @fn         R_INTC_Enable
 * @brief      Enables interrupt
 *
 * @param[in]  int_id : Interrupt ID (0 - 511)
 *
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_ID : Invalid interrupt ID specified by argument
 */
e_r_drv_intc_err_t R_INTC_Enable( e_r_drv_intc_intid_t int_id )
{
    uint32_t mask;
    volatile uint32_t *addr;

    /* Argument Check */
    if( ( intc_id_check( int_id ) ) != INTC_SUCCESS )
    {
        return INTC_ERR_INVALID_ID;
    }

    /* GICD_ISENABLERn has 32 sources in the 32 bits         */
    /* The n can be calculated by int_id / 32                */
    /* The bit field width is 1 bit                          */
    /* The target bit can be calculated by (int_id % 32) * 1 */
    /* GICD_ICENABLERn does not effect on writing "0"        */
    /* The bits except for the target write "0"              */
    addr = (volatile uint32_t *)&INTC.GICD_ISENABLER0.LONG;
    mask = 1;
    mask = mask << (int_id % 32u);   /* Create mask data */

    *(addr + (int_id / 32u)) = mask; /* Write GICD_ISENABLERn */

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_Enable
 ******************************************************************************/

/**
 * @fn         R_INTC_Disable
 * @brief      Disables interrupt
 *
 * @param[in]  int_id : Interrupt ID (0 - 511)
 *
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_ID : Invalid interrupt ID specified by argument
 */
e_r_drv_intc_err_t R_INTC_Disable( e_r_drv_intc_intid_t int_id )
{
    uint32_t mask;
    volatile uint32_t *addr;

    /* Argument Check */
    if( ( intc_id_check( int_id ) ) != INTC_SUCCESS )
    {
        return INTC_ERR_INVALID_ID;
    }

    /* GICD_ICENABLERn has 32 sources in the 32 bits         */
    /* The n can be calculated by int_id / 32                */
    /* The bit field width is 1 bit                          */
    /* The target bit can be calculated by (int_id % 32) * 1 */
    /* GICD_ICENABLERn does no effect on writing "0"         */
    /* Other bits except for the target write "0"            */
    addr = (volatile uint32_t *)&INTC.GICD_ICENABLER0.LONG;
    mask = 1;
    mask = mask << (int_id % 32u);   /* Create mask data */

    *(addr + (int_id / 32u)) = mask; /* Write GICD_ICENABLERn */

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_Disable
 ******************************************************************************/

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
e_r_drv_intc_err_t R_INTC_SetPriority( e_r_drv_intc_intid_t int_id, e_r_drv_intc_priority_t priority )
{
    uint8_t priority_reg;
    uint32_t shift;
    uint32_t mask;
    volatile uint32_t *p_addr;

    if( ( intc_id_check( int_id ) ) != INTC_SUCCESS )
    {
        return INTC_ERR_INVALID_ID;
    }

    if( ( intc_priority_check( priority ) ) != INTC_SUCCESS )
    {
        return INTC_ERR_INVALID_PRIORITY;
    }

    /* transform priority level to register value */
    priority_reg = intc_priority_to_reg( priority );


    /* GICD_IPRIORITYRn has 4 sources in the 32 bits        */
    /* The n can be calculated by int_id / 4                */
    /* The bit field width is 8 bits                        */
    /* The target bit can be calculated by (int_id % 4) * 8 */
    p_addr = (volatile uint32_t *)&INTC.GICD_IPRIORITYR0.LONG;

    shift = (int_id % 4u) * 8u;                 /* 8bits per unit */
    mask = (uint32_t)(0x000000FFuL << shift);   /* 8bits per unit */
    RZA_IO_RegWrite_32((p_addr + (int_id / 4u)), (uint32_t)priority_reg, shift, mask);

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_SetPriority
 ******************************************************************************/

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
e_r_drv_intc_err_t R_INTC_GetPriority( e_r_drv_intc_intid_t int_id, e_r_drv_intc_priority_t *priority )
{
    volatile uint32_t gicd_ipriority_reg;
    uint32_t shift;
    uint32_t mask;
    volatile uint32_t *p_addr;

    /* Argument check */
    if( ( intc_id_check( int_id ) ) != INTC_SUCCESS )
    {
        return INTC_ERR_INVALID_ID;
    }

    if( NULL == priority )
    {
        return INTC_ERR_INVALID;
    }


    /* GICD_IPRIORITYRn has 4 sources in the 32 bits        */
    /* The n can be calculated by int_id / 4                */
    /* The bit field width is 8 bits                        */
    /* The target bit can be calculated by (int_id % 4) * 8 */
    p_addr = (volatile uint32_t *)&INTC.GICD_IPRIORITYR0.LONG;

    shift = (int_id % 4u) * 8u;                 /* 8bits per unit */
    mask = (uint32_t)(0x000000FFuL << shift);   /* 8bits per unit */
    gicd_ipriority_reg = RZA_IO_RegRead_32((p_addr + (int_id / 4u)), shift, mask);

    /* transform register value to priority level */
    *priority = intc_reg_to_priority( (uint8_t)(gicd_ipriority_reg) );

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_GetPriority
 ******************************************************************************/

/**
 * @fn         R_INTC_SetMaskLevel
 * @brief      Sets the interrupt mask level
 *
 * @param[in]  mask_level : Interrupt mask level (0 - 31)
 *
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_PRIORITY : Invalid interrupt ID specified by argument
 */
e_r_drv_intc_err_t R_INTC_SetMaskLevel( e_r_drv_intc_priority_t mask_level )
{
    uint32_t mask_level_reg;

    /* Argument Check */
    if( ( intc_priority_check( mask_level ) ) != INTC_SUCCESS )
    {
        return INTC_ERR_INVALID_PRIORITY;
    }

    /* transform priority level to register value */
    mask_level_reg = (uint32_t)( intc_priority_to_reg( mask_level ) );

    RZA_IO_RegWrite_32(&INTC.GICC_PMR.LONG, mask_level_reg, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegRead_32(&INTC.GICC_PMR.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_SetMaskLevel
 ******************************************************************************/

/**
 * @fn         R_INTC_GetMaskLevel
 * @brief      Get the Interrupt mask level
 *
 * @param[out] *mask_level : Interrupt mask level (0 - 31)
 *
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
e_r_drv_intc_err_t R_INTC_GetMaskLevel( e_r_drv_intc_priority_t *masklevel )
{
    volatile uint32_t mask_level_reg;

    mask_level_reg = RZA_IO_RegRead_32(&INTC.GICC_PMR.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* transform register value to priority level */
    *masklevel = intc_reg_to_priority( (uint8_t)(mask_level_reg) );

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_GetMaskLevel
 ******************************************************************************/

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
e_r_drv_intc_err_t R_IRQ_SetSense( e_r_drv_irq_num_t irq_num, e_r_drv_irq_sense_t sense)
{
    uint16_t dummy16;
    uint16_t mask;

    /* check irq num */
    if( irq_num > INTC_IRQ7 )
    {
        return INTC_ERR_INVALID_NUM;
    }

    /* check sense */
    if( sense > IRQ_SENSE_BOTHEDGE )
    {
        return INTC_ERR_INVALID_SENSE;
    }

    /* make ICR1 IRQ sense register mask */
    mask = (uint16_t)( 3u << (irq_num * 2u) );

    /* read ICR1 register */
    dummy16 = RZA_IO_RegRead_16(&INTC.ICR1.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* clear target bit of ICR1 register */
    dummy16 &= (uint16_t)(~mask);

    /* modify target bit of ICR1 register */
    dummy16 |= (uint16_t)( sense << ( irq_num * 2u) );

    /* write back to ICR1 register */
    RZA_IO_RegWrite_16(&INTC.ICR1.WORD, dummy16, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegRead_16(&INTC.ICR1.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_IRQ_SetSense
 ******************************************************************************/


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
e_r_drv_intc_err_t R_IRQ_GetSense( e_r_drv_irq_num_t irq_num, e_r_drv_irq_sense_t *sense)
{
    uint16_t shift;
    uint32_t mask;
    volatile uint16_t *addr;

    /* check irq num */
    if( irq_num > INTC_IRQ7 )
    {
        return INTC_ERR_INVALID_NUM;
    }

    /* check sense */
    addr = (volatile uint16_t *)&INTC.ICR1.WORD;
    shift = (uint16_t)(irq_num * 2u);                     /* 2bits per unit */
    mask = (uint32_t)(0x00000003uL << shift);   /* 2bits per unit */
    *sense = RZA_IO_RegRead_16(addr, shift, mask);

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_IRQ_GetSense
 ******************************************************************************/


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
e_r_drv_intc_err_t R_TINT_GetSense( e_r_drv_tint_num_t tint_num, e_r_drv_tint_sense_t *sense)
{
    uint32_t shift;
    uint32_t mask;
    volatile uint32_t * reg;

    /* check tint num */
    if( tint_num > INTC_TINT31 )
    {
        return INTC_ERR_INVALID_NUM;
    }

    if ( tint_num >= INTC_TINT16 )
    {
        reg = &INTC.GICD_ICFGR31.LONG;
    }
    else
    {
        reg = &INTC.GICD_ICFGR30.LONG;
    }
    shift = (( tint_num % 16u ) * 2u) + 1u;     /* 1bit per unit */
    mask = (uint32_t)(0x00000001uL << shift);   /* 1bit per unit */
    *sense = RZA_IO_RegRead_32(reg, shift, mask);

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_TINT_GetSense
 ******************************************************************************/


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
e_r_drv_intc_err_t R_INTC_GetPendingStatus( e_r_drv_intc_intid_t int_id, e_r_drv_intc_pending_t *pending_status)
{
    uint32_t shift;
    uint32_t mask;
    volatile uint32_t *addr;

    /* Argument check */
    if( ( intc_id_check( int_id ) ) != INTC_SUCCESS )
    {
        return INTC_ERR_INVALID_ID;
    }

    /* GICD_ICPENDRn has 32 sources in the 32 bits          */
    /* The n can be calculated by int_id / 32               */
    /* The bit field width is 1 bit                         */
    /* The targe bit can be calculated by (int_id % 32) * 1 */
    addr = (volatile uint32_t *)&INTC.GICD_ICPENDR0.LONG;
    shift = int_id % 32u;                       /* 1bit per unit */
    mask = (uint32_t)(0x00000001uL << shift);   /* 1bit per unit */
    *pending_status = RZA_IO_RegRead_32((addr + (int_id / 32u)), shift, mask);

    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_GetPendingStatus
 ******************************************************************************/





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
e_r_drv_intc_err_t R_TINT_SetSense( e_r_drv_tint_num_t tint_num, e_r_drv_tint_sense_t sense)
{
    uint32_t dummy32;
    uint32_t mask;
    volatile uint32_t * reg;

    /* check tint num */
    if( tint_num > INTC_TINT31 )
    {
        return INTC_ERR_INVALID_NUM;
    }

    /* check sense */
    if( ! ( ( TINT_SENSE_LEVEL ==  sense )      ||
            ( TINT_SENSE_RISINGEDGE ==  sense ) ))
    {
        return INTC_ERR_INVALID_SENSE;
    }

    if ( tint_num >= INTC_TINT16 )
    {
        reg = &INTC.GICD_ICFGR31.LONG;
    }
    else
    {
        reg = &INTC.GICD_ICFGR30.LONG;
    }
    mask = ( 1u << ( (( tint_num % 16u ) * 2u) + 1u) );

    dummy32 = RZA_IO_RegRead_32(reg, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    dummy32 &= (~(mask));
    if ( TINT_SENSE_RISINGEDGE == sense )
    {
        dummy32 |= mask;
    }
    RZA_IO_RegWrite_32(reg, dummy32, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    RZA_IO_RegRead_32(reg, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_TINT_SetSense
 ******************************************************************************/



/**
 * @fn         R_INTC_GetVersion
 * @brief      Get driver version number of INTC Driver (LLD)
 *
 * @param      none
 *
 * @retval     DRV_SUCCESS : Succeed (always)
 */
int_t R_INTC_GetVersion(st_ver_info_t *pVerInfo)
{
    pVerInfo->lld.p_szdriver_name = gs_lld_info.p_szdriver_name;
    pVerInfo->lld.version.sub.major = gs_lld_info.version.sub.major;
    pVerInfo->lld.version.sub.minor = gs_lld_info.version.sub.minor;
    pVerInfo->lld.build = gs_lld_info.build;

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_GetVersion
 ******************************************************************************/

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
e_r_drv_intc_err_t R_INTC_SetIrqMask(e_r_drv_irq_mask_t mask)
{
    switch(mask)
    {
        case IRQ_MASK_ON:
        {
            RZA_IO_RegWrite_16(&INTC.IRQRR.WORD, IRQ_MASK_ON, INTC_IRQRR_IRQMSK_SHIFT, INTC_IRQRR_IRQMSK);
            RZA_IO_RegRead_16(&INTC.IRQRR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
            break;
        }

        case IRQ_MASK_OFF:
        {
            RZA_IO_RegWrite_16(&INTC.IRQRR.WORD, IRQ_MASK_OFF, INTC_IRQRR_IRQMSK_SHIFT, INTC_IRQRR_IRQMSK);
            RZA_IO_RegRead_16(&INTC.IRQRR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
            break;
        }

        default:
        {
            return INTC_ERR_INVALID;
            break;
        }
    }
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_SetIrqMask
 ******************************************************************************/

/**
 * @fn         R_INTC_GetIrqMask
 * @brief      Get IRQRR.IRQMSK
 *
 * @param[out] *mask : Value of IRQRR.IRQMSK
 *
 * @retval     INTC_SUCCESS : SUCCESS
 *
 */
e_r_drv_intc_err_t R_INTC_GetIrqMask(e_r_drv_irq_mask_t *mask)
{
    /* Get current interrupt mask level */
    *mask = (e_r_drv_irq_mask_t)RZA_IO_RegRead_16(&INTC.IRQRR.WORD, INTC_IRQRR_IRQMSK_SHIFT, INTC_IRQRR_IRQMSK);
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function R_INTC_GetIrqMask
 ******************************************************************************/

/**
 * @fn         intc_id_check
 * @brief      Check the interrupt ID valid or not
 * @note       This function return the INTC_VID_NMI as error.
 * @param[in]  int_id : interrupt ID
 *
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_ID : Invalid interrupt ID
 *
 */
static e_r_drv_intc_err_t intc_id_check( e_r_drv_intc_intid_t int_id )
{
    if( int_id >= INTC_GIC_ID_TOTAL )
    {
        return INTC_ERR_INVALID_ID;
    }

    return INTC_SUCCESS;

}
/******************************************************************************
 * End of function intc_id_check
 ******************************************************************************/

/**
 * @fn         intc_priority_check
 * @brief      Check the interrupt priority valid or not
 * @param[in]  priority : priority
 *
 * @retval     INTC_SUCCESS : SUCCESS
 * @retval     INTC_ERR_INVALID_PRIORITY : Invalid priority
 *
 */
static e_r_drv_intc_err_t intc_priority_check( e_r_drv_intc_priority_t priority )
{
    if( priority > INTC_PRIORITY_31 )
    {
        return INTC_ERR_INVALID_PRIORITY;
    }
    return INTC_SUCCESS;
}
/******************************************************************************
 * End of function intc_priority_check
 ******************************************************************************/


/**
 * @fn         intc_priority_to_reg
 * @brief      Convert driver priority level to GIC priority level
 * @param[in]  priority : driver priority level
 *
 * @return     GIC priority level
 *
 */
static uint8_t intc_priority_to_reg( e_r_drv_intc_priority_t priority )
{
    /* Convert driver priority level to GIC priority level */
    return ( (uint8_t)( priority << 3 ) );
}
/******************************************************************************
 * End of function intc_priority_to_reg
 ******************************************************************************/

/**
 * @fn         intc_reg_to_priority
 * @brief      Convert GIC priority level to driver priority level
 * @param[in]  reg_value : GIC priority level
 *
 * @return     driver priority level
 *
 */
static e_r_drv_intc_priority_t intc_reg_to_priority( uint8_t reg_value )
{
    /* Convert GIC priority level to driver priority level */
    return ( (e_r_drv_intc_priority_t)( reg_value >> 3 ) );
}
/******************************************************************************
 * End of function intc_reg_to_priority
 ******************************************************************************/

/* End of File */
