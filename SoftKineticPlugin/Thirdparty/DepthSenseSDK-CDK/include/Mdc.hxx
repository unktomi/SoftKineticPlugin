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

#pragma once
#ifndef __DEPTHSENSE_MDC_HXX__
#define __DEPTHSENSE_MDC_HXX__

#include "MdcAPI.hxx"

#ifdef __cplusplus
extern "C" {
#endif

    enum MdcErrorCode
    {
        MDC_SUCCESS = 0,
        MDC_CONNECTION_FAILURE = -1,    ///< Could not connect to the mock enumerator
        MDC_COMMUNICATION_ERROR = -2,   ///< Could not send/receive message to/from the mock enumerator, a disconnection probably occurred during the request
        MDC_INCONSISTENT_REPLY = -3,    ///< Answer from the mock enumerator is not coherent
        MDC_WRONG_PARAMETER = -4,       ///< The parameter provided is not correct (out of bound, etc.)
        MDC_BAD_HANDLE = -5,            ///< The handle provided does not exist.
    };

    /// Add a mock device.
    ///
    /// Add a mock device with a specific serial number, a specified number of depth, color and audio nodes.
    ///
    /// \param serial the new device's serial number.
    /// \param depthNodesCount the number of depth nodes part of the new device.
    /// \param colorNodesCount the number of color nodes part of the new device.
    /// \param audioNodesCount the number of audio nodes part of the new device.
    /// \return a handle to the newly added device, the error code if < 0.
    /// Possible error codes: MDC_CONNECTION_FAILURE, MDC_COMMUNICATION_ERROR, MDC_INCONSISTENT_REPLY, MDC_WRONG_PARAMETER.
    MDC_API int MDC_addDevice(const char* serial, int depthNodesCount, int colorNodesCount, int audioNodesCount, unsigned int REV, unsigned int PID, unsigned int VID, const char* fileNames[] = 0, const char* yaml = 0);

    /// Remove a mock device.
    ///
    /// Remove a mock device.
    ///
    /// \param handle the handle of the device to remove.
    /// \return 0 on success, the error code if < 0.
    /// Possible error codes: MDC_CONNECTION_FAILURE, MDC_COMMUNICATION_ERROR, MDC_INCONSISTENT_REPLY, MDC_WRONG_PARAMETER, MDC_WRONG_HANDLE.
    MDC_API int MDC_removeDevice(int handle);

    /// Get the number of mock devices.
    ///
    /// Get the number of mock devices.
    ///
    /// \return the number of mock devices, the error code if < 0.
    /// Possible error codes: MDC_CONNECTION_FAILURE, MDC_COMMUNICATION_ERROR, MDC_INCONSISTENT_REPLY.
    MDC_API int MDC_getDeviceCount();

    /// Get the handle list of currently connected mock devices.
    ///
    /// Get the handle list of currently connected mock devices.
    /// This function will allocate an integer table using malloc and populate it with the device handles.
    /// It then returns the number of elements in this table.
    /// It's the responsibility of the user to deallocate this table using free().
    ///
    /// \param handles the list of handles.
    /// \return the number of device handles in the list, the error code if < 0.
    /// Possible error codes: MDC_CONNECTION_FAILURE, MDC_COMMUNICATION_ERROR, MDC_INCONSISTENT_REPLY.
    MDC_API int MDC_getDeviceHandles(int** handles);

    /// Only there so that the C# wrapper can call free().
    MDC_API void MDC_freeIntPtr(void* ptr);

#ifdef __cplusplus
}
#endif

#endif // __DEPTHSENSE_MDC_HXX__