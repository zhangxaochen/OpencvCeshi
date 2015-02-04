#include <iostream>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/filters/statistical_outlier_removal.h>
#include <pcl/common/time.h>

int
main (int argc, char** argv)
{
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
  pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered (new pcl::PointCloud<pcl::PointXYZ>);

  // Fill in the cloud data
  pcl::PCDReader reader;
  // Replace the path below with the path where you saved your file
  //reader.read<pcl::PointXYZ> ("table_scene_lms400.pcd", *cloud);
  reader.read<pcl::PointXYZ> (argv[1], *cloud);

  std::cerr << "Cloud before filtering: " << std::endl;
  std::cerr << *cloud << std::endl;

  // Create the filtering object
  pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;
  double begt = pcl::getTime();
  sor.setInputCloud (cloud);
  cout<<"sor.setInputCloud: "<<pcl::getTime()-begt<<endl;

  sor.setMeanK (50);
  sor.setStddevMulThresh (1.0);

  begt = pcl::getTime();
  sor.filter (*cloud_filtered);
  cout<<"sor.filter: "<<pcl::getTime()-begt<<endl;

  std::cerr << "Cloud after filtering: " << std::endl;
  std::cerr << *cloud_filtered << std::endl;

  pcl::PCDWriter writer;
  writer.write<pcl::PointXYZ> ("table_scene_lms400_inliers.pcd", *cloud_filtered, false);

  begt = pcl::getTime();
  sor.setNegative (true);
  sor.filter (*cloud_filtered);
  cout<<"sor.filter: "<<pcl::getTime()-begt<<endl;

  writer.write<pcl::PointXYZ> ("table_scene_lms400_outliers.pcd", *cloud_filtered, false);

  return (0);
}