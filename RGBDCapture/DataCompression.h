#ifndef DATACOMPRESSION_H
#define DATACOMPRESSION_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdint.h>
#include <zlib.h>
#include "global.h"


using namespace cv;
using namespace std;

class DataCompression
{
public:
	DataCompression();

	~DataCompression();

	void compressColor(cv::Vec<unsigned char, 3> * rgb_data, int width, int height);

	void compressDepth(unsigned char* depthDataPtr);
	
	void writeHeader(string klgFilename, int frameNum, int depthWidth, int depthHeight, int colorWidth, int colorHeight);

	void writeBody(int32_t timestamp);

	void closeKLGFile(int frameNum);

	inline int getDepthCompressedSize() { return (int)m_depthCompressSize; }

	inline int getColorCompressedSize() { return m_colorCompressSize; }

	inline uint8_t *getDepthCompressedBuf(){ return m_depthCompressBuf; }

private:
	int m_depthOriginalSize;
	uLong m_depthCompressSize;
	int m_colorCompressSize;
	uint8_t *m_depthCompressBuf;
	CvMat *m_encodedImage;
	FILE* m_file;
};



#endif
