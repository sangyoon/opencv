#include <iostream>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

int main( int argc , char **argv )
{
  	cv::Mat src = cv::imread( "maze.png" , cv::IMREAD_COLOR );

	if( src.empty() )
	{
		std::cerr << "Could not find or open the image." << std::endl;

		return -1;
	}

	cv::Mat grayscale;
	cv::cvtColor( src , grayscale , cv::COLOR_BGR2GRAY );

	cv::Mat thresh;
	cv::threshold( grayscale , thresh , 10.0 , 255.0 , cv::THRESH_BINARY_INV );
	
	std::vector< std::vector< cv::Point > > contours;
	cv::findContours( thresh , contours , cv::RETR_EXTERNAL , cv::CHAIN_APPROX_SIMPLE );

	if( contours.size() != 2 )
		return -1;

	cv::Mat path = cv::Mat::zeros( src.size() , CV_8UC1 );
	cv::drawContours( path , contours , 0 , CV_RGB( 255 , 255 , 255 ) , -1 );

	cv::Mat dPath, ePath;
	cv::Mat kernel = cv::Mat::ones( cv::Size( 19 , 19 ) , CV_8UC1 );
	cv::dilate( path , dPath , kernel );
	cv::erode( dPath , ePath , kernel );

	cv::Mat step;
	cv::absdiff( dPath , ePath , step );

	std::vector< cv::Mat > ch;
	cv::split( src , ch );
	ch[ 0 ] &= ~step;
	ch[ 1 ] &= ~step;
	ch[ 2 ] |= step;

	cv::Mat dst;
	cv::merge( ch , dst );
	
	cv::imshow( "Source" , src );
	cv::imshow( "Find the exit" , dst );

	cv::waitKey( 0 );
	return 0;
}
