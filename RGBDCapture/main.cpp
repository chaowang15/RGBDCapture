#include "OpenNISensor.h"

void printUsage(){
	cout << "Usage: " << endl
		<< "   RGBDCapture [-camera]" << endl << endl;
	cout << "-camera:" << endl
		<< "   -kinect: Microsoft Kinect v1 (by default)" << endl
		<< "   -asus: Asus XTion" << endl;
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