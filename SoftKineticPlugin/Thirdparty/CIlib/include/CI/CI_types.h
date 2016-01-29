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

/// \file CI_types.h

#pragma once

#ifndef SOFTKINETIC_CILIB_TYPES_INCLUDED
#define SOFTKINETIC_CILIB_TYPES_INCLUDED

#include "Platform/SkIntTypes.h"   // uint32_t, int32_t, etc
#include "CI/CI_statuses.h"

/// Enumerated type for truth values
typedef enum {CI_FALSE=0, CI_TRUE=1} CIBool;

/// \brief Camera intrinsics parameters
typedef struct CICameraIntrinsicParameters
{
	/// image width, in pixels
	uint32_t width;
	/// image height, in pixels
	uint32_t height;
	/// The focal length along the x axis expressed in pixel units
	float fx;
	/// The focal length along the y axis expressed in pixel units
	float fy;
	/// The central point along the x axis expressed in pixel units
	float cx;
	/// The central point along the y axis expressed in pixel units
	float cy;
	/// The first radial distortion coefficient
	float k1;
	/// The second radial distortion coefficient
	float k2;
	/// The third radial distortion coefficient
	float k3;
	/// The first tangential distortion coefficient
	float p1;
	/// The second tangential distortion coefficient
	float p2;
} CICameraIntrinsicParameters;


/// \brief Transformation from the depth to color sensor, as provided by the DepthSense driver (row-major)
typedef struct CICameraExtrinsicParameters
{
	/// 3x3 Rotation matrix coefficient
	//@{
	float r11;
	float r12;
	float r13;
	float r21;
	float r22;
	float r23;
	float r31;
	float r32;
	float r33;
	//@}
	/// Translation vector coefficient
	//@{
	float t1;
	float t2;
	float t3;
	//@}
} CICameraExtrinsicParameters;


/// Alias for 4-channels pixeltype. The fourth channel is ignored.
typedef int32_t BGRx ;

/**
\brief Input data for the processing pipeline

The number of elements for the depth and confidence images are
tied to the width and height values of the depthCameraParameters
struct passed to CI_initialize().

The number of elements for the color image are tied to the
width and height values of the colorCameraParameters struct
passed to CI_initialize().

\rst
.. important::

	The size of these buffers **cannot** change during execution.
\endrst
*/
typedef struct CIFrameInputData
{
	/// Pointer to the input depth image buffer.
	int16_t* depthImage;
	/// Pointer to the input confidence image.
	int16_t* confidenceImage;
	/// Pointer to the input color image. Use NULL if unavailable
	BGRx* colorImage;

	/// time elapsed since last call to CI_setInputData(), in milliseconds. 0 for the first frame.
	double timeSinceLastFrame;
} CIFrameInputData;


/**
\brief Lightweight struct that describes a filtered point cloud

\rst
These 3 buffers represent de-interlaced 3D positions in depth camera space.
The dimensions are the same as the input depth image provided.

The following snippet shows how to iterate over all the vertices in the point cloud:

.. code-block:: cpp

	// ... <snip> ...
	void* CIHandle;
	CIFilteredData filteredData;
	// ... <snip> ...

	CI_updateFilteredData(CIHandle, &filteredData);

	for(int i=0; i<depth_width*depth_height; ++i)
	{
		std::cout << "vertex[" << i << "].x = " << filteredData.xImage[i] << std::endl;
		std::cout << "vertex[" << i << "].y = " << filteredData.yImage[i] << std::endl;
		std::cout << "vertex[" << i << "].z = " << filteredData.zImage[i] << std::endl;
	}


- Values greater than 32000 in ``yImage`` are used to flag invalid pixels.
- The pointer are non-owning pointers to buffers managed by CIlib. Their content
  will be modified next time this function is called.
- Each buffer has a size of ``width*height`` elements, width and height being the
  dimensions of the input depth image. The input depth image is configured with
  :cpp:func:`CI_initialize()`, with the first :cpp:type:`CICameraIntrinsicParameters`
  struct given as argument.
\endrst
*/
typedef struct CIFilteredData
{
	/// x coordinates, in millimeters
	int16_t* xImage;
	/// y coordinates, in millimeters (actual depth)
	int16_t* yImage;
	/// z coordinates, in millimeters
	int16_t* zImage;
}CIFilteredData;


