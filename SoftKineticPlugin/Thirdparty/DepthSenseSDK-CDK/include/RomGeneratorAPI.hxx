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
#ifndef __ROM_GENERATOR_API_HXX__
#define	__ROM_GENERATOR_API_HXX__

#ifndef DEPTHSENSE_DOC
#if defined(ROM_GENERATOR_EXPORTS)
#
#  if defined(_MSC_VER)
#    define ROM_GENERATOR_API  __declspec(dllexport)
#  elif defined(__GNUC__)
#    define ROM_GENERATOR_API  __attribute__((visibility("default")))
#  else
#    error Unsupported compiler
#  endif // _MSC_VER || __GNUC__
#
#elif defined(ROM_GENERATOR_STATIC)
#
#    define ROM_GENERATOR_API
#
#else // !ROM_GENERATOR_EXPORTS
#
#  if defined(_MSC_VER)
#    define ROM_GENERATOR_API  __declspec(dllimport)
#  elif defined(__GNUC__)
#    define ROM_GENERATOR_API  __attribute__((visibility("default")))
#  else
#    error Unsupported compiler
#  endif // _MSC_VER || __GNUC__
#
#endif // ROM_GENERATOR_EXPORTS
#else
#define ROM_GENERATOR_EXPORTS
#endif // DEPTHSENSE_DOC

#endif // __ROM_GENERATOR_API_HXX__
