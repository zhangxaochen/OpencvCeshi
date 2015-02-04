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
	cout<<"mdg.GetName(): "<<mdg.GetName()<<", "<<dg.GetName()<<endl;

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
	//while((rc = dg.WaitAndUpdateData()) != XN_STATUS_EOF){
	//rc = ctx.WaitAndUpdateAll(); //× 未解决
	//rc = ctx.WaitAnyUpdateAll(); //√ 不知为什么

	cout<<"==rc: "<<rc<<", "<<XN_STATUS_OK<<", "<<XN_STATUS_EOF<<endl;
	while((rc = ctx.WaitAnyUpdateAll()) != XN_STATUS_EOF){
		//cout<<"---------------"<<endl;
		checkOpenNIError(rc, "dg.WaitAndUpdateData");

		dg.GetMetaData(depthMD);
		ig.GetMetaData(imageMD);

		//---------------depth
		rc = depthMD.MakeDataWritable();
		checkOpenNIError(rc, "depthMD.MakeDataWritable");

		//char *buf = depthMD.WritableData();
		Mat dm(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.Data());
		double dmax, dmin;
		minMaxLoc(dm, &dmin, &dmax);
		Mat dm_draw;
		dm.convertTo(dm_draw, CV_8U, 255./(dmax-dmin), -dmin*255./(dmax-dmin));
		imshow("depth map", dm_draw);

		//depth map writeable
		Mat dmw(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.WritableData()),
			dmw_draw;
		Mat mask(dmw.size(), CV_8UC1, Scalar::all(0));
		mask(Rect(100, 100, 300, 300)).setTo(Scalar(255));
		dmw.copyTo(dmw_draw, mask);
		imshow("dmw_draw", dmw_draw);
		
		
		//mdg.SetData(depthMD);
		//char *buf = dmw.data();
		int dbufSz = dmw_draw.total() * dmw_draw.elemSize();
		
		mdg.SetData(dg.GetFrameID(), dg.GetTimestamp(), dbufSz, (XnDepthPixel *)dmw_draw.data);
		//mdg.SetData(cnt, 1, sz, (XnDepthPixel *)dmw_draw.data);
		//rcdr.Record();

		//---------------image
		Mat im(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (void*)imageMD.Data()),
			im_draw;
		cvtColor(im, im_draw, CV_RGB2BGR);
		imshow("image map", im_draw);

		Mat imw, imw_draw;
		im.copyTo(imw, mask); //√
		//im_draw.copyTo(imw_draw, mask); //×, imw_draw 显示正确， 但后面存回去时 BGR 又错了
		cvtColor(imw, imw_draw, CV_RGB2BGR);
		imshow("imw_draw", imw_draw);

		int ibufSz = imw.total() * imw.elemSize();
		mig.SetData(ig.GetFrameID(), ig.GetTimestamp(), ibufSz, imw.data);
		rcdr.Record();

		cnt++;
		cout<<"cnt: "<<cnt<<endl;
		waitKey(1);
	}//while

}//main