/// Lightweight struct that describes a 3D point in space with floating point coordinates
typedef struct CIVector3
{
	/// 3D vector coefficient
	//@{
	float x, y, z;
	//@}
} CIVector3;


/// \brief Lightweight wrapper on an array of 3D points
typedef struct CIVector3Array
{
	/// Number of points in the array
	uint32_t count;
	/// Pointer to the first point in the array
	const struct  CIVector3* data;
} CIVector3Array;


//! \brief Lightweight struct that describes a 3D point in space with integer coordinates
typedef struct CIVector3i
{
	/// 3D vector coefficient
	//@{
	int32_t x, y,z;
	//@}
} CIVector3i;


/// \brief Lightweight wrapper on an array of 3D points in space with integer coordinates
typedef struct CIVector3iArray
{
	/// Number of points in the array
	uint32_t count;
	/// Pointer to the first point in the array
	const struct CIVector3i* data;
} CIVector3iArray;


/// \brief Lightweight struct that describes a 2D points with float coordinates
typedef struct CIVector2
{
	/// 2D vector coefficient
	//@{
	float x, y;
	//@}
} CIVector2;


/// \brief Lightweight wrapper on an array of 2D points with float coordinates
typedef struct CIVector2Array
{
	/// Number of points in the array
	uint32_t count;
	/// Pointer to the first point in the array
	const struct CIVector2* data;
} CIVector2Array;


/// \brief Lightweight wrapper on an array of integer values
typedef struct CIIntArray
{
	/// Number of values in the array
	uint32_t count;
	/// Pointer to the first value in the array
	const int32_t* data;
} CIIntArray;


/// \brief Lightweight wrapper on an array of float values
typedef struct CIFloatArray
{
	/// Number of values in the array
	uint32_t count;
	/// Pointer to the first value in the array
	const float* data;
} CIFloatArray;


/**
\brief Data computed for one hand on a single frame

- Every positional data is given in both 2D and 3D space
- The image coordinates units are pixels
- The depth camera 3D coordinates units are meters
*/
typedef struct CIHandData
{
	/// Whether a hand is active, detected or tracked
	CITrackingStatus status;
	/// The label value for this hand in the label image (see \a CIHandTrackingData.labelImage)
	int32_t label;

	/// Whether or not the hand is crossing the top image border
	CIBool isHandIntersectingUpperImageBound;
	/// Whether or not the hand is crossing the bottom image border
	CIBool isHandIntersectingLowerImageBound;
	/// Whether or not the hand is crossing the left image border
	CIBool isHandIntersectingLeftImageBound;
	/// Whether or not the hand is crossing the right image border
	CIBool isHandIntersectingRightImageBound;

	/// The 2D palm center position, in depth image coordinates
	CIVector2 position2D;
	/// The 3D palm center position, in depth camera space
	CIVector3 position3D;
	/// The 2D mass center position, in depth image coordinates
	CIVector2 massCenter2D;
	/// The 3D mass center position, in depth camera space
	CIVector3 massCenter3D;
	/// The 2D tip position, in depth image coordinates
	CIVector2 tipPosition2D;
	/// The 3D tip position, in depth camera space
	CIVector3 tipPosition3D;
	/// The 2D forearm position, in depth image coordinates
	CIVector2 forearmPosition2D;
	/// The 3D forearm position, in depth camera space
	CIVector3 forearmPosition3D;
	/// The 3D palm normal, in depth camera space
	CIVector3 palmNormal;

	/// Whether the hand was detected as a right or left hand.
	CIHandSide handSide;
}CIHandData ;


