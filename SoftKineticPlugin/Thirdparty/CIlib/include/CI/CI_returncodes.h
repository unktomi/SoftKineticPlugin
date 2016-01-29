/******************************************************************************/
// SOFTKINETIC DISCLAIMER
//
// COPYRIGHT AND CONFIDENTIALITY NOTICE - SOFTKINETIC CONFIDENTIAL INFORMATION
// The source code of the SoftKinetic CI library is proprietary and confidential
// information of Softkinetic Software SA.
// All rights reserved to SOFTKINETIC SOFTWARE SA (a company incorporated and
// existing under the laws of Belgium, with its principal place of business at
// Boulevard de la Plainelaan 11, 1050 Brussels (Belgium), registered with
// the Crossroads bank for enterprises under company number
// 0890 705 864 - "Softkinetic Software")
//
// For any question about terms and conditions, please contact:
//
//              info@softkinetic.com
//
// Copyright (c) 2002-2015 Softkinetic Software SA
/******************************************************************************/
#pragma once

#ifndef SOFTKINETIC_CILIB_RETURNCODES_INCLUDED
#define SOFTKINETIC_CILIB_RETURNCODES_INCLUDED

/*!
\brief Enumerated type for the possible return codes of CIlib functions

\rst

.. note::

   The return codes are shared for all API functions, but some only
   make sense for a subset of these functions.

\endrst
*/
typedef enum
{
    CI_SUCCESS                          = 0,    ///< Function executed without issue
    CI_INIT_NO_DATABASE                 = 1,    ///< Initialization failed because of a missing database
    CI_INIT_SHUTDOWN_NEEDED             = 2,    ///< Initialization was called with a handle which was not shut down yet
    CI_MESH_STEP_OUT_OF_RANGE           = 3,    ///< The chosen mesh step is not in the accepted range
    CI_CENTROID_COUNT_OUT_OF_RANGE      = 4,    ///< The chosen centroid count is not in the accepted range
    CI_NOT_INITIALIZED                  = 5,    ///< Attempted to use an API function without initializing it first
    CI_NOT_IMPLEMENTED_YET              = 6,    ///< The API function has no implementation
    CI_ACTIVATION_DISTANCE_OUT_OF_RANGE = 7,    ///< The chosen activation distance is out of range
    CI_INVALID_GESTURE_NAME             = 8,    ///< The given gesture name is not supported by CIlib
	CI_INVALID_HANDLE                   = 9,    ///< The given handle does not correspond to a handle created with CI_initialize()
    CI_FAILURE                          = 10,   ///< Generic code for undocumented failures
	CI_NULL_DATAPOINTER                 = 11,   ///< The given handle is a NULL pointer
	CI_MISSING_COLOR_IMAGE              = 13,   ///< The API function requires a color image stream
	CI_INVALID_EXPERIENCE_MODE          = 14,   ///< The given experience mode identifier is invalid

    CI_ERRORTYPE_COUNT

} CIReturnCode;

#endif
