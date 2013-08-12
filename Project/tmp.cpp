#include <iostream>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\contrib\contrib.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

void eular2rot( double yaw , double pitch , double roll , cv::Mat& dst )
{
  double theta = yaw / 180.0 * CV_PI;
	double pusai = pitch / 180.0 * CV_PI;
	double phi = roll / 180.0 * CV_PI;

	double dataX[ 3 ][ 3 ] = {
		{ 1.0 ,          0.0 ,           0.0 },
		{ 0.0 , cos( theta ) , -sin( theta ) },
		{ 0.0 , sin( theta ) ,  cos( theta ) } };

	double dataY[ 3 ][ 3 ] = {
		{  cos( pusai ) , 0.0 , sin( pusai ) },
		{           0.0 , 1.0 ,         0.0  },
		{ -sin( pusai ) , 0.0 , cos( pusai ) } };

	double dataZ[ 3 ][ 3 ] = {
		{ cos( phi ) , -sin( phi ) , 0.0 },
		{ sin( phi ) ,  cos( phi ) , 0.0 },
		{        0.0 ,         0.0 , 1.0 } };

	cv::Mat Rx( 3 , 3 , CV_64F , dataX );
	cv::Mat Ry( 3 , 3 , CV_64F , dataY );
	cv::Mat Rz( 3 , 3 , CV_64F , dataZ );

	cv::Mat R = Rz * Rx * Ry;

	R.copyTo( dst );
}

void lookat( cv::Point3d from , cv::Point3d to , cv::Mat& dst )
{
	double x = ( to.x - from.x );
	double y = ( to.y - from.y );
	double z = ( to.z - from.z );

	double pitch = asin(  x / sqrt( x * x + z * z ) ) / CV_PI * 180;
	double   yaw = asin( -y / sqrt( y * y + z * z ) ) / CV_PI * 180;

	eular2rot( yaw , pitch , 0.0 , dst );
}

