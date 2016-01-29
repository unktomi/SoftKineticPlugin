////////////////////////////////////////////////////////////////////////////////
// SoftKinetic DepthSense SDK
//
// COPYRIGHT AND CONFIDENTIALITY NOTICE - SOFTKINETIC CONFIDENTIAL
// INFORMATION
//
// All rights reserved to SOFTKINETIC SENSORS NV (a
// company incorporated and existing under the laws of Belgium, with
// its principal place of business at Boulevard de la Plainelaan 11,
// 1050 Brussels (Belgium), registered with the Crossroads bank for
// enterprises under company number 0811 341 454 - "Softkinetic
// Sensors").
//
// The source code of the SoftKinetic DepthSense Camera Drivers is
// proprietary and confidential information of Softkinetic Sensors NV.
//
// For any question about terms and conditions, please contact:
// info@softkinetic.com Copyright (c) 2002-2015 Softkinetic Sensors NV
////////////////////////////////////////////////////////////////////////////////

/**
    \file cdepthsense.h

    \brief C bindings for DepthSenseSDK.
*/

/**
    \mainpage The C bindings for DepthSenseSDK Reference manual

    \section overview Overview

    The C bindings for DepthSenseSDK provides a high level API to use DepthSenseSDK cameras.

    Note that the bindings are not thread safe and hence the thread safety should be handled
    by the caller.

    \section initialization Initializing the library

    The depthsense_initialize() and depthsense_initialize_standalone() APIs allow to initialize 
    the library. The former starts DepthSenseSDK in client server mode while the latter starts 
    DepthSenseSDK in standalone mode. Client server mode allows multiple context to be run in 
    parallel while the standalone mode guarantees that at most one context is running.

    The depthsense_shutdown() API must be called to shutdown the library.

    \code{.cpp}

    int main()
    {
        depthsense_handle_t* handle = depthsense_initialize();
        if ( handle != nullptr )
        {
            // ...
            depthsense_shutdown(handle);
        }
    }

    \endcode

    \section device_selection Selecting a device

    The current bindings will automatically select the first depth interface. It is planned in
    a near future that the user can select any device. It should be noted that the device must
    be connected before the library is initialized.


    \section device_configuration Configuring the device
    
    The current bindings only provides APIs to enable or disable the auto configuration of the 
    device, see depthsense_disable_auto_configuration() and depthsense_enable_auto_configuration(), 
    and to set the expected payload size, see depthsense_set_raw_payload_size_per_frame().

    \code{.cpp}

    int main()
    {
        depthsense_handle_t* handle = depthsense_initialize();
        if ( handle != nullptr )
        {
            depthsense_error_code err = depthsense_disable_auto_configuration(handle);
            if ( err != depthsense_error_code_success )
            {
                printf("Failed to disable auto configuration\n");
            }

            err = depthsense_set_raw_payload_size_per_frame(handle, 307200);
            if ( err != depthsense_error_code_success )
            {
                printf("Failed to set payload size\n");
            }

            depthsense_shutdown(handle);
        }
    }

    \endcode

    \section streaming Getting the stream

    Once the library has been initialized, the user can supply callbacks for different types
    of output, namely confidence, depth, vertices and raw data. The stream can be enabled by 
    a call to depthsense_start() and stopped by a call to depthsense_stop(). The depthsense_start()
    function is a blocking function and will get unblocked when depthsense_stop() will be called.
    As soon as a new frame is available, the registered callbacks will be called successively,
    from the same thread and always in the same order.

    The callbacks will receive the following data:
    - A pointer to the data
    - The size in bytes if the buffer
    - The timestamp at which the frame was captured
    - The width and height of the frame if applicable

    The data held in the buffer is guaranteed to be valid only during the execution of the callback.
    In order to not miss frames, the time spent in the callback should be minimal.
    
    In some conditions all the callbacks but the one for raw data might not be triggered even if 
    the stream is active. This is the case when the user explicitely set the expected payload size,
    see depthsense_set_raw_payload_size_per_frame().

    \code{.cpp}
    
    // The callback to be called whenever a depth frame is available.
    void depth_callback(const int16_t* data, int32_t size, int32_t width, int32_t height, uint64_t timestamp)
    {
        printf("New depth frame received\n");
    }

    int main()
    {
        depthsense_handle_t* handle = depthsense_initialize();
        if ( handle != nullptr )
        {
            depthsense_error_code err = depthsense_enable_depth_stream(handle, &depth_callback);
            if ( err == depthsense_error_code_success )
            {
                err = depthsense_start(handle);
                if ( err != depthsense_error_code_success )
                {
                    printf("Failed to start stream\n");
                }
            }
            else
            {
                printf("Failed to enable the depth stream\n");
            }

            depthsense_shutdown(handle);
        }
    }

    \endcode


*/
#pragma once

