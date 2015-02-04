/** 参考： http://blog.csdn.net/chenxin_130/article/details/6696187
 ** by zhangxaochen
 ** 
 **/

#include <iostream>
#include <string>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;
using namespace cv;
namespace fs = boost::filesystem;
using namespace xn;

void checkOpenNIError(XnStatus rc, string status){
	if(rc != XN_STATUS_OK)
		cerr<<status<<" Error: "<<xnGetStatusString(rc)<<endl;
}//checkOpenNIError

void main(int argc, char *argv[]){
	//1. 读目录中的 xx.raw 到 rawDpaths， rawIpaths
	string default_dir = "../../data/CapturedFrames";
	if(argc > 1)
		default_dir = argv[1];

	cout<<"input raw file existing path:"<<endl;
	string fdir;
	//cin>>fpath;
	getline(cin, fdir);
	if(fdir.size() == 0)
		fdir = default_dir;
	cout<<"fdir: "<<fdir<<", "<<fdir.size()<<endl;
	fs::path boost_fdir = fdir;
	fs::directory_iterator it(boost_fdir),
		end_it;

	//vector<string> draws, iraws;
	vector<fs::path> rawDpaths, rawIpaths;
	while(it != end_it){
		fs::path fpath = it->path();//.filename();
		if(fs::is_regular_file(*it) && fpath.extension() == ".raw"){
			string fname = fpath.filename().string();
			if(boost::starts_with(fname, "Image_"))
				//iraws.push_back(fpath.string());
				rawIpaths.push_back(fpath);
			else if(boost::starts_with(fname, "Depth_"))
				//draws.push_back(fpath.string());
				rawDpaths.push_back(fpath);
		}
		it++;
	}

	XnStatus rc = XN_STATUS_OK;
	Context ctx;
	Player ply;
	const char *good_oni = "d:/Users/zhangxaochen/Desktop/1_100cm.oni";
	//ctx.OpenFileRecording(good_oni, ply);
	//---------------Get dummy depth generator to base mock depth generator on
	DepthGenerator dg;
	ImageGenerator ig;
// 	rc = ctx.FindExistingNode(XN_NODE_TYPE_DEPTH, dg);
// 	checkOpenNIError(rc, "ctx.FindExistingNode");

#pragma region load_good_oni
// 	Context ctx2;
// 	ctx2.OpenFileRecording(good_oni, ply);
// 	dg.Create(ctx2);
	


#pragma endregion load_good_oni

	rc = ctx.Init();
	checkOpenNIError(rc, "ctx.Init");
	Recorder rcdr;
	rc = rcdr.Create(ctx);
	checkOpenNIError(rc, "rcdr.Create");
	rc = rcdr.SetDestination(XN_RECORD_MEDIUM_FILE, "raw2oni.oni");
	checkOpenNIError(rc, "rcdr.SetDestination");

	//DepthGenerator dg;
	dg.Create(ctx);
	ig.Create(ctx);

	MockImageGenerator mig;
	//rc = mig.Create(ctx, "MockImage");
	rc = mig.CreateBasedOn(ig, "MockImage");
	checkOpenNIError(rc, "mig.CreateBasedOn");
	MockDepthGenerator mdg;
	//rc = mdg.Create(ctx, "MockDepth");
	rc = mdg.CreateBasedOn(dg, "MockDepth");
	checkOpenNIError(rc, "mdg.Create");

	rc = rcdr.AddNodeToRecording(mig);
	checkOpenNIError(rc, "rcdr.AddNodeToRecording(mig)");
// 	rc = rcdr.AddNodeToRecording(mdg);
// 	checkOpenNIError(rc, "rcdr.AddNodeToRecording(mdg)");
	
	int sz = 1e7;
	char *buf = new char[sz];
	int hh = 480,
		ww = 640;
	double dmin, dmax;
	int key = -1;
	for(int i = 0; i < rawIpaths.size(); i++){
		fs::path ifpath = rawIpaths[i],
			dfpath = rawDpaths[i];
// 		const char *ifname = ifpath.string().c_str(),
// 			*dfname = dfpath.string().c_str();
		string ifname = ifpath.string(), dfname = dfpath.string();
		cout<<"--image & depth fname: "<<ifname<<endl
			<<dfname<<endl;
		rc = xnOSLoadFile(ifname.c_str(), buf, sz);
		checkOpenNIError(rc, "ifname xnOSLoadFile");
// 		ImageMetaData imd;
// 		imd.
		/*xnMockImageSetData*/
		//XnUInt32 nFrameID, XnUInt64 nTimestamp, XnUInt32 nDataSize, const XnUInt8* pImageMap
		//rc = mig.SetData(i, i, sz, (const XnUInt8*)buf);
		rc = mig.SetData(i, i, hh * ww * 3, (const XnUInt8*)buf);
		checkOpenNIError(rc, "mig.SetData");
		void *buf2 = (void *)mig.GetData();
		Mat im2(hh, ww, CV_8UC3, buf2);
		imshow("mig.im2", im2);

		rc = rcdr.Record();
		checkOpenNIError(rc, "rcdr.Record");

		Mat im(hh, ww, CV_8UC3, (void*)buf);
		cvtColor(im, im, CV_RGB2BGR);
		imwrite(ifpath.replace_extension(".jpg").string(), im);
		imshow("image-raw", im);

		rc = xnOSLoadFile(dfname.c_str(), buf, sz);
		checkOpenNIError(rc, "dfname xnOSLoadFile");
		Mat dm(hh, ww, CV_16UC1, (void*)buf);
		minMaxLoc(dm, &dmin, &dmax);
		dm.convertTo(dm, CV_8U, 255./(dmax-dmin), -dmin*255./(dmax-dmin));
		imwrite(dfpath.replace_extension(".jpg").string(), dm);
		imshow("depth-raw", dm);

		key = waitKey();
		if(key == 27)
			break;
	}//for(int i = 0; i < rawIpaths.size(); i++)
	rcdr.Release();
	mig.Release();

// 	while (it != end_it){
// 		if(!(fs::is_regular_file(*it) && it->path().extension() == ".raw")){
// 			cout<<"++"<<it->path()<<endl;
// 			it++;
// 			continue;
// 		}
// 		int sz = 1e7;
// 		char *buf = new char[sz];
// 		rc = xnOSLoadFile(it->path().string().c_str(), buf, sz);
// 		//有 bug， 读取成功， 但是状态描述为 “Failed to read from the file!”
// 		checkOpenNIError(rc, "xnOSLoadFile");
// 		// 	if(rc != XN_STATUS_OK)
// 		// 		return;
// 
// 		//fs::path fname = it->path().stem();
// 		fs::path fpath = it->path();
// 		fs::path fname = fpath.filename();
// 		cout<<"--"<<fname<<endl;
// 		if(boost::starts_with(fname.string(), "Image_")){
// 			Mat im(480, 640, CV_8UC3, (void*)buf);
// 			cvtColor(im, im, CV_RGB2BGR);
// 			//imshow(fname.string(), im);
// 			imshow("image-raw", im);
// 			imwrite(fpath.replace_extension(".jpg").string(), im);
// 			cout<<fname<<endl;
// 			cout<<fpath<<endl;
// 		}
// 		else if(boost::starts_with(fname.string(), "Depth_")){
// 			Mat dm(480, 640, CV_16UC1, (void*)buf);
// 			double dmin, dmax;
// 			minMaxLoc(dm, &dmin, &dmax);
// 			dm.convertTo(dm, CV_8U, 255./(dmax-dmin), -dmin*255./(dmax-dmin));
// 			//imshow(fname.string(), dm);
// 			imshow("depth-raw", dm);
// 			imwrite(fpath.replace_extension(".jpg").string(), dm);
// 			cout<<fname<<endl;
// 			cout<<fpath<<endl;
// 		}
// 		waitKey();
// 
// 		it++;
// 	}


	return;

}//main

