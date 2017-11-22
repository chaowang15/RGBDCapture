#include "RGBDSensor.h"

void RGBDSensor::createRGBDFolders()
{
	string folder = "./save";
	int i = 0;
	while (true){
		string dir = folder + to_string(i);
		if (boost::filesystem::is_directory(dir))
		{
			i++;
		}
		else
		{
			m_strRGBDFolder = dir;
			string dirDepth = dir + "/depth/";
			string dirColor = dir + "/rgb/";
			boost::filesystem::create_directory(dir);
			boost::filesystem::create_directory(dirDepth);
			boost::filesystem::create_directory(dirColor);
			break;
		}
	}
}