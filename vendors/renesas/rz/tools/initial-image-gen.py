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
#* File Name    : 1st-image-gen.py
#* Device(s)    : RZ/A2M
#* Tool-Chain   : python 3
#* OS           : None
#* H/W Platform : PC
#* Description  : Generate RZ/A2M Update image
#**********************************************************************************************************************
import sys
import hashlib

#
# Parameters
#

# Input Filename
input_file			= 'aws_demos.bin'
# Output Filename
output_file			= 'userprog.rsu'
# Input Key
input_key_file		= 'secp256r1.privatekey'
# Firmware version (sequence number)
sequence_number		= 1
# addresses
start_address		= 0x20200300
end_address			= 0x209fffff
execution_address	= 0x209ffffc
# Hardware ID
hardware_id			= 0x00000009
# Image Size
image_size			= 0x00800000
# section Size
section_size		= 4096

# Open File
f = open(input_file,'rb')
sys.stdout.write("Input File:"+input_file+"\n")

# Check image header
buf = f.read(7)
if buf.find(b'Renesas') != -1:
	buf = f.read(0x300-7)
	buf = f.read(7)

# Read whole data
tmp = f.read()
buf = buf + tmp
f.close()

padding_size = section_size - ((len(buf)+0x300) % section_size)

#create header
#typedef struct _firmware_update_control_block
#{
#    uint8_t magic_code[7];
#    uint8_t image_flag;
#    uint8_t signature_type[32];
#    uint32_t signature_size;
#    uint8_t signature[256];
#    uint32_t dataflash_flag;
#    uint32_t dataflash_start_address;
#    uint32_t dataflash_end_address;
#    uint8_t reserved1[200];
#    uint32_t sequence_number;
#    uint32_t start_address;
#    uint32_t end_address;
#    uint32_t execution_address;
#    uint32_t hardware_id;
#    uint32_t image_size;
#    uint8_t reserved2[232];
#}FIRMWARE_UPDATE_CONTROL_BLOCK;

whole_data =  bytearray(b'Renesas')
whole_data += bytearray(b'\xFE')
whole_data += bytearray(b'sig-sha256-ecdsa') + (bytearray(b'\x00')*16)
whole_data += bytearray((64).to_bytes(4, byteorder='little'))
whole_data += (bytearray(b'\x00')*256)
whole_data += bytearray((1).to_bytes(4, byteorder='little'))
whole_data += (bytearray(b'\x00')*4)
whole_data += (bytearray(b'\x00')*4)
whole_data += (bytearray(b'\x00')*200)
whole_data += bytearray(sequence_number.to_bytes(4, 'little')) 
whole_data += bytearray(start_address.to_bytes(4, 'little')) 
whole_data += bytearray(end_address.to_bytes(4, 'little')) 
whole_data += bytearray(execution_address.to_bytes(4, 'little')) 
whole_data += bytearray(hardware_id.to_bytes(4, 'little'))
whole_data += bytearray((len(buf)+0x100+padding_size).to_bytes(4, byteorder='little'))
whole_data += (bytearray(b'\x00')*232)

#
#generate whole data
#
whole_data += buf
whole_data += (bytearray(b'\xFF')*(padding_size))

#
#calc SHA256-hash
#
hash = hashlib.sha256(whole_data[0x200:]).digest()
sys.stdout.write("Hash:"+hash.hex()+"\n")

#
#read key
#
from ecdsa import SigningKey, NIST256p
f = open(input_key_file,'rb')
sk = SigningKey.from_pem(f.read())
f.close()
sys.stdout.write("Input Key File:"+input_key_file+"\n")

#
#generate Signiture
#
pri = sk.privkey
pub = pri.public_key

from ecdsa.ecdsa import string_to_int, Signature
from uuid import uuid1
rnd = uuid1().int
digest = string_to_int(hash)
signature = pri.sign(digest, rnd)
string_r = (signature.r).to_bytes(32,'big')
string_s = (signature.s).to_bytes(32,'big')
sys.stdout.write("signature(r):"+ hex(signature.r) +"\n")
sys.stdout.write("signature(s):"+ hex(signature.s) +"\n")

for num in range(len(string_r)):
	whole_data[0x0000002c + num] = string_r[num]
for num in range(len(string_s)):
	whole_data[0x0000004c + num] = string_s[num]
#
#write file
#
with open(output_file,'wb') as f:
	f.write(whole_data)
f.close()
sys.stdout.write("Output File:"+output_file+"\n")
sys.stdout.write("Complete.\n")