template< class T > static void projectImageFromXYZ_ ( cv::Mat& src , cv::Mat& dst , cv::Mat& disp , cv::Mat& dstdisp , cv::Mat& xyz , cv::Mat& R , cv::Mat& t , cv::Mat& K , cv::Mat& dist , cv::Mat& mask , bool isSub )
{
	if( dst.empty() )
		dst = cv::Mat::zeros( cv::Size( src.size() ) , src.type() );

	if( dstdisp.empty() )
		dstdisp = cv::Mat::zeros( cv::Size( src.size() ) , disp.type() );

	std::vector< cv::Point2f > pt;

	if( dist.empty() )
		dist = cv::Mat::zeros( cv::Size( 5 , 1 ) , CV_32F );

	cv::projectPoints( xyz , R , t , K , dist , pt );

	dst.setTo( 0 );
	dstdisp.setTo( 0 );

	for( int j = 1 ; j < src.rows - 1 ; ++j )
	{
		int cnt = j * src.cols;

		uchar* img = src.ptr< uchar >( j );
		uchar* m = mask.ptr< uchar >( j );

		for( int i = 0 ; i < src.cols ; ++i , cnt++ )
		{
			int x = static_cast< int >( pt[ cnt ].x + 0.5 );
			int y = static_cast< int >( pt[ cnt ].y + 0.5 );

			if( m[ i ] == 255 )
				continue;

			if( pt[ cnt ].x >= 1 && pt[ cnt ].x < src.cols - 1 && pt[ cnt ].y >= 1 && pt[ cnt ].y < src.rows - 1 )
			{
				short d = dstdisp.at< T >( y , x );

				if( d< disp.at< T >( j , i ) )
				{
					dst.at< uchar >( y , 3 * x + 0 ) = img[ 3 * i + 0 ];
					dst.at< uchar >( y , 3 * x + 1 ) = img[ 3 * i + 1 ];
					dst.at< uchar >( y , 3 * x + 2 ) = img[ 3 * i + 2 ];

					dstdisp.at< T >( y , x ) = disp.at< T >( j , i );

					if( isSub )
					{
						if( static_cast< int >( pt[ cnt + src.cols ].y - y > 1 ) && static_cast< int >( pt[ cnt + 1 ].x - x > 1 ) )
						{
							dst.at< uchar >( y , 3 * x + 3 ) = img[ 3 * i + 0 ];
							dst.at< uchar >( y , 3 * x + 4 ) = img[ 3 * i + 1 ];
							dst.at< uchar >( y , 3 * x + 5 ) = img[ 3 * i + 2 ];

							dst.at< uchar >( y + 1 , 3 * x + 0 ) = img[ 3 * i + 0 ];
							dst.at< uchar >( y + 1 , 3 * x + 1 ) = img[ 3 * i + 1 ];
							dst.at< uchar >( y + 1 , 3 * x + 2 ) = img[ 3 * i + 2 ];

							dst.at< uchar >( y + 1 , 3 * x + 3 ) = img[ 3 * i + 0 ];
                            dst.at< uchar >( y + 1 , 3 * x + 4 ) = img[ 3 * i + 1 ];
                            dst.at< uchar >( y + 1 , 3 * x + 5 ) = img[ 3 * i + 2 ];
 
                            dstdisp.at< T >( y     , x + 1 ) = disp.at< T >( j , i );                      
                            dstdisp.at< T >( y + 1 , x     ) = disp.at< T >( j , i );
                            dstdisp.at< T >( y + 1 , x + 1 ) = disp.at< T >( j , i );
						}
						else if( static_cast< int >( pt[ cnt - src.cols ].y - y < -1 ) && static_cast< int >( pt[ cnt - 1 ].x - x < -1 ) )
                        {
							dst.at< uchar >( y , 3 * x - 3 ) = img[ 3 * i + 0 ];
							dst.at< uchar >( y , 3 * x - 2 ) = img[ 3 * i + 1 ];
							dst.at< uchar >( y , 3 * x - 1 ) = img[ 3 * i + 2 ];

							dst.at< uchar >( y - 1 , 3 * x + 0 ) = img[ 3 * i + 0 ];
							dst.at< uchar >( y - 1 , 3 * x + 1 ) = img[ 3 * i + 1 ];
							dst.at< uchar >( y - 1 , 3 * x + 2 ) = img[ 3 * i + 2 ];

							dst.at< uchar >( y - 1 , 3 * x - 3 ) = img[ 3 * i + 0 ];
							dst.at< uchar >( y - 1 , 3 * x - 2 ) = img[ 3 * i + 1 ];
							dst.at< uchar >( y - 1 , 3 * x - 1 ) = img[ 3 * i + 2 ];

							dstdisp.at< T >( y     , x - 1 ) = disp.at< T >( j , i );                      
							dstdisp.at< T >( y - 1 , x )     = disp.at< T >( j , i );
							dstdisp.at< T >( y - 1 , x - 1 ) = disp.at< T >( j , i );
						}
						else if( static_cast< int >( pt[ cnt + 1 ].x - x > 1 ) )
						{
							dst.at < uchar >( y , 3 * x + 3 ) = img[ 3 * i + 0 ];
							dst.at < uchar >( y , 3 * x + 4 ) = img[ 3 * i + 1 ];
							dst.at < uchar >( y , 3 * x + 5 ) = img[ 3 * i + 2 ];

							dstdisp.at< T >( y , x + 1 ) = disp.at < T > ( j , i );
						}
						else if( static_cast< int >( pt[ cnt - 1 ].x - x < -1 ) )
						{
							dst.at< uchar >( y , 3 * x - 3 ) = img[ 3 * i + 0 ];
							dst.at< uchar >( y , 3 * x - 2 ) = img[ 3 * i + 1 ];
							dst.at< uchar >( y , 3 * x - 1 ) = img[ 3 * i + 2 ];

							dstdisp.at< T >( y,x - 1 ) = disp.at < T > ( j,i );
						}
						else if( static_cast< int >( pt[ cnt + src.cols ].y - y > 1 ) )
						{
							dst.at< uchar >( y + 1 , 3 * x + 0 ) = img[ 3 * i + 0 ];
							dst.at< uchar >( y + 1 , 3 * x + 1 ) = img[ 3 * i + 1 ];
							dst.at< uchar >( y + 1 , 3 * x + 2 ) = img[ 3 * i + 2 ];

							dstdisp.at< T >( y + 1 , x ) = disp.at < T >( j , i );
						}
						else if( static_cast< int >( pt[ cnt - src.cols ].y - y < -1 ) )
						{
							dst.at< uchar >( y - 1 , 3 * x + 0 ) = img[ 3 * i + 0 ];
							dst.at< uchar >( y - 1 , 3 * x + 1 ) = img[ 3 * i + 1 ];
							dst.at< uchar >( y - 1 , 3 * x + 2 ) = img[ 3 * i + 2 ];

							dstdisp.at< T >( y - 1 , x ) = disp.at< T >( j,i );
						}
					}
				}
			}
		}
	}

	if( isSub )
	{
		cv::Mat img2, disp2;

		dst.copyTo( img2 );
		dstdisp.copyTo( disp2 );

		const int BS = 1;

		for( int j = BS ;j < src.rows - BS ; ++j )
		{
            uchar* img = dst.ptr< uchar >( j );
            T* m = disp2.ptr< T >( j );
            T* dp = dstdisp.ptr< T >( j );

            for( int i = BS ; i < src.cols - BS ; ++i )
            {
                if( m[ i ] == 0 )
                {
                    int count = 0;
                    int d = 0;
                    int r = 0;
                    int g = 0;
                    int b = 0;

                    for( int l = -BS ;l <= BS ; ++l )
                    {
                        T* dp2 = disp2.ptr< T >( j + l );
                        uchar* img3 = img2.ptr< uchar >( j + l );

                        for( int k = -BS ; k <= BS ; ++k )
                        {
                            if( dp2[ i + k ] != 0 )
                            {
                                count++;

                                d += dp2[ i + k ];
                                r += img3[ 3 * ( i + k ) + 0 ];
                                g += img3[ 3 * ( i + k ) + 1 ];
                                b += img3[ 3 * ( i + k ) + 2 ];
                            }
                        }
                    }

                    if( count != 0 )
                    {
                        double div = 1.0 / count;
                        dp[ i ] = d * div;
                        img[ 3 * i + 0 ] = r * div;
                        img[ 3 * i + 1 ] = g * div;
                        img[ 3 * i + 2 ] = b * div;
                    }
                }
            }
        }
	}
}

