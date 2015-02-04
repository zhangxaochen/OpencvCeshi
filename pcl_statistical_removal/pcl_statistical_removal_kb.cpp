#include <iostream>
#include <string>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/common/time.h>
#include <pcl/visualization/pcl_visualizer.h>

#include <boost/thread.hpp>

using namespace std;
using namespace pcl::visualization;


typedef pcl::PointXYZ PtType;
typedef pcl::PointCloud<PtType> Cloud;
typedef Cloud::Ptr CloudPtr;
typedef PointCloudColorHandlerCustom<PtType> Cloud_CH_Custom;

class SimpleOutlierRemover{
	PCLVisualizer vizer;//("SimpleOutlierRemover");
	CloudPtr pCloud,//(new Cloud),
		pCloud_filtered;//(new Cloud);
	pcl::StatisticalOutlierRemoval<PtType> sor;
public:	
	SimpleOutlierRemover(int argc, char **argv) 
		: vizer("SimpleOutlierRemover"), pCloud(new Cloud), pCloud_filtered(new Cloud)
	{
		pcl::PCDReader reader;
		reader.read(argv[1], *pCloud);

		vizer.addPointCloud(pCloud);
		vizer.registerKeyboardCallback(boost::bind(&SimpleOutlierRemover::keyboardEventOccurred, this, _1));
		vizer.addCoordinateSystem(0.1);

		sor.setInputCloud(pCloud);
		sor.setMeanK(20);
		sor.setStddevMulThresh(1.0);

	}//ctor

	void keyboardEventOccurred(const KeyboardEvent &e){//, CloudPtr pCld){
		if(e.keyDown() && e.getKeySym() == "z"){
			cout<<"pressed keyboard"<<endl
				<<*pCloud<<endl;

			sor.filter(*pCloud);
			vizer.updatePointCloud(pCloud);
		}
	}//keyboardEventOccurred

	void run(){
		vizer.spin();
	}//run

};//SimpleOutlierRemover

int main(int argc, char **argv){
	SimpleOutlierRemover simpOR(argc, argv);
	simpOR.run();

	return 0;
}//main

// int
// main (int argc, char** argv)
// {
//   pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
//   pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);
// 
//   // Fill in the cloud data
//   pcl::PCDReader reader;
//   // Replace the path below with the path where you saved your file
//   //reader.read<pcl::PointXYZ> ("table_scene_lms400.pcd", *cloud);
//   reader.read<pcl::PointXYZ> (argv[1], *cloud);
// 
//   std::cerr << "Cloud before filtering: " << std::endl;
//   std::cerr << *cloud << std::endl;
// 
//   // Create the filtering object
//   pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;
//   double begt = pcl::getTime();
//   sor.setInputCloud (cloud);
//   cout<<"sor.setInputCloud: "<<pcl::getTime()-begt<<endl;
// 
//   sor.setMeanK (50);
//   sor.setStddevMulThresh (1.0);
// 
//   begt = pcl::getTime();
//   sor.filter (*cloud_filtered);
//   cout<<"sor.filter: "<<pcl::getTime()-begt<<endl;
// 
//   std::cerr << "Cloud after filtering: " << std::endl;
//   std::cerr << *cloud_filtered << std::endl;
// 
//   pcl::PCDWriter writer;
//   writer.write<pcl::PointXYZ> ("table_scene_lms400_inliers.pcd", *cloud_filtered, false);
// 
//   begt = pcl::getTime();
//   sor.setNegative (true);
//   sor.filter (*cloud_filtered);
//   cout<<"sor.filter: "<<pcl::getTime()-begt<<endl;
// 
//   writer.write<pcl::PointXYZ> ("table_scene_lms400_outliers.pcd", *cloud_filtered, false);
// 
//   return (0);
// }