#ifndef SOFTKINETIC_CDEPTHSENSE
#define SOFTKINETIC_CDEPTHSENSE

#include "cdepthsense_api.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct depthsense_handle_t;

/**
    \brief The callback definition for the raw stream.

    The callback definition for the raw stream.

    \param[in] data a pointer to the raw data.
    \param[in] size the size of the array in bytes.
    \param[in] timestamp the timestamp of the sample expressed in &mu;s.
*/
typedef void(*raw_stream_callback_t)(const uint8_t* data, int32_t size, uint64_t timestamp);

/**
    \brief The callback definition for the depth stream.

    The callback definition for the depth stream.

    \param[in] data a pointer to the depth data.
    \param[in] size the size of the array in bytes.
    \param[in] width the width of the frame.
    \param[in] height the height of the frame.
    \param[in] timestamp the timestamp of the sample expressed in &mu;s.
*/
typedef void(*depth_stream_callback_t)(const int16_t* data, int32_t size, int32_t width, int32_t height, uint64_t timestamp);

/**
    \brief The callback definition for the confidence stream.

    The callback definition for the confidence stream.

    \param[in] data a pointer to the confidence data.
    \param[in] size the size of the array in bytes.
    \param[in] width the width of the frame.
    \param[in] height the height of the frame.
    \param[in] timestamp the timestamp of the sample expressed in &mu;s.
*/
typedef void(*confidence_stream_callback_t)(const int16_t* data, int32_t size, int32_t width, int32_t height, uint64_t timestamp);

/** 
    \brief A vertex expressed in floating point
*/
typedef struct depthsense_vertex_floating_point
{
    /** \brief The x coordinate in meters */
    float x;

    /** \brief The y coordinate in meters */
    float y;

    /** \brief The z coordinate in meters */
    float z;
} depthsense_vertex_floating_point;

/**
    \brief The callback definition for the vertices floating point stream.

    The callback definition for the vertices floating point stream.

    \param[in] data a pointer to the vertices floating point data.
    \param[in] size the size of the array in bytes.
    \param[in] width the width of the frame.
    \param[in] height the height of the frame.
    \param[in] timestamp the timestamp of the sample expressed in &mu;s.
*/
typedef void(*vertices_floating_point_stream_callback_t)(const depthsense_vertex_floating_point* data, int32_t size, int32_t width, int32_t height, uint64_t timestamp);

/**
    \brief The depthsense_error_code enum.
*/
enum depthsense_error_code
{
    depthsense_error_code_success = 0, ///< the method returned with success.
    depthsense_error_code_other_error = 1, ///< the method returned with failure.
    depthsense_error_code_invalid_handle = 2, ///< the input handle is invalid.
    depthsense_error_code_no_device_found = 3, ///< no device was found.
};

/**
    \brief The version of the library
*/
typedef struct depthsense_version
{
    /** \brief the major version number */
    int32_t major;

    /** \brief the minor version number */
    int32_t minor;

    /** \brief the patch version number */
    int32_t patch;
} depthsense_version;

/**
    \brief Returns the version information of the C++ library.

    This function returns the version information of the C++ library.

    \return The version of the C++ library.
*/
CDS_API depthsense_version depthsense_get_library_version();

