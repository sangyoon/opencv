#ifndef _NOSEDETECTOR_INCLUDED_
#define _NOSEDETECTOR_INCLUDED_

#include <iostream>
#include <string>
#include <opencv2\opencv.hpp>

class NoseDetector
{
public:
  NoseDetector( std::string );

	cv::Point findNose( cv::Mat );

private:
	cv::CascadeClassifier noseCascade;
};

#endif
