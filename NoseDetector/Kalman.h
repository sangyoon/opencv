#ifndef _KALMAN_INCLUDED_
#define _KALMAN_INCLUDED_

#include <iostream>
#include <string>
#include <opencv2\opencv.hpp>

class Kalman
{
public:
  Kalman( void );
	void setKalman( int , int );
	cv::Point predict( void );
	cv::Point estimate( void );
	void changeMeasure( int , int );

private:
	cv::KalmanFilter filter;
	cv::Mat_< float > measure;
};

#endif
