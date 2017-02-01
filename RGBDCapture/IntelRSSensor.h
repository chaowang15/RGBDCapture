#ifndef INTELRSSENSOR_H
#define INTELRSSENSOR_H

#include "RGBDSensor.h"
#include <librealsense/rs.hpp>
#include "DataCompression.h"
#include "global.h"

class IntelRSSensor : public RGBDSensor
{
public:
	IntelRSSensor(string logFilename);
	
	~IntelRSSensor();

	void init(int colorWidth, int colorHeight, int depthWidth, int depthHeight, int fps, bool flagShowImage = false);

	void scan();
};

#endif