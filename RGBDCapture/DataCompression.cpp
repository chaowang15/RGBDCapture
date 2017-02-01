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
	m_depthOriginalSize = depthWidth * depthHeight * sizeof(DepthValueSize);
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
	fseek(m_file, 0, SEEK_SET);
	fwrite(&frameNum, sizeof(int32_t), 1, m_file);
	fflush(m_file);
	fclose(m_file);
}


void DataCompression::decompressKLG(string filename)
{
	FILE* logFile = fopen(filename.c_str(), "rb");
	if (!logFile)
	{
		cerr << "WARNING: cannot open the file " << filename << endl;
		return;
	}

	string folder = "./" + filename.substr(0, filename.length() - 4);
	string depthFolder = folder + "/depth/";
	string colorFolder = folder + "/rgb/";
	boost::filesystem::path dir(folder);
	boost::filesystem::path dirDepth(depthFolder);
	boost::filesystem::path dirColor(colorFolder);

	// Create local folder/subfolders
	if (boost::filesystem::is_directory(dir)){
		boost::filesystem::remove_all(dirDepth);
		boost::filesystem::remove_all(dirColor);
	}
	boost::filesystem::create_directory(dir);
	boost::filesystem::create_directory(dirDepth);
	boost::filesystem::create_directory(dirColor);
	
	int depthWidth = 0, depthHeight = 0, colorWidth = 0, colorHeight = 0;
	int numFrames = 0, frameIdx = 0, depthSize = 0, rgbSize = 0;

	fread(&numFrames, sizeof(int), 1, logFile);
	fread(&depthWidth, sizeof(int), 1, logFile);
	fread(&depthHeight, sizeof(int), 1, logFile);
	fread(&colorWidth, sizeof(int), 1, logFile);
	fread(&colorHeight, sizeof(int), 1, logFile);

	unsigned char *rgbData = new unsigned char[colorWidth * colorHeight];
	unsigned char *depthData = new unsigned char[depthWidth * depthHeight * sizeof(DepthValueSize)];
	int numDigits = getNumberOfDigits(numFrames);
	for (int i = 0; i < numFrames; ++i)
	{
		cout << "Frame " << i << "..." << endl;

		fread(&frameIdx, sizeof(int), 1, logFile);
		fread(&depthSize, sizeof(int), 1, logFile);
		fread(&rgbSize, sizeof(int), 1, logFile);

		memset(depthData, 0, depthWidth * depthHeight * 2);

		// Decompress depth image
		unsigned char *depthDataBinary = new unsigned char[depthSize];
		fread(depthDataBinary, depthSize, 1, logFile);
		unsigned long len = (unsigned long)(depthWidth * depthHeight * 2);
		int res = uncompress(depthData, &len, depthDataBinary, (unsigned long)depthSize);
		delete[]depthDataBinary;

		// Scale depth image
		depthDataBinary = NULL;
		cv::Mat mImageDepth(depthHeight, depthWidth, CV_16UC1, (void *)depthData);
		cv::Mat mScaledDepth;
		mImageDepth.convertTo(mScaledDepth, CV_16UC1, c_depthScaleFactor);
		//cv::flip(mScaledDepth, mScaledDepth, 1);

		// Get filename of depth image
		string depthImageName(depthFolder);
		string strName;
		for (int k = 0; k != numDigits; ++k)
		{
			strName += "0";
		}
		string temp = to_string((long long)i);
		size_t val = temp.length();
		strName.replace(numDigits - val, val, temp);
		strName = "0_" + strName + ".png";
		depthImageName += strName;

		// Write depth image
		cv::imwrite(depthImageName.c_str(), mScaledDepth);

		// Decompress color image
		fread(rgbData, rgbSize, 1, logFile);
		CvMat mat = cvMat(colorHeight, colorWidth, CV_8UC3, rgbData);
		//IplImage *p = cvDecodeImage( &mat, 1 );
		CvMat *p = cvDecodeImageM(&mat, 1);
		cv::Mat m(p);
		cv::cvtColor(m, m, CV_BGR2RGB);

		// Write color image
		//cv::flip(m, m, 1);
		string rgbImageName(colorFolder);
		rgbImageName += strName;
		imwrite(rgbImageName.c_str(), m);
	}
	fclose(logFile);
	delete []rgbData;
	delete []depthData;
}