void projectImageFromXYZ( cv::Mat& src , cv::Mat& dst , cv::Mat& disp , cv::Mat& dstdisp , cv::Mat& xyz , cv::Mat& R , cv::Mat& t , cv::Mat& K , cv::Mat& dist , bool isSub = true , cv::Mat& mask = cv::Mat() )
{
	if( mask.empty() )
		mask = cv::Mat::zeros( src.size() , CV_8U );

	projectImageFromXYZ_< unsigned char >( src , dst ,  disp ,  dstdisp ,  xyz ,  R ,  t ,  K ,  dist ,  mask , isSub );
}

cv::Mat makeQMatrix( cv::Point2d img , double f , double baseline )
{
	cv::Mat Q = cv::Mat::eye( 4 , 4 , CV_64F );

	Q.at< double >( 0 , 3 ) = -img.x;
	Q.at< double >( 1 , 3 ) = -img.y;
	Q.at< double >( 2 , 2 ) = 0.0;
	Q.at< double >( 2 , 3 ) = f;
	Q.at< double >( 3 , 2 ) = 1.0 / baseline;
	Q.at< double >( 3 , 3 ) = 0.0;

	return Q;
}

int main( int argc , char** argv )
{
	cv::Mat imageL = cv::imread( "l.png" , cv::IMREAD_COLOR );
	cv::Mat imageR = cv::imread( "r.png" , cv::IMREAD_COLOR );

	cv::Mat grayL, grayR;
	cv::cvtColor( imageL , grayL , cv::COLOR_BGR2GRAY );
	cv::cvtColor( imageR , grayR , cv::COLOR_BGR2GRAY );

	
	cv::Mat destImage;
	
	cv::Mat disp;
	cv::Mat destdisp;
	cv::Mat dispshow;
	cv::StereoSGBM sgbm( 1 , 16 * 2 , 3 , 200 , 255 , 1 , 0 , 0 , 0 , 0 , true );
	sgbm( imageL , imageR , disp );
	cv::normalize( disp , disp , 0.0 , 255.0 , cv::NORM_MINMAX , CV_8U );

	cv::Mat Q = makeQMatrix( cv::Point2d( ( imageL.cols - 1.0 ) / 2.0 , ( imageL.rows - 1.0 ) / 2.0 ) , 598.57 , 14.0 );
	
	cv::Mat depth;
	cv::reprojectImageTo3D( disp , depth , Q );
	
	cv::Mat xyz;
	xyz = depth.reshape( 3 , depth.size().area() );

	cv::Mat K = cv::Mat::eye( 3 , 3 , CV_64F );
	K.at< double >( 0 , 0 ) = 598.57;
	K.at< double >( 1 , 1 ) = 598.57;
	K.at< double >( 0 , 2 ) = ( imageL.cols - 1.0 ) / 2.0;
	K.at< double >( 1 , 2 ) = ( imageL.rows - 1.0 ) / 2.0;

	cv::Mat dist = cv::Mat::zeros( 5 , 1 , CV_64F );
	cv::Mat R = cv::Mat::eye( 3 , 3 , CV_64F );
	cv::Mat t = cv::Mat::zeros( 3 , 1 , CV_64F );

	cv::Point3d viewPoint( 0.0 , 0.0 , 14.0 * 10.0 );
	cv::Point3d lookAtPoint( 0.0 , 0.0 , -14.0 * 10.0 );

	const double step = 14.0;
	
	int key = 0;
	bool isSub = true;

	while( key != 27 )
	{
		lookat( viewPoint , lookAtPoint , R );

		t.at< double >( 0 , 0 ) = viewPoint.x;
		t.at< double >( 1 , 0 ) = viewPoint.y;
		t.at< double >( 2 , 0 ) = viewPoint.z;

		std::cout << "t = " << t << std::endl;

		t = R * t;

		projectImageFromXYZ( imageL , destImage , disp , destdisp , xyz , R , t , K , dist , isSub );

		cv::imshow( "Depth" , dispshow );
		cv::imshow( "Image" , destImage );

		key = cv::waitKey( 1 );

		if( key == 'f' )
			isSub = ( isSub ) ? false : true;
		else if( key == 'k' )
			viewPoint.y += step;
		else if( key == 'j' )
			viewPoint.y -= step;
		else if( key == 'h' )
			viewPoint.x += step;
		else if( key == 'l' )
			viewPoint.x -= step;
		else if( key == 'K' )
			viewPoint.z += step;
		else if( key == 'J' )
			viewPoint.z -= step;
	}

	return 0;
}
