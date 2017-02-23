#include "IntelRSSensor.h"

IntelRSSensor::IntelRSSensor()
{

}

IntelRSSensor::~IntelRSSensor()
{

}

bool IntelRSSensor::init()
{
	// Use constant resolutions for color and depth data while using Intel RealSense
	m_depthWidth = c_depthWidth;
	m_depthHeight = c_depthHeight;
	m_colorWidth = c_colorWidth;
	m_colorHeight = c_colorHeight;
	m_frameNum = m_frameIdx = 0;
	m_sensorType = 1;
	return true;
}

void IntelRSSensor::scan(string klgFilename /* = "saved.klg" */)
{
	// Create a context object. This object owns the handles to all connected realsense devices.
	rs::context ctx;
	printf("There are %d connected RealSense devices.\n", ctx.get_device_count());
	if (ctx.get_device_count() == 0) return;

	// This tutorial will access only a single device, but it is trivial to extend to multiple devices
	rs::device * dev = ctx.get_device(0);
	printf("\nUsing device 0, an %s\n", dev->get_name());
	printf("    Serial number: %s\n", dev->get_serial());
	printf("    Firmware version: %s\n", dev->get_firmware_version());

	// Configure depth and color to run with the device's preferred settings
	//dev->enable_stream(rs::stream::depth, rs::preset::best_quality);
	//dev->enable_stream(rs::stream::color, rs::preset::best_quality);
	dev->enable_stream(rs::stream::depth, m_depthWidth, m_depthHeight, rs::format::z16, m_fps);
	dev->enable_stream(rs::stream::color, m_colorWidth, m_colorHeight, rs::format::rgb8, m_fps);
	dev->start();

	// Create image compression object
	DataCompression dataComp;
	dataComp.writeHeader(klgFilename, m_frameNum, m_depthWidth, m_depthHeight, m_colorWidth, m_colorHeight);

	// OpenCV variables to show images
	string strDepthWindowName("Depth"), strColorWindowName("Color");
	cv::namedWindow(strDepthWindowName, CV_WINDOW_AUTOSIZE);
	cv::namedWindow(strColorWindowName, CV_WINDOW_AUTOSIZE);

	// Image retrieval and compression process
	unsigned long depthSize = 0;
	while (true)
	{
		dev->wait_for_frames();

		// Retrieve color image
		const uint8_t * color_image = (const uint8_t *)dev->get_frame_data(rs::stream::color);

		// Show image
		cv::Mat mImageRGB(m_colorHeight, m_colorWidth, CV_8UC3, (void*)color_image);
		cv::Mat cImageBGR;
		cv::cvtColor(mImageRGB, cImageBGR, CV_RGB2BGR);
		cv::imshow(strColorWindowName, cImageBGR);

		// Compress color image 
		dataComp.compressColor((cv::Vec<unsigned char, 3> *)color_image, m_colorWidth, m_colorHeight);

		// Retrieve depth image
		//const uint16_t * depth_image = (const uint16_t *)dev->get_frame_data(rs::stream::depth);
		const unsigned char *depth_image = (const unsigned char *)dev->get_frame_data(rs::stream::depth);

		// Convert to opencv variable and show
		cv::Mat mImageDepth(m_depthHeight, m_depthWidth, CV_16UC1, (unsigned char*)depth_image);
		cv::Mat mScaledDepth;
		mImageDepth.convertTo(mScaledDepth, CV_16UC1, c_depthScaleFactor); // with scale
		cv::imshow(strDepthWindowName, mScaledDepth);

		// Compress depth image
		dataComp.compressDepth((unsigned char*)depth_image);

		// Write compressed color and depth data into klg file
		dataComp.writeBody(m_frameIdx);
		m_frameIdx++;
		//cout << "Frame " << m_frameIdx << "..." << endl;

		if (cv::waitKey(1) == 27) // esc to quit
		{
			// Actually current frame index is the total number of frames
			dataComp.closeKLGFile(m_frameIdx);
			break;
		}
	}

}