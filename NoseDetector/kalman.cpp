#include "Kalman.h"

Kalman::Kalman( void )
{
  cv::KalmanFilter KalmanFilter( 4 , 2 , 0 );
	cv::Mat_< float > state( 4 , 1 );
	cv::Mat noise( 4 , 1 , CV_32F );
	cv::Mat_< float > measure( 2 , 1 );
	measure.setTo( cv::Scalar( 0 ) );

	this->filter = KalmanFilter;
	this->measure = measure;

	return;
}

void Kalman::setKalman( int x , int y )
{
	this->filter.statePre.at< float >( 0 ) = x;
	this->filter.statePre.at< float >( 1 ) = y;
	this->filter.statePre.at< float >( 2 ) = 0;
	this->filter.statePre.at< float >( 3 ) = 0;

	this->filter.transitionMatrix = *( cv::Mat_< float >( 4 , 4 ) << 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 );

	cv::setIdentity( this->filter.measurementMatrix );
	cv::setIdentity( this->filter.processNoiseCov , cv::Scalar::all( 1e-4 ) );
	cv::setIdentity( this->filter.measurementNoiseCov , cv::Scalar::all( 1e-1 ) );
	cv::setIdentity( this->filter.errorCovPost , cv::Scalar::all( .1 ) );

	return;
}

cv::Point Kalman::predict( void )
{
	cv::Mat predict = this->filter.predict();
	cv::Point pt( predict.at< float >( 0 ) , predict.at< float >( 1 ) );

	return pt;
}

cv::Point Kalman::estimate( void )
{
	cv::Mat estimate = this->filter.correct( this->measure );
	cv::Point pt( estimate.at< float >( 0 ) , estimate.at< float >( 1 ) );

	return pt;
}

void Kalman::changeMeasure( int x , int y )
{
	this->measure( 0 ) = x;
	this->measure( 1 ) = y;

	return;
}
