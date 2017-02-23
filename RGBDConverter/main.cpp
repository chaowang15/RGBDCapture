#include "RGBDConverter.h"

void printUsage(){
	cout << "Usage: " << endl
		<< "   RGBDConverter -option InputFilePath" << endl << endl;
	cout << "-option:" << endl
		<< "   -k: convert a single KLG file to PNG images" << endl
		<< "   -p: convert PNG images to a singe KLG file" << endl;
	cout << "InputFilePath: " << endl
		<< "   path of KLG file or PNG images" << endl << endl;
	cout << "For instance:" << endl
		<< "    RGBDCapture -k rgbd.klg" << endl
		<< "    RGBDCapture -p C:/rgbd/" << endl << endl;
}

int main(int argc, char** argv)
{
	if (argc != 3 || (string(argv[1]) != "-k" && string(argv[1]) != "-p"))
		printUsage();

	string option(argv[1]), filepath(argv[2]);
	RGBDConverter rgbdconverter;
	if (option == "-k")
		rgbdconverter.klg2png(filepath);
	else
		rgbdconverter.png2klg(filepath);
	return EXIT_SUCCESS;
}