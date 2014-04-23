//objectTrackingTutorial.cpp

//Written by  Kyle Hounslow 2013

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software")
//, to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//IN THE SOFTWARE.

#include "opencv2/opencv.hpp"
#include "Target.h"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <highgui.h>
#include <cv.h>

using namespace std;
using namespace cv;
//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 180;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
int CROSS_X = FRAME_WIDTH / 2;
int CROSS_Y = FRAME_HEIGHT / 2;
//saved last output
string lastTargets = "";
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 10;
//minimum and maximum object area
int MIN_OBJECT = 15;
int ERODE_SIZE = 4;
int DILATE_SIZE = 5;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "Overlay";
const string trackbarWindowName = "Trackbars";
string trackbarSaveFile = "trackbars.conf";

void onTrackbar(int, void*) {//This function gets called whenever a
    // save settings
    ofstream conf;
    conf.open(trackbarSaveFile.data());
    conf << MIN_OBJECT << "\n";
    conf << ERODE_SIZE << "\n";
    conf << DILATE_SIZE << "\n";
    conf << H_MIN << "\n";
    conf << H_MAX << "\n";
    conf << S_MIN << "\n";
    conf << S_MAX << "\n";
    conf << V_MIN << "\n";
    conf << V_MAX << "\n";
    conf << CROSS_X << "\n";
    conf << CROSS_Y << "\n";
    conf.close();
}

string intToString(int number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}

void loadTrackbars() {
    // load settings
    ifstream conf;
    conf.open(trackbarSaveFile.data());
    conf >> MIN_OBJECT;
    conf >> ERODE_SIZE;
    conf >> DILATE_SIZE;
    conf >> H_MIN;
    conf >> H_MAX;
    conf >> S_MIN;
    conf >> S_MAX;
    conf >> V_MIN;
    conf >> V_MAX;
    conf >> CROSS_X;
    conf >> CROSS_Y;
    conf.close();
}

void createTrackbars() {
    //create window for trackbars
    namedWindow(trackbarWindowName, 0);
    //create memory to store trackbar name on window
    loadTrackbars();
    //create trackbars and insert them into window
    createTrackbar("Min Object", trackbarWindowName, &MIN_OBJECT, 50, onTrackbar);
    createTrackbar("Erode", trackbarWindowName, &ERODE_SIZE, 20, onTrackbar);
    createTrackbar("Dilate", trackbarWindowName, &DILATE_SIZE, 20, onTrackbar);
    createTrackbar("Erode", trackbarWindowName, &ERODE_SIZE, 20, onTrackbar);
    createTrackbar("Dilate", trackbarWindowName, &DILATE_SIZE, 20, onTrackbar);
    createTrackbar("H min", trackbarWindowName, &H_MIN, 180, onTrackbar);
    createTrackbar("H max", trackbarWindowName, &H_MAX, 180, onTrackbar);
    createTrackbar("S min", trackbarWindowName, &S_MIN, 256, onTrackbar);
    createTrackbar("S max", trackbarWindowName, &S_MAX, 256, onTrackbar);
    createTrackbar("V min", trackbarWindowName, &V_MIN, 256, onTrackbar);
    createTrackbar("V max", trackbarWindowName, &V_MAX, 256, onTrackbar);
    createTrackbar("Cross X", trackbarWindowName, &CROSS_X, FRAME_WIDTH, onTrackbar);
    createTrackbar("Cross Y", trackbarWindowName, &CROSS_Y, FRAME_HEIGHT, onTrackbar);
}

void drawObject(vector<Target> targets, Mat &frame) {
    for (int i = 0; i < targets.size(); i++) {
        int x = targets[i].getX();
        int y = targets[i].getY();
        cv::circle(frame, cv::Point(x, y), 10, cv::Scalar(0, 0, 255));
        string text = "[" + intToString(x) + " " + intToString(y) + "]";
        cv::putText(frame, text, cv::Point(x - 40, y + 25), 1, 1, Scalar(0, 0, 255));
    }
}

void drawCrosshairs(Mat &frame) {
    // Vertical center
    cv::line(frame, cv::Point(CROSS_X, CROSS_Y + 100), cv::Point(CROSS_X, CROSS_Y - 100), cv::Scalar(0, 255, 0));
    // Horizontal center
    cv::line(frame, cv::Point(CROSS_X - 50, CROSS_Y), cv::Point(CROSS_X + 50, CROSS_Y), cv::Scalar(0, 255, 0));
    // Square
    cv::rectangle(frame, cv::Point(CROSS_X - 10, CROSS_Y - 10), cv::Point(CROSS_X + 10, CROSS_Y + 10), cv::Scalar(0, 255, 0), 1, 1, 0);
    // tag
    string text = "[" + intToString(CROSS_X) + " " + intToString(CROSS_Y) + "]";
    cv::putText(frame, text, cv::Point(CROSS_X + 10, CROSS_Y - 20), 1, 1, Scalar(0, 255, 0));
}

void printTargets(vector<Target> targets) {
    string output = "" + intToString(CROSS_X) + " " + intToString(CROSS_Y);
    for (int i = 0; i < targets.size(); i++) {
        int x = targets[i].getX();
        int y = targets[i].getY();
        output += " " + intToString(x) + " " + intToString(y);
    }
    output += "\n";
    // only send changes
    if (lastTargets != output) {
        lastTargets = output;
        cout << output;
        cout.flush();
    }
}

