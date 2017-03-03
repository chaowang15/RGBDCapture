#include "OpenNISensor.h"
#include "IntelRSSensor.h"
#include "Kinect2Sensor.h"

void printUsage(){
	cout << "Usage: " << endl
		<< "   RGBDCapture [-camera]" << endl << endl;
	cout << "-camera:" << endl
		<< "   -kinect: Microsoft Kinect v1 (by default)" << endl
		<< "   -asus: Asus XTion" << endl
		<< "   -intel: Intel realscene (currently not supported)" << endl
		<< "   -kinect2: Microsoft Kinect v2 (currently not supported)" << endl;
}


int main(int argc, char** argv)
{
	if (argc > 2)
	{
		printUsage();
		return EXIT_FAILURE;
	}
	string cameraType("-kinect");
	if (argc == 2)
	{
		cameraType = string(argv[2]);
	}
	
	RGBDSensor* sensor = NULL;
	if (cameraType == "-kinect" || cameraType == "-asus")
	{
		sensor = new OpenNISensor();
	}
	else if (cameraType == "-intel")
	{
		sensor = new IntelRSSensor();
	}
	else if (cameraType == "-kinect2")
	{
		sensor = new Kinect2Sensor();
	}
	else
	{
		printUsage();
		return EXIT_FAILURE;
	}
	if (sensor)
	{
		sensor->scan();
		delete sensor;
		sensor = nullptr;
	}		

	return 0;
}