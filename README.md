# RGBDCapture
This code is to capture RGB-D data with a depth cameras (Kinect v1, v2, Asus XTion, Intel RealScene, etc). Specifically, this code can
* capture RGB-D data in a single compressed .klg log file (see [ElasticFusion] (https://github.com/mp3guy/ElasticFusion/blob/master/README.md) as well as source codes for more details about klg format).
* decompress the .klg file into png images in [TUM format RGB-D data] (http://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats).

This code is written in C++ and is tested successfully in Visual Studio 2013 in Windows and Ubuntu 14.04.

##Dependencies
- OpenCV 2.4.X
- OpenNI2
- zlib
- Boost (>= 1.50)
- cmake (only required for Linux)
- Driver for Kinect v2: Kinect for Windows SDK 2.0 (Windows) or libfreenect2 (Linux)

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
RGBDCapture [-option] [-camera] [klgFilename]
```
where
* -option: *-c* to use depth camera to scan (by default), *-d* to decompress the klg file
* -camera: camera type. *-kinect* denotes Microsoft kinect v1 (by default), *-asus* Asus XTion, *-intel* Intel RealScene, *-kinect2* Microsoft Kinect v2
* klgFilename: name of the .klg log file ( *saved.klg* by default)

##Note
* The decompressed png files are saved in a newly created folder whose name is the same as klg file's in the local directory.
* Check the path of OPENNI2 driver files if the code cannot start.
* Refer to [TUM format RGB-D data] (http://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats) for more details about how to use the RGB and depth images.
