//**************************************************************
// by 张琛， 11321022
// 要求：
// 学习使用OpenCV，对输入至少五张彩色图像，用OpenCV制作一个视频，满足以下功能: 
// 1. 按输入的一个速度播放这几张图片；
// 	2. 支持不同大小图片的输入；
// 	3. 第一帧播放的是自己通过编程产生的含自己学号与姓名等信息的字幕，具体排版自己设计；
// 	4. 图片与图片的过度可以自己加一些效果； 
// 	5. 做成如下的命令行格式，xxx.exe 输入图像目录  一幅图像停留的秒数 输出视频文件名（例如 MyMakeVideo.exe input\  1 output.avi ）
// 
// 	提交截至时间：2015年1月2日
//**************************************************************

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>

//#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/eigen.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;
namespace fs = boost::filesystem;

void showUsageInfo(int argc, char* argv[]){
	cout<<"USAGE:"<<endl
		<<"\t"<<argv[0]<<" <ImageFolder> <Interval> <OutFileName>\n"
		<<"\tImageFolder(a string):\t folder which contains *.jpg, *.png, *.bmp\n"
		<<"\tInterval(an int):\t the frame interval (in seconds) of the output video file\n"
		<<"\tOutFileName(a string):\t the output video file name\n";
}//showUsageInfo

Mat drawCoverFrame(Size frameSize){
	int hh = frameSize.height,
		ww = frameSize.width;

	//cover matrix
	Mat cmat(hh, ww, CV_8UC3, Scalar(0));
	Point cpt(ww/2, hh/2);
	string nameText = "Name: ZHANG Chen",
		idText = "ID: 11321022";
	
	int fontFace = CV_FONT_HERSHEY_COMPLEX;
	double fontScale = 0.7;
	int thickness = 1;
	Size ntSize = getTextSize(nameText, fontFace, fontScale, thickness, 0),
		itSize = getTextSize(idText, fontFace, fontScale, thickness, 0);
	Point ntOrig((ww-ntSize.width)/2, (hh-ntSize.height)/2-10),
		itOrig((ww-itSize.width)/2, (hh+itSize.height)/2);
	putText(cmat, nameText, ntOrig, fontFace, fontScale, Scalar(255, 255, 255), thickness);
	putText(cmat, idText, itOrig, fontFace, fontScale, Scalar(255, 255, 255), thickness);
	return cmat;
}//drawCoverFrame

int main(int argc, char* argv[]){
	if(argc != 4){
		showUsageInfo(argc, argv);
		return -1;
	}
	string dirIn = argv[1];
	double interval = stod(argv[2]);
	double fps = 1. / interval;
	string fileOut = argv[3];
	cout<<dirIn<<", "<<interval<<", "<<fps<<", "<<fileOut<<endl;
	if(!fs::is_directory(dirIn)){
		cout<<"ERROR: your input <ImageFolder> doesn't exist!"<<endl;
		return -1;
	}
// 	fs::path boost_dirIn = dirIn;
// 	cout<<boost_dirIn<<endl
// 		<<fs::current_path()<<endl
// 		<<"is_directory(boost_dirIn): "<<fs::is_directory(boost_dirIn)<<endl;

	//VideoWriter vwtr(fileOut, CV_FOURCC_DEFAULT, 1./interval, true);
	VideoWriter vwtr;
	bool vwriterIsOpened = false;
	
	fs::directory_iterator it(dirIn),
		it_end;
	while(it != it_end){
		fs::path ext = it->path().extension();
		//cout<<ext<<endl;
		if(fs::is_regular_file(*it) && 
			(ext == ".png" || ext == ".jpg" || ext == ".bmp"))
		{
			cout<<it->path().filename()<<endl;
			Mat img = imread(it->path().string());
			if(!vwriterIsOpened){
				//CV_FOURCC_DEFAULT, CV_FOURCC('P','I','M','1'), CV_FOURCC_PROMPT, 'F','L','V','1'
				cout<<"img.size(): "<<img.size()<<endl;
				vwtr.open(fileOut, CV_FOURCC('F','L','V','1'), fps, img.size(), true);
				vwriterIsOpened = true;
			
				Mat cmat = drawCoverFrame(img.size());
				vwtr.write(cmat);
#ifdef _DEBUG
				imshow("cover", cmat);
				waitKey();
#endif // _DEBUG

			}
			vwtr.write(img);
#ifdef _DEBUG
			imshow(argv[0], img);
			waitKey(interval*1000);
#endif // _DEBUG
		}
		it++;
	}//while
	vwtr.release();

}//main

