#include <iostream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

class Segmenter
{
public:
  	void setMarker( const cv::Mat &markerImage )
	{
		markerImage.convertTo( this->marker , CV_32SC1 );
	}

	cv::Mat segmentation( const cv::Mat &image )
	{
		cv::watershed( image , this->marker );
		//this->marker.convertTo( this->marker , CV_8U );

		return this->marker;
	}

private:
	cv::Mat marker;
};

int main( int argc , const char **argv )
{
	cv::Mat image = cv::imread( "watershed.jpg" , CV_LOAD_IMAGE_COLOR );

	cv::Mat grayscale;
	cvtColor( image , grayscale , cv::COLOR_BGR2GRAY );

	cv::Mat thresh;
	cv::threshold( grayscale , thresh , 0.0 , 255.0 , cv::THRESH_BINARY + cv::THRESH_OTSU );

	cv::Mat foreground;
	cv::erode( thresh , foreground , cv::Mat() , cv::Point( -1 , -1 ) , 2 );

	cv::Mat background;
	cv::dilate( thresh , background , cv::Mat() , cv::Point( -1 , -1 ) , 3 );
	cv::threshold( background , background , 1.0 , 128.0 , cv::THRESH_BINARY_INV );

	cv::Mat marker( thresh.size() , CV_8U , cv::Scalar( 0 ) );
	cv::add( foreground , background , marker );
	
	Segmenter segmenter;
	segmenter.setMarker( marker );
	
	cv::Mat m = segmenter.segmentation( image );
	cv::convertScaleAbs( m , m );

	cv::Mat mask;
	cv::threshold( m , mask , 0.0 , 255.0 , cv::THRESH_BINARY + cv::THRESH_OTSU );

	cv::Mat output = cv::Mat::zeros( image.size() , CV_8UC1 );
	image.copyTo( output , mask );

	imshow( "Source" , image );
	imshow( "Grayscale" , grayscale );
	imshow( "threshold" , thresh );
	imshow( "Foregound" , foreground );
	imshow( "Backgound" , background );
	imshow( "Marker" , marker );
	imshow( "Watershed" , m );
	imshow( "Mask" , mask );
	imshow( "Result" , output );

	cv::waitKey( 0 );

	return -1;
}
