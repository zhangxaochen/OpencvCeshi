#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/voxel_grid.h>

int
	main (int argc, char** argv)
{
	if(argc!=2){
		std::cout<<"USAGE:\n\t argv[0] <pcd_file_name>"<<std::endl;
		return -1;
	}
	sensor_msgs::PointCloud2::Ptr cloud (new sensor_msgs::PointCloud2 ());
	sensor_msgs::PointCloud2::Ptr cloud_filtered (new sensor_msgs::PointCloud2 ());

	// Fill in the cloud data
	pcl::PCDReader reader;
	// Replace the path below with the path where you saved your file
	reader.read (argv[1], *cloud); // Remember to download the file first!

	std::cerr << "PointCloud before filtering: " << cloud->width * cloud->height 
		<< " data points (" << pcl::getFieldsList (*cloud) << ").";

	// Create the filtering object
	pcl::VoxelGrid<sensor_msgs::PointCloud2> vgrid;
	vgrid.setInputCloud (cloud);
	vgrid.setLeafSize (0.01f, 0.01f, 0.01f);
	vgrid.filter (*cloud_filtered);

	std::cerr << "PointCloud after filtering: " << cloud_filtered->width * cloud_filtered->height 
		<< " data points (" << pcl::getFieldsList (*cloud_filtered) << ").";

	pcl::PCDWriter writer;
	writer.write ("table_scene_lms400_downsampled.pcd", *cloud_filtered, 
		Eigen::Vector4f::Zero (), Eigen::Quaternionf::Identity (), false);

	return (0);
}
