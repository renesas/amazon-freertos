#**********************************************************************************************************************
#* DISCLAIMER
#* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
#* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
#* applicable laws, including copyright laws.
#* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
#* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
#* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
#* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
#* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
#* THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
#* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
#* this software. By using this software, you agree to the additional terms and conditions found by accessing the
#* following link:
#* http://www.renesas.com/disclaimer
#*
#* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
#**********************************************************************************************************************
#**********************************************************************************************************************
#* File Name    : update-image-gen.py
#* Device(s)    : RZ/A2M
#* Tool-Chain   : python 3
#* OS           : None
#* H/W Platform : PC
#* Description  : Generate RZ/A2M Update image
#**********************************************************************************************************************
import sys

# Input Filename
input_file			= 'userprog.bin'
# boot Filename
boot_file			= '..\ota_boot_proc\ota_boot_proc.bin'
# Output Filename
output_file			= 'aws_tests.bin'
# Bootloader address
boot_addr			= 0x50000000
# Application address
appli_addr			= 0x50A00000

f = open(boot_file,'rb')
buf = f.read()
f.close()

padding_size = appli_addr - boot_addr - len(buf)
buf += (bytearray(b'\xFF')*padding_size)

f = open(input_file,'rb')
tmp = f.read()
buf = buf + tmp
f.close()

with open(output_file,'wb') as f:
	f.write(buf)
f.close()

