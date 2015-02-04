// #include <pcl/io/openni_grabber.h>
// #include <pcl/visualization/cloud_viewer.h>
// 
// class SimpleOpenNIViewer
// {
// public:
// 	SimpleOpenNIViewer () : viewer ("PCL OpenNI Viewer") {}
// 
// 	void cloud_cb_ (const pcl::PointCloud<pcl::PointXYZ>::ConstPtr &cloud)
// 	{
// 		if (!viewer.wasStopped())
// 			viewer.showCloud (cloud);
// 	}
// 
// 	void run ()
// 	{
// 		pcl::Grabber* interface = new pcl::OpenNIGrabber();
// 
// 		boost::function<void (const pcl::PointCloud<pcl::PointXYZ>::ConstPtr&)> f =
// 			boost::bind (&SimpleOpenNIViewer::cloud_cb_, this, _1);
// 			//boost::bind (&SimpleOpenNIViewer::cloud_cb_, _1); //¡Á
// 		
// 		interface->registerCallback (f);
// 
// 		interface->start ();
// 
// 		while (!viewer.wasStopped())
// 		{
// 			//boost::this_thread::sleep (boost::posix_time::seconds (1));
// 		}
// 
// 		interface->stop ();
// 	}
// 
// 	pcl::visualization::CloudViewer viewer;
// };
// 
// int main ()
// {
// 	SimpleOpenNIViewer v;
// 	v.run ();
// 	return 0;
// }
// 

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/openni_grabber.h>
#include <pcl/io/pcd_io.h>
#include <pcl/common/time.h>
#include <pcl/visualization/cloud_viewer.h>

class SimpleOpenNIProcessor
{
	pcl::visualization::CloudViewer cviewer;

public:
	SimpleOpenNIProcessor(): cviewer ("PCL OpenNI Viewer") {}

	void cloud_cb_ (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr &cloud)
	{
		cviewer.showCloud(cloud);

		static unsigned count = 0;
		static double last = pcl::getTime ();
		if (++count == 30)
		{
			double now = pcl::getTime ();
			std::cout << "distance of center pixel :" << cloud->points [(cloud->width >> 1) * (cloud->height + 1)].z << " mm. Average framerate: " << double(count)/double(now - last) << " Hz" <<  std::endl;
			using namespace std;
			cout<<"w, h: "<<cloud->width<<", "<<cloud->height<<endl;
			count = 0;
			last = now;

			//pcl::io::savePCDFileASCII("openni-pcd.")
		}
	}

	void run ()
	{
		// create a new grabber for OpenNI devices
		pcl::Grabber* interface = new pcl::OpenNIGrabber();

		// make callback function from member function
		boost::function<void (const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr&)> f =
			boost::bind (&SimpleOpenNIProcessor::cloud_cb_, this, _1);

		// connect callback function for desired signal. In this case its a point cloud with color values
		boost::signals2::connection c = interface->registerCallback (f);

		// start receiving point clouds
		interface->start ();

		// wait until user quits program with Ctrl-C, but no busy-waiting -> sleep (1);
// 		while (true)
// 			boost::this_thread::sleep (boost::posix_time::seconds (1));
		while(!cviewer.wasStopped())
			;

		// stop the grabber
		interface->stop ();
	}
};

int main ()
{
	SimpleOpenNIProcessor v;
	v.run ();
	return (0);
}