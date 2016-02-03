#include "cv.h"
#include "highgui.h"

using namespace std;
using namespace cv;

int main()
{
	//create matric to store image
	Mat image;

	//initialize capture
	VideoCapture cap;
	cap.open(0);

	/* '0' adds the nearest webcam on the computer.
	 * for example, the laptop webcam
	 * '1' will open an external camera
	 */

	//create window to show image
	namedWindow("window", 1);

	cout << "OpenCV version: " << CV_VERSION << endl;

	while (1){
		//copy webcam stream to image
		cap >> image;

		//print image to screen
		imshow("window", image);

		//delay 33 ms
		waitKey(33);
	}

	return 0;
}
