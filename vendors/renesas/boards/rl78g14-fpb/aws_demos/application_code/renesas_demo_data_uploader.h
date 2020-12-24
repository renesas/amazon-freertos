#ifndef RENESAS_DEMO_H
#define RENESAS_DEMO_H

/* Standard includes. */
#include <stdbool.h>

int RunRenesasSensorDataUploadDemo(bool awsIotMqttMode, const char *pIdentifier, void *pNetworkServerInfo,
        void *pNetworkCredentialInfo, const IotNetworkInterface_t *pNetworkInterface);

#endif
