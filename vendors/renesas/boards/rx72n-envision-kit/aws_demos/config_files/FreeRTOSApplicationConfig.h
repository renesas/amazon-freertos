#ifndef FREERTOS_APPLICATION_CONFIG_H
#define FREERTOS_APPLICATION_CONFIG_H

#define OTA            (Used)

#define Used           (1)
#define Unused         (0)

#define CONNECTION            (ETHER)

#define ETHER           (1)
#define WIFI         (0)

#if (CONNECTION == 0)
#error "Connection type WIFI not supported"
#endif

#endif