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

/// \file CI_api.h

#ifndef SOFTKINETIC_CI_API_INCLUDED
#define SOFTKINETIC_CI_API_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif


#if defined _WIN32 || defined _WIN64
	#if defined CI_EXPORTS
		#define CI_EXPORT __declspec(dllexport)
	#else
		#ifndef CI_NO_IMPORT
			#define CI_EXPORT __declspec(dllimport)
		#else
			#define CI_EXPORT
		#endif
	#endif
#else
	#define CI_EXPORT
#endif


#include "CI/CI_types.h"
#include "CI/CI_returncodes.h"
#include "CI/version.h"




/**
\brief Initialize the CI processing pipeline using camera settings from the driver.

\param[in]    dataRootPath             Path to the data folder shipped with CIlib (<em><cilib-dir>/bin/data</em>).
\param[in]    depthCameraParameters    Intrinsic parameters for the depth camera.
\param[in]    colorCameraParameters    Intrinsic parameters for the color camera. Pointer can be NULL if color camera is not used.
\param[in]    stereoCalibration        Calibration between the depth and color sensors. Pointer can be NULL if color camera is not used.
\param[out]   CI_handle                Handle of the instance of the CI processing pipeline

\return a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_initialize(const char* dataRootPath, CICameraIntrinsicParameters* depthCameraParameters, CICameraIntrinsicParameters* colorCameraParameters, struct CICameraExtrinsicParameters* stereoCalibration, void** CI_handle);


/**
\brief Shutdown the processing pipeline. Destroys all the internal data.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()

\return a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_shutdown(void* CI_handle);


/**
\brief	Set the experience and camera mode for CI to use, by default it's the Original_CI_DS_325 mode

\param[in]    CI_handle                Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    experienceMode           Value of the enum representing the experience mode to use

\return a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_setExperienceMode(void* CI_handle, CIExperienceMode experienceMode);


/**
\brief	Reset internal tracking state, must be called if temporal coherency is broken

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()

\return a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT  CIReturnCode CI_resetTracking(void* CI_handle);


/**
\brief Set data for a new frame, must be called before invoking any processing function.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    inputData               Frame data. The depthImage and timeSinceLastFrame fields are mandatory. colorImage and confidenceImage can be NULL if unused.

\return a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_setInputData(void* CI_handle, const CIFrameInputData* inputData);


/**
\brief	Check whether the camera is occluded or not. If it is called then it must be called before setFilteredData() or updateFilteredData().

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   isCameraOccluded	       True if the camera is occluded, false otherwise

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_checkCameraOcclusion(void* CI_handle, CIBool* isCameraOccluded);


/**
\brief	Filter out the depth map. Either this function or CI_setFilteredData must be called before CI_updateHandTrackingData.

This function will filter the depth image given by the last call to CI_setInputData().
It is the recommended way to filter noise in the depth-map with CIlib.


\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   outputFilteredData      Output filtered data. Pointer to an allocated CIFilteredData struct to retrieve the filtered data

\pre CI_setInputData() has been called

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_updateFilteredData(void* CI_handle, CIFilteredData* outputFilteredData);


/**
\brief	Set the filtered data. Either this function or CI_updateFilteredData must be called before CI_updateHandTrackingData().

\rst
.. important::

	This function must be called only in case you want to use CIlib with your own
	depth-map filtering algorithm in conjunction with :cpp:func:`CI_updateXZFilteredData()`.
\endrst

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]	   inputFilteredData       Depth buffer (yImage) filtered by your own filtering algorithm and unfiltered xImage and zImage

\pre CI_setInputData() has been called

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_setFilteredData(void* CI_handle, const CIFilteredData* inputFilteredData);


/**
\brief Adjust the X and Z coordinates of the point cloud, based on the filtered depth.

This function must be called in case you want to use CIlib with your own
depth-map filtering algorithm in conjunction with CI_setFilteredData().

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   outputFilteredData      Output filtered data. Pointer to an allocated CIFilteredData struct to retrieve the filtered data

\pre CI_setInputData() and CI_setFilteredData() have been called

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_updateXZFilteredData(void* CI_handle, CIFilteredData* outputFilteredData);


/**
\brief	Set the maximum distance from camera for a hand to be activated

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]	   distance                The distance from camera. Value can range from 0 to 1 meter.

\return	a CIReturnCode specifying if the call was successful or not (e.g. the value was out of bounds)
*/
CI_EXPORT CIReturnCode CI_setActivationMaxDistanceParameter(void* CI_handle, float distance);


