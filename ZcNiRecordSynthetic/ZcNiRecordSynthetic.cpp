#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <XnCppWrapper.h>

using namespace std;
using namespace cv;
namespace fs = boost::filesystem;
using namespace xn;

//zhangxaochen: 自定义全局变量, dirty: //2016-3-9 22:06:36
bool isCropz = false;
int cropz = -1;

bool isXywh = false;
int winx = -1,
	winy = -1,
	winw = -1,
	winh = -1;

//按空格，则手动播放：
bool isManually = false;

void checkOpenNIError(XnStatus rc, string status){
	if(rc != XN_STATUS_OK)
		cerr<<status<<" Error: "<<xnGetStatusString(rc)<<endl;
}//checkOpenNIError

void main(int argc, char *argv[]){
	XnStatus rc = XN_STATUS_OK;
	if(argc < 3){
		cout<<"usage: "<<argv[0]<<" <inuptFile> <outputFile>"<<endl;
		return;
	}

	string strInputFile = argv[1],
		strOutputFile = argv[2];
	if(argc>=3){
		for(size_t i=1; i< argc; i++){
			//“按深度阈值裁剪”功能, 裁掉大于此深度阈值的像素
			if(strcmp(argv[i], "-cropz")==0){
				CV_Assert(argc>i+1);
				isCropz = true;
				cropz = atoi(argv[i+1]);
			}
			if(strcmp(argv[i], "-xywh")==0){
				CV_Assert(argc>i+4);
				isXywh = true;
				winx = atoi(argv[i+1]);
				winy = atoi(argv[i+2]);
				winw = atoi(argv[i+3]);
				winh = atoi(argv[i+4]);
			}
		}

	}

	Context ctx2;
	rc = ctx2.Init();
	checkOpenNIError(rc, "ctx2.Init");
	DepthGenerator dummyDg;
	dummyDg.Create(ctx2);
	checkOpenNIError(rc, "dummyDg.Create");

	Context ctx;
	rc = ctx.Init();
	checkOpenNIError(rc, "ctx.Init");

	Player ply;
	rc = ctx.OpenFileRecording(strInputFile.c_str(), ply);
	checkOpenNIError(rc, "ctx.OpenFileRecording");
	rc = ply.SetRepeat(FALSE);
	checkOpenNIError(rc, "ply.SetRepeat");

	DepthGenerator dg;
	rc = ctx.FindExistingNode(XN_NODE_TYPE_DEPTH, dg);
	checkOpenNIError(rc, "ctx.FindExistingNode");
// 	rc = dg.Create(ctx);
// 	checkOpenNIError(rc, "dg.Create");

	ImageGenerator ig;
	rc = ig.Create(ctx);
	checkOpenNIError(rc, "ig.Create");

	MockDepthGenerator mdg;
	//DepthGenerator mdg;
	rc = mdg.CreateBasedOn(dg); //√
	//rc = mdg.CreateBasedOn(dummyDg); //×
	//rc = mdg.Create(ctx, "MockDepth");
	checkOpenNIError(rc, "mdg.CreateBasedOn");
	cout<<"mdg.GetName(): "<<mdg.GetName()<<", "<<dg.GetName()<<endl; //Depth1_Mock, Depth1

	MockImageGenerator mig;
	rc = mig.CreateBasedOn(ig);
	checkOpenNIError(rc, "mig.CreateBasedOn");

	Recorder rcdr;
	rc = rcdr.Create(ctx);
	//rcdr.Create(ctx2);
	checkOpenNIError(rc, "rcdr.Create");
	rc = rcdr.SetDestination(XN_RECORD_MEDIUM_FILE, "ZcNiRecordSynthetic.oni");
	checkOpenNIError(rc, "rcdr.SetDestination");

	//rc = rcdr.AddNodeToRecording(mdg, XN_CODEC_UNCOMPRESSED);
	rc = rcdr.AddNodeToRecording(mdg);
	checkOpenNIError(rc, "rcdr.AddNodeToRecording(mdg)");

	rc = rcdr.AddNodeToRecording(mig);
	checkOpenNIError(rc, "rcdr.AddNodeToRecording(mig)");

	DepthMetaData depthMD;
	ImageMetaData imageMD;
	int ww = 640,
		hh = 480;

	int cnt = 0;
	//rc = ctx.WaitAndUpdateAll(); //× 未解决
	//rc = ctx.WaitAnyUpdateAll(); //√ 不知为什么
	//while((rc = dg.WaitAndUpdateData()) != XN_STATUS_EOF){ //仅更新 dg
	//while((rc = ctx.WaitAnyUpdateAll()) != XN_STATUS_EOF){ //任意更新
	while(1){
		rc = ctx.WaitOneUpdateAll(dg);
		if (rc == XN_STATUS_EOF){
			break;
		}
		
		checkOpenNIError(rc, "WaitAndUpdate xxx");

		dg.GetMetaData(depthMD);
		ig.GetMetaData(imageMD);
		cout<<"depthMD.FrameID: "<<depthMD.FrameID()<<", "<<imageMD.FrameID()<<endl;
		ww = depthMD.FullXRes();
		hh = depthMD.FullYRes();


		Mat mask; //用于剪裁画面
		Mat res2show; //四个屏拼接显示

		//---------------depth
		rc = depthMD.MakeDataWritable();
		checkOpenNIError(rc, "depthMD.MakeDataWritable");

		if(depthMD.Data() != NULL){
			//char *buf = depthMD.WritableData();
			Mat dm(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.Data());
			double dmax, dmin;
			minMaxLoc(dm, &dmin, &dmax);
			Mat dm8u;
			dm.convertTo(dm8u, CV_8U, 255./(dmax-dmin), -dmin*255./(dmax-dmin));

			mask = dm>0;
			if(isXywh){
				//边界情况处理：
				winx = winx<0 ? 0 : (winx>ww ? ww-1 : winx);
				winy = winy<0 ? 0 : (winy>hh ? hh-1 : winy);
				winw = winw<0 ? 0 : (winx+winw>ww ? ww-winx : winw);
				winh = winh<0 ? 0 : (winy+winh>hh ? hh-winy : winh);

				Mat xywhMask(mask.size(), CV_8UC1, Scalar::all(0));
				xywhMask(Rect(winx, winy, winw, winh)).setTo(Scalar(255));
				mask = mask & xywhMask;
			}
			if(isCropz){
				mask.setTo(0, dm>cropz);
			}

			dm.setTo(0, mask==0);
			Mat dm8uCrop;
			dm8u.copyTo(dm8uCrop, mask);
			cv::rectangle(dm8uCrop, Rect(winx, winy, winw, winh), 255);
			//imshow("win1", dm8u);
			//imshow("win3", dm8uCrop);
			cv::vconcat(dm8u, dm8uCrop, res2show);

			//+++++++++++++++传入mock-dg:
			mdg.SetData(depthMD);
		}

		//---------------image
		if(imageMD.Data() != NULL){
			//rc = imageMD.MakeDataWritable(); //对 imageMD 无效, 不懂
			//checkOpenNIError(rc, "imageMD.MakeDataWritable");

			//Mat im(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (void*)imageMD.WritableData());
			Mat im(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (void*)imageMD.Data());
			
			Mat im8u3c, im8u3cCrop;
			cvtColor(im, im8u3c, CV_RGB2BGR);
			if(0){//rgb要不要mask? 1: 要; 0: 不要
				im.setTo(0, mask==0);
				im8u3c.copyTo(im8u3cCrop, mask);
			}
			else{
				im8u3c.copyTo(im8u3cCrop);
			}
			//imshow("win2", im8u3c);
			//imshow("win4", im8u3cCrop);
			Mat rightWin; //右侧两个窗口上下拼接
			cv::vconcat(im8u3c, im8u3cCrop, rightWin);

			//d, rgb左右两窗口拼接
			Mat cn3[]={res2show, res2show, res2show};
			cv::merge(cn3, 3, res2show);
			cv::hconcat(res2show, rightWin, res2show);

			//+++++++++++++++传入mock-ig:
			mig.SetData(imageMD);
		}
		else{
			//测试wait-depth情形下, imageMD会不会出现无Data的情况? 
			//目前没有，说明无论是否imageMD有数据, 都不会为 NULL?
			cout<<"imageMD.Data() == NULL"<<endl;
		}
		//cv::pyrDown(res2show, res2show);
		imshow("res2show", res2show);

// 		//depth map writeable
// 		//Mat dmw(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.WritableData()), //此处不必 writable
// 		Mat dmw(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.Data()),
// 			dmw_draw;
// 		//Mat mask(dmw.size(), CV_8UC1, Scalar::all(0));
// 		mask(Rect(100, 100, 300, 300)).setTo(Scalar(255));
// 		dmw.copyTo(dmw_draw, mask);
// 		imshow("dmw_draw", dmw_draw);
// 		
// 		
// 		//mdg.SetData(depthMD);
// 		//char *buf = dmw.data();
// 		int dbufSz = dmw_draw.total() * dmw_draw.elemSize();
// 		
// 		mdg.SetData(dg.GetFrameID(), dg.GetTimestamp(), dbufSz, (XnDepthPixel *)dmw_draw.data);
// 		//mdg.SetData(cnt, 1, sz, (XnDepthPixel *)dmw_draw.data);
// 		//rcdr.Record();
// 
// 		//---------------image
// 		Mat im(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (void*)imageMD.Data()),
// 			im_draw;
// 		cvtColor(im, im_draw, CV_RGB2BGR);
// 		imshow("image map", im_draw);
// 
// 		Mat imw, imw_draw;
// 		im.copyTo(imw, mask); //√
// 		//im_draw.copyTo(imw_draw, mask); //×, imw_draw 显示正确， 但后面存回去时 BGR 又错了
// 		cvtColor(imw, imw_draw, CV_RGB2BGR);
// 		imshow("imw_draw", imw_draw);
// 
// 		int ibufSz = imw.total() * imw.elemSize();
// 		mig.SetData(ig.GetFrameID(), ig.GetTimestamp(), ibufSz, imw.data);
		rcdr.Record();

		cnt++;
		cout<<"cnt: "<<cnt<<endl;
		int key = waitKey(isManually ? 0 : 1);
		if(key == 27)
			break;
		else if(key == ' ')
			isManually = !isManually;

	}//while

}//main




