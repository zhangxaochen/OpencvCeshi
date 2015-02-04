//**************************************************************
// by ��衣� 11321022
// Ҫ��
//	ȫ����Ƭƴ��ϵͳ������ʵ��������Ƭ��ƴ��
// 	���룺
// 		��������ͬһ��������ֻ�в��ֳ����ص���ͼ��
// 	����
// 		1. ���� OpenCV ��ͼƬƴ��ģ�飨 stitching������������ʵ��һ��ͼƬƴ��ϵͳ��
// 		2. ����չʾ 5 ���ĳɹ�ƴ�ӵĽ������ָ��ÿ��ʵ����ƴ�ӵ��ѵ㣨 �������Ѷȵ�ͼƬ����ƴ�ӣ��� ����ԭʼ��Ƭ��ƴ�ӽ���������ҵ�����Ͻ���
// 		3. ָ��ʵ�ֵ�ϵͳ�Ķ̰壬�������������ƴ��Ч�����ò�������Ӧ���ӣ���������͡�
// 		4. ����������ʵ���������Ϸ�����������Ч���Ƚ�
// 	���Ҫ��
// 		1. �Զ�����ƴ�ӹ��̣�����ƴ�ӽ����ʾ������
// 		2. ͳ�Ƹ�������������ʱ�䣬ͼƬ��С����Ϣ��
// 		3. �ܹ����н�����ָ�������ͼƬ�����ܶ����������б��档
// 		4. ϵͳ����ʾһЩ�м䲽�裬����������������ʱ�䣬ƴ��ʱ�䣬��⵽��������ƴ�ӽ�������ڸ���ԭ��ͼƬ�Ĳ��ֵȡ�
// 		5. �л����Ľ������档
// 	
// 	�ύ����ʱ�䣺2015 �� 2 �� 5 ������ 12 ��
//**************************************************************

#include <iostream>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace cv;
using namespace boost;
namespace fs = boost::filesystem;

const int win_flag = WINDOW_NORMAL;
string wn1 = "img1", wn2 = "img2", wn_res = "result",
	wn_matches = "matches";
bool try_use_gpu = false;
vector<Mat> imgs;
vector<string> imgFnames;
string resFname = "result.jpg";

void printHelp(int argc, char** argv);
int parseCmdArgs(int argc, char** argv);
int stitchPairwise();
int stitchAll();

int main(int argc, char** argv){
	int retval = parseCmdArgs(argc, argv);
	if(retval)
		return -1;

	if(imgFnames.size() == 2){
		return stitchPairwise();
	}
	else if(imgFnames.size() == 1){ //��ʱ imgFnames[0] Ӧ����ͼƬ����Ŀ¼��
		string dirIn = argv[1];
		if(!fs::is_directory(dirIn)){
			cout<<"ERROR: your input <imageFolder> doesn't exist!"<<endl;
			return -1;
		}
		fs::directory_iterator it(dirIn),
			it_end;
		while(it != it_end){
			fs::path ext = it->path().extension();
			//cout<<ext<<endl;
			if(fs::is_regular_file(*it) && 
				(ext == ".png" || ext == ".jpg" || ext == ".bmp"))
			{
				string fname = it->path().string();
				Mat img = imread(fname);
				if (img.empty())
				{
					cout << "Can't read image '" << fname << "'\n";
					return -1;
				}
				imgs.push_back(img);
			}
			it++;
		}//while
		return stitchAll();
	}
	else{
		cout<<"ERROR: image arguments passed should be \"<img1> <img2>\", or \"<imageFolder>\"!!"<<endl;
		printHelp(argc, argv);
		return -1;
	}

	return 0;
}//main

/** @function printHelp */
void printHelp(int argc, char** argv){
	cout <<"Usage:\n"
		<<argv[0]<< " <img1> <img2>" <<endl
		<<"\tor\n"
		<<argv[0]<<" <imageFolder>"<<endl;
}//printHelp

int parseCmdArgs(int argc, char** argv){
	if (argc == 1)
	{
		printHelp(argc, argv);
		return -1;
	}
	for (int i = 1; i < argc; ++i)
	{
		if (string(argv[i]) == "--help" || string(argv[i]) == "/?")
		{
			printHelp(argc, argv);
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
			resFname = argv[i + 1];
			i++;
		}
		else
		{
// 			Mat img = imread(argv[i]);
// 			if (img.empty())
// 			{
// 				cout << "Can't read image '" << argv[i] << "'\n";
// 				return -1;
// 			}
			imgFnames.push_back(argv[i]);
		}
	}
	return 0;
}//parseCmdArgs