/**
\brief	Retrieve the maximum distance from camera in meters for a hand to be activated

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()

\return	a float corresponding to the current parameter value. Returns 0.f if the given handle is invalid.
*/
CI_EXPORT float CI_getActivationMaxDistanceParameter(void* CI_handle);


/**
\brief	Process a new frame and extracts fundamental hand tracking data. Must be called after CI_updateFilteredData() or CI_setFilteredData().

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   trackingData            Pointer to an allocated CITrackingData struct to retrieve the tracking data

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_updateHandTrackingData(void* CI_handle, CIHandTrackingData* trackingData);


/**
\brief	Get fingers tracking data, for all active hands. Must be called after CI_updateHandTrackingData().

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   trackingData            Pointer to an allocated CIFingersTrackingData struct to copy the tracking data in

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_computeFingersTrackingData(void* CI_handle, CIFingersTrackingData* trackingData);


/**
\brief	Detect whether tracked hands is open or not. Must be called after CI_updateHandTrackingData() and CI_computeFingersTrackingData()

\param[in]     CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]    opennessStatus          Pointer to an allocated CIOpennessStatus array(of size CI_HAND_COUNT) to copy the openness status for all hands.
\param[out]    opennessValue           Pointer to an allocated float array(of size CI_HAND_COUNT) to copy the openness value for all hands. The openness value ranges from 0 (closed) to 1 (open).

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_computeHandOpenness(void* CI_handle, CIOpennessStatus* opennessStatus, float* opennessValue);


/**
\brief	Compute and retrieves a mesh for a given hand

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    handID                  ID of the hand (0 or 1) to get the data from
\param[out]   mesh                    Pointer to an allocated CIMesh struct to copy the data in

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_computeMesh(void* CI_handle, uint32_t handID, CIMesh* mesh);


/**
\brief	Compute and retrieves the centroids for a given hand

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    handID                  ID of the hand (0 or 1) to get the data from
\param[out]   centroids               Pointer to an allocated CICentroids struct to copy the data in

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_computeCentroids(void* CI_handle, uint32_t handID, CICentroids* centroids);


/**
\brief	Retrieve grabbing data for a given hand.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]	   handID                  ID of the hand (0 or 1) to get the data from
\param[out]   grabberData             Pointer to an allocated CIHandGrabberData struct to copy the data in

\pre the function CI_updateHandTrackingData() should be called before.

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_updateGrabberData(void* CI_handle, uint32_t handID, CIHandGrabberData* grabberData);


/**
\brief	Retrieve head tracking data.

A call to this function does not trigger any computing process, it only copies
head tracking information to an output struct.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by \ref CI_initialize()
\param[out]   headTrackingData        Pointer to an allocated CIHeadTrackingData struct to copy the data in

\pre the function CI_updateHandTrackingData() has been called before.

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_updateHeadTrackingData( void* CI_handle, CIHeadTrackingData* headTrackingData  );


/**
\brief	Set the resolution for mesh that will be generated when calling CI_computeMesh( void* CI_handle )

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    step                    Parameter to tweak the mesh resolution. Value ranges from 0.001 to 0.5

\return	a CIReturnCode specifying if the call was successful or not (e.g. the value was out of bounds)
*/
CI_EXPORT CIReturnCode CI_setMeshStepParameter(void* CI_handle, float step);


/**
\brief	Retrieve the current mesh step used when calling CI_computeMesh( void* CI_handle )

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()

\return	a float corresponding to the current parameter value
*/
CI_EXPORT float CI_getMeshStepParameter( void* CI_handle );


/**
\brief	Set the number of centroids that will be generated when calling CI_computeCentroids( void* CI_handle )

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    count                   The number of centroids to generate. Value can range from 1 to 500.

\return	a CIReturnCode specifying if the call was successful or not (e.g. the value was out of bounds)
*/
CI_EXPORT CIReturnCode CI_setCentroidCountParameter( void* CI_handle, uint32_t count );


