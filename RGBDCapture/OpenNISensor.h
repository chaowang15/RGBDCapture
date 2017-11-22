#ifndef OPENNISENSOR_H
#define OPENNISENSOR_H

#include "RGBDSensor.h"
#include "global.h"
#include "OpenNI.h"
#include <opencv2/opencv.hpp>

class OpenNISensor : public RGBDSensor
{
public:
	OpenNISensor();

	~OpenNISensor();

	bool init();

	void scan();

private:
	openni::Device			m_device;
	openni::VideoStream		m_depthStream;
	openni::VideoStream		m_colorStream;
	openni::VideoFrameRef	m_depthFrame;
	openni::VideoFrameRef	m_colorFrame;
	bool m_flagInitSuccessful;
	bool m_flagShowImage;
};


#endif