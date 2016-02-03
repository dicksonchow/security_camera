#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <stdlib.h>
#include <wiringPi.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <mysql++.h>
#include "userclass.h"

#define LED_PIN 0
#define MOTION_PIN 2

using namespace cv;
using namespace std;
using namespace mysqlpp;

StoreQueryResult execute_select_query(string SELECT_STRING)
{
	try {
                Connection conn(false);
                conn.connect("secamadm", "localhost", "secamadm", "p@ssw0rd");
                Query query = conn.query();

                query << SELECT_STRING; 
                return query.store();
        }
        catch (BadQuery e){
                cerr << "Error: " << e.what() << endl;
                exit(1);
        }
        catch (const BadConversion& e){
                cerr << "Conversion error: " << e.what() << endl <<
                        "\tretrieved data size: " << e.retrieved <<
                        ", actual size: " << e.actual_size << endl;
                exit(1);
        }
        catch (const mysqlpp::Exception& e){
                cerr << "Error: " << e.what() << endl;
                exit(1);
        }
}

void get_labels_and_images(vector<Mat>& images, vector<int>& labels)
{
	StoreQueryResult results = execute_select_query("SELECT user.id, data.pic_path FROM secamuser_tb user INNER JOIN secamdata_tb data ON user.id = data.user_id ORDER BY user.id;");

	for (size_t i = 0; i < results.num_rows(); i++){
		images.push_back(imread(results[i]["pic_path"].c_str(), 0));
		labels.push_back(atoi(results[i]["id"].c_str()));
	}
}

User get_user_by_id(int id)
{
	StoreQueryResult result = execute_select_query("SELECT id, name, register FROM secamuser_tb WHERE id = " + to_string(id) + ";");
	User user(atoi(result[0]["id"].c_str()), result[0]["name"].c_str(), !!atoi(result[0]["register"].c_str()));
	return user;
}

void cleanup(int sig)
{
        cout << "Detect Interrupt Signal." << endl;
        pinMode(0, OUTPUT);
        digitalWrite(0, HIGH);
        exit(0);
}

int catch_signal(int sig, void (*handler)(int))
{
        struct sigaction action;
        action.sa_handler = handler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        return sigaction (sig, &action, NULL);
}


int main(int argc, const char *argv[]) {
	if (catch_signal(SIGINT, cleanup) == -1){
                cerr << "Failed to map the handler" << endl;
                exit(2);
        }
	// Get the path to your CSV:
	string fn_haar = "/usr/share/faces_meta/haarcascade_frontalface_alt.xml";
	int deviceId = 0;
	// These vectors hold the images and corresponding labels:
	vector<Mat> images;
	vector<int> labels;
	// Read in the data (fails if no valid input filename is given, but you'll get an error message):
	get_labels_and_images(images, labels);
	
	int im_width = images[0].cols;
	int im_height = images[0].rows;
	cout << "Welcome to face recognition program\nStart training..." << endl;
	// Create a FaceRecognizer and train it on the given images:
	Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
	model->train(images, labels);

	cout << "Finishing training and start recognition" << endl;
	CascadeClassifier haar_cascade;
	haar_cascade.load(fn_haar);
	// Get a handle to the Video device:
	VideoCapture cap(deviceId);
	// Check if we can use this device at all:
	if (!cap.isOpened()) {
		cerr << "Capture Device ID " << deviceId << " cannot be opened." << endl;
		return -1;
	}

	if (wiringPiSetup() == -1){
		cerr << "Failed to setup wiringPi..." << endl;
		return -1;
	}
	pinMode(LED_PIN, OUTPUT);
	// Holds the current frame from the Video device:
	Mat frame;
	Mat background = imread("/usr/share/faces_meta/door_locked.jpg", CV_LOAD_IMAGE_COLOR);

	for(;;)
	{
		cap >> frame;
		// Clone the current frame:
		Mat original = frame.clone();
		// Convert the current frame to grayscale:
		Mat gray;
		cvtColor(original, gray, CV_BGR2GRAY);
		// Find the faces in the frame:
		vector< Rect_<int> > faces;
		haar_cascade.detectMultiScale(gray, faces);

		for(int i = 0; i < faces.size(); i++) {
			// Process face by face:
			Rect face_i = faces[i];
			// Crop the face from the image. So simple with OpenCV C++:
			Mat face = gray(face_i);
			double confidence = 0.0;

			Mat face_resized;
			cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
			// Now perform the prediction, see how easy that is:
			int prediction = model->predict(face_resized);
			model->predict(face_resized, prediction, confidence);

			cout << "Predicted label: " << prediction << " confidence: " << confidence << endl;

			User user = get_user_by_id(prediction);
			rectangle(original, face_i, CV_RGB(0, 255, 0), 1);
			string box_text = format("Prediction = %s, Confindence = %f", user.get_user_name(), confidence);

			int pos_x = std::max(face_i.tl().x - 10, 0);
			int pos_y = std::max(face_i.tl().y - 10, 0);
			putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
			
			if (user.get_register_status() && confidence <= 1000){
				cout << "Welcome home: " << user.get_user_name() << endl;
				background = imread("/usr/share/faces_meta/door_unlocked.jpg", CV_LOAD_IMAGE_COLOR);
				digitalWrite(LED_PIN, LOW);
				delay(500);
				sleep(10);
			}
			else{
				background = imread("/usr/share/faces_meta/door_locked.jpg", CV_LOAD_IMAGE_COLOR);
				digitalWrite(LED_PIN, HIGH);
				delay(500);
			}

			original.copyTo(background(cv::Rect(0, 0, original.cols, original.rows)));
		}

		imshow("Face Recognition", background);

		char key = (char) waitKey(20);
		// Exit this loop on escape:
		if(key == 27)
			break;
	}
	return 0;
}
