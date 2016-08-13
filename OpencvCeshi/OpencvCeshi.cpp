#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/core/eigen.hpp>
#include <ctime>
#include <fstream>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace Eigen;
using namespace cv;
using namespace boost;
namespace fs = boost::filesystem;

fs::path operator+(const fs::path& lhs, const fs::path& rhs){ 
	//return fs::path(lhs) += rhs; 

	return fs::path(lhs.string() + rhs.string());
}


typedef Eigen::Matrix<float, 3, 3, Eigen::RowMajor> Matrix3frm;
//typedef Eigen::Vector3f Vector3f;

//@brief mean(sum(|m1-m2|^2)) 均方误差(不开方), 对应像素都有效才计入
float twoDmatsMSE(Mat m1, Mat m2){
    Mat validMsk = (m1 != 0 & m2 != 0); //m1, m2 都不为零的像素
    int validCnt = countNonZero(validMsk);
    Mat m12diff;
    cv::absdiff(m1, m2, m12diff);
    m12diff.setTo(0, validMsk == 0); //去除无效区域
    Mat m12sqr = m12diff.mul(m12diff);
    float sumOfSqr = cv::sum(m12sqr)[0];
    return sumOfSqr / validCnt;
}//twoDmatsMSE

//@brief testRmseTwoDmats 的单元测试
void testTwoDmatsMSE(){
    //python测试代码:
    //a=ones((5,6))*3
    //b=ones((5,6))*5
    //a[0,:]=0
    //b[:,0]=2
    //msk=(a!=0)&(b!=0)
    //c=(a-b)[msk]
    //sum(square(c))/len(c)==3.5 #True

    int rr = 5,
        cc = 6;
    Mat m1(rr, cc, CV_16UC1, 3);
    Mat m2(rr, cc, CV_16UC1, 5);

    m1.row(0) = 0;
    m2.col(0) = 2;

    CV_Assert(twoDmatsMSE(m1, m2) == 3.5); //已测试 @OpencvCeshi.cpp //2016-6-20 22:11:48
    cout<<"---------------testTwoDmatsMSE OK---------------"<<endl;
}//testTwoDmatsMSE

