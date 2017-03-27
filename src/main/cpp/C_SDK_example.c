// ************************************************************************************************************
// ******                                                                                                ******
// ****** PROGRAM: C_SDK_example.cpp                                                                     ******
// ****** AUTHOR:  Robb Kane                                                                             ******
// ****** DATE:    August 15, 2016                                                                       ******
// ******                                                                                                ******
// ******                                                                                                ******
// ****** SOLE PROPERTY OF APPDYNAMICS, INC.  ALL RIGHTS RESERVED.                                       ******
// ******                                                                                                ******
// ****** This program is soley for instructional use.  Its purpose is only to indicate how one "might"  ******
// ****** implement the C SDK Agent calls.   This software will not be supported by AppDynnamics nor is  ******
// ****** there any guarantee, implied or otherwise, that it will work.   Use at your own risk.          ******
// ******                                                                                                ******
// ************************************************************************************************************


// If you are using an Agent release 4.2.1 or later, and wish to take advantage of the additional
// SDK capabilities i.e. appd_{bt,exitcall}_get, appd_{bt,exitcall}_store use this define.
#define _4_2_1_or_later

// If you are using a Windows version SDK, make sure that this flag is included.
#ifndef _WIN32
#include <stdarg.h>
#include <unistd.h>
#include <stdbool.h>
#else // _WIN32

#if !defined(__BOOL_DEFINED)
#define true 1
#define false 0
#endif

#endif


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "appdynamics.h"

#define                 FAIL_RC              1                          // Exit program on error with RC set to 1, 
int                      rc                = 0;                         // The return code used for all non-void SDK API calls not returning a handle.

const char*              backend_type      = "APPD_BACKEND_RABBITMQ";   // The backend type, for this example we'll use RABBITMQ.
const char*              backend_name      = "C_SDK_test_backend";      // The backend unique name we'll give it.
const char*              backend_key       = "HOST";                    // This backend type has a few possible keys (HOST, PORT, ROUTING KEY, EXCHANGE).
const char*              backend_value     = "C_SDK_test_RMQ_HOST";     // For the backend type & key we've used (RABBITMQ/HOST) assign a hostname string.

const char*              bt_name           = "C_SDK_test_BT";           // The string used to create the BT handle.
const char*              bt_key            = "C_SDK_test_BT_key";       // The user-defined key string used in an appd_bt_add_user_data() call.
const char*              bt_value          = "C_SDK_test_BT_value";     // The user-defined value string associated with the key string in an appd_bt_add_user_data() call.

appd_bt_handle           bt_handle         = NULL;                      // The handle returned by call to appd_bt_begin().
appd_bt_handle           bt_guid_handle    = NULL;                      // The handle returned by appd_bt_get().

                                                                        // For BTs marked as errors:
enum appd_error_level    bt_error_level    = APPD_LEVEL_WARNING;        // If we're going mark a BT as an error, we can set its level.
const char*              bt_error_msg      = "sample_bt_add_error_msg"; // And label the BT's error message.
const int                bt_error_flag     = true;                      // If this flag is TRUE, then the BT is treated only as an error; not counted as slow,
                                                                        // very slow, or stalled, even if it was.

const char*              bt_guid           = "C_SDK_test_BT_guid";      // The string used for the appd_bt_store() call.
const char*              ec_guid           = "C_SDK_test_EC_guid";      // The string used to create the exitcall get/store calls.

appd_exitcall_handle     ec_handle         = NULL;                      // The handle returned from an appd_exitcall_begin() call.
appd_exitcall_handle     ec_guid_handle    = NULL;                      // The handle returned by appd_exitcall_get(). 

                                                                        // For ExitCall's marked as errors:
enum appd_error_level    ec_error_level    = APPD_LEVEL_WARNING;        // If we're going mark a BT as an error, we can set its level.
const char*              ec_error_msg      = "sample_ec_add_error_msg"; // And label the ExitCall's error message.
const int                ec_error_flag     = false;                      // If this flag is TRUE, then the BT is treated only as an error; not counted as slow,
                                                                        // very slow, or stalled, even if it was.
 
const int                loop_cnt_total    = 100;                    // how many transactions to run
const int                error_rate_pct    = 20;                        // the rate at which to report a BT as an error (%)

