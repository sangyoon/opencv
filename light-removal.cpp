#include <iostream>
#include <string>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace std;
using namespace cv;

int main( int argc , const char **argv )
{
 	Mat src = imread( "drug.jpg" , CV_LOAD_IMAGE_COLOR );
	Mat dst( src.size() , src.type() );
	src.copyTo( dst );

	GaussianBlur( src , src , Size( 11 , 11 ) , 10 );

	Mat yuv, channel[ 3 ];
	cvtColor( src , yuv , CV_BGR2YCrCb );
	split( yuv , channel );
	
	channel[ 0 ].setTo( Scalar::all( 0 ) );
	merge( channel , 3 , yuv );
	cvtColor( yuv , yuv , CV_YCrCb2BGR );

	Mat thresh;
	cvtColor( yuv , thresh , CV_BGR2GRAY );
	equalizeHist( thresh , thresh );
	medianBlur( thresh , thresh , 21 );
	threshold( thresh , thresh , 125.0 , 255.0 , THRESH_BINARY + THRESH_OTSU );
	erode( thresh , thresh , Mat() , Point( -1 , -1 ) , 5 );
	dilate( thresh , thresh , Mat() );	

	vector< vector< Point > > contours;
	vector< Vec4i > hierarchy;

	findContours( thresh , contours , hierarchy , RETR_EXTERNAL , CHAIN_APPROX_SIMPLE );
	
	vector< RotatedRect > minRect( contours.size() );
	vector< RotatedRect > minEllipse( contours.size() );

	for( size_t i = 0 ; i < contours.size() ; ++i )
	{
		minRect[ i ] = minAreaRect( Mat( contours[ i ] ) );

		if( contours[ i ].size() > 5 )
			minEllipse[ i ] = fitEllipse( Mat( contours[ i ] ) );
	}

	drawContours( dst , contours , -1 , CV_RGB( 255 , 0 , 0 ) , 2 );
	
	for( size_t i = 0 ; i < contours.size() ; ++i )
	{
		ellipse( dst , minEllipse[ i ] , CV_RGB( 0 , 255 , 0 ) , 2 );
	}

	imshow( "dst" , dst );

	waitKey( 0 );
	return 0;
}
