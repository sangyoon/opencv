#include <iostream>
#include <string>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect\objdetect.hpp>

int main( int argc , const char **argv )
{
  	cv::Mat src, dst;

	src = cv::imread( "person.jpg" , CV_LOAD_IMAGE_COLOR );
	
	if( src.empty() )
	{
		std::cerr << "Could not open or find the image!" << std::endl;

		return -1;
	}

	cv::namedWindow( "Source Image" , CV_WINDOW_AUTOSIZE );
		
	cv::Mat grayscale;
	cv::cvtColor( src , grayscale , CV_BGR2GRAY );
	cv::equalizeHist( grayscale , grayscale );
	
	cv::CascadeClassifier faceCascade, leftEyeCascade, rightEyeCascade, noseCascade, mouthCascade;
	faceCascade.load( "frontalface.xml" );
	leftEyeCascade.load( "lefteye.xml" );
	rightEyeCascade.load( "righteye.xml" );
	noseCascade.load( "nose.xml" );
	mouthCascade.load( "mouse.xml" );

	std::vector< cv::Rect > faces;
	faceCascade.detectMultiScale( grayscale , faces , 1.2 , 2 , CV_HAAR_DO_CANNY_PRUNING , cv::Size( 50 , 50 ) );

	dst.create( src.size() , src.type() );
	src.copyTo( dst );

	for( std::vector< cv::Rect >::iterator i = faces.begin() ; i != faces.end() ; ++i )
	{
		cv::rectangle( dst , cv::Rect( int( ( *i ).x ) , int( ( *i ).y ) , int( ( *i ).width ) , int( ( *i ).height ) ) , CV_RGB( 0 , 255 , 0 ) , 3 );

		cv::Mat face = grayscale( *i );
		
		std::vector< cv::Rect > leftEyes;
		leftEyeCascade.detectMultiScale( grayscale , leftEyes , 1.2 , 2 , CV_HAAR_DO_CANNY_PRUNING , cv::Size( 30 , 30 ) );
		
		cv::Point center( leftEyes[ 1 ].x + leftEyes[ 1 ].width * 0.5 , leftEyes[ 1 ].y + leftEyes[ 1 ].height * 0.5 );
		int radius = cvRound( ( leftEyes[ 1 ].width + leftEyes[ 1 ].height ) * 0.25 );
		cv::circle( dst , center , radius , CV_RGB( 255 , 0 , 0 ) , 2 );

		std::vector< cv::Rect > rightEyes;
		rightEyeCascade.detectMultiScale( grayscale , rightEyes , 1.2 , 2 , CV_HAAR_DO_CANNY_PRUNING , cv::Size( 30 , 30 ) );
		center = cv::Point( rightEyes[ 1 ].x + rightEyes[ 1 ].width * 0.5 , rightEyes[ 1 ].y + rightEyes[ 1 ].height * 0.5 );
		radius = cvRound( ( rightEyes[ 1 ].width + rightEyes[ 1 ].height ) * 0.25 );
		cv::circle( dst , center , radius , CV_RGB( 0 , 0 , 255 ) , 2 );

		std::vector< cv::Rect > noses;
		noseCascade.detectMultiScale( grayscale , noses , 1.2 , 2 , CV_HAAR_DO_CANNY_PRUNING , cv::Size( 30 , 30 ) );

		if( !noses.empty() )
			cv::rectangle( dst , cv::Rect( int( noses[ 0 ].x ) , int( noses[ 0 ].y ) , int( noses[ 0 ].width ) , int( noses[ 0 ].height ) ) , CV_RGB( 0 , 255 , 255 ) , 2 );
		
		std::vector< cv::Rect > mouthes;
		mouthCascade.detectMultiScale( grayscale , mouthes , 1.2 , 2 , CV_HAAR_DO_CANNY_PRUNING , cv::Size( 30 , 30 ) );

		if( !mouthes.empty() )
			cv::rectangle( dst , cv::Rect( int( mouthes[ 0 ].x ) , int( mouthes[ 0 ].y ) , int( mouthes[ 0 ].width ) , int( mouthes[ 0 ].height ) ) , CV_RGB( 255 , 255 , 0 ) , 2 );
	}

	imshow( "Source Image" , src );
	imshow( "Grayscale Image" , grayscale );
	imshow( "Face Detector" , dst );

	cv::waitKey( 0 );
	cv::destroyAllWindows();
	return 0;
}
