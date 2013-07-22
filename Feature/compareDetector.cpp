#include <iostream>
#include <vector>
#include <string>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\features2d\features2d.hpp>

int main( int argc , char **argv )
{
    std::string item[] = { "FAST" , "STAR" , "SIFT" , "SURF" , "ORB" , "MSER" , "GFTT" , "HARRIS" , "Dense" , "SimpleBlob" , "GridFAST" , "GridSTAR" , "GridSIFT" , "GridSURF" , "GridORB" , "GridMSER" , "GridGFTT" , "GridHARRIS" , "GridDense" , "GridSimpleBlob" , "PyramidFAST" , "PyramidSTAR" , "PyramidSIFT" , "PyramidSURF" , "PyramidORB" , "PyramidMSER" , "PyramidGFTT" , "PyramidHARRIS" , "PyramidDense" , "PyramidSimpleBlob" };	
	std::vector< std::string > list( item , item + 30 );

	cv::Mat img, gray;
	
	img = cv::imread( "feature.jpg" , cv::IMREAD_COLOR );
	
	for( std::vector< std::string >::iterator d = list.begin() ; d != list.end() ; ++d )
	{
		gray.create( img.size() , img.type() );

		cv::Ptr< cv::FeatureDetector > detector = cv::FeatureDetector::create( "FAST" );
		std::vector< cv::KeyPoint > keypoint;
		int n = 0;

		keypoint.clear();
	
		cv::cvtColor( img , gray , cv::COLOR_BGR2GRAY );
		detector->detect( gray , keypoint );
		n = static_cast< int >( keypoint.size() );

		for( int i = 0 ; i < n ; ++i )
			cv::circle( img , cv::Point2i( keypoint[ i ].pt.x , keypoint[ i ].pt.y ) , 2 , cv::Scalar( 255. , 0. , 0. , 255. ) );

		cv::imshow( *d , img );

		std::cout << *d << " number of Keypoints objects " << n << std::endl;
	}

	cv::waitKey( 0 );
	cv::destroyAllWindows();
	
	return 0;
}
