#include "openni2interface.h"

OpenNI2Interface::OpenNI2Interface():m_flagInitSuccessful(false),
	m_flagScan(true),
	logFile(NULL),
	m_frameIndex(0)
{
	m_depthWidth = m_depthHeight = m_colorHeight = m_colorWidth = 0;
	m_flagShowImage = true;
	m_flagScaleImage = false;
}

OpenNI2Interface::~OpenNI2Interface()
{
	if(m_flagInitSuccessful)
	{
		for (int i = 0; i != m_deviceNumber; ++i)
		{
			m_depthStream[i].stop();
			m_colorStream[i].stop();
			m_depthStream[i].destroy();
			m_colorStream[i].destroy();
			m_device[i].close();
		}
		openni::OpenNI::shutdown();
	}
}


void OpenNI2Interface::initKinectDevice()
{
	// 1. Initial OpenNI
	if( OpenNI::initialize() != STATUS_OK )
	{
		std::cerr << "OpenNI Initial Error: " << OpenNI::getExtendedError() << std::endl;
		openni::OpenNI::shutdown();
		m_flagInitSuccessful = false;
	}
	else
	{
		Array<DeviceInfo> deviceInfoList;
		openni::OpenNI::enumerateDevices(&deviceInfoList);
		m_deviceNumber = deviceInfoList.getSize();
		if (m_deviceNumber > 0 && m_deviceNumber <= cst_deviceNumber)
		{
			for (int i = 0; i != m_deviceNumber; ++i)
			{
				if( m_device[i].open( deviceInfoList[i].getUri() ) != STATUS_OK )
				{
					cerr << "ERROR: Can't Open Device: " << OpenNI::getExtendedError() << endl;
					openni::OpenNI::shutdown();
					m_flagInitSuccessful = false;
				}
				else
				{
					// Detect depth camera and color camera
					if( m_device[i].hasSensor( SENSOR_DEPTH ) )
					{
						if(m_depthStream[i].create( m_device[i], SENSOR_DEPTH ) != STATUS_OK )
						{
							cerr << "ERROR: Can't create depth stream on device: "<< OpenNI::getExtendedError() << endl;
							openni::OpenNI::shutdown();
							m_flagInitSuccessful = false;
						}
					}
					else
					{
						cerr << "ERROR: This device does not have depth sensor" << endl;
						openni::OpenNI::shutdown();
						m_flagInitSuccessful = false;
					}
					if( m_device[i].hasSensor( SENSOR_COLOR ) )
					{
						if( m_colorStream[i].create( m_device[i], SENSOR_COLOR ) != STATUS_OK )
						{
							cerr << "ERROR: Can't create color stream on device: " << OpenNI::getExtendedError() << endl;
							openni::OpenNI::shutdown();
							m_flagInitSuccessful = false;
						}
					}
					else
					{
						cerr << "ERROR: This device does not have color sensor" << endl;
						openni::OpenNI::shutdown();
						m_flagInitSuccessful = false;
					}

	
					///**
					//Get all supported video mode for depth and color sensors, since each device
					//always supports multiple video modes, such as 320x240/640x480 resolution,
					//30/60 fps for depth or color sensors.
					//*/
					//const SensorInfo* depthSensorInfo = m_device[i].getSensorInfo(SENSOR_DEPTH);
					//int depthVideoModeNum = depthSensorInfo->getSupportedVideoModes().getSize();
					//cout << "Depth video modes: " << endl;
					//for (int j = 0; j < depthVideoModeNum; ++j)
					//{
					//	VideoMode videomode = depthSensorInfo->getSupportedVideoModes()[j];
					//	cout << "Mode " << j << ": Resolution = (" << videomode.getResolutionX() 
					//		<< "," << videomode.getResolutionY() << ")"
					//		<< ", PixelFormat = "<< videomode.getPixelFormat()
					//		<< ", FPS = " << videomode.getFps() << endl;
					//}
					//const SensorInfo* colorSensorInfo = m_device[i].getSensorInfo(SENSOR_COLOR);
					//int colorVideoModeNum = colorSensorInfo->getSupportedVideoModes().getSize();
					//cout << "Color video modes: " << endl;
					//for (int j = 0; j < colorVideoModeNum; ++j)
					//{
					//	VideoMode videomode = colorSensorInfo->getSupportedVideoModes()[j];
					//	cout << "Mode " << j << ": Resolution = (" << videomode.getResolutionX() 
					//		<< "," << videomode.getResolutionY() << ")"
					//		<< ", PixelFormat = "<< videomode.getPixelFormat()
					//		<< ", FPS = " << videomode.getFps() << endl;
					//}

					//// Set video modes for depth and color streams if the default ones are not
					//// waht we need. You can check the video modes using above codes.
					////m_depthStream[i].setVideoMode(depthSensorInfo->getSupportedVideoModes()[0]);
					////VideoMode videomode = colorSensorInfo->getSupportedVideoModes()[9];
					////videomode.setPixelFormat(openni::PIXEL_FORMAT_RGB888);
					////openni::Status status = m_colorStream[i].setVideoMode(colorSensorInfo->getSupportedVideoModes()[9]);

					// Open depth stream and color stream
					if (m_depthStream[i].start() != openni::STATUS_OK)
					{
						std::cout << "WARNING: Couldn't start depth stream. Error: " << openni::OpenNI::getExtendedError() << std::endl;
						openni::OpenNI::shutdown();
						m_flagInitSuccessful = false;
					}

					if ( m_colorStream[i].start() != openni::STATUS_OK)
					{
						std::cout << "WARNING: Couldn't start color stream. Error: " << openni::OpenNI::getExtendedError() << std::endl;
						openni::OpenNI::shutdown();
						m_flagInitSuccessful = false;
					}

					// image registration
					if (m_flagInitSuccessful)
					{
						//m_device[i].setDepthColorSyncEnabled(true);
						//if( m_device[i].isImageRegistrationModeSupported(IMAGE_REGISTRATION_DEPTH_TO_COLOR ) )
						//{
						//	m_device[i].setImageRegistrationMode( IMAGE_REGISTRATION_DEPTH_TO_COLOR );
						//}
					}

					if (m_depthStream[i].isValid() && m_colorStream[i].isValid())
					{
						//if (i == 0) // only set depth/color width and height once
						//{
						openni::VideoMode depthVideoMode = m_depthStream[i].getVideoMode();
						openni::VideoMode colorVideoMode = m_colorStream[i].getVideoMode();
						m_depthWidth = depthVideoMode.getResolutionX();
						m_depthHeight = depthVideoMode.getResolutionY();
						m_colorWidth = colorVideoMode.getResolutionX();
						m_colorHeight = colorVideoMode.getResolutionY();
						cout << "Depth = (" << m_depthWidth << "," << m_depthHeight << ")" << endl;
						cout << "Color = (" << m_colorWidth << "," << m_colorHeight << ")" << endl;

						//if (depthWidth == colorWidth &&
						//	depthHeight == colorHeight)
						//{
						//	m_width = depthWidth;
						//	m_height = depthHeight;
						//}
						//else
						//{
						//	cout << "ERROR: expect color and depth to be in same resolution:" << endl;
						//	openni::OpenNI::shutdown();
						//	m_flagInitSuccessful = false;
						//}
						//}

						m_colorStream[i].getCameraSettings()->setAutoWhiteBalanceEnabled(true);
						int exposure = m_colorStream[i].getCameraSettings()->getExposure();
						float delta = 100;
						m_colorStream[i].getCameraSettings()->setExposure(exposure + delta);
						// DO not need this line
						//m_colorStream[i].getCameraSettings()->setAutoExposureEnabled(false);

						m_flagInitSuccessful = true;
					}
				}
			}
		}
	}
}

