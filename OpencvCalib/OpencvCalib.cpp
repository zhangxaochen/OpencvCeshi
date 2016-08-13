/** 
 ** zhangxaochen
 ** 参考 openni_load_oni_file.cpp
 **/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

#include <XnCppWrapper.h>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>

using namespace std;
using namespace cv;
namespace fs = boost::filesystem;

const char *iframeStr = "image_frame",
	*iframeIdStr = "id",
	*iframeRotStr = "r",
	*iframeTransStr = "t",
	*iframeRtStr = "rt"
	;

#define _WRITE_EXTRINSICS

fs::path operator+(const fs::path& lhs, const fs::path& rhs){ 
	return fs::path(lhs.string() + rhs.string());
}

void checkOpenNIError(XnStatus rc, string status){
	if(rc != XN_STATUS_OK){
		cerr<<status<<" Error: "<<xnGetStatusString(rc)<<endl;

	}
}//checkOpenNIError

// CV_EXPORTS_W void projectPoints( InputArray objectPoints,
// 	InputArray rvec, InputArray tvec,
// 	InputArray cameraMatrix, InputArray distCoeffs,
// 	OutputArray imagePoints,
// 	OutputArray jacobian=noArray(),
// 	double aspectRatio=0 );

//先不管 distCoeffs, etc.
//效果还可以， 不精确
void zcProjectPoints(InputArray objectPoints, 
	InputArray rvec, InputArray tvec,
	InputArray cameraMatrix, InputArray distCoeffs,
	OutputArray imagePoints)
{
	Mat _objectPoints = objectPoints.getMat(),
		_rvec = rvec.getMat(), _tvec = tvec.getMat(),
		_cameraMatrix = cameraMatrix.getMat(), _distCoeffs = distCoeffs.getMat();

	int npoints = _objectPoints.checkVector(3), depth = _objectPoints.depth();
	CV_Assert(npoints >= 0 && (depth == CV_32F || depth == CV_64F));
	
	imagePoints.create(npoints, 1, CV_MAKETYPE(depth, 2), -1, true);
	Mat _imagePoints = imagePoints.getMat();

	Mat rmat, extrin;
	cv::Rodrigues(rvec, rmat);
	//rmat.convertTo(rmat, CV_32F);
	cv::hconcat(rmat, tvec, extrin);
	//extrin.convertTo(extrin, CV_32F);
	
	cout<<"extrin: "<<extrin<<endl
		<<extrin.type()<<rmat.type()<<endl;;

	Mat projMat = cameraMatrix.getMat() * extrin;
	cout<<"projMat: "<<projMat<<endl;
	for(size_t i = 0; i < npoints; i++){
		Vec3f opt_i = _objectPoints.at<Vec3f>(0, i);
		//Vec3d opt_i = _objectPoints.at<Vec3d>(0, i);
		//cout<<"##"<<Scalar(opt_i)<<endl;
		Mat opti4(1, 4, CV_32F, opt_i.val);
		opti4.convertTo(opti4, CV_64F);
		//Mat opti4(1, 4, CV_32F, Scalar(opt_i));
		//Mat opti4(1, 4, CV_32F, opt_i.val);
		
		//cout<<"$$"<<opti4<<","<<opti4.at<double>(0, 0)<<endl;
		opti4.at<double>(0, 3) = 1;
		//cout<<opti4<<endl;
		//Mat(1, 1, CV_32FC3, opt_i);
		//cv::convertPointsToHomogeneous(opt_i, opti4);
		//cout<<"opt_i: "<<opt_i<<endl
		//	<<opti4<<"--"<<opti4.size()<<endl;
		//cout<<"++"<<opt_i<<"=="<<opti4<<endl;

		Mat ipti3 = cameraMatrix.getMat() * extrin * opti4.t();
		//ipti3.convertTo(ipti3, CV_32F);
		double x = ipti3.at<double>(0, 0),
			y = ipti3.at<double>(1, 0),
			z = ipti3.at<double>(2, 0);
		double coord[] = {x/z, y/z};
		Mat ipti2(1, 2, CV_64F, coord);
		//cv::convertPointsFromHomogeneous(ipti3.t(), ipti2);

// 		cout<<"ipti3: "<<ipti3<<endl
// 			<<"--ipti2: "<<ipti2<<endl;
		_imagePoints.at<Vec2f>(0, i)[0] = x/z;
		_imagePoints.at<Vec2f>(0, i)[1] = y/z;
	}
}//zcProjectPoints