void morphOps(Mat &thresh) {
    // for safety, reset broken erode and dilate values
    if (ERODE_SIZE == 0)
        ERODE_SIZE = 1;
    if (DILATE_SIZE == 0)
        DILATE_SIZE = 1;
    //create structuring element that will be used to "dilate" and "erode" image.
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(ERODE_SIZE, ERODE_SIZE));
    //dilate with larger element so make sure object is nicely visible
    Mat dilateElement = getStructuringElement(MORPH_RECT, Size(DILATE_SIZE, DILATE_SIZE));

    erode(thresh, thresh, erodeElement);
    erode(thresh, thresh, erodeElement);

    dilate(thresh, thresh, dilateElement);
    dilate(thresh, thresh, dilateElement);
}

void trackFilteredObject(Mat threshold, Mat HSV, Mat &cameraFeed) {
    vector<Target> targets;

    Mat temp;
    threshold.copyTo(temp);
    //these two vectors needed for output of findContours
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    //use moments method to find our filtered object
    double refArea = 0;
    bool objectFound = false;
    if (hierarchy.size() > 0) {
        int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if (numObjects < MAX_NUM_OBJECTS) {
            for (int index = 0; index >= 0; index = hierarchy[index][0]) {
                Moments moment = moments((cv::Mat)contours[index]);
                double area = moment.m00;

                //if the area is less than 20 px by 20px then it is probably just noise
                //if the area is the same as the 3/2 of the image size, probably just a bad filter
                //we only want the object with the largest area so we safe a reference area each
                //iteration and compare it to the area in the next iteration.
                if (area > MIN_OBJECT * MIN_OBJECT) {
                    Target target;
                    target.setX(moment.m10 / area);
                    target.setY(moment.m01 / area);
                    targets.push_back(target);
                    objectFound = true;
                } else {
                    objectFound = false;
                }
            }
            //let user know you found an object
            if (objectFound == true) {
                //draw object location on screen
                drawObject(targets, cameraFeed);
                printTargets(targets);
            }

        } else {
            putText(cameraFeed, "Sensory overload. Adjust filters.", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
        }
    }
}

void generateOverlay(Mat camera, Mat threshold, Mat &merged) {
    merged = camera.clone();
    double alpha = 0.75;
    for (int y = 0; y < camera.rows; y++)
        for (int x = 0; x < camera.cols; x++) {
            if (threshold.at<Vec3b>(y, x / 3)[0] > 0 && threshold.at<Vec3b>(y, x / 3)[1] > 0 && threshold.at<Vec3b>(y, x / 3)[2] > 0) {
//                merged.at<Vec3b>(y, x)[0] = (1 - alpha) * camera.at<Vec3b>(y, x)[0] + (alpha * threshold.at<Vec3b>(y, x / 3)[0]);
//                merged.at<Vec3b>(y, x)[1] = (1 - alpha) * camera.at<Vec3b>(y, x)[1] + (alpha * threshold.at<Vec3b>(y, x / 3)[1]);
                merged.at<Vec3b>(y, x)[2] = (1 - alpha) * camera.at<Vec3b>(y, x)[2] + (alpha * threshold.at<Vec3b>(y, x / 3)[2]);
            } else {
                merged.at<Vec3b>(y, x)[0] = camera.at<Vec3b>(y, x)[0];
                merged.at<Vec3b>(y, x)[1] = camera.at<Vec3b>(y, x)[1];
                merged.at<Vec3b>(y, x)[2] = camera.at<Vec3b>(y, x)[2];
            }
        }
}

int main(int argc, char* argv[]) {
    //Matrix to store each frame of the webcam feed
    Mat cameraFeedMatrix;
    Mat thresholdFeedMatrix;
    Mat hsvMatrix;
    Mat mergedFeedMatrix;

    //create slider bars for HSV filtering
    createTrackbars();
    //video capture object to acquire webcam feed
    VideoCapture capture;
    //open capture object at location zero (default location for webcam)
    capture.open(1);
    //set height and width of capture frame
    capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
    //start an infinite loop where webcam feed is copied to cameraFeed matrix
    //all of our operations will be performed within this loop
    while (1) {
        //store image to matrix
        capture.read(cameraFeedMatrix);
        //convert frame from BGR to HSV colorspace
        cvtColor(cameraFeedMatrix, hsvMatrix, COLOR_BGR2HSV);

        // track objects
        inRange(hsvMatrix, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), thresholdFeedMatrix);
        morphOps(thresholdFeedMatrix);
        trackFilteredObject(thresholdFeedMatrix, hsvMatrix, cameraFeedMatrix);
        drawCrosshairs(cameraFeedMatrix);
        generateOverlay(cameraFeedMatrix, thresholdFeedMatrix, mergedFeedMatrix);

        // display feeds
//        imshow(windowName2, thresholdFeedMatrix);
//        imshow(windowName, cameraFeedMatrix);
        imshow(windowName3, mergedFeedMatrix);
        waitKey(30);
    }
    return 0;
}