int main(int argc, char* argv[])
{

    struct appd_config     cfg;                                             // The SDK configuration structure.  This is passed on initialization to the Controller.
	int                    loop_count;
    appd_exitcall_handle   ec_handle;

#ifdef _WIN32
	fprintf(stdout, "Running Visual Studio Version: %d, Full Version: %d\n", _MSC_VER, _MSC_FULL_VER);
#endif

    
    appd_config_init(&cfg);                                          // First clear out the structure contents, set all fields empty.

    cfg.init_timeout_ms          =  60000;                           // This is the amount of time in milliseconds we'll wait to get context during init.  If
                                                                     // the context is returned before the timeout, then the appd_sdk_init() call will return
                                                                     // sooner and all following appd_bt_begin() calls will return a valid handle.

    cfg.controller.host          = "hawaii";                     // The Host name of the Controller.
    cfg.controller.port          = 8090;                             // The Port on which we'll connect to the Controller.
    cfg.controller.use_ssl       = 0;                                // Set false means no SSL.
    cfg.controller.account       = "customer1";                      // The Account name.
    cfg.controller.access_key    = "5c139c01-cc8b-4b34-9166-d75236150dcd";      // Key shown under "license" within the Controller.
    cfg.app_name                 = "C_SDK_test_App";                 // The Applicaion name string that will appear in the U/I.
    cfg.tier_name                = "C_SDK_test_Tier1";               // The Tier name string that will appear in the U/I. 
    cfg.node_name                = "C_SDK_test_Tier1_Node1";         // The Node name string that will appear in the U/I. 

#ifdef _WIN32
    cfg.agent_proxy.tcp_control_port =   10101;                      // Windows
    cfg.agent_proxy.tcp_reporting_port = 10102;                      // Windows
    cfg.agent_proxy.tcp_request_port =   10103;                      // Windows
#else
    cfg.agent_proxy.ipc_comm_dir = "";                               // Linux
#endif

    fprintf(stdout, "\n\n\nConnecting to Proxy & Controller using the following:\n");
    fprintf(stdout, "Controller Host:       \"%s\"\n", cfg.controller.host);
    fprintf(stdout, "Controller Port:       %d\n", cfg.controller.port);
    fprintf(stdout, "Use SSL:               \"%s\"\n", cfg.controller.use_ssl ? "ON" : "OFF");
    fprintf(stdout, "Controller Account:    \"%s\"\n", cfg.controller.account);
    fprintf(stdout, "Controller Key:        \"%s\"\n", cfg.controller.access_key);
    fprintf(stdout, "Application:           \"%s\"\n", cfg.app_name);
    fprintf(stdout, "Tier:                  \"%s\"\n", cfg.tier_name);
    fprintf(stdout, "Node:                  \"%s\"\n", cfg.node_name);
    fprintf(stdout, "Timeout:               %d\n", cfg.init_timeout_ms);
    fprintf(stdout, "Loop Count:            %d\n", loop_cnt_total);
    fprintf(stdout, "Error Rate:            %d%%\n", error_rate_pct);

#ifdef _WIN32
    fprintf(stdout, "Proxy Control Port:    %d\n", cfg.agent_proxy.tcp_control_port);
    fprintf(stdout, "Proxy Reporting Port:  %d\n", cfg.agent_proxy.tcp_reporting_port);
    fprintf(stdout, "Proxy Request Port:    %d\n", cfg.agent_proxy.tcp_request_port);
#else
    fprintf(stdout, "Proxy IPC Comm Dir:    \"%s\"\n", cfg.agent_proxy.ipc_comm_dir);
#endif


    fprintf(stdout, "\n\n\nWaiting at most %d seconds for Controller connection and context response...\n", cfg.init_timeout_ms / 1000);

    rc = appd_sdk_init(&cfg);                                        // Having it return NULL here usually indicates that the proxy isn't running, but
                                                                     // If the timeout value was set to short you may also get this error. All following
                                                                     // SDK calls will be ignored if there isn't a valid SDK initialization context.
    if (rc)
    {
        fprintf(stderr, "appd_sdk_init() returned NULL, is the proxy running?\n\n");
        exit(FAIL_RC);
    }

    fprintf(stdout, "\n\nCreating Backend \"%s\"...\n", backend_name);

    appd_backend_declare(backend_type, backend_name);                // Create a backend.   

    rc = appd_backend_set_identifying_property(backend_name, backend_key, backend_value);       // You must assign it at least one property.

    if (rc)
    {
        fprintf(stderr, "appd_backend_set_identifying_propert(\"%s\", \"%s\", \"%s\", ) failed.\n\n", backend_name, backend_key, backend_value);
        exit(FAIL_RC);
    }

    rc = appd_backend_add(backend_name);                             // You must add it to the context.

    if (rc)
    {
        fprintf(stderr, "(\"%s\") failed.\n\n", backend_name);
        exit(FAIL_RC);
    }

    rc = appd_backend_prevent_agent_resolution(backend_name);        // Use this call to tell the Controller to display this
                                                                     // backend (normally they wouldn't be) instead of the 
                                                                     // downstream Agent on the flowmap.
    if (rc)
    {
        fprintf(stderr, "appd_backend_prevent_agent_resolution() failed.\n\n");
        exit(FAIL_RC);
    }

    for (loop_count = 0 ; loop_count < loop_cnt_total ; loop_count++) {

        fprintf(stdout, "Creating BT \"%s\"...\n", bt_name);
        bt_handle = appd_bt_begin(bt_name, NULL);                        // Create a BT.   
            
#ifdef _4_2_1_or_later
        appd_bt_store(bt_handle, bt_guid);                               // It is possible to store a bt_handle in one thread and pass it along to
#endif
        if (loop_count % (100 / error_rate_pct) == 0) {
            appd_bt_add_error(bt_handle, bt_error_level,                // Mark the BT as an error (or whatever you've set the flag & level to).
                    bt_error_msg, bt_error_flag);
        }

        rc = appd_bt_is_snapshotting(bt_handle);                         // Find out if the BT in question was snapshotting.

        if (rc)                                                          // If it was marked as an error, it should be, otherwise note.
        {
            // fprintf(stdout, "BT is snapshotting.\n");
        }
        else
        {
            // fprintf(stdout, "BT is not snapshotting.\n");
        }

        appd_bt_add_user_data(bt_handle, bt_key, bt_value);              // Attach user defined data to this BT.

        fprintf(stdout, "Creating Exitcall for Backend \"%s\"...\n", backend_name);
        ec_handle = appd_exitcall_begin(bt_handle, backend_name);        // Exit Call Creation.

#ifdef _4_2_1_or_later
        appd_exitcall_store(ec_handle, ec_guid);                         // Store the handle of the Exit Call, we'll use this guid ID to retrieve the handle via appd_exitcall_get().
#endif // _4_2_1_or_later


        rc = appd_exitcall_set_details(ec_handle, bt_name);

        if (rc)
        {
            printf("appd_exitcall_set_details(%p, \"%s\") failed.", ec_handle, bt_name);
            exit(FAIL_RC);
        } 

        appd_exitcall_add_error(ec_handle, ec_error_level, ec_error_msg, ec_error_flag);

#ifdef _4_2_1_or_later
        ec_guid_handle = appd_exitcall_get(ec_guid);                     // Test the BT set/get FunctionalityRetrieve the handle of 
                                                                         // the Exit Call earlier assoicated with this guid ID via 
                                                                         // the appd_exitcall_set().
        if (ec_guid_handle != ec_handle)
        {
            fprintf(stderr, "appd_exitcall_get(\"%s\") returned mismatched handle %p, should have been %p.\n\n", ec_guid, ec_guid_handle, ec_handle);
            exit(FAIL_RC);
        }
#endif // _4_2_1_or_later

        fprintf(stdout, "Ending Exitcall...\n");
        appd_exitcall_end(ec_handle);                                    // Exit Call Termination

#ifdef _4_2_1_or_later
        bt_guid_handle = appd_bt_get(bt_guid);                           // Test the BT set/get Functionality. We stored a bt_handle (based on 
                                                                         // a bt_guid string we created) earlier, now
                                                                         // retrieve it and see if it still matches.
        if (bt_guid_handle != bt_handle)                                 // If it doesn't match, something is very wrong.
        {
            fprintf(stderr, "appd_bt_get(\"%s\") returned mismatched handle %p, should have been %p.\n\n", bt_guid, bt_guid_handle, bt_handle);
            exit(FAIL_RC);
        }
#endif // _4_2_1_or_later

        fprintf(stdout, "Ending BT...\n");
        appd_bt_end(bt_handle);                                          // BT Termination: Completes the BT.
    }

    fprintf(stdout, "Terminating SDK...\n");
    appd_sdk_term();                                                 // Context Termination: we're done.

    fprintf(stdout, "\n\nCompleted successfully.\n\n");

}  // end of main().

// end of C_SDK_example.c file.
