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
#ifndef CDEPTHSENSE_API
#define CDEPTHSENSE_API

#if defined(CDS_EXPORT)
    #if defined(_MSC_VER)
        #define CDS_API  __declspec(dllexport)
    #elif defined(__GNUC__)
        #define CDS_API  __attribute__((visibility("default")))
    #else
        #error Unsupported compiler
    #endif
#else
    #if defined(_MSC_VER)
        #define CDS_API  __declspec(dllimport)
    #elif defined(__GNUC__)
        #define CDS_API  __attribute__((visibility("default")))
    #else
        #error Unsupported compiler
    #endif
#endif

#endif
