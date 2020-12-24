/*
   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
 */
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define PRND_N 624
#define PRND_M 397
#define MATRIX_A 0x9908b0dfU   /* constant vector a */
#define UPPER_MASK 0x80000000U /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffU /* least significant r bits */

#define BITWIDTH			32u

#define ALL1_MASK 0xffffffffU
#define INIT_MASK 1812433253U
#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define SEVEN 7
#define EIGHT 8
#define NINE 9
#define ELEVEN 11
#define FORTEEN 14
#define FIFTEEN 15
#define SIXTEEN 16
#define EIGHTEEN 18
#define TWENTY 20
#define THIRTY 30
#define FORTY 40
#define SIXTY 60

#define MAX_BUF 80

#define INIT_VALUE 5489U

#define TEMPERING_MASK1 0x9d2c5680U
#define TEMPERING_MASK2 0xefc60000U

#define SHA1_K_1 0x5a827999u
#define	SHA1_K_2 0x6ed9eba1u
#define	SHA1_K_3 0x8f1bbcdcu
#define	SHA1_K_4 0xca62c1d6u

#define SHA1_INIT_1 0x67452301u
#define SHA1_INIT_2 0xEFCDAB89u
#define SHA1_INIT_3 0x98BADCFEu
#define SHA1_INIT_4 0x10325476u
#define SHA1_INIT_5 0xC3D2E1F0u

#define VALID_BIT 256

#define TOP_BIT 0x80000000u

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint32_t mt[PRND_N]; /* the array for the state vector  */
static int_t mti=PRND_N+1;  /* mti==N+1 means mt[N] is not initialized */

static uint32_t genrand_int32(void);
static uint32_t Sha1_int32(uint32_t w[]);
static uint32_t Ch(const uint32_t x, const uint32_t y, const uint32_t z);
static uint32_t Maj(const uint32_t x, const uint32_t y, const uint32_t z);
static uint32_t Parity(const uint32_t x, const uint32_t y, const uint32_t z);
static uint32_t Rotl(const uint32_t bits, const uint32_t word);

/******************************************************************************
* Function Name: R_PRND_Init_Genrand
* Description  : Initializes the random number
* Arguments    : const uint32_t seed; I : The seed value for generating a random number
* Return Value : none
******************************************************************************/
/* initializes mt[N] with a seed */
void R_Prnd_Init_Genrand(const uint32_t seed)
{
	mt[0]= seed & ALL1_MASK;
	for ( mti=1 ; mti<PRND_N ; mti++ )
	{
		mt[mti] = ((INIT_MASK * (mt[mti-1] ^ (mt[mti-1] >> THIRTY))) + (uint32_t)mti);
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		mt[mti] &= ALL1_MASK;
		/* for >32 bit machines */
	}

    return;
}

