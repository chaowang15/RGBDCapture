#ifndef OPENNISENSOR_H
#define OPENNISENSOR_H

#include "RGBDSensor.h"
#include "DataCompression.h"
#include "global.h"
#include "OpenNI.h"

class OpenNISensor : public RGBDSensor
{
public:
	OpenNISensor();

	~OpenNISensor();

	void init();

	void scan(string klgFilename = "saved.klg");

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