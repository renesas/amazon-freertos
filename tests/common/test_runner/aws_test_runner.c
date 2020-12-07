/*
 * Amazon FreeRTOS Test Runner V1.1.4
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */


/**
 * @file aws_test_runner.c
 * @brief The function to be called to run all the tests.
 */

/* Test runner interface includes. */
#include "aws_test_runner.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Unity framework includes. */
#include "unity_fixture.h"
#include "unity_internals.h"

/* Application version info. */
#include "aws_application_version.h"

const AppVersion32_t xAppFirmwareVersion =
{
    .u.x.ucMajor = APP_VERSION_MAJOR,
    .u.x.ucMinor = APP_VERSION_MINOR,
    .u.x.usBuild = APP_VERSION_BUILD,
};

char cBuffer[ testrunnerBUFFER_SIZE ];

/* Heap leak variables. */
unsigned int xHeapBefore;
unsigned int xHeapAfter;
/*-----------------------------------------------------------*/
__attribute__((weak)) void WIFI_Off(void)
{
}
volatile int_t ota_mode = 0;

/* This function will be generated by the test automation framework,
 * do not change the signature of it. You could, however, add or remove
 * RUN_TEST_GROUP statements.
 */
static void RunTests( void )
{
    /* Tests can be disabled in aws_test_runner_config.h */

    /* The Amazon FreeRTOS qualification program requires that Wi-Fi and TCP be the
     * first tests in this function. */
    #if ( testrunnerFULL_WIFI_ENABLED == 1 )
        RUN_TEST_GROUP( Full_WiFi );
    #endif

    #if ( testrunnerFULL_TCP_ENABLED == 1 )
        RUN_TEST_GROUP( Full_TCP );
    #endif

    #if ( testrunnerFULL_GGD_ENABLED == 1 )
        RUN_TEST_GROUP( Full_GGD );
    #endif

    #if ( testrunnerFULL_GGD_HELPER_ENABLED == 1 )
        RUN_TEST_GROUP( Full_GGD_Helper );
    #endif

    #if ( testrunnerFULL_SHADOW_ENABLED == 1 )
        RUN_TEST_GROUP( Full_Shadow );
    #endif

    #if ( testrunnerFULL_MQTT_ENABLED == 1 )
        RUN_TEST_GROUP( Full_MQTT );
    #endif

    #if ( testrunnerFULL_MQTT_STRESS_TEST_ENABLED == 1 )
        RUN_TEST_GROUP( Full_MQTT_Agent_Stress_Tests );
    #endif

    #if ( testrunnerFULL_MQTT_AGENT_ENABLED == 1 )
        RUN_TEST_GROUP( Full_MQTT_Agent );
    #endif

    #if ( testrunnerFULL_MQTT_ALPN_ENABLED == 1 )
        RUN_TEST_GROUP( Full_MQTT_Agent_ALPN );
    #endif

    #if ( testrunnerFULL_OTA_CBOR_ENABLED == 1 )
        RUN_TEST_GROUP( Full_OTA_CBOR );
    #endif

    #if ( testrunnerFULL_OTA_AGENT_ENABLED == 1 )
        RUN_TEST_GROUP( Full_OTA_AGENT );
    #endif

    #if ( testrunnerFULL_OTA_PAL_ENABLED == 1 )
        RUN_TEST_GROUP( Full_OTA_PAL );
    #endif

    #if ( testrunnerFULL_PKCS11_ENABLED == 1 )
        RUN_TEST_GROUP( Full_PKCS11_CryptoOperation );
        RUN_TEST_GROUP( Full_PKCS11_GeneralPurpose );

    #endif

    #if ( testrunnerFULL_CRYPTO_ENABLED == 1 )
        RUN_TEST_GROUP( Full_CRYPTO );
    #endif

    #if ( testrunnerFULL_TLS_ENABLED == 1 )
        RUN_TEST_GROUP( Full_TLS );
    #endif

    #if ( testrunnerFULL_CBOR_ENABLED == 1 )
        RUN_TEST_GROUP( Full_CBOR );
    #endif

    #if ( testrunnerFULL_DEFENDER_ENABLED == 1 )
        RUN_TEST_GROUP( Full_DEFENDER );
    #endif

    #if ( testrunnerFULL_POSIX_ENABLED == 1 )
        RUN_TEST_GROUP( Full_POSIX_CLOCK );
        RUN_TEST_GROUP( Full_POSIX_MQUEUE );
        RUN_TEST_GROUP( Full_POSIX_PTHREAD );
        RUN_TEST_GROUP( Full_POSIX_SEMAPHORE );
        RUN_TEST_GROUP( Full_POSIX_TIMER );
        RUN_TEST_GROUP( Full_POSIX_UTILS );
        RUN_TEST_GROUP( Full_POSIX_STRESS );
    #endif

    #if ( testrunnerFULL_FREERTOS_TCP_ENABLED == 1 )
        RUN_TEST_GROUP( Full_FREERTOS_TCP );
    #endif

    #if ( testrunnerOTA_END_TO_END_ENABLED == 1 )
        extern void vStartOTAUpdateDemoTask( void );
        vStartOTAUpdateDemoTask();
    #endif
}
/*-----------------------------------------------------------*/

void TEST_RUNNER_RunTests_task( void * pvParameters )
{
    /* Initialize unity. */
    UnityFixture.Verbose = 1;
    UnityFixture.GroupFilter = 0;
    UnityFixture.NameFilter = testrunnerTEST_FILTER;
    UnityFixture.RepeatCount = 1;

    UNITY_BEGIN();

    /* Give the print buffer time to empty */
    vTaskDelay( 500 );
    /* Measure the heap size before any tests are run. */
    #if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 )
        xHeapBefore = xPortGetFreeHeapSize();
    #endif

    RunTests();

    #if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 )

        /* Measure the heap size after tests are done running.
         * This test must run last. */

        /* Perform any global resource cleanup necessary to avoid memory leaks. */
        #ifdef testrunnerMEMORYLEAK_CLEANUP
            testrunnerMEMORYLEAK_CLEANUP();
        #endif

        /* Give the print buffer time to empty */
        vTaskDelay( 500 );
        xHeapAfter = xPortGetFreeHeapSize();
        RUN_TEST_GROUP( Full_MemoryLeak );
    #endif /* if ( testrunnerFULL_MEMORYLEAK_ENABLED == 1 ) */

    /* Currently disabled. Will be enabled after cleanup. */
    if(ota_mode==0) {
        vTaskDelay( 500 );
    	WIFI_Off();
    }
    UNITY_END();

    #ifdef CODE_COVERAGE
        exit( 0 );
    #endif

    /* This task has finished.  FreeRTOS does not allow a task to run off the
     * end of its implementing function, so the task must be deleted. */
    vTaskDelete( NULL );
}
/*-----------------------------------------------------------*/
