#include "OpenNISensor.h"
#include "Kinect2Sensor.h"

void printUsage(){
	cout << "Usage: " << endl
		<< "   RGBDCapture [-camera] [filename]" << endl << endl;
	cout << "-camera:" << endl
		<< "   -kinect: Microsoft Kinect v1 (by default)" << endl
		<< "   -asus: Asus XTion" << endl
		<< "   -intel: Intel realscene (currently not supported)" << endl
		<< "   -kinect2: Microsoft Kinect v2 (currently not supported)" << endl;
	cout << "filename: " << endl
		<< "   KLG filename (\"saved.klg\" by default)" << endl << endl;
	cout << "For instance:" << endl
		<< "    RGBDCapture -kinect" << endl
		<< "    RGBDCapture -kinect2 rgbd.klg" << endl << endl;
}


int main(int argc, char** argv)
{
	if (argc > 4)
	{
		printUsage();
		return EXIT_FAILURE;
	}
	string cameraType("-kinect"), filename("saved.klg");
	if (argc >= 2)
	{
		string option(argv[1]);
		if (option == "-d")
		{
			if (argc == 3) 
				filename = string(argv[2]);
			DataCompression dataComp;
			//dataComp.decompressKLG(filename);
			return 0;
		}
		else if (option == "-c")
		{
			if (argc >= 3)
				cameraType = string(argv[2]);
			if (argc == 4)
				filename = string(argv[3]);
		}
		else
		{
			printUsage();
			return EXIT_FAILURE;
		}
	}
	

	RGBDSensor* sensor = NULL;
	if (cameraType == "-kinect" || cameraType == "-asus")
	{
		sensor = new OpenNISensor();
		sensor->scan(filename);
	}
	else if (cameraType == "-kinect2")
	{
		sensor = new Kinect2Sensor();
		sensor->scan(filename);
	}
	if (sensor)
	{
		delete sensor;
		sensor = nullptr;
	}		

	return 0;
}