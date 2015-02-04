/** 参考： http://blog.csdn.net/chenxin_130/article/details/6696187
 ** by zhangxaochen
 ** 
 **/

#include <iostream>
#include <fstream>
#include <string>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;
using namespace boost;
namespace fs = boost::filesystem;

fs::path operator+(const fs::path& lhs, const fs::path& rhs){ 
	return fs::path(lhs) += rhs; 
}

void checkOpenNIError(XnStatus rc, string status){
	if(rc != XN_STATUS_OK){
		cerr<<status<<" Error: "<<xnGetStatusString(rc)<<endl;
		
	}
}//checkOpenNIError

void main(int argc, char *argv[]){
	cout<<"hello"<<endl;
	//1.
	XnStatus rc = XN_STATUS_OK;
	xn::DepthMetaData depthMD;
	xn::ImageMetaData imageMD;
	
	//2.
	xn::Context ctx;
	rc = ctx.Init();
	checkOpenNIError(rc, "init context");
	
// 	string dataPath = "../../data/",
// 		dataFile = "t-fast.oni";
	//fs::path dataFile("../../data/t-fast.oni");
	//fs::path dataFile("../../data/openNI1.3_data_registration_7x9/1_100cm.oni");
	//fs::path dataFile("../../data/openNI132_data_4x4/test_4x4.oni");
	const char *default_fpath = "../../data/openNI132_data_registration_4x3/test_smt_far_4x3.oni";
	if(argc>1){
		default_fpath = argv[1];
	}
	cout<<"input ONI file's absolute path:"<<endl;
	string fpath;
	getline(cin, fpath);
	if(fpath.size() == 0)
		fpath = default_fpath;
	//fs::path boost_fpath = fpath;

	fs::path boost_fpath = fpath;
	cout<<"==boost: dataFile: "<<boost_fpath<<", "<<boost_fpath.string()<<endl
		<<boost_fpath.parent_path()<<", "<<fs::canonical(boost_fpath)<<endl
		<<boost_fpath.filename()<<", "<<boost_fpath.stem()<<", "<<boost_fpath.extension()<<endl
		<<boost_fpath.parent_path()/boost_fpath.stem()<<endl;
	//rc = ctx.OpenFileRecording((dataPath+dataFile).c_str());
	rc = ctx.OpenFileRecording(boost_fpath.string().c_str());
	
	checkOpenNIError(rc, "OpenFileRecording");

	xn::DepthGenerator dg;
	rc = dg.Create(ctx);
	//rc = ctx.FindExistingNode(XN_NODE_TYPE_DEPTH, dg);
	checkOpenNIError(rc, "create dg");

	xn::ImageGenerator ig;
	rc = ig.Create(ctx);
	//rc = ctx.FindExistingNode(XN_NODE_TYPE_IMAGE, ig);
	checkOpenNIError(rc, "create ig");

// 	bool mirror = false;
// 	dg.GetMirrorCap().SetMirror(mirror);
// 	ig.GetMirrorCap().SetMirror(mirror);


	//3. 
	XnMapOutputMode mapMode;
	mapMode.nXRes = 640;
	mapMode.nYRes = 480;
	mapMode.nFPS = 30;
	
	rc = dg.SetMapOutputMode(mapMode);
	checkOpenNIError(rc, "dg.SetMapOutputMode");
	rc = ig.SetMapOutputMode(mapMode);
	checkOpenNIError(rc, "ig.SetMapOutputMode");

	// 帧序号对齐：
	if(dg.GetFrameSyncCap().CanFrameSyncWith(ig)){
		dg.GetFrameSyncCap().FrameSyncWith(ig);
		cout<<"dg.GetFrameSyncCap().CanFrameSyncWith(ig)"<<endl;
	}
	if(ig.GetFrameSyncCap().CanFrameSyncWith(dg)){
		cout<<"--dg.GetFrameSyncCap().CanFrameSyncWith(ig)"<<endl;
	}
	dg.GetFrameSyncCap().FrameSyncWith(ig);

	
	//4. 两镜头切换视角，重要
	//dg.GetAlternativeViewPointCap().SetViewPoint(ig);
	//ig.GetAlternativeViewPointCap().SetViewPoint(dg);

	//5. read data
	rc = ctx.StartGeneratingAll();

	//6. 
	//rc = ctx.WaitNoneUpdateAll();

	char key = 0;
// 	while((key != 27 ) && !(rc = ctx.WaitAnyUpdateAll()) ){
	while(key != 27 ){
		ctx.WaitAndUpdateAll();
		dg.GetMetaData(depthMD);
		ig.GetMetaData(imageMD);
		cout<<"dg.GetFrameID(): "<<dg.GetFrameID()<<", "<<ig.GetFrameID()<<endl;
		

		//7.
		// 		Mat dm(depthMD.YRes(), depthMD.XRes(), CV_16UC1, (void*)depthMD.Data());
		Mat dm(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.Data());
		Mat im(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (void*)imageMD.Data());
		//金字塔
		int ratio = 2;
		cv::pyrDown(dm, dm, Size(dm.cols/ratio, dm.rows/ratio));
		cv::pyrDown(dm, dm, Size(dm.cols/ratio, dm.rows/ratio));
		cv::pyrDown(dm, dm, Size(dm.cols/ratio, dm.rows/ratio));
		cv::pyrDown(im, im, Size(im.cols/ratio, im.rows/ratio));
		cv::pyrDown(im, im, Size(im.cols/ratio, im.rows/ratio));
		cv::pyrDown(im, im, Size(im.cols/ratio, im.rows/ratio));

		double dmin, dmax;
		minMaxLoc(dm, &dmin, &dmax);
		cout<<"dmin, dmax: "<<dmin<<",\t"<<dmax<<",,"<<dm.type()<<endl;
		Mat dm_draw;
		dm.convertTo(dm_draw, CV_8U, 255./(dmax-dmin), -dmin*255./(dmax-dmin));

		imshow("depth map", dm_draw);
		//cout<<depthMD.XRes()<<","<<depthMD.YRes()<<"--"<<depthMD.FullXRes()<<","<<depthMD.FullYRes()<<endl;

		//Mat im_draw;
		cvtColor(im, im, CV_BGR2RGB);
		//cout<<im.type()<<endl;


		imshow("color image", im);
		//if(ig.GetFrameID() % 30 == 0){
		if(ig.GetFrameID() % 300 != 0){ //随便 "% 300 != 0" 乱弄一下
			//string img2save = dataPath+dataFile+"/"+std::to_string((long double)ig.GetFrameID())+".jpg";
			//fs::path dataDir = dataFile.parent_path();
			//fs::create_directories(dataFile.);
			fs::path imgSavePath = boost_fpath.parent_path()/boost_fpath.stem()+"_frames_1out10";
			if(!fs::exists(imgSavePath))
				fs::create_directories(imgSavePath);
			
			//cout<<img2save<<endl;
			//imwrite(img2save, im);
			fs::path img2save = imgSavePath/std::to_string((long double)ig.GetFrameID())+".jpg",
				depth2save = imgSavePath/std::to_string((long long)dg.GetFrameID())+".csv";
			//写文件: image -> 1.jpg; depth -> 1.csv
			imwrite(img2save.string(), im);
			
			ofstream fout(depth2save.string());
			fout<<cv::format(dm, "csv")<<endl;
			
		}

		key = waitKey(1);
	}//while

	cv::Mat mat(3, 2, CV_32F, 1);
	cout<<mat<<endl;
	
	
}//main

