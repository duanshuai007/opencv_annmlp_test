#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "image.hpp"
#include "train.hpp"
#include "predict.hpp"

using namespace std;

int main(int argc, char *argv[]) 
{
	int threshvalue;
	int option;

	if (argc < 2) {
		cout << "paramters error" << endl;
		return -1;
	}

	option = atoi(argv[1]);

	switch(option) {
		case 0:
			if (argc < 3) {
				cout << "please input thresh value" << endl;
				cout << "example:./main 0 [value]" << endl;
				return -1;
			}
			threshvalue = atoi(argv[2]);
			Image image;
			image.image_process(threshvalue);
			break;
		case 1:
			Train train;
			train.train();
			cout << "train complete" << endl;
			break;
		case 2:
			Predict pre;
			pre.predict(argv[2]);
			cout << "predict complete" << endl;
			break;
		default:
			cout << "switch value error" << endl;
			break;
	}

	return 0;
}
