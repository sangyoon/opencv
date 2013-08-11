#include <iostream>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <pcl/common/common_headers.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <boost/thread/thread.hpp>

boost::shared_ptr< pcl::visualization::PCLVisualizer > createVisualizer( pcl::PointCloud< pcl::PointXYZRGB >::ConstPtr cloud )
{
  boost::shared_ptr< pcl::visualization::PCLVisualizer > viewer( new pcl::visualization::PCLVisualizer ( "3D Viewer" ) );
	
	viewer->setBackgroundColor ( 0 , 0 , 0 );
	pcl::visualization::PointCloudColorHandlerRGBField< pcl::PointXYZRGB > rgb( cloud );
	
	viewer->addPointCloud< pcl::PointXYZRGB > ( cloud , rgb , "reconstruction" );
	
	viewer->addCoordinateSystem ( 1.0 );
	viewer->initCameraParameters ();
	
	return viewer;
}

int main( int argc , char** argv )
{
	cv::FileStorage fs( "Q.xml" , cv::FileStorage::READ );
	if( !fs.isOpened() )
		return -1;

	cv::Mat Q;
	fs[ "Q" ] >> Q;

	if( Q.cols != 4 || Q.rows != 4 )
		return -1;

	double Q03 = Q.at< double >( 0 , 3 );
	double Q13 = Q.at< double >( 1 , 3 );
	double Q23 = Q.at< double >( 2 , 3 );
	double Q32 = Q.at< double >( 3 , 2 );
	double Q33 = Q.at< double >( 3 , 3 );

	cv::Mat image = cv::imread( "s.png" , cv::IMREAD_COLOR );
	cv::Mat dispImage = cv::imread( "disp.pgm" , cv::IMREAD_GRAYSCALE );


	cv::Mat reconsturction3D( dispImage.size() , CV_32FC3 );
	cv::reprojectImageTo3D( dispImage , reconsturction3D , Q , false , CV_32F );

	pcl::PointCloud< pcl::PointXYZRGB >::Ptr pointCloudPointer( new pcl::PointCloud< pcl::PointXYZRGB > );

	double px, py, pz;
	uchar pr, pg, pb;

	for( int i = 0 ; i < image.rows ; ++i )
	{
		uchar* rgbPointer = image.ptr< uchar >( i );
		uchar* disparityPointer = dispImage.ptr< uchar >( i );

		for( int j = 0 ; j < image.cols ; ++j )
		{
			uchar d = disparityPointer[ j ];

			if( d == 0 )
				continue;

			double pw = 1.0 * static_cast< double >( d ) * Q32 + Q33; 

			px = static_cast< double >( j ) + Q03;
			py = static_cast< double >( i ) + Q13;
			pz = Q23;

			px = px / pw;
			py = py / pw;
			pz = pz / pw;

			pb = rgbPointer[ 3 * j ];
			pg = rgbPointer[ 3 * j + 1 ];
			pr = rgbPointer[ 3 * j + 2 ];

			pcl::PointXYZRGB point;
			point.x = px;
			point.y = py;
			point.z = pz;

			uint32_t rgb = ( static_cast< uint32_t >( pr ) << 16 | static_cast< uint32_t >( pg ) << 8 | static_cast< uint32_t >( pb ) );
			point.rgb = *reinterpret_cast< float* >( &rgb );

			pointCloudPointer->points.push_back( point );
		}
	}

	pointCloudPointer->width = static_cast< int >( pointCloudPointer->points.size() );
	pointCloudPointer->height = 1;

	boost::shared_ptr< pcl::visualization::PCLVisualizer > viewer;
	viewer = createVisualizer( pointCloudPointer );
	
	while ( !viewer->wasStopped() )
	{
		viewer->spinOnce( 100 );
		
		boost::this_thread::sleep( boost::posix_time::microseconds( 100000 ) );
	}

	return 0;
}
