#include "OpenNISensor.h"

void printUsage(){
	cout << "Usage: " << endl
		<< "   RGBDCapture [-option] [-camera] [filename]" << endl << endl;
	cout << "-option:" << endl
		<< "   -c: use depth devices to scan and compress RGBD data into a KLG log file (by default)" << endl
		<< "   -d: decompress the klg file into png images" << endl;
	cout << "-camera:" << endl
		<< "   -kinect: Microsoft Kinect v1 (by default)" << endl
		<< "   -asus: Asus XTion" << endl
		<< "   -intel: Intel realscene (currently not supported)" << endl
		<< "   -kinect2: Microsoft Kinect v2 (currently not supported)" << endl;
	cout << "filename: " << endl
		<< "   KLG file (\"saved.klg\" by default)" << endl << endl;
	cout << "For instance:" << endl
		<< "    RGBDCapture -c -kinect rgbd.klg" << endl
		<< "    RGBDCapture -d rgbd.klg" << endl << endl;
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
			dataComp.decompressKLG(filename);
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
	if (sensor)
	{
		delete sensor;
		sensor = nullptr;
	}		

	return 0;
}