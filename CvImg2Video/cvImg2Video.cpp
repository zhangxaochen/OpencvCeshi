//**************************************************************
// by ��衣� 11321022
// Ҫ��
// ѧϰʹ��OpenCV���������������Ų�ɫͼ����OpenCV����һ����Ƶ���������¹���: 
// 1. �������һ���ٶȲ����⼸��ͼƬ��
// 	2. ֧�ֲ�ͬ��СͼƬ�����룻
// 	3. ��һ֡���ŵ����Լ�ͨ����̲����ĺ��Լ�ѧ������������Ϣ����Ļ�������Ű��Լ���ƣ�
// 	4. ͼƬ��ͼƬ�Ĺ��ȿ����Լ���һЩЧ���� 
// 	5. �������µ������и�ʽ��xxx.exe ����ͼ��Ŀ¼  һ��ͼ��ͣ�������� �����Ƶ�ļ��������� MyMakeVideo.exe input\  1 output.avi ��
// 
// 	�ύ����ʱ�䣺2015��1��2��
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

