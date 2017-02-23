#ifndef GLOBAL_H
#define GLOBAL_H

//const int c_colorWidth = 1920;
//const int c_colorHeight = 1080;
const int c_colorWidth = 640;
const int c_colorHeight = 480;
const int c_depthWidth = 640;
const int c_depthHeight = 480;
const int c_fps = 30;
const float c_depthScaleFactor = 5.0;

typedef unsigned short DepthValueType;

inline int getNumberOfDigits(int number){
	int digits = 0;
	while (number){
		number /= 10;
		digits++;
	}
	return digits;
}

#endif