/******************************************************************************
* Function Name: genrand_int32
* Description  : generates a random number on [0,0xffffffff]-interval
* Arguments    : none
* Return Value : random number
******************************************************************************/
static uint32_t genrand_int32(void)
{
    uint32_t y;
    static uint32_t mag01[TWO]={0x0U, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if ( mti >= PRND_N )  /* generate N words at one time */
    {
        int_t kk;

        if (mti == (PRND_N+1))   /* if init_genrand() has not been called, */
        {
            R_Prnd_Init_Genrand(INIT_VALUE); /* a default initial seed is used */
        }

        for ( kk=0 ; kk<(PRND_N-PRND_M) ; kk++ )
        {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+PRND_M] ^ (y >> 1) ^ mag01[y & 0x1U];
        }
        for ( ; kk<(PRND_N-1) ; kk++ )
        {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(PRND_M-PRND_N)] ^ (y >> 1) ^ mag01[y & 0x1U];
        }
        y = (mt[PRND_N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[PRND_N-1] = mt[PRND_M-1] ^ (y >> 1) ^ mag01[y & 0x1U];

        mti = 0;
    }

    y = mt[mti];
    mti++;

    /* Tempering */
    y ^= (y >> ELEVEN);
    y ^= (y << SEVEN)   & TEMPERING_MASK1;
    y ^= (y << FIFTEEN) & TEMPERING_MASK2;
    y ^= (y >> EIGHTEEN);

    return y;
}

/******************************************************************************
* Function Name: Ch
* Description  : 1 processing to constitute a random number generation function
* Arguments    : const uint32_t x; I :
*              : const uint32_t y; I :
*              : const uint32_t z; I :
* Return Value : result
******************************************************************************/
static uint32_t Ch(const uint32_t x, const uint32_t y, const uint32_t z)
{
	return ((x & y) ^ (~x & z));
}

/******************************************************************************
* Function Name: Maj
* Description  : 1 processing to constitute a random number generation function
* Arguments    : const uint32_t x; I :
*              : const uint32_t y; I :
*              : const uint32_t z; I :
* Return Value : result
******************************************************************************/
static uint32_t Maj(const uint32_t x, const uint32_t y, const uint32_t z)
{
	return ((x & y) ^ (x & z) ^ (y & z));
}

/******************************************************************************
* Function Name: Parity
* Description  : 1 processing to constitute a random number generation function
* Arguments    : const uint32_t x; I :
*              : const uint32_t y; I :
*              : const uint32_t z; I :
* Return Value : result
******************************************************************************/
static uint32_t Parity(const uint32_t x, const uint32_t y, const uint32_t z)
{
	return (x ^ y ^ z);
}

/******************************************************************************
* Function Name: Rotl
* Description  : 1 processing to constitute a random number generation function (left rotation)
* Arguments    : const uint32_t bits; I : Bit length of a shift
*              : const uint32_t word; I : Data shifted
* Return Value : result
******************************************************************************/
static uint32_t Rotl(const uint32_t bits, const uint32_t word)
{
	return ((word << bits) | (word >> (BITWIDTH-bits)));
}

/******************************************************************************
* Function Name: Sha1_int32
* Description  : The value of 32 bits is extracted from the hash value calculated by SHA-1.
* Arguments    : uint32_t w[]; I : Input data
* Return Value : result
******************************************************************************/
static uint32_t Sha1_int32(uint32_t w[])
{
	const uint32_t sha1_K[FOUR] = {
			SHA1_K_1,
			SHA1_K_2,
			SHA1_K_3,
			SHA1_K_4
	};
	uint32_t atmp, btmp, ctmp, dtmp, etmp;
	uint32_t H[FIVE];
	uint32_t T;
	int_t i;

	H[ZERO]  = SHA1_INIT_1;
	H[ONE]   = SHA1_INIT_2;
	H[TWO]   = SHA1_INIT_3;
	H[THREE] = SHA1_INIT_4;
	H[FOUR]  = SHA1_INIT_5;

	for( i=SIXTEEN ; i<MAX_BUF ; i++ )
	{
		w[i] = Rotl(ONE, w[i-THREE]^w[i-EIGHT]^w[i-FORTEEN]^w[i-SIXTEEN]);
	}

	atmp = H[ZERO];
	btmp = H[ONE];
	ctmp = H[TWO];
	dtmp = H[THREE];
	etmp = H[FOUR];

	for( i=0 ; i<TWENTY ; i++ )
	{
		T = Rotl(FIVE, atmp) + Ch(btmp, ctmp, dtmp) + etmp + sha1_K[0] + w[i];
		etmp = dtmp;
		dtmp = ctmp;
		ctmp = Rotl(THIRTY, btmp);
		btmp = atmp;
		atmp = T;
	}

	for( i=TWENTY ; i<FORTY ; i++ )
	{
		T = Rotl(FIVE, atmp) + Parity(btmp, ctmp, dtmp) + etmp + sha1_K[ONE] + w[i];
		etmp = dtmp;
		dtmp = ctmp;
		ctmp = Rotl(THIRTY, btmp);
		btmp = atmp;
		atmp = T;
	}

	for( i=FORTY ; i<SIXTY ; i++ )
	{
		T = Rotl(FIVE, atmp) + Maj(btmp, ctmp, dtmp) + etmp + sha1_K[TWO] + w[i];
		etmp = dtmp;
		dtmp = ctmp;
		ctmp = Rotl(THIRTY, btmp);
		btmp = atmp;
		atmp = T;
	}

	for( i=SIXTY ; i<MAX_BUF ; i++ )
	{
		T = Rotl(FIVE, atmp) + Parity(btmp, ctmp, dtmp) + etmp + sha1_K[THREE] + w[i];
		etmp = dtmp;
		dtmp = ctmp;
		ctmp = Rotl(THIRTY, btmp);
		btmp = atmp;
		atmp = T;
	}

/*	H[0] += a;
	H[1] += b;  */
	H[TWO] += ctmp;
/*	H[3] += d;
	H[4] += e;  */

	return H[TWO];
}

/******************************************************************************
* Function Name: R_PRND_Genrand_int32
* Description  : A 32-bit encryption pseudo random number is generated.
*              : SHA-1 is applied to eight random numbers generated by the Mersenne Twister method,
*              : it is with a hash value of 160 bits and does a value from the 64th bit of those
*              : to the 95th bit with a return value.
* Arguments    : none
* Return Value : random number
******************************************************************************/
uint32_t R_Prnd_Genrand_int32(void)
{
	/* input data */
	uint32_t w[MAX_BUF];
	int_t i;

	/* Input data generated */
	for( i=0 ; i<EIGHT ; i++ )
	{
		w[i] = genrand_int32();  /* Mersenne Twister */
	}
	w[i] = TOP_BIT;
	for( i=NINE ; i<FIFTEEN ; i++ )
	{
		w[i] = 0;
	}
	w[i] = VALID_BIT;

	return Sha1_int32(w);
}

/* End of File */