/**
    \brief Initializes DepthSenseSDK in client/server mode.
   
    This function initializes a DepthSenseSDK context object in client/server mode and register the first depth node found.
    
    \return A handle. The handle will be nullptr in case of failure.
    \sa depthsense_shutdown(depthsense_handle_t* handle)
    \sa depthsense_initialize_standalone()
*/
CDS_API depthsense_handle_t* depthsense_initialize();

/**
    \brief Initializes DepthSenseSDK in standalone mode.

    This function initializes a DepthSenseSDK context object in standalone mode and register the first depth node found.
    A standalone context guarantees that there is a single instance of DepthSenseSDK running.

    \return A handle. The handle will be nullptr in case of failure.
    \sa depthsense_shutdown(depthsense_handle_t* handle)
    \sa depthsense_initialize()
*/
CDS_API depthsense_handle_t* depthsense_initialize_standalone();

/**
    \brief Enables the raw data stream.
   
    This function enables the raw data stream for the selected device. The payload data received in the callback is valid
    only during the callback.
   
    \param[in] handle A pointer to a depthsense_handle_t.
    \param[in] callback A pointer to a raw stream handler.
    \return Whether the call succeeded or not.
*/
CDS_API depthsense_error_code depthsense_enable_raw_data_stream(depthsense_handle_t* handle, raw_stream_callback_t callback);

/**
    \brief Enables the confidence stream.

    This function enables the confidence stream for the selected device. The payload data received in the callback is valid
    only during the callback. In the case the depthsense_set_raw_payload_size_per_frame() has been called, the callback might
    not be triggered.

    \param[in] handle A pointer to a depthsense_handle_t.
    \param[in] callback A pointer to a confidence stream handler.
    \return whether the call succeeded or not.
    \sa depthsense_set_raw_payload_size_per_frame(depthsense_handle_t* handle, int32_t payload_size)
*/
CDS_API depthsense_error_code depthsense_enable_confidence_stream(depthsense_handle_t* handle, confidence_stream_callback_t callback);

/**
    \brief Set the confidence threshold for the selected device.

    This function set the confidence threshold for the selected device. This method requires the device not to be 
    controlled by any other DepthSenseSDK context.

    \param[in] handle A pointer to a depthsense_handle_t.
    \param[in] confidence_threshold The confidence threshold.
    \return Whether the call succeeded or not.
*/
CDS_API depthsense_error_code depthsense_set_confidence_threshold(depthsense_handle_t* handle, int16_t confidence_threshold);

/**
    \brief Enables the depth stream.

    This function enables the depth stream for the selected device. The payload data received in the callback is valid
    only during the callback. In the case the depthsense_set_raw_payload_size_per_frame() has been called, the callback might
    not be triggered.

    \param[in] handle A pointer to a depthsense_handle_t.
    \param[in] callback A pointer to a depth stream handler.
    \return Whether the call succeeded or not.
    \sa depthsense_set_raw_payload_size_per_frame(depthsense_handle_t* handle, int32_t payload_size)
*/
CDS_API depthsense_error_code depthsense_enable_depth_stream(depthsense_handle_t* handle, depth_stream_callback_t callback);

/**
    \brief Enables the vertices floating point stream.

    This function enables the vertices floating point stream for the selected device. The payload data received in the callback is valid
    only during the callback. In the case the depthsense_set_raw_payload_size_per_frame() has been called, the callback might
    not be triggered.

    \param[in] handle A pointer to a depthsense_handle_t.
    \param[in] callback A pointer to a vertices floating point stream handler.
    \return Whether the call succeeded or not.
    \sa depthsense_set_raw_payload_size_per_frame(depthsense_handle_t* handle, int32_t payload_size)
*/
CDS_API depthsense_error_code depthsense_enable_vertices_floating_point_stream(depthsense_handle_t* handle, vertices_floating_point_stream_callback_t callback);

