/*
 * term_out.c
 *
 *  Created on: 2020/04/24
 *      Author: hmU11282
 */

#include "term_out.h"

int putchar (int ch);
void send(unsigned char ch);

void term_start( void )
{
	R_UART3_Start();
}

void term_stop( void )
{
	R_UART3_Stop();
}

int putchar (int ch)
{
	send((unsigned char)ch);	/* 1 byte transmission */
#if 0
	if(ch == '\r')			/* Send CR when LF */
	{
		ch = '\n';
		send((unsigned char)ch);	/* 1 byte transmission */
	}
#endif
	return 0;
}

void send(unsigned char ch)
{
	while((SSR12 & 0x20) != 0)	/* Waiting for empty transmission buffer */
	{
		;
	}
    STMK3 = 1U;		/* disable INTST3 interrupt */
	TXD3 = ch;		/* Write to transmit buffer */
    STMK3 = 0U;		/* enable INTST3 interrupt */
	return;
}
