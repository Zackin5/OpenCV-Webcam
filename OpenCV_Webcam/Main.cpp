#include "opencv2\opencv.hpp"
#include <iostream>
#include <ctime>
#include <string>

int updateWindow(cv::VideoCapture * capture_stream);

int main()
{
    cv::VideoCapture captureStream(0);
    if (!captureStream.isOpened())
    {
        std::cout << "Failed to get webcam." << std::endl;
        return -1;
    }

    int updateStatus;
        
    updateStatus = updateWindow(&captureStream);

    switch (updateStatus)
    {
    case -1:
        std::cout << "Failed to draw: frame data was empty." << std::endl;
        break;
    default:
        break;
    }

    return updateStatus;
}

int updateWindow(cv::VideoCapture * captureStream)
{
    std::time_t epochtime;
    std::string timestr;
    cv::Mat frame;

    // Loop until we hit ESC
    for (;;)
    {
        // Get the camera frame
        *captureStream >> frame;

        // Ensure the frame is valid
        if (frame.empty())
            return -1;

        // Get the time and update the timestr
        epochtime = std::time(nullptr);
        timestr = std::asctime(std::localtime(&epochtime));
        timestr.pop_back(); // Delete the linebreak char since putText does not render it

        // Draw the timestamp
        cv::rectangle(frame, cvPoint(0, frame.size().height), cvPoint(timestr.length() * 8 + 1, frame.size().height - 13), cvScalar(0, 0, 0), CV_FILLED, 8, 0);
        cv::putText(frame, timestr, cvPoint(0, frame.size().height - 2.0), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(255, 255, 255), 1, 8, false);

        // Upscale the image
        cv::Mat resizedFrame;
        cv::resize(frame, resizedFrame, cv::Size(1280, 900), 0, 0, cv::INTER_CUBIC);

        // Draw the window
        cv::imshow("OpenCV Webcam", resizedFrame);

        // Check for the ESC keystroke and abort if hit
        if (cv::waitKey(10) == 27)
            break;
    }

    return 0;
}