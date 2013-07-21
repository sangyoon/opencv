#include "NoseDetector.h"

NoseDetector::NoseDetector( std::string filename )
{
  filename = ( filename.empty() ) ? "haarcascade_mcs_nose.xml" : filename;

	if( !this->noseCascade.load( filename ) )
		std::cerr << "Could not find or open cascade xml file." << std::endl;
}

cv::Point NoseDetector::findNose( cv::Mat frame )
{
	cv::Mat grayFrame;
	std::vector< cv::Rect > noses;

	cv::cvtColor( frame , grayFrame , cv::COLOR_BGR2GRAY );
	cv::equalizeHist( grayFrame , grayFrame );

	this->noseCascade.detectMultiScale( grayFrame , noses , 1.1 , 2. , 0 | cv::CASCADE_SCALE_IMAGE , cv::Size( 30 , 30 ) );

	cv::Point found( noses[ 0 ].x , noses[ 0 ].y );


	return found;
}
