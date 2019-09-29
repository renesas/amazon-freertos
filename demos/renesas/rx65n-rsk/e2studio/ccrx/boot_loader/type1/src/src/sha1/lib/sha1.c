/*	Copyright(C) 2005(2006-2009) Renesas Solutions Corp. All rights reserved.	*/

/* $Id: //depot/cryptogram/Cmodel/P_Sha1.c#2 $ */
/* $Date: 2007/05/03 $ */

#include <stdio.h>
#if defined(MACOS) || defined(__RX)
#include <stdint.h>
typedef int32_t	natural_int_t;
#else
#include "r_stdint.h"
#endif
#include "r_cryptogram.h"

#include <string.h>
#include "platform.h"
#ifdef SECURE_BOOT
#if SECURE_BOOT == 1
#pragma section SECURE_BOOT
#endif
#endif

/*
 The words of the second 5-word buffer are labeled H0, H1, H2, H3, H4. 
 The words of the 80-word sequence are labeled W0, W1, ... , W79.
 A single word buffer TEMP is also employed.
*/
/*
The above assumes that the sequence W0, ... , W79 is implemented as an array of eighty 32-bit words.
This is efficient from the standpoint of minimization of execution time, since the addresses of Wt-3, ... , Wt-16 
in step (b) are easily computed. If space is at a premium, an alternative is to regard { Wt } as a circular queue, 
which may be implemented using an array of sixteen 32-bit words W[0], ... W[15]. 
 In this case, in hex let MASK = 0000000F. Then processing of Mi is as follows:
*/

#define WORDS 16
#define FIVE_WORD_BUFFER 5

/* circular left shift operation S(x, n) */
#define S(x,n) (((x)<<(n))^((x)>>(32-(n))))	/* left rotate shift by n bits   */
#define S1(x) S(x,1)
#define S5(x) S(x,5)
#define S30(x) S(x,30)

/*
 A sequence of constant words K0, K1, ... , K79 is used in the SHA-1. In hex these are given by
    Kt = 5A827999 (0 <= t <= 19)
    Kt = 6ED9EBA1 (20 <= t <= 39)
    Kt = 8F1BBCDC (40 <= t <= 59)
    Kt = CA62C1D6 (60 <= t <= 79).
*/
#define K0_19	0x5A827999
#define K20_39	0x6ED9EBA1
#define K40_59	0x8F1BBCDC
#define K60_79	0xCA62C1D6
/******************************************************************************
Macro definitions
******************************************************************************/
#define SHA1_BLOCK_SIZE		(64)
#define SHA1_DIGEST_SIZE	(20)
#define BLOCK_BIT_LENGTH	(512)
#define PADDING_LENGTH		(56)

const uint8_t hdat2[SHA1_DIGEST_SIZE] = 
{
	0x67,0x45,0x23,0x01, 0xef,0xcd,0xab,0x89, 0x98,0xba,0xdc,0xfe, 
	0x10,0x32,0x54,0x76, 0xc3,0xd2,0xe1,0xf0
};

