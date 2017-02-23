#include "DataCompression.h"

DataCompression::DataCompression()
{
	m_encodedImage = nullptr;
	m_file = nullptr;
}


DataCompression::~DataCompression()
{
	delete[]m_depthCompressBuf;
	if (m_encodedImage != nullptr)
		cvReleaseMat(&m_encodedImage);

}

void DataCompression::compressColor(cv::Vec<unsigned char, 3> * rgb_data, int width, int height)
{
	cv::Mat3b rgb(height, width, rgb_data, width * 3);
	IplImage * img = new IplImage(rgb);
	int jpeg_params[] = { CV_IMWRITE_JPEG_QUALITY, 90, 0 };
	if (m_encodedImage != 0)
		cvReleaseMat(&m_encodedImage);
	m_encodedImage = cvEncodeImage(".jpg", img, jpeg_params);
	m_colorCompressSize = m_encodedImage->width;
	delete img;
}

void DataCompression::compressDepth(unsigned char* depthDataPtr){
	// NOTE: in order to use compress2(), you have to initialize the destination length (the 2nd parameter) a 
	// value that is large enough to hold all compressed data.
	m_depthCompressSize = m_depthOriginalSize;
	int res = compress2(m_depthCompressBuf, &m_depthCompressSize, depthDataPtr, m_depthOriginalSize, Z_BEST_SPEED);
	if (res != 0)
		cerr << "WARNING: Compression Error !" << endl;
}


void DataCompression::writeHeader(string klgFilename, int frameNum, int depthWidth, int depthHeight, int colorWidth, int colorHeight)
{
	m_file = fopen(klgFilename.c_str(), "wb+");
	fwrite(&frameNum, sizeof(int32_t), 1, m_file); // write the number of frames as 0 in the head and update it later after scanning
	//fwrite(&deviceNum, sizeof(int32_t), 1, file); 
	fwrite(&depthWidth, sizeof(int32_t), 1, m_file);
	fwrite(&depthHeight, sizeof(int32_t), 1, m_file);
	fwrite(&colorWidth, sizeof(int32_t), 1, m_file);
	fwrite(&colorHeight, sizeof(int32_t), 1, m_file);

	// NOTE: we save one depth value with 16-bit storage
	m_depthOriginalSize = depthWidth * depthHeight * sizeof(DepthValueType);
	m_depthCompressBuf = new uint8_t[m_depthOriginalSize];
}

void DataCompression::writeBody(int32_t timestamp)
{
	fwrite(&timestamp, sizeof(int32_t), 1, m_file);
	fwrite(&m_depthCompressSize, sizeof(int32_t), 1, m_file);
	fwrite(&m_colorCompressSize, sizeof(int32_t), 1, m_file);
	fwrite((unsigned char *)m_depthCompressBuf, m_depthCompressSize, 1, m_file);
	fwrite((unsigned char *)m_encodedImage->data.ptr, m_colorCompressSize, 1, m_file);
}

void DataCompression::closeKLGFile(int frameNum)
{
	// Update the header with number of frames
	fseek(m_file, 4, SEEK_SET); // frame number is stored in 4th position (first 4 bytes are used to save sensor type)
	fwrite(&frameNum, sizeof(int32_t), 1, m_file);
	fflush(m_file);
	fclose(m_file);
}
