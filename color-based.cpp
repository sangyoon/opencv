#include <iostream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

int main( int argc , const char **argv )
{
	cv::Mat src = cv::imread( "profile.jpg" , CV_LOAD_IMAGE_COLOR );

	if( src.empty() )
	{
		std::cerr << "Could not find or open the image." << std::endl;

		return -1;
	}

	cv::namedWindow(             "Original" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow(                  "HSV" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow(   	              "Hue" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow(           "Saturation" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow(                "Value" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow(        "Threshold Hue" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow( "Threshold Saturation" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow(      "Threshold Value" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow(                 "Mask" , CV_WINDOW_AUTOSIZE );
	cv::namedWindow(        "Skin Detector" , CV_WINDOW_AUTOSIZE );

	cv::resize( src , src , cv::Size( src.cols / 4 , src.rows / 4 ) );
	
	cv::Mat hsv;
	hsv.create( src.size() , CV_8UC3 );
	cv::cvtColor( src , hsv , CV_BGR2HSV );

	std::vector< cv::Mat > plane;
	cv::split( hsv , plane );

	cv::Mat planeH, planeS, planeV;
	planeH.create( src.size() , CV_8U );
	planeS.create( src.size() , CV_8U );
	planeV.create( src.size() , CV_8U );
	cv::threshold( plane[ 0 ] , planeH , 18.0 , UCHAR_MAX , cv::THRESH_BINARY_INV );
	cv::threshold( plane[ 1 ] , planeS , 50.0 , UCHAR_MAX ,     cv::THRESH_BINARY );
	cv::threshold( plane[ 2 ] , planeV , 80.0 , UCHAR_MAX ,     cv::THRESH_BINARY );

	cv::Mat skin;
	skin.create( src.size() , CV_8U );
	cv::bitwise_and( planeH , planeS , skin );
	cv::bitwise_and( skin , planeV , skin );
	cv::convertScaleAbs( skin , skin );

	cv::Mat binary;
	std::vector< std::vector< cv::Point2i > > blobs;
	cv::threshold( skin , binary , 0.0 , UCHAR_MAX , cv::THRESH_BINARY );
	cv::medianBlur( binary , binary , 7 );
	
	cv::Mat kernel = getStructuringElement( cv::MORPH_ELLIPSE , cv::Size( 7 , 7 ) );
	cv::dilate( binary , binary , kernel );
	cv::erode(  binary , binary , kernel );

	std::vector< std::vector< cv::Point > > contours;
	std::vector< cv::Vec4i > hierarchy;
	cv::findContours( binary , contours , hierarchy , cv::RETR_EXTERNAL , cv::CHAIN_APPROX_SIMPLE );

	cv::RotatedRect minRect;
	cv::RotatedRect minEllipse;
	
	double maxArea = 0.0;
	for( size_t i = 0 ; i < contours.size() ; ++i )
	{
		double area = cv::contourArea( contours[ i ] );

		if( area > maxArea )
		{
			area = maxArea;
			minRect = cv::minAreaRect( cv::Mat( contours[ i ] ) );

			if( contours[ i ].size() > 5 )
				minEllipse = cv::fitEllipse( cv::Mat( contours[ i ] ) );
		}
	}
	
	cv::Mat mask = cv::Mat::zeros( src.size() , CV_8UC3 );
	cv::ellipse( mask , minEllipse , CV_RGB( 255 , 255 , 255 ) , -1 , 255 );	
	
	cv::Mat dst;
	dst.create( src.size() , src.type() );
	src.copyTo( dst , mask );

	cv::imshow(             "Original" ,        src );
	cv::imshow(                  "HSV" ,        hsv );
	cv::imshow(                  "Hue" , plane[ 0 ] );
	cv::imshow(           "Saturation" , plane[ 1 ] );
	cv::imshow(                "Value" , plane[ 2 ] );
	cv::imshow(        "Threshold Hue" ,     planeH );
	cv::imshow( "Threshold Saturation" ,     planeS );
	cv::imshow(      "Threshold Value" ,     planeV );
	cv::imshow(                 "Mask" ,       mask );
	cv::imshow(        "Skin Detector" ,        dst );

	cv::waitKey( 0 );
	cv::destroyAllWindows();

	return 0;
}
