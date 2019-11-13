#include "train.hpp"
#include <image.hpp>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <chrono>
#include <memory>
#include <iomanip>
#include <climits>

#include <dirent.h>
#include <unistd.h>

using namespace cv;
using namespace ml;
using namespace std;

const int IMAGE_CLASS_SUM = 10;
const int IMAGE_NUM_PERCLASS = 10;

void print(Mat& mat, int prec)
{
	for (int i = 0; i<mat.size().height; i++)
	{
		cout << "[ ";
		for (int j = 0; j<mat.size().width; j++)
		{
			cout << fixed << setw(2) << setprecision(prec) << mat.at<float>(i, j);
			if (j != mat.size().width - 1) {
				cout << ", ";
			} else {
				cout << " ]" << endl;
			}
		}
	}
}

class Sign {
	public:
		Mat image;
		int number;

		Sign(Mat& image, string name) :image(image) {
			string fname = name.substr(name.rfind('/') - 1, 1);
			cout << "name:" << name << ",fname:" << fname << endl;
			number = stoi(fname);
		};
};


bool loadImage(string imagePath, Mat& outputImage, int u32Width, int u32Height)
{
	// load image in grayscale
	cout << "loadImage:imagePath=" << imagePath << endl;
	Mat image = imread(imagePath, IMREAD_GRAYSCALE);
	Mat resizeimg;
	Mat threshimg;

	// check for invalid input
	if (image.empty()) {
		cout << "Could not open or find the image" << std::endl;
		return false;
	}

	// resize the image
	Size size(u32Width, u32Height);
	resize(image, resizeimg, size, 0, 0, CV_INTER_AREA);

	// convert to float 1-channel
	threshold(resizeimg, threshimg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	threshimg.convertTo(outputImage, CV_32FC1, 1.0/255.0);

	return true;
}

vector<Sign> loadSignsFromFolder(string DirName, int u32ClassNum, int u32NumPerClass, int u32Width, int u32Height) {
	vector<Sign> roadSigns;
	DIR *pDir;
	struct dirent *pDirent;
	int count, class_count;
	class_count = 0;
	int i;
	char sImagePath[128];
	
	for (i = 0; i < u32ClassNum; i++) {
		memset(sImagePath, 0, sizeof(sImagePath));
		snprintf(sImagePath, sizeof(sImagePath), "%s/%d", DirName.c_str(), i);
		pDir = opendir(sImagePath);
		if (pDir == NULL) {
			cout << "open dir[" << sImagePath << "] failed" << endl;
			exit(1);
		}

		count = 0;
		while((pDirent = readdir(pDir)) != NULL) {
			if ((strncmp(pDirent->d_name, ".", strlen(".")) == 0) || (strncmp(pDirent->d_name, "..", strlen("..")) == 0)) {
				cout << "dir:" << pDirent->d_name << endl;
				continue;
			}

			char sFileName[256];
			string filename;
			Mat image;

			memset(sFileName, 0, sizeof(sFileName));
			snprintf(sFileName, sizeof(sFileName), "%s/%s", sImagePath, pDirent->d_name);

			filename = sFileName;
			loadImage(filename, image, u32Width, u32Height);
			//cout << "LoadImage OK" << endl;
			roadSigns.emplace_back(image, filename.substr(0, filename.length() - 4));	
			//cout << "roadSigns.emplace_back OK" << endl;
			count++;
			if (count >= u32NumPerClass) {
				break;
			}
		}
		closedir(pDir);
	}

	return roadSigns;
}

Mat getInputDataFromSignsVector(vector<Sign> roadSigns) {
	Mat roadSignsImageData;

	for (Sign sign : roadSigns) {
		//reshape(int cn, int rows) 
		//cn表示通道数，如果为0，表示保持通道数不变，否则变为设置的通道数
		//rows表示矩阵行数，如果为0，表示保持原有行数不变，否则变为设置的行数。
		Mat signImageDataInOneRow = sign.image.reshape(0, 1);
		roadSignsImageData.push_back(signImageDataInOneRow);
	}

	return roadSignsImageData;
}

#if 1
Mat getOutputDataFromSignsVector(vector<Sign> roadSigns, int u32ClassNum, int u32NumPerClass) {
	
	int i, j, k;
	Mat roadSignsData(0, u32ClassNum, CV_32FC1);
	
	for (i = 0; i < u32ClassNum; i++) {
		for (j = 0; j < u32NumPerClass; j++) {
			vector<float> classVector(u32ClassNum + 1);
			fill(classVector.begin(), classVector.end(), -0.0);
			for (k = 0; k < u32ClassNum; k++) {
				if (k == i) {
					classVector[i + 1] = 1.0;
				}
			}
			Mat tempMatrix(classVector, false);
			roadSignsData.push_back(tempMatrix.reshape(0, 1));
		}
	}
	
	//cout << "response:\n" << roadSignsData << endl;
	return roadSignsData;
}
#else
Mat getOutputDataFromSignsVector(vector<Sign> roadSigns, int u32ClassNum, int u32NumPerClass) {
	int signsCount = (int) roadSigns.size();
	int signsVectorSize = signsCount + 1;

	Mat roadSignsData(0, signsVectorSize, CV_32FC1);

	int i = 1;
	int count = 0;
	for (Sign sign : roadSigns) {
		vector<float> outputTraningVector(signsVectorSize);
		fill(outputTraningVector.begin(), outputTraningVector.end(), -1.0);
		count++;
		if (count ==  u32NumPerClass) {
			count = 0;
			i++;	
		}
		outputTraningVector[i] = 1.0;

		Mat tempMatrix(outputTraningVector, false);
		roadSignsData.push_back(tempMatrix.reshape(0, 1));
	}

	cout << "response:\n" << roadSignsData << endl;

	return roadSignsData;
}
#endif

void Train::train(void)
{
	//char targetdir[] = "/home/swann/opencv/picture";
	char targetdir[128];

	memset(targetdir, 0, sizeof(targetdir));
	int rslt = readlink("/proc/self/cwd", targetdir, sizeof(targetdir) - 1); 
	if (rslt < 0 || (rslt >= sizeof(targetdir) - 1)) 
	{   
		cout << "readlink error" << endl;
		exit(1);
	}   
	targetdir[rslt] = '\0';
	printf("readlink:%s\n", targetdir);

	strncat(targetdir, "/picture", strlen("/picture"));

#if 1
	const int classSum = 10;
	const int imageSum = 20;
	//20x40
	const int imgRows = 40;
	const int imgCols = 20;
#endif
	const int hiddenLayerSize = 100;

	vector<Sign> roadSigns = loadSignsFromFolder(targetdir, classSum, imageSum, imgCols, imgRows);
	cout << "loadSignsFromFolder OK " << endl;
#if 0
	for (Sign s : roadSigns) {
		cout << "------num:" << s.number << " --------" << endl;
		cout << s.image << endl;
	}
	cout << "-----+++++++++++++++++++++++++++---------" << endl;
#endif
	Mat inputTrainingData = getInputDataFromSignsVector(roadSigns);
	Mat outputTrainingData = getOutputDataFromSignsVector(roadSigns, classSum, imageSum);

	Ptr<ANN_MLP> mlp = ANN_MLP::create();
	
	Mat layersSize = Mat(3, 1, CV_16U);
	layersSize.row(0) = Scalar(inputTrainingData.cols);
	layersSize.row(1) = Scalar(hiddenLayerSize);
	layersSize.row(2) = Scalar(outputTrainingData.cols);
	mlp->setLayerSizes(layersSize);

	cout << "cols=============" << outputTrainingData.cols << endl;

	mlp->setActivationFunction(ANN_MLP::ActivationFunctions::SIGMOID_SYM, 1.0, 1.0);

	mlp->setTrainMethod(ANN_MLP::TrainingMethods::BACKPROP, 0.0001);

	TermCriteria termCrit = TermCriteria(
			TermCriteria::Type::MAX_ITER //| TermCriteria::Type::EPS,
			,100 //(int) INT_MAX
			,0.000001
			);
	mlp->setTermCriteria(termCrit);

	Ptr<TrainData> trainingData = TrainData::create( inputTrainingData, SampleTypes::ROW_SAMPLE, outputTrainingData);

	mlp->train( trainingData
			//, //ANN_MLP::TrainFlags::UPDATE_WEIGHTS
			, ANN_MLP::TrainFlags::NO_INPUT_SCALE
			+ ANN_MLP::TrainFlags::NO_OUTPUT_SCALE
			);

	mlp->save("./ANN_MLP.xml");

	cout << "***************Start Predict****************" << endl;

	for (int i = 0; i < inputTrainingData.rows; i++) {
		Mat result;
		//cout << "image[" << i << "]:" << roadSigns[i].image << endl;
		//mlp->predict(inputTrainingData.row(i), result);
		mlp->predict(roadSigns[i].image.reshape(0, 1), result);
		
		double maxVal = 0;
		Point maxLoc;
		minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
		//cout << result << endl;
		cout << "image number = " << roadSigns[i].number << " ,test result:" << maxLoc.x << " ,zhixindu:" << maxVal * 100 << "%" << endl;
	}
}
