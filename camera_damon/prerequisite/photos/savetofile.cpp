#include <iostream>
#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	if (argc != 3){
		cout << "You must provide 2 parameters" << endl;
		return 1;
	}
	
	string image_name = string(argv[2]);
	string fn_haar = string(argv[1]);
	int im_width = 92;
	int im_height = 112;
	
	VideoCapture cap(0);
	if (!cap.isOpened()){
		cout << "Cannot open camera" << endl;
		return 1;
	}
	
	Mat frame;
	
	for (;;){
		cap >> frame;
		Mat orginal = frame.clone();
		Mat gray;
		cvtColor(orginal, gray, CV_BGR2GRAY);
		vector< Rect_<int> > faces;
		
		CascadeClassifier haar_cascade;
		haar_cascade.load(fn_haar);
		haar_cascade.detectMultiScale(gray, faces);
		
		cout << "No. of faces: " << faces.size() << endl;
		for (int i = 0; i < faces.size(); i++){\
			Rect face_i = faces[i];
			Mat face = gray(face_i);
			
			Mat face_resized;
			cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
		}
		
		imshow("test", orginal);
	}
}