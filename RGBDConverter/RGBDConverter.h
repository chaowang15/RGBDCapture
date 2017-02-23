#ifndef RGBDCONVERTER_H
#define RGBDCONVERTER_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <zlib.h>
#include "global.h"

using namespace std;

class RGBDConverter{
public:
	RGBDConverter();

	~RGBDConverter();

	void klg2png(string filename);

	void png2klg(string filepath);

private:

};

#endif