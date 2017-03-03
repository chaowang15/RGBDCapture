#ifndef INTELRSSENSOR_H
#define INTELRSSENSOR_H

#include "RGBDSensor.h"
#include <rs.hpp>
#include <zlib.h>
#include "global.h"
#include <opencv2/opencv.hpp>

class IntelRSSensor : public RGBDSensor
{
public:
	IntelRSSensor();

	~IntelRSSensor();

	bool init();

	void scan();

private:

};

#endif
