#ifndef KINECT2SENSOR_H
#define KINECT2SENSOR_H

#include "RGBDSensor.h"
#include "global.h"
#include <opencv2/opencv.hpp>

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif

#if defined(WIN32) || defined(_WIN32)
#include <Kinect.h>

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL) {
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

#elif defined (LINUX) || defined(__linux__)
typedef unsigned char BYTE;

//typedef struct _ColorSpacePoint
//{
//	float X;
//	float Y;
//} 	ColorSpacePoint;
//
//typedef struct _DepthSpacePoint
//{
//	float X;
//	float Y;
//} 	DepthSpacePoint;


#endif

class Kinect2Sensor : public RGBDSensor{
public:

	Kinect2Sensor();

	~Kinect2Sensor();

	bool init();

	void scan();

private:
	bool captureDepthAndColor();

private:
	DepthValueType* m_pDepth;
	BYTE*			m_pDepthColor; // color pixels for all depth pixels
	BYTE*			m_pColor;
	DepthValueType* m_pColorDepth; // depth values for all color pixels

	bool			m_flagInitSuccessful;
	bool			m_flagCaptureSuccessful;

#if defined(WIN32) || defined(_WIN32)
	IMultiSourceFrameReader*	m_pMultiSourceFrameReader;// depth and color Frame reader
	IKinectSensor*				m_pKinectSensor;
	ICoordinateMapper*			m_pCoordinateMapper;
	ColorSpacePoint*			m_pColorCoordinates; // depth pixel's corresponding color coordinate in color image
	DepthSpacePoint*			m_pDepthCoordinates; // color pixel's corresponding depth coordinate in depth image
#endif


};

#endif