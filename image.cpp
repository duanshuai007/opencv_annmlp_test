#include "image.hpp"
#include <iostream>
#include <string>
#include <dirent.h>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int Image::image_process(int threshvalue)
{
	string path="/home/swann/opencv/picture/num_8.png";
	const char dirname[] = "/home/swann/opencv/picture";
	char targetdir[] = "/home/swann/opencv/target";
	char picName[128];
	char picAllPathName[128];
	char picNewName[128];
	DIR *pDir;
	struct dirent *pDirent;
	Mat SrcImg, InputImg, ThreshImg;
	
	pDir = opendir(dirname);
	if (pDir == NULL) {
		cout << "open dir failed" << endl;
		return -1;
	}

	while (( pDirent = readdir(pDir)) != NULL) {
		//cout << "read dir" << endl;
		if ((strncmp(pDirent->d_name, ".", strlen(".")) == 0) || (strncmp(pDirent->d_name, "..", strlen("..")) == 0)) {
			continue;
		}
		//cout << "name:" << pDirent->d_name << endl;

		memset(picAllPathName, 0, sizeof(picAllPathName));
		snprintf(picAllPathName, sizeof(picAllPathName), "%s/%s", dirname, pDirent->d_name);
		
		SrcImg = imread(picAllPathName,  IMREAD_GRAYSCALE);
		if (SrcImg.empty()) {
			cout << "read image empty" << endl;
			continue;
		}

		memset(picName, 0, sizeof(picName));
		strncpy(picName, pDirent->d_name, strlen(pDirent->d_name) - 4);

		resize(SrcImg, InputImg, Size(20, 40), INTER_AREA);
#if 0
		memset(picNewName,  0, sizeof(picNewName));
		snprintf(picNewName , sizeof(picNewName), "%s/%s_%s.jpg", targetdir, picName, "resize");
		cout << "resize:" << picNewName<< endl;
		imwrite(picNewName, InputImg);
#endif
		cv::threshold(InputImg, ThreshImg, threshvalue, 255, cv::THRESH_BINARY_INV);
		memset(picNewName, 0, sizeof(picNewName));
		snprintf(picNewName, sizeof(picNewName), "%s/%s.jpg", targetdir, picName);
		cout << "thresh:" << picNewName << endl;
		cout << ThreshImg << endl;
		imwrite(picNewName, ThreshImg);

		cout << "row=" << ThreshImg.rows << " col=" << ThreshImg.cols << endl;
	}

	closedir(pDir);

	return 0;
}