int stitchPairwise(){
	// Load the images
	Mat image1= imread(imgFnames[0]);
	Mat image2= imread(imgFnames[1]);
	// 	Mat image1= imread(argv[1], 0);
	// 	Mat image2= imread(argv[2], 0);

	namedWindow(wn1, win_flag);
	namedWindow(wn2, win_flag);
	imshow(wn1, image1);
	imshow(wn2, image2);
	waitKey();

	Mat gray_image1;
	Mat gray_image2;
	// Convert to Grayscale
	cvtColor(image1, gray_image1, CV_RGB2GRAY);
	cvtColor(image2, gray_image2, CV_RGB2GRAY);

	// 	imshow(wn1,image2);
	// 	imshow(wn2,image1);

	if(!gray_image1.data || !gray_image2.data){ 
		cout<<" --(!) Error reading images " <<endl; 
		return -1; 
	}

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	//SurfFeatureDetector detector(minHessian);
	SiftFeatureDetector detector;
	vector<KeyPoint> kp1, kp2;

	clock_t begt;
	begt = clock();
	detector.detect(gray_image1, kp1);
	detector.detect(gray_image2, kp2);
	cout<<"detector.detect: "<<(clock()-begt)*1.0/CLOCKS_PER_SEC<<endl;

	// 	detector.detect(image1, keypoints_object);
	// 	detector.detect(image2, keypoints_scene);

	//���� KeyPoints:
	Mat kpImg1, kpImg2;
	drawKeypoints(image1, kp1, kpImg1, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	drawKeypoints(image2, kp2, kpImg2, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow(wn1, kpImg1);
	imshow(wn2, kpImg2);
	waitKey();

	//-- Step 2: Calculate descriptors (feature vectors)
	//SurfDescriptorExtractor extractor;
	SiftDescriptorExtractor extractor;

	Mat descriptors1, descriptors2;

	begt = clock();
	extractor.compute(gray_image1, kp1, descriptors1);
	extractor.compute(gray_image2, kp2, descriptors2);
	// 	extractor.compute(image1, keypoints_object, descriptors_object);
	// 	extractor.compute(image2, keypoints_scene, descriptors_scene);
	cout<<"extractor.compute: "<<(clock()-begt)*1.0/CLOCKS_PER_SEC<<endl;
	

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	//FlannBasedMatcher matcher;
	BFMatcher matcher;

	begt = clock();
	vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);
	cout<<"matcher.match: "<<(clock()-begt)*1.0/CLOCKS_PER_SEC<<endl;

	//���� matches��
	Mat outImg;
	drawMatches(image1, kp1, image2, kp2, matches, outImg);
	//drawMatches(image2, kp2, image1, kp1, matches, outImg);
	namedWindow(wn_matches, win_flag);
	imshow(wn_matches, outImg);
	waitKey();


	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for(int i = 0; i <descriptors1.rows; i++)
	{ 
		double dist = matches[i].distance;
		if(dist <min_dist) min_dist = dist;
		if(dist> max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

	//-- Use only "good" matches (i.e. whose distance is less than 3*min_dist)
	vector<DMatch> good_matches;

	begt = clock();
	for(int i = 0; i <descriptors1.rows; i++)
	{ 
		//if(matches[i].distance <3*min_dist)
		if(matches[i].distance < 3*min_dist)
			good_matches.push_back(matches[i]);
	}
	vector<Point2f> obj;
	vector<Point2f> scene;

	for(int i = 0; i <good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		scene.push_back(kp1[ good_matches[i].queryIdx ].pt);
		obj.push_back(kp2[ good_matches[i].trainIdx ].pt);
	}
	cout<<"find good_matches: "<<(clock()-begt)*1.0/CLOCKS_PER_SEC<<endl;

	// Find the Homography Matrix
	Mat H = findHomography(obj, scene, CV_RANSAC);
	// Use the Homography Matrix to warp the images
	cv::Mat result(image1.rows, image1.cols + image2.cols, CV_8UC3);
	//image1.copyTo(result); //��Ч 

	warpPerspective(image2,result,H,cv::Size(image1.cols+image2.cols,image2.rows));
	namedWindow(wn_res, 0);
	imshow(wn_res, result);
	waitKey();

	//cv::Mat half(result,cv::Rect(0,0,image1.cols,image1.rows));
	Mat half = result(Rect(0,0,image1.cols,image1.rows));
	image1.copyTo(half);
	namedWindow(wn_res, win_flag);
	imshow(wn_res, result);
	waitKey(0);

	//reduce ȥ���Ҳ��ɫ��Ч����
	Mat res_refined,
		max_elem_row;

	Mat res_gray;
	cvtColor(result, res_gray, CV_RGB2GRAY);
	reduce(res_gray, max_elem_row, 0, CV_REDUCE_MAX);
	cout<<"max_elem_row: "<<max_elem_row.channels()<<", "<<max_elem_row.depth()<<endl;
	int i = 0;
	for(; i<max_elem_row.cols; i++){
		//cout<<max_elem_row.at<uchar>(0, i)<<", ";
		if(max_elem_row.at<uchar>(0, i) == 0)
			break;
	}
	res_refined = result(Rect(0,0,i,image1.rows));
	imshow(wn_res, res_refined);
	waitKey();

	imwrite(resFname, res_refined);

	return 0;
}//stitchPairwise

int stitchAll(){
	Mat pano;
	Stitcher stitcher = Stitcher::createDefault(try_use_gpu);
	Stitcher::Status status = stitcher.stitch(imgs, pano);

	if (status != Stitcher::OK)
	{
		cout << "Can't stitch images, error code = " << int(status) << endl;
		return -1;
	}

	namedWindow(wn_res, win_flag);
	imshow(wn_res, pano);
	waitKey();
	imwrite(resFname, pano);

	return 0;
}//stitchAll
