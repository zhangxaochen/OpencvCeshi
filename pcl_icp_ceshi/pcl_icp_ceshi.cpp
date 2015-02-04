#include <iostream>
#include <vector>
#include <string>

#include <pcl/console/parse.h>
#include <pcl/io/pcd_io.h>
#include <pcl/registration/icp.h>
#include <pcl/point_types.h>
#include <pcl/visualization/pcl_visualizer.h>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace Eigen;
using namespace pcl::visualization;

typedef pcl::PointXYZ PtType;
typedef pcl::PointCloud<PtType> Cloud;
typedef Cloud::Ptr CloudPtr;
typedef PointCloudColorHandlerCustom<PtType> CloudCHCustom;

const double rad2angle = M_PI / 180;
vector<CloudPtr> clds;
const int frame_num = 6;
PCLVisualizer vizer("pcl_icp_ceshi");

void keyboardEventOccurred(const KeyboardEvent &e, CloudPtr pCld){
	if(e.getKeySym() == "z" && e.keyDown()){
		cout<<"pressed keyboard"<<endl;
			//<<t1<<", "<<t2<<endl;
		static int cnt = 0;
		if(cnt > frame_num - 1)
			cnt = 0;
		
		//CloudPtr pCld = clds[cnt];
		*pCld = *clds[cnt];
		cnt++;
		vizer.updatePointCloud(pCld);
	}
}//keyboardEventOccurred

int main(int argc, char *argv[]){
	float ax = 0,
		ay = 0, 
		az = 0;
	
	cout<<ax<<", "<<ay<<", "<<az<<endl;

	//²ÎÊý½âÎö£º
	pcl::console::parse_3x_arguments(argc, argv, "-axyz", ax, ay, az);
	cout<<ax<<", "<<ay<<", "<<az<<endl;
	Matrix3f rot;
	float rx = ax * rad2angle,
		ry = ay * rad2angle,
		rz = az * rad2angle;
	cout<<"rxyz: "<<rx<<", "<<ry<<", "<<rz<<endl;

	rot = AngleAxisf(rx, Vector3f::UnitX())
		* AngleAxisf(ry, Vector3f::UnitY())
		* AngleAxisf(rz, Vector3f::UnitZ());
	cout<<"rot:\n"<<rot<<endl;

	Affine3f aff;
	//aff.linear() = aff.rotation()
	//aff.linear() = rot;
	
	cout<<"aff, rot:\n"<<aff.rotation()<<endl
		<<aff.linear()<<endl
		<<"trans:\n"<<aff.translation()<<endl;
	aff = rot;
	cout<<"aff:\n"<<aff.rotation()<<endl
		<<aff.linear()<<endl;

	
	string pcdFname;
	pcl::console::parse_argument(argc, argv, "-fin", pcdFname);

	CloudPtr pCloud(new Cloud),
		dummy_cloud;
	
	pcl::PCDReader reader;
	reader.read(pcdFname, *pCloud);

	

	vizer.registerKeyboardCallback(boost::bind(&keyboardEventOccurred, _1, pCloud));
	CloudCHCustom cloud_ch(pCloud, 255, 255, 0);
	vizer.addPointCloud(pCloud, cloud_ch);
	vizer.resetCameraViewpoint();

	clds.push_back(pCloud);
	for(int i = 0; i < frame_num - 1; i++){
		//boost::this_thread::sleep(boost::posix_time::seconds(1));
		CloudPtr lastCld = clds.back(),
			tmpCld(new Cloud);
		clds.push_back(tmpCld);
		pcl::transformPointCloud(*lastCld, *tmpCld, aff);
		//vizer.updatePointCloud(tmpCld);

		pcl::io::savePCDFileASCII("test_"+to_string(long double(i+1))+".pcd", *tmpCld);
	}

	vizer.addCoordinateSystem(0.1);

	vizer.spin();
	
}//main

