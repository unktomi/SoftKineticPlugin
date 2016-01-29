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
//				info@softkinetic.com
//
// Copyright (c) 2002-2015 Softkinetic Software SA
/******************************************************************************/

#pragma once

#ifndef SOFTKINETIC_CI_STATUSES_INCLUDED
#define SOFTKINETIC_CI_STATUSES_INCLUDED

#include <Platform/SkIntTypes.h>

// Global constants
#define CI_HAND_COUNT    2
#define CI_FINGER_COUNT  5

/// \brief Statuses for detected hands and fingers
enum CITrackingStatus
{
	CI_STATUS_INACTIVE		= 0,   ///< The hand or finger is not tracked
	CI_STATUS_DETECTED		= 1,   ///< The hand or finger is detected for the first time
	CI_STATUS_TRACKED		= 2,   ///< The hand or finger is tracked
	CI_STATUS_EXTRAPOLATED	= 3    ///< Unused
};


/// \brief Statuses for centroids
enum CICentroidStatus
{
	CI_CENTROID_TIME_COHERENT = 0,  ///< The centroid is correctly tracked with regards to the last frame
	CI_CENTROID_JUMP          = 1   ///< The centroid has jumped position and is not reliably tracked
};


/* \brief Handedness of detected hand

Once the handedness has been detected, it stays the same until
tracking of the hand is lost.

Every detected hand starts as CI_HANDSIDE_UNKNOWN, until CIlib collects enough
information to classify whether it's a left or right hand.
*/
enum CIHandSide
{
	CI_HANDSIDE_UNKNOWN = 0,    ///< CIlib cannot reliably classify the handedness
	CI_HANDSIDE_LEFT    = 1,    ///< The hand is detected as a left hand
	CI_HANDSIDE_RIGHT   = 2     ///< The hand is detected as a right hand
};


/// \brief Classification of the hand openness
enum CIOpennessStatus
{
	CI_OPENNESS_STATUS_UNKNOWN	= 0,   ///< CIlib cannot reliably detect if the hand is open or closed
	CI_OPENNESS_STATUS_CLOSED	= 1,   ///< The hand is detected as closed
	CI_OPENNESS_STATUS_OPEN	= 2        ///< The hand is detected as open
};



#endif
