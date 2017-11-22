#ifndef RGBDSENSOR_H
#define RGBDSENSOR_H

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

using namespace std;

class RGBDSensor{
public:
	RGBDSensor(){}

	~RGBDSensor(){}

	virtual bool init() = 0;

	virtual void scan() = 0;

protected:
	void createRGBDFolders();

	int m_sensorType; // 0 - OpenNI, 1 - RealSense, 2 - Kinect v2
	int m_colorWidth, m_colorHeight;
	int m_depthWidth, m_depthHeight;
	int m_depthCount, m_colorCount;
	int m_fps;
	int m_frameNum;
	int m_frameIdx;
	string m_strRGBDFolder;
};

#endif