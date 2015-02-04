#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/ModelCoefficients.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/visualization/cloud_viewer.h>

int main (int argc, char** argv)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_projected (new pcl::PointCloud<pcl::PointXYZ>);

	// Fill in the cloud data
	cloud->width  = 5;
	cloud->height = 1;
	cloud->points.resize (cloud->width * cloud->height);

	for (size_t i = 0; i < cloud->points.size (); ++i)
	{
// 		cloud->points[i].x = 1024 * rand () / (RAND_MAX + 1.0f);
// 		cloud->points[i].y = 1024 * rand () / (RAND_MAX + 1.0f);
// 		cloud->points[i].z = 1024 * rand () / (RAND_MAX + 1.0f);
		cloud->points[i].x = rand () / (RAND_MAX + 1.0f);
		cloud->points[i].y = rand () / (RAND_MAX + 1.0f);
		cloud->points[i].z = rand () / (RAND_MAX + 1.0f);

	}

	std::cerr << "Cloud before projection: " << std::endl;
	for (size_t i = 0; i < cloud->points.size (); ++i){
		std::cerr << "    " << cloud->points[i].x << " " 
			<< cloud->points[i].y << " " 
			<< cloud->points[i].z << std::endl;
	}

	// Create a set of planar coefficients with X=Y=0,Z=1
	pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients ());
	coefficients->values.resize (4);
	coefficients->values[0] = coefficients->values[1] = 0;
	coefficients->values[2] = 1.0;
	coefficients->values[3] = 0;

	// Create the filtering object
	pcl::ProjectInliers<pcl::PointXYZ> proj;
	proj.setModelType (pcl::SACMODEL_PLANE);
	proj.setInputCloud (cloud);
	proj.setModelCoefficients (coefficients);
	proj.filter (*cloud_projected);

	std::cerr << "Cloud after projection: " << std::endl;
	for (size_t i = 0; i < cloud_projected->points.size (); ++i){
		std::cerr << "    " << cloud_projected->points[i].x << " " 
			<< cloud_projected->points[i].y << " " 
			<< cloud_projected->points[i].z << std::endl;
	}

// 	pcl::visualization::CloudViewer cviewer("project inliers");
// 	cviewer.showCloud(cloud);
// 	cviewer.showCloud(cloud_projected, "cloud_projected");
// 	while (!cviewer.wasStopped())
// 		;	
	pcl::visualization::PCLVisualizer vizer ("Matrix transformation example");

	// Define R,G,B colors for the point cloud
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> source_cloud_color_handler (cloud, 255, 255, 255);
	// We add the point cloud to the viewer and pass the color handler
	vizer.addPointCloud (cloud, source_cloud_color_handler);

	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> transformed_cloud_color_handler (cloud, 230, 20, 20); // Red
	vizer.addPointCloud (cloud_projected, transformed_cloud_color_handler, "cloud_projected");

	//viewer.addCoordinateSystem (1.0, "cloud", 0);
	vizer.addCoordinateSystem (1.0, 0);
	vizer.setBackgroundColor(0.05, 0.05, 0.05, 0); // Setting background to a dark grey
	vizer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10);
	vizer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 2, "cloud_projected");

	vizer.spin();

	return (0);
}