/**
\brief Data computed by the hand tracking

- Scene segmentation information is provided as a label image
- The label image is a single channel, 8-bit image. Each pixel has a value ranging from 0 to 255
- The label image width and height are the same as the input depth image dimensions
*/
typedef struct CIHandTrackingData
{
	/// Pointer to the 8-bit label image
	const uint8_t* labelImage;
	/// The value assigned to the background of the scene in the label image
	int32_t backgroundLabel;

	/// Hand tracking data, for all hands
	struct CIHandData hands[CI_HAND_COUNT];
} CIHandTrackingData;


/**
\brief Data computed for the fingers of one hand

\rst
- Each array has 5 entries
- Each entry in these arrays correspond to a finger, which can be active or not
- Once activated and tracked, the index of a finger is the same in all the arrays

.. important::

	The indices in these arrays **do not** correspond to finger IDs.
    The tracked fingers are not sorted or identified.

\endrst
*/
typedef struct CIFingersData
{
	/// Whether or not the finger is active, detected or tracked. See the CITrackingStatus enum for possible values.
	int32_t fingerStatus[CI_FINGER_COUNT];
	/// The 3D fingertip positions, in depth camera space
	CIVector3 fingerTipPosition3D[CI_FINGER_COUNT];
	/// The 2D fingertip positions, in depth image coordinates
	CIVector2 fingerTipPosition2D[CI_FINGER_COUNT];
	/// The 2D fingertip radiuses, in depth image coordinates
	float fingerTipRadius2D[CI_FINGER_COUNT];
	/// The 3D fingertip radiuses, in depth camera space
	float fingerTipRadius3D[CI_FINGER_COUNT];
} CIFingersData;


/// \brief Finger data for all hands
typedef struct CIFingersTrackingData
{
	/// Finger data per tracked hand. Indices are the same as those used in CIFHandTrackingData.hands
	CIFingersData hands[CI_HAND_COUNT];
} CIFingersTrackingData;


/// \brief Status and positions of a tracked head
typedef struct CIHeadTrackingData
{
	/// Whether a head was detected or not.
	CIBool headDetected;
	/// Head 3D position (3D camera space) if a headDetected is true, (0,0,0) otherwise.
	CIVector3 headPosition3D;
	/// Head 2D position (in depth image coordinates) if a headDetected is true, (0,0) otherwise.
	CIVector2 headPosition2D;
} CIHeadTrackingData;


/// \brief Vertex, normal and color buffers for a 3D mesh of a tracked hand
typedef struct CIMesh
{
	/// array of 3D coordinates of the points of the mesh
	struct CIVector3Array points3D;
	/// array of normal vectors to the mesh points
	struct CIVector3Array normals;
	///  array of 2D coordinates of the projected mesh points
	struct CIVector2Array points2D;
	/// array of intensities (or confidence) on the points of the mesh
	struct CIFloatArray intensities;

	/// Index buffer of triangles.
	/** array of triplet of indices, one triplet by triangle which constitute the mesh.
	    The indices are usable with the points3D, normals, points3D and intensities array.
	    To get the triangle count, divide the triangleList.count by 3.
	*/
	struct CIIntArray triangleList;
	/// The ordered list of indices of the points that describe the contour of the hand
	struct CIIntArray contourPoints;
} CIMesh;


/**
\brief Lightweight struct that describes the centroids of a tracked hand

Centroids are spheres that fill up the 3D space occupied a tracked hand. They
can be used for fast volume-based collision detection in a user interface.

The number of spheres can be adjusted using \a CI_setCentroidCountParameter
*/
typedef struct CICentroids
{
	/// 3D positions that correspond to the center of the centroids
	struct CIVector3Array positions;
	/// Whether or not the i^th sphere is detected as a jumper
	struct CIIntArray jumpStatus;
	/// The number of centroids
	uint32_t count;
} CICentroids;


