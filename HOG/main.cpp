#include <iostream>
#include <cstring>
#include <vector>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\gpu\gpu.hpp>

cv::RNG rng( 12345 );

int main( int argc , char **argv )
{
  	cv::VideoCapture video( "people-small.avi" );

	if( !video.isOpened() )
	{
		std::cerr << "Cannot find or open the file." << std::endl;

		return 1;
	}

	cv::namedWindow( "People" , cv::WINDOW_AUTOSIZE );

	cv::Mat frame, gray;
	cv::gpu::GpuMat gpuFrame , gpuGray;

	cv::HOGDescriptor Descriptor;
	Descriptor.setSVMDetector( cv::HOGDescriptor::getDefaultPeopleDetector() );

	std::vector< cv::Rect > person;

	std::stringstream str;

	while( true )
	{
		person.clear();
		str.str( "" );
		
		video >> frame;

		if( frame.empty() ) return 1;

		double tick = static_cast< double >( cv::getTickCount() );
		Descriptor.detectMultiScale( frame , person , 0.0 , cv::Size( 8 , 8 ) , cv::Size( 0 , 0 ) , 1.05 , 1.0 );
		tick = static_cast< double >( cv::getTickCount() ) - tick;

		str << "FPS:" << 1000.0 * tick / cv::getTickFrequency();
		cv::putText( frame , str.str() , cv::Point( 50 , 50 ) , cv::FONT_HERSHEY_SIMPLEX , 1.0 , cv::Scalar( 0 , 0 , 0 ) , 2 );

		for( std::vector< cv::Rect >::iterator i = person.begin() ; i != person.end() ; ++i )
		{
			cv::Rect found = *i;

			cv::rectangle( frame , found.tl() , found.br() , cv::Scalar( 255 , 0 , 0 ) , 1 );
		}

		cv::imshow( "People" , frame );

		if( cv::waitKey( 10 ) == 27 )
			break;
	}

	cv::destroyAllWindows();
	
	return 0;
}
