#include "predict.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

#include <dirent.h>

using namespace cv;
using namespace ml;
using namespace std;

void Predict::predict(string filename)
{
	const int image_cols = 20;
	const int image_rows = 40;

	Ptr<ANN_MLP>model = StatModel::load<ANN_MLP>("./ANN_MLP.xml");

	Mat testMat;

	cout << "测试：" << endl;
#if 0
	string dirPath = "./picture";
	string imgPath = dirPath + "/" + filename;
#else
	string imgPath = "./" + filename;
#endif
	cout << "img:" << imgPath << endl;
	testMat = imread(imgPath, IMREAD_GRAYSCALE);
	if (testMat.empty()) {
		cout << "read image failed" << endl;
		exit(1);
	}

	cout << "read img OK" << endl;

	Mat resizeImg;
	Mat threshImg;
	Mat floatImg;

	resize(testMat, resizeImg, Size(image_cols, image_rows),  0,  0, CV_INTER_AREA);//使用象素关系重采样。当图像缩小时候，该方法可以避免波纹出现  
	threshold(resizeImg, threshImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cout << "do....." << endl;
	cout << threshImg << endl;
	threshImg.convertTo(floatImg, CV_32FC1, 1.0/255.0);

	cout << "floatimg" << endl;
	cout << floatImg << endl;

	Mat sampleMat = floatImg.reshape(0, 1);

	cout << "do 2 ..." << endl;
	cout << sampleMat << endl;

	Mat responseMat;
	model->predict(sampleMat, responseMat);
	cout << "do 3" << endl;
	Point maxLoc;
	double maxVal = 0;
	minMaxLoc(responseMat, NULL, &maxVal, NULL, &maxLoc);
	char temp[256];

	if (maxLoc.x <= 9)//0-9  
	{
		sprintf(temp, "%d", maxLoc.x);
		//printf("%d\n", i);  
	}
	else//A-Z  
	{
		sprintf(temp, "%c", maxLoc.x + 55);
		//printf("%c\n", i+55);  
	}

	cout << "识别结果：" << temp << "    相似度:" << maxVal * 100 << "%" << endl;
}
