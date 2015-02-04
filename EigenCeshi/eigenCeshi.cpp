#include <iostream>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>

using namespace std;
//using MatrixXd;
using namespace Eigen;
using namespace cv;

void main(int argc, char *argv[]){
	MatrixXd m(2,2);
	m(0,0) = 3;
	m(1,0) = 2.5;
	m(0,1) = -1;
	m(1,1) = m(1,0) + m(0,1);
	cout << m << endl;
	cout<< MatrixXd::Constant(3,3,1.2) << endl;

	float vsz = 3;
	Vector3f volume_size = Vector3f::Constant (vsz/*meters*/);
	Vector3f t = volume_size * 0.5f - Vector3f (0, 0, volume_size (2) / 2 * 1.2f);
	Matrix3f R = Matrix3f::Identity ();   // * AngleAxisf( pcl::deg2rad(-30.f), Vector3f::UnitX()

	cout<<volume_size<<endl
		<<t<<endl
		<<Vector3f::UnitX()<<endl
		<<"---------------a"<<endl
		<<R<<endl;

	cout<<"---------------b"<<endl;
	R = AngleAxisf(0.25*M_PI, Vector3f::UnitX())
		* AngleAxisf(1./3*M_PI, Vector3f::UnitY())
		* AngleAxisf(0.25*M_PI, Vector3f::UnitZ());

	Affine3f pose = Translation3f (t) * AngleAxisf (R);
	//Affine3f pose = AngleAxisf (R) * Translation3f (t);
	//cout<<Translation3f (t)<<endl;
	//cout<<AngleAxisf (R)<<endl;
	cout<<pose.rotation()<<endl
		<<pose.linear()<<endl
		<<pose.translation()<<endl;
	cout<<"---------------c"<<endl;
	Quaternionf q(pose.rotation());
	//pose.rotate(AngleAxisf())
	cout<<"Quaternionf q: "<<q.w()<<","<<q.x()<<","<<q.y()<<","<<q.z()<<endl;

	Matrix3f mm;
	mm = AngleAxisf(0.25*M_PI, Vector3f::UnitX());
	cout<<mm<<endl;
	cout<<"+++++++++++++++"<<endl;
	//mm.row(0) = -mm.row(0);
	//mm = q;
	cout<<mm<<endl;
	cout<<mm.eulerAngles(0,1,2)<<endl;

	Mat me = Mat::eye(3, 3, CV_32F), rme;
	cv::Rodrigues(me, rme);
	cout<<"cv: "<<me<<endl
		<<rme<<endl;

	cv::eigen2cv(mm, me);
	cv::Rodrigues(me, rme);
	cout<<"eigen2cv: "<<me<<endl
		<<rme<<endl;

	//cout<<AngleAxisf(mm).axis()<<endl;
	AngleAxisf aaf(0.25*M_PI, Vector3f::UnitX());
	cout<<aaf.axis()<<endl;

	cout<<"---------------"<<endl;
	cout<<R.transpose()<<endl
		<<R.inverse()<<endl
		<<R<<endl;
}//main