void OpenNI2Interface::run()
{
	if (m_flagInitSuccessful && m_flagScan)
	{
		ImageCompression* imageComp = new ImageCompression(m_colorWidth, m_colorHeight);
		unsigned long depthSize;

		logFile = fopen(m_filename.c_str(), "wb+");
		fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile); // write the number of frames as 0 in the head and update it later after scanning
		fwrite(&m_deviceNumber, sizeof(int32_t), 1, logFile); // write the number of devices
		fwrite(&m_depthWidth, sizeof(int32_t), 1, logFile);
		fwrite(&m_depthHeight, sizeof(int32_t), 1, logFile);
		fwrite(&m_colorWidth, sizeof(int32_t), 1, logFile);
		fwrite(&m_colorHeight, sizeof(int32_t), 1, logFile);
		string strDepthWindowName("Depth"), strColorWindowName("Color");
		if (m_flagShowImage)
		{
			cv::namedWindow( strDepthWindowName,  CV_WINDOW_AUTOSIZE );
			cv::namedWindow( strColorWindowName,  CV_WINDOW_AUTOSIZE );
		}
		time_t t1 = clock();
		while (m_flagScan)
		{
			for (int i = 0; i != m_deviceNumber; ++i)
			{
				//cout << "Read frame ..." << endl;
				m_colorStream[i].readFrame(&m_colorFrame[i]);
				//cout << "done." << endl;
				if(m_colorFrame[i].isValid())
				{
					if (m_flagShowImage)
					{
						int width = m_colorFrame[0].getWidth();
						int height = m_colorFrame[0].getHeight();
						//cout << width << " " << height << endl;
						cv::Mat mImageRGB(height, width, CV_8UC3, (void*)m_colorFrame[0].getData() );
						cv::Mat cImageBGR;
						cv::cvtColor( mImageRGB, cImageBGR, CV_RGB2BGR );
						cv::imshow( strColorWindowName, cImageBGR );
					}
					imageComp->encodeJpeg((cv::Vec<unsigned char, 3> *)m_colorFrame[i].getData(), m_colorWidth, m_colorHeight);
				}
				else
				{
					cerr << "ERROR: Cannot read color frame from color stream. Quitting..." << endl;
					return;
				}
				m_depthStream[i].readFrame(&m_depthFrame[i]); 
				if (m_depthFrame[i].isValid())
				{
					if (m_flagShowImage)
					{
						int width = m_depthFrame[0].getWidth();
						int height = m_depthFrame[0].getHeight();
						cv::Mat mImageDepth(height, width, CV_16UC1, (void*)m_depthFrame[0].getData() );
						cv::Mat mScaledDepth;
						mImageDepth.convertTo( mScaledDepth, CV_16UC1, cst_depthScaleFactor );
						cv::imshow(strDepthWindowName, mScaledDepth );
					}
					depthSize = imageComp->depth_compress_buf_size;
					imageComp->compressDepth((unsigned char*)m_depthFrame[i].getData(), m_depthWidth, m_depthHeight, depthSize);
				}
				int32_t rgbSize = imageComp->encodedImage->width;
				unsigned char * rgbData = (unsigned char *)imageComp->encodedImage->data.ptr;
				unsigned char * depthData = (unsigned char *)imageComp->depth_compress_buf;
				logData((int32_t *)&m_frameIndex, (int32_t *)&depthSize, &rgbSize, depthData, rgbData); // write compressed depth and color data for each device
				cout << "Frame " << m_frameIndex <<", Device " << i << "..." << endl;
				//cout << "  depthSize = " << depthSize <<", colorSize = " << rgbSize << endl;
			}
			m_frameIndex++;

			// Check keyboard
			if (m_flagShowImage)
			{
				if( cv::waitKey( 1 ) == 27) // esc to quit
				{
					fseek(logFile, 0, SEEK_SET);
					fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile);
					fflush(logFile);
					fclose(logFile);
					break;
				}
			}
			else
			{
				time_t t2 = clock();
				double timeInterval = (double)(t2 - t1);
				if (timeInterval > cst_timeInterval * 1000)
				{
					fseek(logFile, 0, SEEK_SET);
					fwrite(&m_frameIndex, sizeof(int32_t), 1, logFile);
					fflush(logFile);
					fclose(logFile);
					break;
				}
			}
		}
	}
}

