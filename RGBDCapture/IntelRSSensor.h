#ifndef INTELRSSENSOR_H
#define INTELRSSENSOR_H

#include "RGBDSensor.h"
#include <rs.hpp>
#include <zlib.h>
#include "DataCompression.h"
#include "global.h"

class IntelRSSensor : public RGBDSensor
{
public:
	IntelRSSensor();

	~IntelRSSensor();

	void init();

	void scan(string klgFilename = "saved.klg");

private:

};

#endif
