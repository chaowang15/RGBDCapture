# RGBDCapture
This code is to capture RGB-D data into PNG images with a depth camera, including OpenNI-drived devices (Kinect v1, Asus XTion, etc), Intel RealScene and Kinect v2.
This code is written in C++ and is tested successfully in Visual Studio 2013 in Windows and Ubuntu 14.04.
##Dependencies
- OpenCV 2.4.X -- render and write images
- OpenNI2 -- driver
- Boost (>= 1.50) -- create folders
- cmake -- compile codes (only required in Linux)
- Kinect v2 driver -- Kinect for Windows SDK 2.0 (Windows) or libfreenect2 (Linux, currently not supported)
##Build
To compile the code:
* In Windows, use Visual Studio to open .sln file and compile.
* In Linux, use the standard approach
```
  $ mkdir build
  $ cd build
  $ cmake ..
  $ make
```
Note to modify corresponding paths of dependencies.
##Usage
```bash
RGBDCapture [-camera]
```
where *-camera*:
* -kinect: Microsoft Kinect v1 (by default)
* -asus: Asus XTion
* -intel: Intel realscene
* -kinect2: Microsoft Kinect v2

##Note
* Press ESC to quit scanning.
* The RGB-D data will be saved in a newly created folder named "saveX", where X is a counting number starting from 0.
* Check the path of OPENNI2 driver files if the code cannot start.
* The depth images are stored in TUM RGB-D data format. Refer to [TUM format RGB-D data] (http://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats) for more details about how to use the RGB and depth images.
* When using Kinect v2, two other kinds of images will also be captured: color-depth mapping images (containing each color pixel's depth values) and depth-color mapping images (containing each depth pixel's color values).
* Another code [RGBDConverter] (https://github.com/chaowang15/RGBDConverter) can be used to convert the PNG sequence images to a single KLG log file and vice versa. See [ElasticFusion] (https://github.com/mp3guy/ElasticFusion) code for details about the detailed format.
