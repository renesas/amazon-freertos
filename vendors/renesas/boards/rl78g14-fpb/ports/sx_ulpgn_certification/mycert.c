 /**
  *  SharkSSLParseCert.  Build 3370.
  *  Copyright (c) 2014 Real Time Logic.
  */

#if (__CCRX__)
#include "platform.h"
#endif
#if (__CCRL__)
#include "rl_platform.h"
#endif

/*
 * @todo : Set SharkSSL-encoded client certificate for AWS IoT Broker.
 */
const uint8_t sharkSslRSACert[] = { 0x00 };

const uint32_t sharkSslRSACertLength = sizeof(sharkSslRSACert);
