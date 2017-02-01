# RGBDCapture
This code is to capture RGB-D data with one OpenNI-supported depth cameras (Kinect, Asus XTion, etc). Specifically, this code can

* capture RGB-D data in a single compressed .klg log file (refer to [ElasticFusion] (https://github.com/mp3guy/ElasticFusion/blob/master/README.md) for more details about KLG format).

* decompress the .klg file into png images in [TUM format RGB-D data] (http://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats).

This code is compiled and tested successfully in Visual Studio 2013.

##Dependencies:
- OpenCV 2.4.x
- OpenNI2
- zlib
- Boost (>= 1.50)

##Usage:
```bash
RGBDCapture [-option] [-camera] [klgFilename]
```
where
* -option: *-c* to use depth camera to scan (by default), *-d* to decompress the klg file
* -camera: camera type. *-kinect* denotes Microsoft kinect v1 (by default), *-asus* Asus XTion, *-intel* Intel RealScene, *-kinect2* Microsoft Kinect v2
* klgFilename: name of the .klg log file ( *saved.klg* by default)

##Note:
* The decompressed png files are saved in a newly created folder whose name is the same as klg file's in the local directory.
* Check the path of OPENNI2 driver files if the code cannot start.
* Refer to [TUM format RGB-D data] (http://vision.in.tum.de/data/datasets/rgbd-dataset/file_formats) for more details about how to use the RGB and depth images.
