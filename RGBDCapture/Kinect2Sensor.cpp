#include "Kinect2Sensor.h"

Kinect2Sensor::Kinect2Sensor()
{
	// Kinect v2 has fixed resolution
	m_colorHeight = 1080;
	m_colorWidth = 1920;
	m_depthHeight = 424;
	m_depthWidth = 512;
	m_depthCount = m_depthWidth* m_depthHeight;
	m_colorCount = m_colorWidth * m_colorHeight;
	m_flagCaptureSuccessful = false;

#if defined(WIN32) || defined(_WIN32)
	m_pKinectSensor = NULL;
	m_pMultiSourceFrameReader = NULL;
	m_pCoordinateMapper = NULL;
	m_pColorCoordinates = NULL;
	m_pDepthCoordinates = NULL;
#elif defined (LINUX) || defined(__linux__)

#endif

	m_flagInitSuccessful = init();
	if (m_flagInitSuccessful)
	{
		m_pDepth = new DepthValueType[m_depthCount];
		m_pDepthColor = new BYTE[m_depthCount * 4]; // RGBA or BGRA
		m_pColorDepth = new DepthValueType[m_colorCount];
		m_pColor = new BYTE[m_colorCount * 4]; // RGBA or BGRA
		m_pColorCoordinates = new ColorSpacePoint[m_depthCount]; // Note that its dimension is the same as depth image
		m_pDepthCoordinates = new DepthSpacePoint[m_colorCount]; // Note that its dimension is the same as color image
	}	
}

Kinect2Sensor::~Kinect2Sensor()
{
#if defined(WIN32) || defined(_WIN32)
	if (m_pKinectSensor)			SafeRelease(m_pKinectSensor);
	if (m_pMultiSourceFrameReader)	SafeRelease(m_pMultiSourceFrameReader);
	if (m_pCoordinateMapper)		SafeRelease(m_pCoordinateMapper);
#elif defined (LINUX) || defined(__linux__)

#endif

	SAFE_DELETE_ARRAY(m_pDepth);
	SAFE_DELETE_ARRAY(m_pDepthColor);
	SAFE_DELETE_ARRAY(m_pColorDepth);
	SAFE_DELETE_ARRAY(m_pColor);
	SAFE_DELETE_ARRAY(m_pColorCoordinates);
	SAFE_DELETE_ARRAY(m_pDepthCoordinates);

}

bool Kinect2Sensor::init()
{
#if defined(WIN32) || defined(_WIN32)
	HRESULT hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)) return false;

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the frame reader
		hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		if (FAILED(hr)) return false;

		hr = m_pKinectSensor->Open();
		if (FAILED(hr)) return false;

		hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color,
				&m_pMultiSourceFrameReader);
		if (FAILED(hr)) return false;
		return true;
	}
	return false;

#elif defined (LINUX) || defined(__linux__)

#endif
}

