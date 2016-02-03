This project is about to use a Raspberry Pi to create a face recognition unlock App.
The project consists of 2 parts

The first part is a face recognition program to unlock the door
we have implemented:
-   OpenCV to do the real face recognition, a predicted value and confidence value will be returned.
-   wiringPi.h(http://wiringpi.com/) to control breadboard.
-   mysql++ to connect to MySQL database
The program also combined with signal handling.

The second part is an admin page for administrators to control the App.
we use Pyramid web framework to build the application.
it involves:
-   JavaScript accesses webcam to take picture of a registered user.
-   Server-sided OpenCV to detect faces and crop to appropriate size
-   MySQLdb to connect to database to update and get data.
