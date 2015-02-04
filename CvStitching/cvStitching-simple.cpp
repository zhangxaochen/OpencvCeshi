//**************************************************************
// by 张琛， 11321022
// 要求：
//	全景照片拼接系统：至少实现两张照片的拼接
// 	输入：
// 		至少两张同一个场景但只有部分场景重叠的图像。
// 	任务：
// 		1. 利用 OpenCV 的图片拼接模块（ stitching）或其他方法实现一个图片拼接系统。
// 		2. 至少展示 5 个的成功拼接的结果，并指出每次实验中拼接的难点（ 包括有难度的图片进行拼接）。 所有原始照片和拼接结果打包在作业里面上交。
// 		3. 指出实现的系统的短板，即在哪种情况下拼接效果不好并给出相应例子，并给予解释。
// 		4. 若有条件，实现两个以上方法，并进行效果比较
// 	输出要求：
// 		1. 自动进行拼接过程，并将拼接结果显示出来。
// 		2. 统计各个步骤所花费时间，图片大小等信息。
// 		3. 能够进行交互，指定输入的图片，并能对输出结果进行保存。
// 		4. 系统能显示一些中间步骤，包括：计算特征点时间，拼接时间，检测到的特征，拼接结果中属于各自原来图片的部分等。
// 		5. 有基本的交互界面。
// 	
// 	提交截至时间：2015 年 2 月 5 日晚上 12 点
//**************************************************************


#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"

using namespace std;
using namespace cv;

bool try_use_gpu = false;
vector<Mat> imgs;
string result_name = "result.jpg";

void printUsage();
int parseCmdArgs(int argc, char** argv);

int main(int argc, char* argv[])
{
	int retval = parseCmdArgs(argc, argv);
	if (retval) return -1;

	Mat pano;
	Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
	Stitcher::Status status = stitcher.stitch(imgs, pano);

	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
		return -1;
	}

	imwrite(result_name, pano);
	return 0;
}


void printUsage()
{
	cout <<
		"Rotation model images stitcher.\n\n"
		"stitching img1 img2 [...imgN]\n\n"
		"Flags:\n"
		"  --try_use_gpu (yes|no)\n"
		"      Try to use GPU. The default value is 'no'. All default values\n"
		"      are for CPU mode.\n"
		"  --output <result_img>\n"
		"      The default is 'result.jpg'.\n";
}


int parseCmdArgs(int argc, char** argv)
{
	if (argc == 1)
	{
		printUsage();
		return -1;
	}
	for (int i = 1; i < argc; ++i)
	{
		if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
		{
			printUsage();
			return -1;
		}
		else if (string(argv[i]) == "--try_use_gpu")
		{
			if (string(argv[i + 1]) == "no")
				try_use_gpu = false;
			else if (string(argv[i + 1]) == "yes")
				try_use_gpu = true;
			else
			{
				cout << "Bad --try_use_gpu flag value\n";
				return -1;
			}
			i++;
		}
		else if (string(argv[i]) == "--output")
		{
			result_name = argv[i + 1];
			i++;
		}
		else
		{
			Mat img = imread(argv[i]);
			if (img.empty())
			{
				cout << "Can't read image '" << argv[i] << "'\n";
				return -1;
			}
			imgs.push_back(img);
		}
	}
	return 0;
}
