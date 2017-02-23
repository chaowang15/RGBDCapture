#ifndef GLOBAL_H
#define GLOBAL_H

#include <iostream>
#include <exception>
#include <string>

#ifndef _MSC_VER
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif

#if defined (LINUX) || defined(__linux__)
#define __FUNCTION__ __func__
#ifndef __LINE__
#define __LINE__
#endif
#endif

//const int c_colorWidth = 1920;
//const int c_colorHeight = 1080;
const int c_colorWidth = 640;
const int c_colorHeight = 480;
const int c_depthWidth = 640;
const int c_depthHeight = 480;
const int c_fps = 30;
const float c_depthScaleFactor = 5.0;

typedef unsigned short DepthValueType;

// Get number of digits of an input integer number
inline int getNumberOfDigits(int number){
	int digits = 0;
	while (number){
		number /= 10;
		digits++;
	}
	return digits;
}


class MLibException : public std::exception {
public:
	MLibException(const std::string& what) : std::exception() {
		m_msg = what;
	}
	MLibException(const char* what) : std::exception() {
		m_msg = std::string(what);
	}
	const char* what() const NOEXCEPT{
		return m_msg.c_str();
	}
private:
	std::string m_msg;
};

#ifndef MLIB_EXCEPTION
#define MLIB_EXCEPTION(s) MLibException(std::string(__FUNCTION__).append(":").append(std::to_string(__LINE__)).append(": ").append(s).c_str())
#endif

#endif