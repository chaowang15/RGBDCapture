#include "IntelRSSensor.h"

IntelRSSensor::IntelRSSensor(string logFilename)
{
	m_logFilename = logFilename;
}

IntelRSSensor::~IntelRSSensor()
{
	
}

void IntelRSSensor::init(int colorWidth, int colorHeight, int depthWidth, int depthHeight, int fps, bool flagShowImage)
{
	m_colorWidth = colorWidth;
	m_colorHeight = colorHeight;
	m_depthWidth = depthWidth;
	m_depthHeight = depthHeight;
	m_fps = fps;

	m_flagInitSuccessful = true;
	m_flagScan = true;
	m_numFrames = 5;
	m_numDevices = 1; 
	m_frameIndex = 0;
	m_flagShowImage = flagShowImage;	
}

void IntelRSSensor::scan()
{
	// Create a context object. This object owns the handles to all connected realsense devices.
	rs::context ctx;
	printf("There are %d connected RealSense devices.\n", ctx.get_device_count());
	if (ctx.get_device_count() == 0) return;

	m_numDevices = ctx.get_device_count();

	// Create image compression object
	DataCompression* imageComp = new DataCompression(m_depthWidth, m_depthHeight);

	// Open log file to compress
	FILE* logFile = fopen(m_logFilename.c_str(), "wb+");
	fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile); // write the number of frames as 0 in the head and update it later after scanning
	fwrite(&m_numDevices, sizeof(int32_t), 1, logFile); // write the number of devices
	fwrite(&m_depthWidth, sizeof(int32_t), 1, logFile);
	fwrite(&m_depthHeight, sizeof(int32_t), 1, logFile);
	fwrite(&m_colorWidth, sizeof(int32_t), 1, logFile);
	fwrite(&m_colorHeight, sizeof(int32_t), 1, logFile);

	//// OpenCV variables to show images (You can commit these lines if you do not want to show images during scaning)
	//string strDepthWindowName("Depth"), strColorWindowName("Color");
	//if (m_flagShowImage)
	//{
	//	cv::namedWindow(strDepthWindowName, CV_WINDOW_AUTOSIZE);
	//	cv::namedWindow(strColorWindowName, CV_WINDOW_AUTOSIZE);
	//}

	// Scaning process
	int scanTime = 5, start = 0;
	while (start++ < scanTime)
	{
		for (int i = 0; i < ctx.get_device_count(); i++)
		{
			rs::device * dev = ctx.get_device(i);
			printf("\nUsing device %d, an %s\n", i, dev->get_name());
			printf("    Serial number: %s\n", dev->get_serial());
			printf("    Firmware version: %s\n", dev->get_firmware_version());

			// Configure depth and color to run with the device's preferred settings
			//dev->enable_stream(rs::stream::depth, rs::preset::best_quality);
			//dev->enable_stream(rs::stream::color, rs::preset::best_quality);
			dev->enable_stream(rs::stream::depth, m_depthWidth, m_depthHeight, rs::format::z16, m_fps);
			dev->enable_stream(rs::stream::color, m_colorWidth, m_colorHeight, rs::format::rgb8, m_fps);
			//dev->set_option(rs::option::color_enable_auto_exposure, 1);
			dev->start();

			// Skip the first several frames which contain dark images
			int count = 0;
			while (count++ < 30){
				dev->wait_for_frames();
				dev->get_frame_data(rs::stream::color);
				dev->get_frame_data(rs::stream::depth);
			}
			
			// Image retrieval and compression process
			unsigned long depthSize = 0;
			count = 0;
			while (count++ < 1) // only capture and compres 1 image
			{
				dev->wait_for_frames();

				// Retrieve color image
				const uint8_t * color_image = (const uint8_t *)dev->get_frame_data(rs::stream::color);

				//// Covert to opencv variable and show
				//if (m_flagShowImage)
				//{
				//	cv::Mat mImageRGB(m_colorHeight, m_colorWidth, CV_8UC3, (void*)color_image);
				//	cv::Mat cImageBGR;
				//	cv::cvtColor(mImageRGB, cImageBGR, CV_RGB2BGR);
				//	cv::imshow(strColorWindowName, cImageBGR);
				//}

				// Compress color image 
				imageComp->encodeJpeg((cv::Vec<unsigned char, 3> *)color_image, m_colorWidth, m_colorHeight);

				// Retrieve depth image
				//const uint16_t * depth_image = (const uint16_t *)dev->get_frame_data(rs::stream::depth);
				const unsigned char *depth_image = (const unsigned char *)dev->get_frame_data(rs::stream::depth);

				//// Convert to opencv variable and show
				//if (m_flagShowImage)
				//{
				//	cv::Mat mImageDepth(m_depthHeight, m_depthWidth, CV_16UC1, (unsigned char*)depth_image);
				//	cv::Mat mScaledDepth;
				//	mImageDepth.convertTo(mScaledDepth, CV_16UC1, c_depthScaleFactor); // with scale
				//	cv::imshow(strDepthWindowName, mScaledDepth);
				//}

				// Compress depth image
				depthSize = imageComp->depth_compress_buf_size;
				imageComp->compressDepth((unsigned char*)depth_image, m_depthWidth, m_depthHeight, depthSize);

				// Write compressed data into log file
				int32_t rgbSize = imageComp->encodedImage->width;
				unsigned char * rgbData = (unsigned char *)imageComp->encodedImage->data.ptr;
				unsigned char * depthData = (unsigned char *)imageComp->depth_compress_buf;
				imageComp->logData(logFile, (int32_t *)&m_frameIndex, (int32_t *)&depthSize, &rgbSize, depthData, rgbData); // write compressed depth and color data for each device
				cout << "Frame " << m_frameIndex << "..." << endl;
				m_frameIndex++;
			}
			dev->stop();
		}

		//// Check keyboard
		//if (m_flagShowImage && cv::waitKey(1) == 27)
		//{
		//	fseek(logFile, 0, SEEK_SET);
		//	fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile);
		//	fflush(logFile);
		//	fclose(logFile);
		//	break;
		//}

	}

	// Do NOT forget to update the header with number of frames
	fseek(logFile, 0, SEEK_SET);
	fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile);
	fflush(logFile);
	fclose(logFile);
}

void IntelRSSensor::logData(FILE* logFile,
	int32_t * timestamp,
	int32_t * depthSize,
	int32_t * rgbSize,
	unsigned char * depthData,
	unsigned char * rgbData)
{
	fwrite(timestamp, sizeof(int32_t), 1, logFile);
	fwrite(depthSize, sizeof(int32_t), 1, logFile);
	fwrite(rgbSize, sizeof(int32_t), 1, logFile);
	fwrite(depthData, *depthSize, 1, logFile);
	fwrite(rgbData, *rgbSize, 1, logFile);
}