void OpenNI2Interface::logData(int32_t * timestamp,
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

void OpenNI2Interface::decompressLog(char* filename)
{
	string strFilename(filename);
	strFilename = strFilename.substr(0, strFilename.length() - 4);
	string currentFolder("./");
	currentFolder += strFilename + "/";
	wstring wstrCurrent(currentFolder.begin(), currentFolder.end());
	if (CreateDirectory(wstrCurrent.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		string outputFolderColor = currentFolder + "rgb/";
		wstring wstrColor(outputFolderColor.begin(), outputFolderColor.end());
		if (CreateDirectory(wstrColor.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
		{
			string outputFolderDepth = currentFolder + "depth/";
			wstring wstrDepth(outputFolderDepth.begin(), outputFolderDepth.end());
			if (CreateDirectory(wstrDepth.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError())
			{
				string strDepthWindowName("Depth"), strColorWindowName("Color");
				if (m_flagShowImage)
				{
					cv::namedWindow( strDepthWindowName,  CV_WINDOW_AUTOSIZE );
					cv::namedWindow( strColorWindowName,  CV_WINDOW_AUTOSIZE );
				}
				int numFrames = 0, numDevices = 0, frameIdx = 0, depthSize = 0, rgbSize = 0;
				logFile = fopen(filename, "rb");
				if (logFile)
				{
					fread(&numFrames, sizeof(int32_t), 1, logFile);
					fread(&numDevices, sizeof(int32_t), 1, logFile);
					fread(&m_depthWidth, sizeof(int32_t), 1, logFile);
					fread(&m_depthHeight, sizeof(int32_t), 1, logFile);
					fread(&m_colorWidth, sizeof(int32_t), 1, logFile);
					fread(&m_colorHeight, sizeof(int32_t), 1, logFile);
					unsigned char *rgbData = new unsigned char[m_colorWidth * m_colorHeight];
					unsigned char *depthData = new unsigned char[m_depthWidth * m_depthHeight * 2];
					int numDigits = getNumberOfDigits(numFrames);
					for (int i = 0; i < numFrames; ++i)
					{
						for (int j = 0; j < numDevices; ++j)
						{
							fread(&frameIdx, sizeof(int32_t), 1, logFile);
							fread(&depthSize, sizeof(int32_t), 1, logFile);
							fread(&rgbSize, sizeof(int32_t), 1, logFile);

							memset(depthData, 0, m_depthWidth * m_depthHeight * 2);
							// Decompress depth image
							unsigned char *depthDataBinary = new unsigned char[depthSize];
							fread(depthDataBinary, depthSize, 1, logFile);
							unsigned long len = (unsigned long)(m_depthWidth * m_depthHeight * 2);
							//unsigned long len = 0;
							//unsigned long len = 1000000;
							int res = uncompress(depthData, &len, depthDataBinary, (unsigned long)depthSize);
							delete []depthDataBinary;
							//if (res != 0)
							//{
							//	cout << "-------------------------------------" << endl;
							//}
							depthDataBinary = NULL;
							cv::Mat mImageDepth(m_depthHeight, m_depthWidth, CV_16UC1, (void *)depthData);
							cv::Mat mScaledDepth;
							mImageDepth.convertTo( mScaledDepth, CV_16UC1, cst_depthScaleFactor );
							if (m_flagScaleImage)
							{
								cv::Size dstSize;
								dstSize.width = m_colorWidth;
								dstSize.height = m_colorHeight;
								cv::resize(mScaledDepth, mScaledDepth, dstSize);
							}
							//cv::flip(mScaledDepth, mScaledDepth, 1);

							string depthImageName(outputFolderDepth);
							string strName;
							for (int k = 0; k != numDigits; ++k)
							{
								strName += "0";
							}
							string temp = to_string((long long)i);
							int val = temp.length();
							strName.replace(numDigits - val, val, temp);
							strName = to_string((long long)j) + "_" + strName + ".png";
							depthImageName += strName;
							cv::imwrite(depthImageName.c_str(), mScaledDepth);
							if (m_flagShowImage && j == 0)
							{
								cv::imshow(strDepthWindowName, mScaledDepth );
							}
							cout << "Frame " << i <<", Device " << j << "..." << endl;
							//cout << "  depthSize = " << depthSize << endl;

							// Decompress color image
							fread(rgbData, rgbSize, 1, logFile);
							CvMat mat = cvMat(m_colorHeight, m_colorWidth, CV_8UC3, rgbData);
							//IplImage *p = cvDecodeImage( &mat, 1 );
							CvMat *p = cvDecodeImageM(&mat, 1);
							cv::Mat m(p);
							cv::cvtColor(m, m, CV_BGR2RGB);

							//cv::flip(m, m, 1);
							string rgbImageName(outputFolderColor);
							rgbImageName += strName;
							imwrite(rgbImageName.c_str(), m);
							if (m_flagShowImage && j == 0)
							{
								cv::imshow(strColorWindowName, m );
							}
							if(m_flagShowImage && cv::waitKey( 1 ) == 27) // esc to quit
							{
								break;
							}
						}
					}
					fclose(logFile);
					delete []rgbData;
					delete []depthData;
				}
				else
				{
					cerr << "WARNING: cannot open the file " << filename << endl;
				}
			}
		}
		else
		{
			// Failed to create directory.
			cout << "WARNING: cannot create the directory for saving decompressed images." << endl;
			return;
		}
	}
	else
	{
		// Failed to create directory.
		cout << "WARNING: cannot create the directory for saving decompressed images." << endl;
		return;
	}
}