/**
\brief Describe a name to id mapping for detected static (posing) gestures

Only gesture names are guaranteed to remain constant across
different runs and versions of the library.
*/
typedef struct CIPosingGestureMapping
{
	/// The gesture name
	const char* gestureName;
	/// The associated gesture ID
	int32_t gestureID;
} CIPosingGestureMapping;


/**
\brief Describe a list of supported static gestures and their ID

\note
    - Even though suffixed with "List", this structure is a wrapper of an array
*/
typedef struct CIPosingGestureMappingList
{
	/// Pointer to the first mapping in the list
	struct CIPosingGestureMapping* mappings;
	/// The number of gesture in the list
	uint32_t count;
} CIPosingGestureMappingList;


/// \brief Describe a name to id mapping for detected dynamic (moving) gestures
typedef struct CIMovingGestureMapping
{
	/// The gesture name
	const char* gestureName;
	/// The associated gesture ID
	int32_t gestureID;
} CIMovingGestureMapping;


/**
\brief Describe a list of supported dynamic gestures and their ID
\note
  - Even though suffixed with "List", this structure is a wrapper of an array
*/
typedef struct CIMovingGestureMappingList
{
	/// Pointer to the first mapping in the list
	CIMovingGestureMapping* mappings;
	/// The number of gesture in the list
	uint32_t count;
} CIMovingGestureMappingList;


/**
\brief Describe the static gesture status for all hands
\note
   - Use \a CI_initializeOneHandPosingGestureMappings to populate a list of
     mapping and associate the ID to a gesture name.
*/
typedef struct CIHandPosingGestureData
{
	/// Current pose ID, for all hands
	int32_t hands[CI_HAND_COUNT];
} CIHandPosingGestureData;


/**
\brief Lightweight struct to describe a detected moving gesture

To get a human-readable string of the event, use CI_getMovingGestureName().
To avoid doing string comparisons in the mainloop of your application, use
the gesture mapping mechanism to map the IDs to application-specific constants.
*/
typedef struct CIMovingGestureData
{
	/// The ID for the detected gesture.
	int32_t gestureID;
	/// The ID of the hand doing the gesture.
	int32_t handID1;
	/// Unused.
	int32_t handID2;
} CIMovingGestureData;


/**
\brief The list of moving gestures detected for the current frame

Array of detected moving gestures. Unlike for posing gestures,
moving gestures are per-frame events, and not states.
*/
typedef struct CIMovingGestureEventList
{
	/// Pointer to the first event in the array
	CIMovingGestureData* events;
	/// Number of events
	uint32_t count;
} CIMovingGestureEventList;


/// \brief Stable points for grabbing and pinching
typedef struct CIHandGrabberData
{
	/// Upper Grabber 2D position, in depth image coordinates
	CIVector2 upperGrabberPosition2D;
	/// Upper Grabber 3D position, in depth camera space
	CIVector3 upperGrabberPosition3D;
	/// Lower Grabber 2D position, in depth image coordinates
	CIVector2 lowerGrabberPosition2D;
	/// Lower Grabber 3D position, in depth camera space
	CIVector3 lowerGrabberPosition3D;
	/// Grabber pointer position, in depth image coordinates
	CIVector2 pointerPosition2D;
	/// Grabber pointer position, in depth camera space
	CIVector3 pointerPosition3D;
} CIHandGrabberData;


/// Enum defining the various supported experience modes
typedef enum
{
	CI_ORIGINAL_CI_DS_325=0,        ///< Mode user-facing close-interaction with a DS325 camera
	CI_HEADMOUNTED_VR_DS_325=1,     ///< Mode for DS325 attached to an head-mounted VR display
	CI_HEADMOUNTED_VR_DS_320_W=2,   ///< Mode for a non-commercialized camera. Do not use unless you know what you are doing.
	CI_EXPERIENCEMODES_COUNT        ///< The number of values in the enum
}CIExperienceMode;

#endif
