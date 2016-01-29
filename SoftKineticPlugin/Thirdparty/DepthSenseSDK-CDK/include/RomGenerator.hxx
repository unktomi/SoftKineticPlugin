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
#ifndef __DEPTHSENSE_ROM_GENERATOR_HXX__
#define __DEPTHSENSE_ROM_GENERATOR_HXX__

#include <iostream>
#include "RomGeneratorAPI.hxx"

#ifdef __cplusplus
extern "C" {
#endif

    ROM_GENERATOR_API int romGenerator_generate(unsigned char* buffer, unsigned int length, const char* yaml,
                                                unsigned int rom_start_offset, unsigned int rom_chunk_size, unsigned int rom_forward_storage,
                                                std::ostream& out);

    ROM_GENERATOR_API int romGenerator_generate_cout(unsigned char* buffer, unsigned int length, const char* yaml,
                                                unsigned int rom_start_offset, unsigned int rom_chunk_size, unsigned int rom_forward_storage);

    ROM_GENERATOR_API int romGenerator_generate_auto(const unsigned char* buffer, unsigned int length, const char* yaml, const char* outputPath,
                                                     std::ostream& out);

    ROM_GENERATOR_API int romGenerator_generate_auto_cout(const unsigned char* buffer, unsigned int length, const char* yaml, const char* outputPath);

    ROM_GENERATOR_API const char* romGenerator_getLastErrorMessage();

#ifdef __cplusplus
}
#endif

#endif // __DEPTHSENSE_ROM_GENERATOR_HXX__

