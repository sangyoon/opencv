#include <iostream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

int hue = 0, sat = 0, val = 0;
int hue2 = 0, sat2 = 0, val2 = 0;
const int hueMax = 180, satMax = 255, valMax = 255;

cv::Mat src, thresh;

cv::Mat threshold( const cv::Mat &img , cv::Scalar lower , cv::Scalar upper )
{
    cv::Mat result;
	cv::Mat thresh;

	thresh.create( img.size() , img.type() );
	cv::cvtColor( img , thresh , CV_BGR2HSV );

	result.create( img.size() , CV_8U );
	cv::inRange( thresh , lower , upper , result );
	cv::imshow( "Converted" , thresh );
	
	return result;
}

void onTrackbar( int , void* )
{
	 thresh = threshold( src , cv::Scalar( hue , sat , val ) , cv::Scalar( hue2 , sat2 , val2 ) );
	 cv::imshow( "Threshold" , thresh );
}

int main( int argc , const char **argv )
{
 	src = cv::imread( "color.jpg" , CV_LOAD_IMAGE_COLOR );
 	
 	if( src.empty() )
 		return -1;
 	
	cv::namedWindow( "Original Image" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow( "Control Window" , CV_WINDOW_AUTOSIZE );

	cv::imshow( "Original Image" , src );
	
	cv::createTrackbar( "Hue Min" , "Control Window" , &hue , hueMax , onTrackbar );
	cv::createTrackbar( "Hue Max" , "Control Window" , &hue2 , hueMax , onTrackbar );
	cv::createTrackbar( "Saturation Min" , "Control Window" , &sat , satMax , onTrackbar );
	cv::createTrackbar( "Saturation Max" , "Control Window" , &sat2 , satMax , onTrackbar );
	cv::createTrackbar( "Value Min" , "Control Window" , &val , valMax , onTrackbar );
	cv::createTrackbar( "Value Max" , "Control Window" , &val2 , valMax , onTrackbar );	

	cv::waitKey( 0 );

	return 0;
}