void Kinect2Sensor::scan(string klgFilename)
{
	if (!m_flagInitSuccessful)
	{
		throw MLIB_EXCEPTION("WARNING: Failed to initialize kinect v2. Quitting...");
		return;
	}

	string strDepthWindowName("Depth"), strColorWindowName("Color");
	cv::namedWindow(strDepthWindowName, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(strColorWindowName, CV_WINDOW_AUTOSIZE);

#if defined(WIN32) || defined(_WIN32)

	while (true)
	{
		if (captureDepthAndColor())
		{
			// depth image
			cv::Mat cDepthImage(m_depthHeight, m_depthWidth, CV_16UC1, (void*)m_pDepth);
			cv::Mat cScaledDepthImage;
			cDepthImage.convertTo(cScaledDepthImage, CV_16UC1, c_depthScaleFactor);
			cv::flip(cScaledDepthImage, cScaledDepthImage, 1);
			cv::imshow(strDepthWindowName, cScaledDepthImage);

			// color image
			cv::Mat cBGRAImage(m_colorHeight, m_colorWidth, CV_8UC4, (void*)m_pColor);
			cv::Mat cBGRImage;
			cv::cvtColor(cBGRAImage, cBGRImage, CV_BGRA2BGR);
			cv::flip(cBGRImage, cBGRImage, 1);
			cv::imshow(strColorWindowName, cBGRImage);

			// depth-color mapping image
			cv::Mat cDepth2BGRAImage(m_depthHeight, m_depthWidth, CV_8UC4, (void*)m_pDepthColor);
			cv::Mat cDepth2BGRImage;
			cv::cvtColor(cDepth2BGRAImage, cDepth2BGRImage, CV_BGRA2BGR);
			cv::flip(cDepth2BGRImage, cDepth2BGRImage, 1);
			cv::imwrite("depth-color.png", cDepth2BGRImage);

			// color-depth mapping image
			cv::Mat cBGRA2DepthImage(m_colorHeight, m_colorWidth, CV_16UC1, (void*)m_pColorDepth);
			cv::Mat cScaledBGRA2DepthImage;
			cBGRA2DepthImage.convertTo(cScaledBGRA2DepthImage, CV_16UC1, c_depthScaleFactor);
			cv::flip(cScaledBGRA2DepthImage, cScaledBGRA2DepthImage, 1);
			cv::imwrite("color-depth.png", cScaledBGRA2DepthImage);

			if (cv::waitKey(1) == 27) // esc to quit
			{
				// Actually current frame index is the total number of frames
				//dataComp.closeKLGFile(m_frameIdx);
				break;
			}
		}

	}





#elif defined (LINUX) || defined(__linux__)
#endif
}

bool Kinect2Sensor::captureDepthAndColor()
{
	HRESULT hr = S_FALSE;
	IMultiSourceFrame* pMultiSourceFrame = NULL;
	IDepthFrame* pDepthFrame = NULL;
	IColorFrame* pColorFrame = NULL;

	// Get latest frame from all sources including color, depth and 
	hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
	if (SUCCEEDED(hr))
	{
		// Get depth frame
		IDepthFrameReference* pDepthFrameReference = NULL;
		hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		}
		SafeRelease(pDepthFrameReference);
	}

	if (SUCCEEDED(hr))
	{
		// Get color frame
		IColorFrameReference* pColorFrameReference = NULL;
		hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}
		SafeRelease(pColorFrameReference);
	}
	if (SUCCEEDED(hr))
	{
		// Get depth buffer
		UINT nDepthBufferSize = 0;
		UINT nColorBufferSize = m_colorHeight * m_colorWidth * 4 * sizeof(BYTE);
		UINT16 *pDepthBuffer = NULL;
		BYTE *pColorBuffer = m_pColor;
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);
		}

		// Get color buffer
		if (SUCCEEDED(hr))
		{
			// NOTE: here we capture image buffer in BGRA order, which can be directly used in 
			// opencv-based image matrix type. If you need buffer in RGBA order, change the last
			// parameter to 'ColorImageFormat_Rgba'.
			hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, pColorBuffer, ColorImageFormat_Bgra);
		}

		// Map depth pixels to color image and get the color coordinates
		if (SUCCEEDED(hr))
		{
			//const UINT16 * const depthBuffer = pDepthBuffer;
			hr = m_pCoordinateMapper->MapDepthFrameToColorSpace(m_depthCount, pDepthBuffer, m_depthCount, m_pColorCoordinates);
		}

		// Map color pixels to depth image and get the depth coordinates
		if (SUCCEEDED(hr))
		{
			//const UINT16 * const depthBuffer = pDepthBuffer;
			hr = m_pCoordinateMapper->MapColorFrameToDepthSpace(m_depthCount, pDepthBuffer, m_colorCount, m_pDepthCoordinates);
		}

		if (SUCCEEDED(hr))
		{
			// Copy depth buffer into local array
			// NOTE: seems the original raw data in the array 'pBuffer' can not
			// be utilized directly. It has to be copied into another array.
			const UINT16* const depthPtr = pDepthBuffer;
			const BYTE* const colorPtr = pColorBuffer; // color buffer in RGRA order
			if (m_pColorCoordinates)
			{
#pragma omp parallel for
				for (UINT i = 0; i < nDepthBufferSize; ++i)
				{
					m_pDepth[i] = depthPtr[i];

					// copy depth-color mapping
					int x = (int)(m_pColorCoordinates[i].X + 0.5);
					int y = (int)(m_pColorCoordinates[i].Y + 0.5);
					if (x < 0 || x >= m_colorWidth || y < 0 || y >= m_colorHeight)
					{
						for (int j = 0; j < 4; ++j)
						{
							m_pDepthColor[4 * i + j] = 0;
						}
					}
					else
					{
						int idx = x + y * m_colorWidth;
						for (int j = 0; j < 4; ++j)
						{
							// NOTE for the multiplier 4 here, since each depth pixel's
							// corresponding color pixel has 4 bytes
							m_pDepthColor[4 * i + j] = colorPtr[4 * idx + j];
						}
					}
				}
			}		

			// Copy color buffer into local array
#pragma omp parallel for
			for (UINT i = 0; i < nColorBufferSize; ++i)
			{
				m_pColor[i] = colorPtr[i];

				if (i % 4 == 0)
				{
					// copy color-depth mapping
					int j = i / 4;
					int x = (int)(m_pDepthCoordinates[j].X + 0.5);
					int y = (int)(m_pDepthCoordinates[j].Y + 0.5);
					if (x < 0 || x >= m_depthWidth || y < 0 || y >= m_depthHeight)
					{
						m_pColorDepth[j] = 0;
					}
					else
					{
						int idx = x + y * m_depthWidth;
						m_pColorDepth[j] = m_pDepth[idx];
					}
				}
			}
			
			m_flagCaptureSuccessful = true;
		}
	}
	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pMultiSourceFrame);
	return m_flagCaptureSuccessful;
}