#ifndef OPENNI2INTERFACE_H
#define OPENNI2INTERFACE_H

#include <iostream>
#include <OpenNI.h>
#include <string>
#include <zlib.h>
#include <fstream>
#include "imagecompression.h"


#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#include <time.h>
#endif
#if defined(LINUX)
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#endif

using namespace std;
using namespace openni;

const int cst_deviceNumber = 3;
const float cst_depthScaleFactor = 5.0; // depth value scale factor in millimeter
const int cst_timeInterval = 10;

template <class T>
int getNumberOfDigits(T number)
{
	int digits = 0;
	if (number < 0) digits = 1; // remove this line if '-' counts as a digit
	while (number) {
		number /= 10;
		digits++;
	}
	return digits;
}

class OpenNI2Interface
{
public:
	// inputDataType: 1 for kinect, 2 for kinect-scanned data, 3 for benchmark data
	OpenNI2Interface(); 
	~OpenNI2Interface();

	void initKinectDevice();
	void run();
	void logData(int32_t * timestamp, int32_t * depthSize, int32_t * rgbSize, unsigned char * depthData, unsigned char * rgbData);
	void setFilename(string filename){m_filename = filename;}
	void decompressLog(char* filename);
	void setFlagShowImage(bool flag){
		m_flagShowImage = flag;
	}

private:
	openni::Device			m_device[cst_deviceNumber];
	openni::VideoStream		m_depthStream[cst_deviceNumber];
	openni::VideoStream		m_colorStream[cst_deviceNumber];
	openni::VideoFrameRef	m_depthFrame[cst_deviceNumber];
	openni::VideoFrameRef	m_colorFrame[cst_deviceNumber];
	int m_deviceNumber;
	int m_colorWidth, m_colorHeight;
	int m_depthWidth, m_depthHeight;
	bool m_flagInitSuccessful;
	bool m_flagScan;
	FILE* logFile;
	int m_frameIndex;
	string m_filename;
	bool m_flagShowImage; // If true, show images during compression and decompression processes, and quit compression with ESC;
	bool m_flagScaleImage;
};

#endif