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
#include <boost/format.hpp>

using namespace std;
using namespace cv;
using namespace boost;
namespace fs = boost::filesystem;
using namespace xn;

fs::path operator+(const fs::path& lhs, const fs::path& rhs){ 
	return fs::path(lhs.string() + rhs.string()); 
}

void checkOpenNIError(XnStatus rc, string status){
	if(rc != XN_STATUS_OK){
		cerr<<status<<" Error: "<<xnGetStatusString(rc)<<endl;
		
	}
}//checkOpenNIError

void main(int argc, char *argv[]){
	//参数 or 交互输入 oni 绝对路径：
	string fpath;
	if(argc > 1){
		fpath = argv[1];
	}
	else{ //(argc <= 1)
		cout<<"input ONI file's absolute path:"<<endl;
		getline(cin, fpath);
		if(fpath.size() == 0){
			cout<<"No input, and (argc <= 1)"<<endl;
			return;
		}
	}
	cout<<"FPATH: "<<fpath<<endl;
	fs::path boost_fpath = fpath;
	fs::path saveFdir = boost_fpath.parent_path()/boost_fpath.stem()+"-raw_frames";
	if(!fs::exists(saveFdir))
		fs::create_directories(saveFdir);

	//初始化 context：
	XnStatus rc = XN_STATUS_OK;
	Context ctx;
	rc = ctx.Init();
	checkOpenNIError(rc, "ctx.Init");

	Player plyr;
	rc = ctx.OpenFileRecording(fpath.c_str(), plyr);
	checkOpenNIError(rc, "OpenFileRecording");
	rc = plyr.SetRepeat(FALSE);
	checkOpenNIError(rc, "ply.SetRepeat");
	DepthGenerator dg;
	rc = dg.Create(ctx);
	checkOpenNIError(rc, "create dg");
	bool dgOk = (rc == XN_STATUS_OK);

	ImageGenerator ig;
	rc = ig.Create(ctx);
	checkOpenNIError(rc, "create ig");
	bool igOk = (rc == XN_STATUS_OK);

	xn::IRGenerator irg;
	rc = irg.Create(ctx);
	checkOpenNIError(rc, "create irg");
	bool irgOk = (rc == XN_STATUS_OK);

	DepthMetaData depthMD;
	ImageMetaData imageMD;
	xn::IRMetaData irMD; //若无 img, 尝试用 ir 数据代替(未必有)
	xn::MapMetaData *mmd; //父类指针可能指向 imageMD or irMD; (非实例 no appropriate default constructor available)

	xn::MapGenerator mapg;
	if(igOk)
		mapg = ig;
	else if(irgOk)
		mapg = irg;

	//貌似没这句也行：
	rc = ctx.StartGeneratingAll();
	checkOpenNIError(rc, "ctx.StartGeneratingAll");

	int key = -1,
		cnt = 0;
	boost::format fmt("%04d");
	string ext = ".raw";

	//while((rc = ctx.WaitAnyUpdateAll()) != XN_STATUS_EOF){
	while(key != 27){
		cnt++;

		//rc = ctx.WaitAndUpdateAll();
		//rc = ctx.WaitAnyUpdateAll();
		rc = ctx.WaitOneUpdateAll(dg);
 		checkOpenNIError(rc, "ctx.WaitAndUpdateAll");
		if(rc != XN_STATUS_OK)
			break;

		dg.GetMetaData(depthMD);
		Mat dm(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.Data()),
			dm8u;
		double dmin, dmax;
		minMaxLoc(dm, &dmin, &dmax);
		//cout<<"dmin, dmax: "<<dmin<<",\t"<<dmax<<",,"<<dm.type()<<endl;
		dm.convertTo(dm8u, CV_8U, 255./(dmax-dmin), -dmin*255./(dmax-dmin));
		imshow("depth map", dm8u);

		int imgType = 0;
		if(igOk){
			ig.GetMetaData(imageMD);
			mmd = &imageMD;
			imgType = CV_8UC3;
		}
		else if(irgOk){
			irg.GetMetaData(irMD);
			mmd = &irMD;
			imgType = CV_16UC1;
		}
		Mat im(mmd->FullYRes(), mmd->FullXRes(), imgType, (void*)mmd->Data());
		//Mat im(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (void*)imageMD.Data()),
		//	im_draw;
		//cvtColor(im, im_draw, CV_BGR2RGB);

		Mat im_draw;
		if(igOk)
			cvtColor(im, im_draw, CV_BGR2RGB);
		else if(irgOk){
			Mat im8u;
			im.convertTo(im8u, CV_8UC1, 1./3);
			//imshow("im8u", im8u); //ok√

			Mat cn3[3]={im8u, im8u, im8u};
			cv::merge(cn3, 3, im_draw);
		}

		imshow("color image", im_draw);

		fmt % cnt; //fmt 变了
		fs::path rawIpath = saveFdir/"Image_"+fmt.str()+ext,
			rawDpath = saveFdir/"Depth_"+fmt.str()+ext,
			pngDpath = saveFdir/"Depth_"+fmt.str()+".png",
			rawIRpath = saveFdir/"IR_"+fmt.str()+ext,
			pngIRpath = saveFdir/"IR_"+fmt.str()+".png";

		if(igOk){
			xnOSSaveFile(rawIpath.string().c_str(), mmd->Data(), mmd->DataSize());
		}
		else if(irgOk){
			xnOSSaveFile(rawIRpath.string().c_str(), mmd->Data(), mmd->DataSize());

			//imwrite(pngIRpath.string(), ); //暂不生成IR-png, 若要, 参考NiViewer/capture.cpp
		}

		xnOSSaveFile(rawDpath.string().c_str(), depthMD.Data(), depthMD.DataSize());
		Mat dmat(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.Data());
		cv::imwrite(pngDpath.string(), dmat);
		cout<<"cnt: "<<cnt<<endl
			<<rawIpath.leaf()<<endl;
		key = waitKey(1);
	}//while

}//main