#include <iostream>
#include <string>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "NoseDetector.h"
#include "Kalman.h"

int main( int argc , char **argv )
{
  cv::Mat img( cv::Size( 500 , 500 ) , CV_8UC3 );

	cv::VideoCapture camera( "http://10.10.10.2:8080/videofeed?frame.mjpeg" );

	NoseDetector detector( "nose.xml" );
	Kalman kalman;
	std::vector< cv::Point > mv;
	std::vector< cv::Point > kv;

	char key;

	while( camera.isOpened() )
	{
		kalman.setKalman( 0 , 0 );

		while( true )
		{
			camera >> img;

			cv::Point predict = kalman.predict();
			cv::Point found = detector.findNose( img );

			int x = found.x, y = found.y;

			kalman.changeMeasure( x , y );
			cv::Point measurePoint( x , y );
			mv.push_back( measurePoint );

			cv::Point statePoint = kalman.estimate();
			kv.push_back( statePoint );

			cv::circle( img , statePoint , 5 , cv::Scalar( 0 , 255 , 0 ) );
			cv::circle( img , measurePoint , 5 , cv::Scalar( 0 , 0 , 255 ) );

			for( size_t i = 0 ; i < mv.size() - 1 ; ++i )
				cv::line( img , mv[ i ] , mv[ i + 1 ] , cv::Scalar( 0 , 0 , 255 ) );

			for( size_t i = 0 ; i < kv.size() - 1 ; ++i )
				cv::line( img , kv[ i ] , kv[ i + 1 ] , cv::Scalar( 0 , 255 , 0 ) );

			imshow( "Nose Detector" , img );

			key = static_cast< char >( cv::waitKey( 100 ) );

			if( key > 0 )
				break;
		}

		if( key == 27 )
			break;
	}

	return 0;
}
