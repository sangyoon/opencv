#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\legacy\legacy.hpp>

int main( int argc , char** argv )
{
	cv::Mat  leftImage = cv::imread( "aloe-L.png" , cv::IMREAD_GRAYSCALE );
	cv::Mat rightImage = cv::imread( "aloe-R.png" , cv::IMREAD_GRAYSCALE );

	if( !leftImage.data || !rightImage.data )
		return -1;

	std::vector< cv::KeyPoint >  leftKeyPoint;
	std::vector< cv::KeyPoint > rightKeyPoint;

	cv::SiftFeatureDetector sift( 0.01 , 10.0 );
	sift.detect(  leftImage ,  leftKeyPoint );
	sift.detect( rightImage , rightKeyPoint );

	std::cout << "Number of SURF points (1): " <<  leftKeyPoint.size() << std::endl;
	std::cout << "Number of SURF points (2): " << rightKeyPoint.size() << std::endl;

	cv::SurfDescriptorExtractor surf;
	cv::Mat leftDescriptor, rightDescriptor;
	surf.compute(  leftImage ,  leftKeyPoint ,  leftDescriptor );
	surf.compute( rightImage , rightKeyPoint , rightDescriptor );

	std::cout << "Descriptor matrix size: " << leftDescriptor.rows << " by " << leftDescriptor.cols << std::endl;

	cv::BruteForceMatcher< cv::L2< float > > matcher;
	std::vector< cv::DMatch > matches;
	matcher.match( leftDescriptor , rightDescriptor , matches );

	std::cout << "Number of matched points: " << matches.size() << std::endl;

	std::vector< int >  leftPointIndexes;
	std::vector< int > rightPointIndexes;

	for( std::vector< cv::DMatch >::const_iterator it = matches.begin() ; it != matches.end() ; ++it )
	{
		 leftPointIndexes.push_back( it->queryIdx );
		rightPointIndexes.push_back( it->trainIdx );
	}

	std::vector< cv::Point2f > leftSelPoints, rightSelPoints;
	cv::KeyPoint::convert(  leftKeyPoint ,  leftSelPoints ,  leftPointIndexes );
	cv::KeyPoint::convert( rightKeyPoint , rightSelPoints , rightPointIndexes );

	cv::Mat fundemental = cv::findFundamentalMat( cv::Mat( leftSelPoints ) , cv::Mat( rightSelPoints ) , CV_FM_RANSAC );
	
	std::cout<< "F-Matrix size: " << fundemental.rows << ", " << fundemental.cols << std::endl;

	cv::Mat H1( 4 , 4 , rightImage.type() );
	cv::Mat H2( 4 , 4 , rightImage.type() );
	cv::stereoRectifyUncalibrated( rightSelPoints , leftSelPoints , fundemental , rightImage.size() , H1 , H2 );

	cv::Mat disparityImage16S = cv::Mat( leftImage.rows , leftImage.cols , CV_16S );
	cv::Mat disparityImage = cv::Mat( leftImage.rows , leftImage.cols , CV_8UC1 );

	int disparities = 16 * 5;
	int SADWindowSize = 5;
	cv::StereoBM sbm( cv::StereoBM::BASIC_PRESET , disparities , SADWindowSize );
	sbm( leftImage , rightImage , disparityImage16S , CV_16S );

	double minValue, maxValue;
	cv::minMaxLoc( disparityImage16S , &minValue , &maxValue );
	disparityImage16S.convertTo( disparityImage , CV_8UC1 , 255 / ( maxValue - minValue ) );

	std::cout << "Minimum dispairty: " << minValue << " Maximum value: " << maxValue << std::endl;

	cv::namedWindow( "Disparity" , cv::WINDOW_NORMAL );
	cv::imshow( "Disparity" , disparityImage );

	cv::imwrite( "disp.pgm" , disparityImage );

	cv::waitKey( 0 );
	cv::destroyAllWindows();

	return 0;
}