/**
\brief	Retrieve the current number of centroids that are generated when calling CI_computeCentroids( void* CI_handle )

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()

\return	an uint32_t corresponding to the current parameter value. Returns 0 if the given handle is invalid.
*/
CI_EXPORT uint32_t CI_getCentroidCountParameter( void* CI_handle );


/**
\brief	Update and retrieves the current posing gesture for a given hand.

\note    Must be called after CI_updateHandTrackingData( void* CI_handle ) and CI_computeFingersTrackingData( void* CI_handle ).

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   posingGestureData       Pointer to an allocated posingGestureData struct to copy the posing gesture information to

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_updateHandPosingGestureData( void* CI_handle, CIHandPosingGestureData* posingGestureData );


/**
\brief	Update and retrieves the current moving gesture list

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   movingGestures          Pointer to an allocated CIMovingGestureEventList struct to copy the event list to

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_updateMovingGestureEventList( void* CI_handle, CIMovingGestureEventList* movingGestures );


/**
\brief	Enable the detection of a given posing gesture for a hand. Does nothing if the gesture is already active.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    gestureName             The name of the gesture to activate.

\return	a CIReturnCode specifying if the call was successful or not (e.g. Gesture name does not exist)
*/
CI_EXPORT CIReturnCode CI_enableOneHandPosingGesture( void* CI_handle, const char* gestureName );


/**
\brief	Disable the detection of a given posing gesture for a hand. Does nothing if the gesture is already inactive.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    gestureName             The name of the gesture to deactivate.

\return	a CIReturnCode specifying if the call was successful or not (e.g. Gesture name does not exist)
*/
CI_EXPORT CIReturnCode CI_disableOneHandPosingGesture( void* CI_handle, const char* gestureName );


/**
\brief	Enable the detection of a given moving gesture. Does nothing if the gesture is already active.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]	   gestureName 	           The name of the gesture to activate.

\return	a CIReturnCode specifying if the call was successful or not (e.g. Gesture name does not exist)
*/
CI_EXPORT CIReturnCode CI_enableMovingGesture( void* CI_handle, const char* gestureName );


/**
\brief	Disable the detection of a given moving gesture for a hand. Does nothing if the gesture is already inactive.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    gestureName             The name of the gesture to deactivate.

\return	a CIReturnCode specifying if the call was successful or not (e.g. Gesture name does not exist)
*/
CI_EXPORT CIReturnCode CI_disableMovingGesture( void* CI_handle, const char* gestureName );


/**
\brief	Retrieves the list of ID-to-name mappings for posing gestures.

Use these mappings at application startup to build a custom mapper and avoid
string comparisons in the main application loop.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   posingGestureMappings   A pointer to an allocated CIPosingGestureMappingList struct

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_initializeOneHandPosingGestureMappings( void* CI_handle, CIPosingGestureMappingList* posingGestureMappings );


/**
\brief	Retrieve the list of ID-to-name mappings for moving gestures.

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[out]   movingGestureMappings   A pointer to an allocated CIMovingGestureMappingList struct

\return	a CIReturnCode specifying if the call was successful or not
*/
CI_EXPORT CIReturnCode CI_initializeMovingGestureMappings( void* CI_handle, CIMovingGestureMappingList* movingGestureMappings );


/**
\brief	Get a posing gesture name using a gesture ID

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]	   gestureID               The gesture ID

\return	A pointer to a null-terminated string with the gesture name
*/
CI_EXPORT const char* CI_getOneHandPosingGestureName( void* CI_handle, int32_t gestureID );


/**
\brief Get a posing gesture name using a gesture ID

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    gestureID               The gesture ID

* \return	A pointer to a null-terminated string with the gesture name
*/
CI_EXPORT const char* CI_getMovingGestureName( void* CI_handle, int32_t gestureID );


/**
\brief Retrieve a human-readable string from an error code

\param[in]    CI_handle               Handle of the instance of the CI processing pipeline created by CI_initialize()
\param[in]    errorID                 The error code ID

\return A pointer to a null-terminated string with an error description
*/
CI_EXPORT const char* CI_getErrorDescription ( void* CI_handle, CIReturnCode errorID );

#ifdef __cplusplus
}
#endif

#endif