/**
    \brief Starts the stream.
   
    This function starts the stream and the main loop of DepthSenseSDK.
    This function is blocking and will be unblocked whenever \c depthsense_stop is called.
   
    \param[in] handle A pointer to a depthsense_handle_t.
    \return Whether the call succeeded or not.
    \sa depthsense_stop(depthsense_handle_t* handle)
*/
CDS_API depthsense_error_code depthsense_start(depthsense_handle_t* handle);

/**
    \brief Stops the stream.
   
    This function stops the stream and exit the main loop of DepthSenseSDK.
   
    \param[in] handle A pointer to a depthsense_handle_t.
    \return Whether the call succeeded or not.
    \sa depthsense_start(depthsense_handle_t* handle)
*/
CDS_API depthsense_error_code depthsense_stop(depthsense_handle_t* handle);

/**
    \brief Shutdown DepthSenseSDK.
   
    This function releases the DepthSenseSDK context.
   
    \param[in] handle A pointer to a depthsense_handle_t.
    \return Whether the call succeeded or not.
    \sa depthsense_initialize()
    \sa depthsense_initialize_standalone()
*/
CDS_API depthsense_error_code depthsense_shutdown(depthsense_handle_t* handle);

/**
    \brief Enables the auto-configuration of the device.
   
    This function set the DepthSense::DepthNode::DisableAutoConfiguration property to the desired state.
    When auto-configuration is enabled, DepthSenseSDK will automatically configure the device before
    starting to fetch the data and will shutdown the stream of the device when the stop function is called.
    This method requires the device not to be controlled by any other DepthSenseSDK context.
   
    \param[in] handle A pointer to a depthsense_handle_t.
    \return Whether the call succeeded or not.
    \sa depthsense_start(depthsense_handle_t* handle)
    \sa depthsense_stop(depthsense_handle_t* handle)
*/
CDS_API depthsense_error_code depthsense_enable_auto_configuration(depthsense_handle_t* handle);

/**
    \brief Disables the auto-configuration of the device.

    This function set the DepthSense::DepthNode::DisableAutoConfiguration property to the desired state.
    When auto-configuration is disabled, DepthSenseSDK will not configure the device before starting to fetch the
    data and will not shutdown the stream of the device when the stop function is called.
    This method requires the device not to be controlled by any other DepthSenseSDK context.

    \param[in] handle A pointer to a depthsense_handle_t.
    \return Whether the call succeeded or not.
    \sa depthsense_start(depthsense_handle_t* handle)
    \sa depthsense_stop(depthsense_handle_t* handle)
*/
CDS_API depthsense_error_code depthsense_disable_auto_configuration(depthsense_handle_t* handle);

/**
    \brief Set the DepthSense::DepthNode::RawPayloadSizePerFrame property to the desired payload size.
   
    This function set the DepthSense::DepthNode::RawPayloadSizePerFrame property to the desired payload size.
    This method requires the device not to be controlled by any other DepthSenseSDK context and the auto-configuration
    to be disabled. Setting the payload size will invalidate the current configuration and thus only the raw data stream
    will be available. Any other registered callback than the raw data stream handler won't be called.
   
    \param[in] handle A pointer to a depthsense_handle_t.
    \param[in] payload_size The payload size to be expected by the DepthSenseSDK backend.
    \return Whether the call succeeded or not.
    \sa depthsense_disable_auto_configuration(depthsense_handle_t* handle)
    \sa depthsense_enable_auto_configuration(depthsense_handle_t* handle)
    \sa depthsense_enable_confidence_stream(depthsense_handle_t* handle, confidence_stream_callback_t callback)
    \sa depthsense_enable_depth_stream(depthsense_handle_t* handle, depth_stream_callback_t callback)
    \sa depthsense_enable_vertices_floating_point_stream(depthsense_handle_t* handle, vertices_floating_point_stream_callback_t callback)
*/
CDS_API depthsense_error_code depthsense_set_raw_payload_size_per_frame(depthsense_handle_t* handle, int32_t payload_size);


#ifdef __cplusplus
}
#endif

#endif
