#include "OpenNISensor.h"

OpenNISensor::OpenNISensor()
{
	m_flagInitSuccessful = m_flagShowImage = true;
	m_frameNum = m_frameIdx = 0;
	m_sensorType = 0;
	init();
}

OpenNISensor::~OpenNISensor()
{
	m_depthStream.stop();
	m_colorStream.stop();
	m_depthStream.destroy();
	m_colorStream.destroy();
	m_device.close();
	openni::OpenNI::shutdown();
}


bool OpenNISensor::init()
{
	openni::Status rc = openni::STATUS_OK;
	rc = openni::OpenNI::initialize();
	if (rc != openni::STATUS_OK)
	{
		std::cerr << "OpenNI Initial Error: " << openni::OpenNI::getExtendedError() << std::endl;
		openni::OpenNI::shutdown();
		m_flagInitSuccessful = false;
		return m_flagInitSuccessful;
	}


	const char* deviceURI = openni::ANY_DEVICE;
	rc = m_device.open(deviceURI);
	if (rc != openni::STATUS_OK)
	{
		cerr << "ERROR: Can't Open Device: " << openni::OpenNI::getExtendedError() << endl;
		openni::OpenNI::shutdown();
		m_flagInitSuccessful = false;
		return m_flagInitSuccessful;
	}


	rc = m_depthStream.create(m_device, openni::SENSOR_DEPTH);

	if (rc == openni::STATUS_OK)
	{
		rc = m_depthStream.start();

		if (rc != openni::STATUS_OK)
		{
			cerr << "ERROR: Can't start depth stream on device: " << openni::OpenNI::getExtendedError() << endl;
			m_depthStream.destroy();
			m_flagInitSuccessful = false;
			return m_flagInitSuccessful;
		}
	}
	else
	{
		cerr << "ERROR: This device does not have depth sensor" << endl;
		openni::OpenNI::shutdown();
		m_flagInitSuccessful = false;
		return m_flagInitSuccessful;
	}

	rc = m_colorStream.create(m_device, openni::SENSOR_COLOR);
	if (rc == openni::STATUS_OK)
	{
		rc = m_colorStream.start();
		if (rc != openni::STATUS_OK)
		{
			cerr << "ERROR: Can't start color stream on device: " << openni::OpenNI::getExtendedError() << endl;
			m_colorStream.destroy();
			m_flagInitSuccessful = false;
			return m_flagInitSuccessful;
		}
	}
	else
	{
		cerr << "ERROR: This device does not have color sensor" << endl;
		openni::OpenNI::shutdown();
		m_flagInitSuccessful = false;
		return m_flagInitSuccessful;
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


	if (!m_depthStream.isValid() || !m_colorStream.isValid())
	{
		cerr << "SimpleViewer: No valid streams. Exiting" << endl;
		m_flagInitSuccessful = false;
		openni::OpenNI::shutdown();
		return m_flagInitSuccessful;
	}

	openni::VideoMode depthVideoMode = m_depthStream.getVideoMode();
	openni::VideoMode colorVideoMode = m_colorStream.getVideoMode();
	m_depthWidth = depthVideoMode.getResolutionX();
	m_depthHeight = depthVideoMode.getResolutionY();
	m_colorWidth = colorVideoMode.getResolutionX();
	m_colorHeight = colorVideoMode.getResolutionY();
	cout << "Depth = (" << m_depthWidth << "," << m_depthHeight << ")" << endl;
	cout << "Color = (" << m_colorWidth << "," << m_colorHeight << ")" << endl;

	// Set exposure if needed
	m_colorStream.getCameraSettings()->setAutoWhiteBalanceEnabled(false);
	int exposure = m_colorStream.getCameraSettings()->getExposure();
	int delta = 100;
	m_colorStream.getCameraSettings()->setExposure(exposure + delta);
	m_flagInitSuccessful = true;

	return m_flagInitSuccessful;
}

void OpenNISensor::scan()
{
	if (!m_flagInitSuccessful){
		cout << "WARNING: initialize the device at first!" << endl;
		return;
	}

	createRGBDFolders();

	string strDepthWindowName("Depth"), strColorWindowName("Color");
	cv::namedWindow(strDepthWindowName, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(strColorWindowName, CV_WINDOW_AUTOSIZE);
	
	while (true)
	{
		m_colorStream.readFrame(&m_colorFrame);
		if (m_colorFrame.isValid())
		{
			cv::Mat mImageRGB(m_colorHeight, m_colorWidth, CV_8UC3, (void*)m_colorFrame.getData());
			cv::Mat cImageBGR;
			cv::cvtColor(mImageRGB, cImageBGR, CV_RGB2BGR);
			if (m_sensorType == 0)
				cv::flip(cImageBGR, cImageBGR, 1);
			cv::imshow(strColorWindowName, cImageBGR);
			cv::imwrite(m_strRGBDFolder + "/rgb/" + to_string(m_frameIdx) + ".png", cImageBGR);
		}
		else
		{
			cerr << "ERROR: Cannot read color frame from color stream. Quitting..." << endl;
			return;
		}

		m_depthStream.readFrame(&m_depthFrame);
		if (m_depthFrame.isValid())
		{
			cv::Mat mImageDepth(m_depthHeight, m_depthWidth, CV_16UC1, (void*)m_depthFrame.getData());
			cv::Mat cScaledDepth;
			mImageDepth.convertTo(cScaledDepth, CV_16UC1, c_depthScaleFactor);
			if (m_sensorType == 0)
				cv::flip(cScaledDepth, cScaledDepth, 1);
			cv::imshow(strDepthWindowName, cScaledDepth);
			cv::imwrite(m_strRGBDFolder + "/depth/" + to_string(m_frameIdx) + ".png", cScaledDepth);
		}
		else
		{
			cerr << "ERROR: Cannot read depth frame from depth stream. Quitting..." << endl;
			return;
		}

		m_frameIdx++;
		if (cv::waitKey(1) == 27) // esc to quit
		{
			break;
		}
	}
}