void R_Sha1_Hash(uint8_t *mdat, uint8_t *hdat, uint32_t block)
{
	uint32_t  A,B,C,D,E,TEMP,H[FIVE_WORD_BUFFER],W[WORDS];
	natural_int_t  s, t;

	for (s=0; s<FIVE_WORD_BUFFER; ++s) {
		H[s] = ((uint32_t)hdat[s*4+0]<<24) ^ ((uint32_t)hdat[s*4+1]<<16) ^ ((uint32_t)hdat[s*4+2]<<8) ^ ((uint32_t)hdat[s*4+3]);
	}

	while (block != 0) {

		for (t=0; t<WORDS; ++t) {
			W[t] = ((uint32_t)mdat[t*4+0]<<24) ^ ((uint32_t)mdat[t*4+1]<<16) ^ ((uint32_t)mdat[t*4+2]<<8) ^ ((uint32_t)mdat[t*4+3]);
		}

		A = H[0];
		B = H[1];
		C = H[2];
		D = H[3];
		E = H[4];

		for (t=0; t<80; ++t) {

			s = t & 0xf;

			if (t >= 16) {
				W[s] ^= W[(s+13)&0xf] ^ W[(s+8)&0xf] ^ W[(s+2)&0xf];
				W[s]  = S1(W[s]);
			}

			if (t <= 19) {
				TEMP = S5(A) + ((B&C) | (~B&D)) + E + W[s] + K0_19;
			} else if (t <= 39) {
				TEMP = S5(A) + (B^C^D) + E + W[s] + K20_39;
			} else if (t <= 59) {
				TEMP = S5(A) + ((B&C) | (B&D) | (C&D)) + E + W[s] + K40_59;
			} else {
				TEMP = S5(A) + (B^C^D) + E + W[s] + K60_79;
			}

			E = D;
			D = C;
			C = S30(B);
			B = A;
			A = TEMP;
		}

		H[0] += A;
		H[1] += B;
		H[2] += C;
		H[3] += D;
		H[4] += E;

		mdat += 64;
		--block;
	}

	for (s=0; s<FIVE_WORD_BUFFER; ++s) {
		hdat[s*4+0] = (uint8_t)(H[s] >> 24);
		hdat[s*4+1] = (uint8_t)((H[s] >> 16) & 0xff);
		hdat[s*4+2] = (uint8_t)((H[s] >> 8)  & 0xff);
		hdat[s*4+3] = (uint8_t)(H[s] & 0xff);
	}
}
/******************************************************************************
* ID : 1.0
* Declaration : void conv_data_length(uint8_t *, uint32_t )
* Function Name: conv_data_length
* Description : convert Byte length -> Bit length
* Argument : none
* Return Value : none
******************************************************************************/
void
conv_data_length(uint8_t *data_adr, uint32_t byte_len)
{
	*data_adr     = (0xE0000000&byte_len)>>29;
	*(data_adr+1) = (0x1fe00000&byte_len)>>21;
	*(data_adr+2) = (0x001fe000&byte_len)>>13;
	*(data_adr+3) = (0x00001fe0&byte_len)>> 5;
	*(data_adr+4) = (0x0000001f&byte_len)<< 3;
	return;
}

/******************************************************************************
* ID : 1.0
* Declaration : void R_Sha1_Hash_Padding(uint8_t *mdat_adr, uint8_t *hdat_adr, uint32_t data_len)

* Function Name: R_Sha1_Hash_Padding
* Description : Hash & Padding
* Argument : uint8_t *mdat_adr		: 
			 uint8_t *hdat_adr		: 
			 uint32_t data_len		: Data length
* Return Value : none
******************************************************************************/
void
R_Sha1(uint8_t *mdat_adr, uint8_t *hdat_adr, uint32_t data_len)
{
	int i;
	uint32_t w_quotient;								// 
	uint32_t w_remainder;								// 
	uint8_t padding_area[SHA1_BLOCK_SIZE*2];
	uint8_t padding_offset;

	for (i=0;i<SHA1_DIGEST_SIZE; ++i) 
	{
		*(hdat_adr+i) = hdat2[i];
	}
	w_quotient = data_len/SHA1_BLOCK_SIZE;
	w_remainder = data_len%SHA1_BLOCK_SIZE;

	if( w_quotient != 0)
	{
		R_Sha1_Hash( mdat_adr, hdat_adr, w_quotient);
	}

	for( i = 0; i < SHA1_BLOCK_SIZE*2; i++)
	{
		padding_area[i] = 0;
	}
	mdat_adr = mdat_adr+(w_quotient * SHA1_BLOCK_SIZE);
	for( i = 0; i < w_remainder; i++)
	{
		padding_area[i] = *(mdat_adr+i);
	}
	padding_area[i] = 0x80;
	
	if( w_remainder < PADDING_LENGTH)
	{
		padding_offset = PADDING_LENGTH;
		w_quotient = 1;
	}else{
		padding_offset = (SHA1_BLOCK_SIZE+PADDING_LENGTH);
		w_quotient = 2;
	}
	/* data length  Byte length -> bit length */
	conv_data_length( &padding_area[padding_offset+3], data_len);
	R_Sha1_Hash( padding_area, hdat_adr, w_quotient);

	return;
}

#ifdef SECURE_BOOT
#if SECURE_BOOT == 1
#pragma section
#endif
#endif
