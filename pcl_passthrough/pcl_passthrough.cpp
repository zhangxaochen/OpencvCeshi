#include <iostream>
#include <pcl/point_types.h>
#include <pcl/filters/passthrough.h>
#include <pcl/visualization/pcl_visualizer.h>
namespace vis = pcl::visualization;

int
	main (int argc, char** argv)
{
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);

	// Fill in the cloud data
	cloud->width  = 5;
	cloud->height = 1;
	cloud->points.resize (cloud->width * cloud->height);

	for (size_t i = 0; i < cloud->points.size (); ++i)
	{
		//cloud->points[i].x = 1024 * rand () / (RAND_MAX + 1.0f);
		//cloud->points[i].y = 1024 * rand () / (RAND_MAX + 1.0f);
		//cloud->points[i].z = 1024 * rand () / (RAND_MAX + 1.0f);
		cloud->points[i].x = (rand () * 2 - RAND_MAX) / (RAND_MAX + 1.0f);
		cloud->points[i].y = (rand () * 2 - RAND_MAX) / (RAND_MAX + 1.0f);
		cloud->points[i].z = (rand () * 2 - RAND_MAX) / (RAND_MAX + 1.0f);
	}

	std::cerr << "Cloud before filtering: " << std::endl;
	for (size_t i = 0; i < cloud->points.size (); ++i)
		std::cerr << "    " << cloud->points[i].x << " " 
		<< cloud->points[i].y << " " 
		<< cloud->points[i].z << std::endl;

	// Create the filtering object
	pcl::PassThrough<pcl::PointXYZ> pass;
	pass.setInputCloud (cloud);
	pass.setFilterFieldName ("z");
	pass.setFilterLimits (0.0, 1.0);
	//pass.setFilterLimitsNegative (true);
	pass.filter (*cloud_filtered);

	std::cerr << "Cloud after filtering: " << std::endl;
	for (size_t i = 0; i < cloud_filtered->points.size (); ++i)
		std::cerr << "    " << cloud_filtered->points[i].x << " " 
		<< cloud_filtered->points[i].y << " " 
		<< cloud_filtered->points[i].z << std::endl;

	vis::PCLVisualizer viewer("test viewer");
	viewer.addCoordinateSystem();
	//viewer.setBackgroundColor(0, 0, 0);

	vis::PointCloudColorHandlerCustom<pcl::PointXYZ> orig_cloud_ch(cloud, 255, 0, 0),
		filtered_cloud_ch(cloud_filtered, 0, 255, 0);
	viewer.addPointCloud(cloud, orig_cloud_ch, "orig_cloud");
	viewer.addPointCloud(cloud_filtered, filtered_cloud_ch, "filtered_cloud");
	viewer.setPointCloudRenderingProperties(vis::PCL_VISUALIZER_POINT_SIZE, 2, "orig_cloud");
	viewer.setPointCloudRenderingProperties(vis::PCL_VISUALIZER_POINT_SIZE, 2, "filtered_cloud");

// 	while(!viewer.wasStopped()){
// 		viewer.spin();
// 	}
	viewer.spin();
	return (0);
}
