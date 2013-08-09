#include <iostream>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

int main( int argc , char** argv )
{
	int boardNumber = atoi( argv[ 1 ] );
	int  boardWidth = atoi( argv[ 2 ] );
	int boardHeight = atoi( argv[ 3 ] );

	cv::Size boardSize( boardWidth , boardHeight );
	int numberOfBoard = boardWidth * boardHeight;
		
	std::vector< cv::Point2f > corners;
	std::vector< std::vector< cv::Point2f > > imagePoints;
	std::vector< std::vector< cv::Point3f > > objectPoints;

	std::vector< cv::Point3f > objects;
	for( int i = 0 ; i < numberOfBoard ; ++i )
		objects.push_back( cv::Point3f( i / boardWidth  , i % boardWidth , 0.0f ) );

	cv::Mat img , gray;

	cv::VideoCapture camera = cv::VideoCapture( "http://10.10.10.2:8080/videofeed?frame.mjpeg" );

	if( !camera.isOpened() )
	{
		std::cerr << "Cannot to find or open connected camera." << std::endl;

		return -1;
	}

	int success = 0;
	bool found = false;
	char key;

	while( success < boardNumber )
	{
		camera >> img;

		cv::cvtColor( img , gray , cv::COLOR_BGR2GRAY );

		found = cv::findChessboardCorners( gray , boardSize , corners , cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS );

		if( found )
		{
			cv::cornerSubPix( gray , corners , cv::Size( 11 , 11 ) , cv::Size( -1 , -1 ) , cv::TermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS , 30 , 0.1 ) );
			cv::drawChessboardCorners( gray , boardSize , corners , found );
		}

		cv::imshow(     "Color" , img );
		cv::imshow( "Grayscale" , gray );

		key = static_cast< char >( cv::waitKey( 1 ) );

		if( found )
			key = static_cast< char >( cv::waitKey( 0 ) );

		if( key == 27 )
			break;

		if( key == 32 && found != false )
		{
			imagePoints.push_back( corners );
			objectPoints.push_back( objects );

			std::cout << "Corners stored." << std::endl;

			success++;

			if( success >= boardNumber )
				break;
		}
	}

	cv::destroyAllWindows();

	std::cout << "Start Calibration..." << std::endl;
		cv::Mat cameraMatrix = cv::Mat( cv::Size( 3 , 3 ) , CV_32FC1 );
		cv::Mat distceoffs;
		std::vector< cv::Mat > rvecs, tvecs;

		cameraMatrix.at< float >( 0 , 0 ) = 1;
		cameraMatrix.at< float >( 1 , 1 ) = 1;

		cv::calibrateCamera( objectPoints , imagePoints , img.size() , cameraMatrix , distceoffs , rvecs , tvecs );
				
		cv::FileStorage fs( "CalibrationData.yml" , cv::FileStorage::WRITE );
		fs << "CM1" << cameraMatrix << "D1" << distceoffs;
	std::cout << "Done Calibration!" << std::endl;
	
	cv::Mat undistortImg;
	while( true )
	{
		camera >> img;

		cv::undistort( img , undistortImg , cameraMatrix , distceoffs );

		imshow( "Color" , img );
		imshow( "Undistort", undistortImg );

		key = cv::waitKey( 5 );

		if( key == 27 )
			break;
	}

	fs.release();
	camera.release();
	cv::destroyAllWindows();

	return 0;
}