void main(){
    {
        testTwoDmatsMSE();
        return;
    }

    {
        const char *pose_csv = "D:/Users/zhangxaochen/Desktop/a.csv";
        CvMLData mlData;
        mlData.read_csv(pose_csv);
        Mat csvMat(mlData.get_values());
        cout<<csvMat<<endl;
        return;
    }
	int first[] = {5,10,15,20,20};
	int second[] = {50,40,30,20,10};
	std::vector<int> v(10);                      // 0  0  0  0  0  0  0  0  0  0
	std::vector<int>::iterator vit;

// 	std::sort (first,first+5);     //  5 10 15 20 25
 	std::sort (second,second+5);   // 10 20 30 40 50
	vit = std::set_difference(first, first+5, second, second+5, v.begin());

	//return;

	Mat cmat(2, 4, CV_8UC1, 200);
	cout<<cmat<<cmat.at<char>(0,0)<<endl;

	//return;

	imshow("test putText", cmat);
	waitKey();
	return;

	fs::path test = "test";
	cout<<test + " sth."<<endl;
	return;

	std::ostringstream ss;
	ss << std::setw(5) << std::setfill('0') << 12 ;//<< "\n";
	std::string s2(ss.str());
	std::cout << s2<<endl;

	boost::format fmt("%05d\n");
	fmt % 12;
	std::string s3 = fmt.str();
	std::cout << s3<<endl;
	
	boost::format ttt = fmt % 33;
	cout<<ttt<<endl;
	return;

	for(int i=0; i<3; i++){
		Mat im(100, 200, CV_8UC3, Scalar(222));
		imshow(to_string((long double)i), im);
	}
	waitKey();
	return;

	int aa[3] = {1,2,3}, bb[3] = {4,5,6};
	int (*p)[3] = &aa; // okay, address of a can be taken
	int *q = aa;
	cout<<(&aa)<<endl
		<<p<<endl
		<<aa<<endl
		<<q<<endl;
	return;

	int xx = 257.6;
	char cc = xx;

	cout<<xx<<endl
		<<(int)cc<<endl;
	return;

	char a[20];
	cout<<sizeof a<<", "<<int(a[0])<<", "<<int(a)<<endl;
	char *b = new char[20];
	cout<<sizeof b<<", "<<int(b[0])<<", "<<int(b)<<endl;
	return;


	Mat image = imread("d:/Users/zhangxaochen/Desktop/1.jpg");
	//cvtColor(image, image, CV_BGR2GRAY);

	Mat mask(image.size(), CV_8UC1, Scalar::all(0));
	//Mat mask(3, 5, CV_8UC2, (20));
	//cout<<mask<<endl;
	//return;

	Rect roi(50, 50, 100, 100);
	mask(roi).setTo(Scalar::all(255));

	Mat im2show;
	//bitwise_and(image, mask, im2show);
	//im2show = image * mask;
	image.copyTo(im2show, mask);
	imshow("im2show", im2show);
	waitKey();
	return;

	char buf[100];
	strcpy_s(buf, "test char arr");
	cout<<buf<<", "<<sizeof buf<<endl;

	printf("%04d\n", 98);

	Point2f p1(1,2),
		p2(3,4);
	cout<<p1+p2<<endl;
	//return;

	cout<<string("abc")+std::to_string(long long(111))<<endl;
	cout<<"abc"+("_"+std::to_string(long long(111)))<<endl;
	cv::FileStorage test_fst("shit.xml", FileStorage::WRITE);
	int arr[] = {1,2,3,4,};
	vector<double> vec;
	vec.assign(arr, arr + sizeof arr / sizeof arr[0]);
	//vec.insert(vec.end(), vec.begin(), vec.end());
	vec = std::vector<double>(vec.begin(), vec.end()-2);
	for(int i = 0; i<vec.size(); i++)
		cout<<vec[i]<<",";
	cout<<endl;
	return;

	// 	vector<double> v2(Mat(1, 3, CV_64F, 2));
	// 	vec = Mat(1, 3, CV_64F, 2);
	// 
	// 	vector<vector<double>> vs;
	// 	vs.push_back(vec);
	// 	vs.push_back(vec);
	// 	vs.push_back((Mat(1, 3, CV_64F, 2)));
	// 	//Mat vsmat(2, 4, CV_64F, vs.data());
	// 	//Mat vsmat(vs.size(), vs[0].size(), CV_64F);
	// 	Mat vsmat;
	// 	//cout<<Mat(vec)<<endl;
	// 	for(int i=0; i<vs.size(); i++)
	// 		//vsmat.row(i) = Mat(vs[i]).t();
	// 		vsmat.push_back(Mat(vs[i]));
	// 	vsmat = vsmat.reshape(1, vs.size());
	// 	cout<<vsmat.type()<<vsmat.size()<<endl
	// 		<<vsmat<<endl;
	// 	test_fst<<"shitKey"<<vec;
	// 	test_fst<<"vecs"<<vs;
	// 
	// 	return;

	const char *extrinsic_xml = "E:/GitHub/OpenniCeshi/data/openNI132_data_registration_4x3/test_smt_far_4x3_calib/extrinsics.xml";
	cv::FileStorage fst(extrinsic_xml, FileStorage::READ);
	FileNode fnrt = fst.root();
	cout<<"name:"<<fnrt.name()<<","<<fnrt.type()<<","<<fnrt.size()<<","<<fnrt.isNone()<<endl;

	//---------------
	FileNodeIterator it = fnrt.begin(),
		it_end = fnrt.end();
	Mat rv, tv;
	for(; it != it_end; it++){
		cout<<(*it).name()<<endl;
		(*it)["r"]>>rv;
		(*it)["t"]>>tv;
		Matrix3frm zc_rmat;// = rvec;
		zc_rmat.transpose();
		Vector3f zc_tvec;// = tv;
		Rodrigues(rv, rv);
		cv2eigen(rv, zc_rmat);
		cv2eigen(tv, zc_tvec);
		cout<<"zc_rmat:\n"<<zc_rmat<<endl
			<<"zc_rmat.t:\n"<<zc_rmat.transpose()<<endl
			<<"===\n"<<zc_rmat*zc_rmat.transpose()<<endl
			<<"--"<<tv / 1000<<endl;

	}

	// 	//---------------
	// 	int cnt = 0;
	// 	while(true){
	// 		cv::FileNode fn = fst["image_frame"+to_string(long long(cnt))];
	// 		if(fn.isNone())
	// 			break;
	// 		cout<<fn.name()<<endl;
	// 		cnt++;
	// 	}
	cv::FileNode fn = fst["image_frame"];
	cout<<"fn: "<<fn.type()<<endl;

	int id = -1;
	fst["image_frame"]["id"] >> id;
	cout<<id<<endl;
	FileNode iframesNode = fst["image_frame"];
	//cout<<iframesNode<<endl;
	id = fst["image_frames"];//["aid"];
	cout<<id<<endl;
	return;

	const char *pose_csv = "E:/ABOUT C++/libs/pcl-master-6d0343d1b7/_build/gpu/kinfu/tools/pose_file.csv";
	CvMLData mlData;
	mlData.read_csv(pose_csv);
	Mat csvMat(mlData.get_values());
	cout<<csvMat.type()<<","<<csvMat.depth()<<endl;
	for(int j = 0; j < csvMat.rows; ++j){
		Mat row = csvMat.row(j);
		// 		printf("row %03d: ", y);
		// 		cout<<row<<endl;
		//cout<<row(cv::Rect(0, 0, 3, 1))<<endl;
	}

	return;

	float dat[] = {1,2,3};
	Mat tmp(1, 3, CV_32F, dat);
	cout<<dat<<endl
		<<tmp<<endl;
	cout<<Scalar(3)<<endl;
	return;

	cout<<CV_MAT_CN(0x42424005)<<endl;
	cout<<CV_MAT_CN(0x4242400d)<<endl;
	Mat coord(1, 3, CV_32F, Scalar(3)),
		homo;
	cout<<coord<<endl
		<<coord.channels()<<", "<<coord.size()<<endl;
	cv::convertPointsFromHomogeneous(coord, homo);
	cout<<"homo: "<<coord<<endl
		<<homo<<endl;

	Vec3f v3f(1,2,3);
	cout<<v3f<<endl
		<<Point3f(v3f)<<endl;

	Mat mm(v3f);
	cout<<mm<<endl
		<<mm.channels()<<mm.type()<<mm.depth()<<mm.size()<<endl;
	cout<<Mat(v3f)*mm.t()<<endl;
	return;

	cout<<to_string((long long)1000)<<endl;
	cv::VideoCapture cap(1);
	if(!cap.isOpened())
		return;
	Mat frame;
	cap>>frame;
	imshow("frame", frame);
	waitKey();

	int key = 0;
	cout<<"--key: "<<key<<endl;

	while(true){
		cap>>frame;
		if(!frame.data || key == 27)
			break;
		cv::Canny(frame, frame, 100, 200);
		imshow("frame", frame);
		key = waitKey(1);
	}//while


	//while()
	return;

	Mat m(1, 3, CV_8U, 11);
	cout<<m<<endl;
	cout<<cv::format(m, "NUMPY")<<endl;
	const Formatter *npfmt = cv::Formatter::get("numpy");
	npfmt->write(cout, m);
	ofstream fout("test.csv");
	npfmt->write(fout, m);
	cout<<endl;
	//return;

	const char *fpath = "../../data/Dun9/size_half/pic_0.jpg";
	//Mat img = imread(fpath, CV_LOAD_IMAGE_GRAYSCALE);
	Mat img = imread(fpath);
	Mat pyrimg;
	int ratio = 2;
	cv::pyrDown(img, pyrimg, Size(img.cols/ratio, img.rows/ratio));
	imshow("pyrimg", pyrimg);
	waitKey();

	Mat imggray;
	//img.convertTo(imggray, CV_8UC1); //8uc1 无效， 还是 8uc3
	cvtColor(img, imggray, CV_BGR2GRAY);
	cout<<"imggray.type(): "<<imggray.type()<<", "<<imggray.depth()<<endl;
	imshow(fpath, imggray);
	waitKey();
	//cv::pyrDown(img, img, Size(img.cols/2, img.rows/2));
	Size sz(10, 20);
	cout<<sz*2<<endl;
	//cout<<img.size()/2<<endl;
	int ww = 7, hh = 10;
	cv::Size bsz(ww, hh);
	vector<Point2f> ptvec;
	clock_t begt = clock();
	bool found = cv::findChessboardCorners(img, bsz, ptvec, CALIB_CB_ADAPTIVE_THRESH);
	begt = clock() - begt;
	cout<<"begt: "<<begt<<endl;

	cv::drawChessboardCorners(img, bsz, ptvec, found);
	int half_win_size = 3;//the window size will be 3+1+3=7
	int iteration = 20;  
	double epislon = 0.1;  
	cv::cornerSubPix(imggray, ptvec, 
		Size(half_win_size, half_win_size), 
		Size(-1, -1), 
		TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,iteration,epislon)
		);

	const char *cam_xml = "D:/opencv249/sources/_build/samples/cpp/cam.xml";
	//const char *cam_xml = "E:/GitHub/OpenniCeshi/data/openNI132_data_registration_4x3/test_smt_far_4x3/cam.xml";

	FileStorage fs(cam_xml, FileStorage::READ);
	Mat intrinsics, distortion;
	fs["camera_matrix"] >> intrinsics;
	fs["distortion_coefficients"] >> distortion;
	cout<<intrinsics<<endl
		<<distortion<<endl;

	vector<Point3f> boardPoints;
	//棋盘格一格 50mm
	int length = 1;
	for(int j = 0; j < hh; j++){
		for(int i = 0; i < ww; i++){
			int x = i * length,
				y = j * length,
				z = 0;
			boardPoints.push_back(Point3f(x, y, z));
		}
	}

	vector<double> rvec, tvec;
	solvePnP(boardPoints, ptvec, intrinsics, distortion, rvec, tvec);

	//namedWindow(fpath, WINDOW_NORMAL);
	imshow(fpath, img);
	waitKey();

}//main
