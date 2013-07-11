#include <iostream>
#include <string>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

int hue = 0, sat = 0, val = 0;
int hue2 = 0, sat2 = 0, val2 = 0;
const int hueMax = 180, satMax = 255, valMax = 255;

cv::Mat frame, thresh;

cv::Mat threshold( const cv::Mat &img , cv::Scalar lower , cv::Scalar upper )
{
  cv::Mat result;
	cv::Mat thresh;

	thresh.create( img.size() , img.type() );
	cv::cvtColor( img , thresh , CV_BGR2HSV );

	result.create( img.size() , CV_8U );
	cv::inRange( thresh , lower , upper , result );
	cv::imshow( "Converted Frame" , thresh );
	
	return result;
}

void onTrackbar( int , void* )
{
	 thresh = threshold( frame , cv::Scalar( hue , sat , val ) , cv::Scalar( hue2 , sat2 , val2 ) );
	 cv::imshow( "Threshold Frame" , thresh );
}

int main( int argc , const char **argv )
{
	std::string remoteAddr;

	std::cout << "IP: ";
	std::cin >> remoteAddr;

	std::cout << remoteAddr << std::endl;
	
	cv::VideoCapture camera( remoteAddr );

	if( !camera.isOpened() )
	{
		std::cerr << "Cannot find or connect the camera." << std::endl;

		return -1;
	}

	cv::namedWindow( "Control Window" , CV_WINDOW_FREERATIO );

	while( true )
	{
		camera >> frame;

		cv::imshow( "Original Frame" , frame );
		
		cv::createTrackbar( "Hue Min" , "Control Window" , &hue , hueMax , onTrackbar );
		cv::createTrackbar( "Hue Max" , "Control Window" , &hue2 , hueMax , onTrackbar );
		cv::createTrackbar( "Saturation Min" , "Control Window" , &sat , satMax , onTrackbar );
		cv::createTrackbar( "Saturation Max" , "Control Window" , &sat2 , satMax , onTrackbar );
		cv::createTrackbar( "Value Min" , "Control Window" , &val , valMax , onTrackbar );
		cv::createTrackbar( "Value Max" , "Control Window" , &val2 , valMax , onTrackbar );

		if( cv::waitKey( 30 ) >= 0 )
			break;
	}

	return 0;
}