void main(int argc, char *argv[]){
	//const char *default_fpath = "E:/GitHub/OpenniCeshi/data/openNI132_data_registration_4x3/test_smt_far_4x3.oni";
	const char *default_fpath = "D:/Program Files/OpenNI/Samples/Bin/Release/smt-d100-sz35.8-r2l-t2b-refine.oni";
	const char *def_cam_xml = "D:/Program Files (x86)/OpenNI/Samples/Bin/Debug/CapturedFrames_ir_5x8A3/cam.xml";
	int def_ww = 4,
		def_hh = 3;
	float def_length = 1.;
	int foundCnt = 0,
		notFoundCnt = 0;

// 	fs::path test = "test_path";
// 	cout<<test<<endl;
// 	test += "_concat";
// 	cout<<test/"shit"<<endl;
// 	cout<<(test+="ddddd")<<endl;
// 	cout<<test+"zzzz"<<endl;
// 	
// 	return;

	if(argc>1){
		default_fpath = argv[1];
// 		cout<<argv[0]<<endl
// 			<<argv[1]<<endl;
	}
	//内参xml文件做参数, ww,hh 都从此xml读取	//2015-12-13 17:36:17
// 	if(argc>4){
// 		def_length = std::stof(argv[4]);
// 	}
	if(argc>2){
		def_cam_xml = argv[2];
	}
	if(argc>3){
		def_length = std::stof(argv[3]);
	}
	if(argc>4){
		def_ww = std::stoi(argv[4]);
		def_hh = std::stoi(argv[5]);
	}

	cout<<"input ONI file's absolute path:"<<endl;
	string fpath;
	getline(cin, fpath);
	if(fpath.size() == 0)
		fpath = default_fpath;
	fs::path boost_fpath = fpath;
	cout<<"boost_fpath.extension(): "<<boost_fpath.extension()<<endl;
		//<<boost_fpath.leaf()+"__sss"<<endl;

	//if(boost_fpath.extension() != ".oni"){
	if(boost_fpath.extension() != ".oni"){
		cout<<"the file you input must be an ONI file!"<<endl;
		return;
	}
	
	//此程序生成的任何数据放在 ..._calib 目录中
	fs::path saveFdir = boost_fpath.parent_path()/boost_fpath.stem()+"_calib";
	if(!fs::exists(saveFdir))
		fs::create_directories(saveFdir);

	//去畸变的深度图存放路径：	//2016-5-13 16:42:10
	fs::path dmUndistDir = saveFdir/"dm_undistort";
	if(!fs::exists(dmUndistDir))
		fs::create_directories(dmUndistDir);

// 	fs::directory_iterator it(saveFdir),
// 		end_it;
// 	while(it != end_it){
// 		
// 	}//while

	//*.oni.txt 文件存着起始时间戳(millis)	//2015-11-23 21:12:12
	long long startTstamp = -1; //start-timestamp
	string tsFpath = fpath + ".txt";
	ifstream tsFin(tsFpath);
	if(tsFin.good()){
		tsFin >> startTstamp;
		cout << "startTstamp: " << startTstamp << endl;
	}
	else{
		tsFin.close();
		cout << tsFpath << " does NOT exist!" << endl;
	}

	//1.
	XnStatus rc = XN_STATUS_OK;
	xn::DepthMetaData depthMD;
	xn::ImageMetaData imageMD;
	xn::IRMetaData irMD; //若无 img, 尝试用 ir 数据代替(未必有)
	xn::MapMetaData *mmd; //父类指针可能指向 imageMD or irMD; (非实例 no appropriate default constructor available)

	//2.
	xn::Context ctx;
	xn::Player plyr;
	rc = ctx.Init();
	checkOpenNIError(rc, "init context");
	//rc = ctx.OpenFileRecording(boost_fpath.string().c_str());
	rc = ctx.OpenFileRecording(boost_fpath.string().c_str(), plyr);
	checkOpenNIError(rc, "OpenFileRecording");

	rc = plyr.SetRepeat(false);
	checkOpenNIError(rc, "plyr.SetRepeat");

	xn::DepthGenerator dg;
	rc = dg.Create(ctx);
	checkOpenNIError(rc, "create dg");
	bool dgOk = (rc == XN_STATUS_OK);

	xn::ImageGenerator ig;
	rc = ig.Create(ctx);
	//rc = ctx.FindExistingNode(XN_NODE_TYPE_IMAGE, ig);
	checkOpenNIError(rc, "create ig");
	bool igOk = (rc == XN_STATUS_OK);

	xn::IRGenerator irg;
	rc = irg.Create(ctx);
	checkOpenNIError(rc, "create irg");
	bool irgOk = (rc == XN_STATUS_OK);
	
	xn::MapGenerator mapg;
	if(igOk)
		mapg = ig;
	else if(irgOk)
		mapg = irg;

	//5. read data
	rc = ctx.StartGeneratingAll();

	//---------------外定标准备
	int ww = def_ww, hh = def_hh;
// 	cv::Size boardSize(ww, hh);

	//const char *cam_xml = "E:/GitHub/OpenniCeshi/data/CapturedFrames_openni132_7x10/cam.xml";
	//const char *cam_xml = "E:/oni_data/CapturedFrames_openni132_7x10/cam.xml";
// 	const char *cam_xml = "D:/Program Files (x86)/OpenNI/Samples/Bin/Debug/CapturedFrames_ir_5x8A3/cam.xml";
	
	FileStorage cvfs(def_cam_xml, FileStorage::READ);

	//int ww, hh;
// 	cvfs["board_width"] >> ww; //不用此 xml 参考值, 此xml仅用于提供内参+畸变系数
// 	cvfs["board_height"] >> hh;
	cv::Size boardSize(ww, hh);

	Mat intrinsics, distCoeffs;
	cvfs["camera_matrix"] >> intrinsics;
	cvfs["distortion_coefficients"] >> distCoeffs;
	cout << intrinsics <<endl
		<< distCoeffs <<endl;

	vector<Point3f> boardPoints;
	//float length = 35.8; //3x4
	//float length = 22.3; //7x8
	float length = def_length;

	for(int j = 0; j < hh; j++){
		for(int i = 0; i < ww; i++){
			float x = i * length,
				y = j * length,
				z = 0;
			boardPoints.push_back(Point3f(x, y, z));
		}
	}
	//最后加一个 (0,0,z), 用于画Z轴调试：
	vector<Point3f> pt3fDraw = boardPoints;
	pt3fDraw.push_back(Point3f(0, 0, 0));
	pt3fDraw.push_back(Point3f(111, 0, 0));
	pt3fDraw.push_back(Point3f(0, 111, 0));
	pt3fDraw.push_back(Point3f(0, 0, 111));

	vector<vector<double>> rvecs, tvecs;

	//保存每一RGB帧外定标 6DOF tuple：
	fs::path extrinsics_xml = saveFdir/"extrinsics.xml";
	fs::path extrTxt = saveFdir/"extrinsics-c6.txt"; //(t+R) 6列形式 //2016-5-8 21:33:52
	fs::path extrTxtKf = saveFdir/"extrinsics-c12-kf.txt"; //以及在此程序中直接转到 pcl_kinfu_app 的 TSDF volume 坐标系, 且为 (t+R) 12列形式

#ifdef _WRITE_EXTRINSICS
	cv::FileStorage fst(extrinsics_xml.string(), FileStorage::WRITE);
	ofstream foutExtrTxt(extrTxt.string(), ios::out);
	ofstream foutExtrTxtKf(extrTxtKf.string(), ios::out);
	
#endif // _WRITE_EXTRINSICS

	int maxImgFrameID = -1,
		maxDepFrameID = -1,
		curImgFrameID = 0,
		curDepFrameID = 0,
		key = 0;
	bool paused = false;

	clock_t currt = clock(), prevt, dt;
	
	//vector<double> rvec, tvec;
	Mat rvec, tvec;

	int fcnt = 0;

	//while(key != 27 && (curImgFrameID = ig.GetFrameID()) >= maxImgFrameID){
	while(key != 27){
		fcnt++; //是循环计数器, 与 curDepFrameID 有差异, 因为即便 curDepFrameID 不更新, 此 fcnt 也会更新	//2016-5-14 12:06:14

		if(key == 's')
			paused = !paused;
		else if(key == ' ')
			paused = true;

		currt = clock();
		//ctx.WaitAnyUpdateAll();
		//rc = ctx.WaitAndUpdateAll();
		//rc = ctx.WaitOneUpdateAll(mapg);
		rc = ctx.WaitOneUpdateAll(dg); //要存 dm-undistort 时, 使用此项 //错! dg 更新时, 可能还没有 ig, 会 crash   //2016-5-14 12:02:41

		//checkOpenNIError(rc, "ctx.WaitAndUpdateAll");
		if(rc != XN_STATUS_OK){
			cerr<<"ctx.WaitAndUpdateAll Error: "<<xnGetStatusString(rc)<<endl;
			break;
		}

		if(dgOk)
			curDepFrameID = dg.GetFrameID();

		//curImgFrameID 同时做 ig or irg 帧序号
		curImgFrameID = mapg.GetFrameID(); //从0开始, NiViewer从1开始, 原因是bSkipFirstFrame? 不确定...

		if(curDepFrameID >= maxDepFrameID)
			maxDepFrameID = curDepFrameID;
		else
			cout<<"============================="<<endl;
		if(curImgFrameID >= maxImgFrameID)
			maxImgFrameID = curImgFrameID;
		else
			cout<<"+++++++++++++++++++++++++++++"<<endl;

		dg.GetMetaData(depthMD);

		//没法省代码, GetMetaData 非虚函数:
		if(igOk){
			ig.GetMetaData(imageMD);
			mmd = &imageMD;
		}
		else if(irgOk){
			irg.GetMetaData(irMD);
			mmd = &irMD;
		}

		//curImgFrameID = ig.GetFrameID();
		//cout<<"--curFrameID: "<<curFrameID<<", "<<dg.GetFrameID()<<endl;

		//7.
		// 		Mat dm(depthMD.YRes(), depthMD.XRes(), CV_16UC1, (void*)depthMD.Data());
		Mat dm(depthMD.FullYRes(), depthMD.FullXRes(), CV_16UC1, (void*)depthMD.Data());
		double dmin, dmax;
		minMaxLoc(dm, &dmin, &dmax);
		cout<<"dmin, dmax: "<<dmin<<",\t"<<dmax<<",,"<<dm.type()<<endl;
		Mat dm8u;
		dm.convertTo(dm8u, CV_8U, 255./(dmax-dmin), -dmin*255./(dmax-dmin));
		imshow("depth map", dm8u);

		//去畸变测试:	//2016-5-13 16:18:40
		Mat dm_undist;
		cv::undistort(dm, dm_undist, intrinsics, distCoeffs);
		Mat dmundist8u;
		dm_undist.convertTo(dmundist8u, CV_8U, 255./(dmax-dmin), -dmin*255./(dmax-dmin));

		imshow("depth-map-undistort", dmundist8u);
		//保存此去畸变的深度图
		char buf[111];
		sprintf(buf, "Depth_%04d.png", curDepFrameID);
		//sprintf(buf, "Depth_%04d.png", fcnt);
		fs::path dmUndistFn = dmUndistDir/buf;
		imwrite(dmUndistFn.string(), dm_undist);

		//Mat im(imageMD.FullYRes(), imageMD.FullXRes(), CV_8UC3, (void*)imageMD.Data());

		int imgType = 0;
		if(igOk)
			imgType = CV_8UC3;
		else if(irgOk)
			imgType = CV_16UC1;
		Mat im(mmd->FullYRes(), mmd->FullXRes(), imgType, (void*)mmd->Data());

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
		//若不用 im_draw，WaitAnyUpdateAll 导致 RGB/BGR 频繁切换，闪烁：
// 		cvtColor(im, im, CV_BGR2RGB);
// 		imshow("color image2", im);
		dt = clock()-currt;
		cout<<"==time consumed: "<<dt<<", fps: "
			<< (dt != 0 ? to_string((long long)1000/dt) : "INF")<<endl;
		currt = clock();

		vector<Point2f> pt2fVec;
		bool foundCorners = cv::findChessboardCorners(im_draw, boardSize, pt2fVec);
		//if(curImgFrameID>403)
		if(foundCorners){
			foundCnt++;

			solvePnP(boardPoints, pt2fVec, intrinsics, distCoeffs, rvec, tvec);
// 			cout<<"+++++++++++++++rvec, tvec: "<<endl
// 				<<rvec<<endl
// 				<<tvec<<endl;

			//rvecs.push_back(rvec);
			//tvecs.push_back(tvec);
			cout<<"--curFrameID: "<<curImgFrameID<<", "<<dg.GetFrameID()<<endl;
			dt = clock()-currt;
			cout<<"====time consumed: "<<dt<<", fps: "
				<< (dt != 0 ? to_string((long long)1000/dt) : "INF")<<endl;

// #ifdef _WRITE_EXTRINSICS
// 			fst<<iframeStr+to_string(long long(curImgFrameID))<<"{"
// 				<<iframeIdStr<<curImgFrameID
// 				<<iframeRotStr<<rvec
// 				<<iframeTransStr<<tvec
// 				<<"}";
// #endif // _WRITE_EXTRINSICS

			
			//Mat imagePoints;
			vector<Point2f> imagePoints; //用于对比 pt2fVec
			//zcProjectPoints(boardPoints, rvec, tvec, intrinsics, distCoeffs, imagePoints);
			//cout<<"@@@"<<imagePoints[0]<<","<<imagePoints[1]<<endl;
			//cv::projectPoints(boardPoints, rvec, tvec, intrinsics, distCoeffs, imagePoints);
			cv::projectPoints(pt3fDraw, rvec, tvec, intrinsics, distCoeffs, imagePoints);
			//cout<<"$$$"<<imagePoints[0]<<","<<imagePoints[1]<<endl;
			Mat im_draw2 = im_draw.clone();
			cv::drawChessboardCorners(im_draw2, boardSize, imagePoints, foundCorners);
			//cv::line(im_draw2, imagePoints[0], imagePoints[0]+Point2f(111, 0), CV_RGB(255, 0, 0), 2);
			//cv::line(im_draw2, imagePoints[0], imagePoints[ww], CV_RGB(255, 0, 0), 2);
			//cv::line(im_draw2, imagePoints[0], imagePoints[0]+Point2f(0, 111), CV_RGB(0, 255, 0), 2);
			//cv::line(im_draw2, imagePoints[0], imagePoints.back(), CV_RGB(0, 0, 255), 2);
			int ipsz = imagePoints.size();
			Point2f oo = imagePoints[ipsz-4],
				xx = imagePoints[ipsz-3],
				yy = imagePoints[ipsz-2],
				zz = imagePoints[ipsz-1];
			cv::line(im_draw2, oo, xx, CV_RGB(255, 0, 0), 2);
			cv::line(im_draw2, oo, yy, CV_RGB(0, 255, 0), 2);
			cv::line(im_draw2, oo, zz, CV_RGB(0, 0, 255), 2);
			cv::putText(im_draw2, "curImgFrameID: " + std::to_string((long long) curImgFrameID), 
				Point(0, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255));
			imshow("projectPoints", im_draw2);
		}//if(foundCorners)
		else{
			notFoundCnt++;
			cout<<"---------------!foundCorners: "<<curImgFrameID<<", "<<dg.GetFrameID()<<endl;
		}
		rvecs.push_back(rvec);
		tvecs.push_back(tvec);

		Mat rmat;
		cv::Rodrigues(rvec, rmat);
		Mat extrinsics;
		cv::hconcat(rmat, tvec, extrinsics);
		//cout<<rmat<<endl
		//	<<extrinsics<<endl;

#ifdef _WRITE_EXTRINSICS
		fst<<iframeStr+to_string(long long(curImgFrameID))<<"{"
			<<iframeIdStr<<curImgFrameID
			<<iframeRotStr<<rvec
			<<iframeTransStr<<tvec
			<<"}";
		
		//之前上面是写 xml, 后面在 pcl 里不知道怎么用的了 (代码丢了, 改参数 "-cv_extr_hint" 之类那部分), 所以下面改成 6列 (t+R) txt
		foutExtrTxt//<<tvec.at<double>(0, 0)<<endl //√, 但是丑
			<<tvecs.back()[0]<<' '
			<<tvecs.back()[1]<<' '
			<<tvecs.back()[2]<<' '
			<<rvecs.back()[0]<<' '
			<<rvecs.back()[1]<<' '
			<<rvecs.back()[2]<<endl;

		//以及 12列 (t+R) txt：
		//double t0dBuf[3]={1.5, 1.5, -0.3}; //单位: m
		double t0dBuf[3]={1500, 1500, -300}; //单位: mm
		Mat t0d(3, 1, CV_64FC1, t0dBuf); //pcl_kinfu 中默认初始 tvec0, 强调 'd'epth

		static Mat t0 = tvec.clone(); //static 只在第一次初始化
		static Mat R0 = rmat.clone();

		Mat Ri2 = R0 * rmat.t(); //即 Ri2 = R0 * Ri.T
		Mat Ti2 = -R0 * rmat.t() * tvec + t0 + t0d; //Ti2 = -R0 * Ri.T * ti + t0 + t0d
		
		foutExtrTxtKf
			<<Ti2.at<double>(0, 0)<<' '
			<<Ti2.at<double>(1, 0)<<' '
			<<Ti2.at<double>(2, 0)<<' '
			<<Ri2.at<double>(0, 0)<<' '
			<<Ri2.at<double>(0, 1)<<' '
			<<Ri2.at<double>(0, 2)<<' '
			<<Ri2.at<double>(1, 0)<<' '
			<<Ri2.at<double>(1, 1)<<' '
			<<Ri2.at<double>(1, 2)<<' '
			<<Ri2.at<double>(2, 0)<<' '
			<<Ri2.at<double>(2, 1)<<' '
			<<Ri2.at<double>(2, 2)<<endl;

#endif // _WRITE_EXTRINSICS

		cv::drawChessboardCorners(im_draw, boardSize, pt2fVec, foundCorners);
		imshow("color image", im_draw);

		//去畸变测试:	//2016-5-13 16:18:40
		Mat im_undist;
		cv::undistort(im_draw, im_undist, intrinsics, distCoeffs);
		imshow("color-map-undistort", im_undist);

		key = waitKey(paused ? 0 : 1);
	}//while
	cout<<"rvecs.size(): "<<rvecs.size()<<", "<<tvecs.size()<<endl
		<<"max depth & image ID: "<<maxDepFrameID<<", "<<maxImgFrameID<<endl
		<<"foundCnt: "<<foundCnt<<", "<<notFoundCnt<<endl;

	return;
}//main

