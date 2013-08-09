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
		
	std::vector< cv::Point2f > corners[ 2 ];
	std::vector< std::vector< cv::Point2f > > imagePoints[ 2 ];
	std::vector< std::vector< cv::Point3f > > objectPoints;

	std::vector< cv::Point3f > objects;
	for( int i = 0 ; i < numberOfBoard ; ++i )
		objects.push_back( cv::Point3f( i / boardWidth  , i % boardWidth , 0.0f ) );

	cv::Mat leftEye, rightEye, leftGray, rightGray;

	cv::VideoCapture  leftCamera = cv::VideoCapture( 0 );
	cv::VideoCapture rightCamera = cv::VideoCapture( 1 );

	if( !leftCamera.isOpened() || !rightCamera.isOpened() )
	{
		std::cerr << "Cannot to find or open connected camera." << std::endl;

		return -1;
	}

	int success = 0;
	bool found[ 2 ] = { false , false };
	char key;

	while( success < boardNumber )
	{
		 leftCamera >> leftEye;
		rightCamera >> rightEye;

		cv::cvtColor(  leftEye ,  leftGray , cv::COLOR_BGR2GRAY );
		cv::cvtColor( rightEye , rightGray , cv::COLOR_BGR2GRAY );
		
		found[ 0 ] = cv::findChessboardCorners(  leftEye , boardSize , corners[ 0 ] , cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS );
		found[ 1 ] = cv::findChessboardCorners( rightEye , boardSize , corners[ 1 ] , cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS );

		if( found[ 0 ] )
		{
			cv::cornerSubPix( leftGray , corners[ 0 ] , cv::Size( 11 , 11 ) , cv::Size( -1 , -1 ) , cv::TermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS , 30 , 0.1 ) );
			cv::drawChessboardCorners( leftGray , boardSize , corners[ 0 ] , found[ 0 ] );
		}

		if( found[ 1 ] )
		{
			cv::cornerSubPix( rightGray , corners[ 1 ] , cv::Size( 11 , 11 ) , cv::Size( -1 , -1 ) , cv::TermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS , 30 , 0.1 ) );
			cv::drawChessboardCorners( rightGray , boardSize , corners[ 1 ] , found[ 1 ] );
		}

		cv::imshow(  "Left Eye" , leftGray );
		cv::imshow( "Right Eye" , rightEye );

		key = static_cast< char >( cv::waitKey( 10 ) );

		if( found[ 0 ] && found[ 1 ] )
			key = static_cast< char >( cv::waitKey( 0 ) );

		if( key == 27 )
			break;

		if( key == 32 && found[ 0 ] != false && found[ 1 ] != false )
		{
			imagePoints[ 0 ].push_back( corners[ 0 ] );
			imagePoints[ 1 ].push_back( corners[ 1 ] );

			objectPoints.push_back( objects );

			std::cout << "Corners stored." << std::endl;

			success++;

			if( success >= boardNumber )
				break;
		}
	}

	cv::destroyAllWindows();

	std::cout << "Start Calibration..." << std::endl;
		cv::Mat CM1 = cv::Mat( cv::Size( 3 , 3 ) , CV_64FC1 );
		cv::Mat CM2 = cv::Mat( cv::Size( 3 , 3 ) , CV_64FC1 );
		cv::Mat D1, D2;
		cv::Mat R, T, E, F;

		cv::stereoCalibrate( objectPoints , imagePoints[ 0 ] , imagePoints[ 1 ] ,
							 CM1 , D1 ,
							 CM2 , D2 ,
							 leftEye.size() ,
							 R , T , E , F ,
							 cv::TermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS , 100 , 1e-5 ) ,
							 cv::CALIB_SAME_FOCAL_LENGTH | cv::CALIB_ZERO_TANGENT_DIST );

		cv::FileStorage fs( "CalibrationData.yml" , cv::FileStorage::WRITE );
		fs << "CM1" << CM1 << "CM2" << CM2 << "D1" << D1 << "D2" << D2 << "R" << R << "T" << T << "E" << E << "F" << F;
	std::cout << "Done Calibration!" << std::endl;

	std::cout << "Start Rectification..." << std::endl;
		cv::Mat R1, R2;
		cv::Mat P1, P2;
		cv::Mat Q;

		cv::stereoRectify( CM1 , D1 , CM2 , D2 , leftEye.size() , R , T , R1 , R2 , P1 , P2 , Q );

		fs << "R1" << R1 << "R2" << R2 << "P1" << P1 << "P2" << P2 << "Q" << Q;
	std::cout << "Done Rectification!" << std::endl;

	std::cout << "Applying Undistord..." << std::endl;
		cv::Mat mapX[ 2 ], mapY[ 2 ];
		cv::Mat remapImage[ 2 ];

		cv::initUndistortRectifyMap( CM1 , D1 , R1 , P1 ,  leftEye.size() , CV_32FC1 , mapX[ 0 ] , mapY[ 0 ] );
		cv::initUndistortRectifyMap( CM2 , D2 , R2 , P2 , rightEye.size() , CV_32FC1 , mapX[ 1 ] , mapY[ 1 ] );
	std::cout << "Undistort Complete!" << std::endl;

	while( true )
	{
		 leftCamera >> leftEye;
		rightCamera >> rightEye;

		cv::remap(  leftEye , remapImage[ 0 ] , mapX[ 0 ] , mapY[ 0 ] , cv::INTER_LINEAR , cv::BORDER_CONSTANT , cv::Scalar() );
		cv::remap( rightEye , remapImage[ 1 ] , mapX[ 1 ] , mapY[ 1 ] , cv::INTER_LINEAR , cv::BORDER_CONSTANT , cv::Scalar() );
		
		imshow(  "Left Eye" , remapImage[ 0 ] );
		imshow( "Right Eye" , remapImage[ 1 ] );

		key = cv::waitKey( 5 );

		if( key == 27 )
			break;
	}

	fs.release();
	leftCamera.release();
	rightCamera.release();

	cv::destroyAllWindows();

	return 0;
}
