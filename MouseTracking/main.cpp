#include <iostream>
#include <vector>
#include <opencv2\video\tracking.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

struct MousePointer
{
	int x;
	int y;
};

MousePointer mouse = { -1 , -1 }, lastMouse;

void mouseEvent( int event , int x , int y , int flags , void *param )
{
	lastMouse = mouse;

	mouse.x = x;
	mouse.y = y;

	std::cout << "Current Mouse Position: " << x << ", " << y << std::endl;
}

int main( int argc , char *argv[] )
{
	cv::Mat img( cv::Size( 640 , 480 ) , CV_8UC3 );

	cv::KalmanFilter filter( 4 , 2 , 0 );
	cv::Mat processNoise( 4 , 1 , CV_32F );
	
	cv::Mat_< float > state( 4 , 1 );
	cv::Mat_< float > measure( 2 , 1 );
	measure.setTo( cv::Scalar( 0 ) );

	char key = -1;

	std::vector< cv::Point > mouseVector;
	std::vector< cv::Point > kalmanVector;

	cv::namedWindow( "Mouse Tracking" );
	cv::setMouseCallback( "Mouse Tracking" , mouseEvent , 0 );

	while( true )
	{
		if( mouse.x < 0 || mouse.y < 0 )
		{
			imshow( "Mouse Tracking" , img );
			cv::waitKey( 30 );

			continue;
		}

		filter.statePre.at< float >( 0 ) = mouse.x;
		filter.statePre.at< float >( 1 ) = mouse.y;
		filter.statePre.at< float >( 2 ) = 0;
		filter.statePre.at< float >( 3 ) = 0;

		filter.transitionMatrix = *( cv::Mat_< float >( 4 , 4 ) << 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 );

		cv::setIdentity( filter.measurementMatrix );
		cv::setIdentity( filter.processNoiseCov , cv::Scalar::all( 1e-4 ) );
		cv::setIdentity( filter.measurementNoiseCov , cv::Scalar::all( 1e-1 ) );
		cv::setIdentity( filter.errorCovPost , cv::Scalar::all( .1 ) );

		mouseVector.clear();
		kalmanVector.clear();

		while( true )
		{
			cv::Mat predict = filter.predict();
			cv::Point predictPoint( predict.at< float >( 0 ) , predict.at< float >( 1 ) );

			measure( 0 ) = mouse.x;
			measure( 1 ) = mouse.y;

			cv::Point measurePoint( measure( 0 ) , measure( 1 ) );
			mouseVector.push_back( measurePoint );

			cv::Mat estimate = filter.correct( measure );
			cv::Point statePoint( estimate.at< float >( 0 ) , estimate.at< float >( 1 ) );
			kalmanVector.push_back( statePoint );

			cv::circle( img , measurePoint , 5 , cv::Scalar( 0 , 255 , 0 ) , -1 );
			for( size_t i = 0 ; i < mouseVector.size() - 1 ; ++ i )
				cv::line( img , mouseVector[ i ] , mouseVector[ i + 1 ] , cv::Scalar( 0 , 255 , 0 ) );

			cv::circle( img , statePoint , 5 , cv::Scalar( 255 , 0 , 0 ) , -1 );
			for( size_t i = 0 ; i < kalmanVector.size() - 1 ; ++ i )
				cv::line( img , kalmanVector[ i ] , kalmanVector[ i + 1 ] , cv::Scalar( 255 , 0 , 0 ) );

			imshow( "Mouse Tracking" , img );
			
			key = static_cast< char >( cv::waitKey( 10 ) );

			if( key > 0 )
				break;			
		}

		if( key == 27 )
			break;
	}

	return 0;
}
