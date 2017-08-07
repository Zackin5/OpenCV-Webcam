#include "Main.h"

bool saveFrames = false; // Should we save frames at every n interval?
unsigned int savedFrameInterval = 30; // How many seconds inbetween frame captures
int webcamID = 0; // What device number is your webcam
float windowscale = 1; // Scale of render window

int main(int argc, char* argv[])
{
    cv::VideoCapture captureStream;
    int updateStatus;

    // Handle any passed args
    if (argc > 0)
        handleArgs(argc, argv);

    // Start the camera stream if valid
    captureStream.open(webcamID);
    if (!captureStream.isOpened())
    {
        std::cout << "Failed to get webcam." << std::endl;
        return -1;
    }

    // Run the main update logic and save the resulting run code once finished
    updateStatus = updateWindow(&captureStream);

    //Error printout
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

void handleArgs(int argc, char* argv[])
{
    // Future notes: look into getopt (Unix!!!) or stdarg.h (Win)
    // Definity replace alll thiiss with something more legit e.g. ^^^ becuase good god these indexes are all over the place

    for (int i = 0; i < argc; i++)
    {
        switch (i)
        {
        case 0: // Save frame interval, negitive numbers disable
            savedFrameInterval = std::atoi(argv[0]);
            if (savedFrameInterval < 1)
            {
                savedFrameInterval = 60;
                saveFrames = false;
            }
            break;
        case 1: // ID of camera
            webcamID = std::atoi(argv[2]);
            break;
        case 2: // Window scale
            windowscale = std::atof(argv[3]);
            break;
        default:
            break;
        }
    }
}

void timestamp(cv::Mat * frame)
{
    std::time_t epochtime;
    char timestr[26];
    std::tm timestruct;

    // Get the time and update the timestr
    epochtime = std::time(nullptr);
    localtime_s(&timestruct, &epochtime);
    asctime_s(timestr, sizeof timestr, &timestruct);
    timestr[24] = '\0'; // Clear the last character, else opencv's putText draws it as an unknown

    // Draw the timestamp
    cv::rectangle(*frame, cvPoint(0, frame->size().height), cvPoint((sizeof(timestr) - 1) * 8 + 1, frame->size().height - 13), cvScalar(0, 0, 0), CV_FILLED, 8, 0);
    cv::putText(*frame, timestr, cvPoint(0, frame->size().height - 2.0), CV_FONT_HERSHEY_PLAIN, 0.9, cvScalar(255, 255, 255), 1, 8, false);
}

int updateWindow(cv::VideoCapture * captureStream)
{
    // Time stuff
    std::time_t epochtime;
    std::tm timestruct;

    // frame stuff
    cv::Mat * frame = new cv::Mat();
    bool savedFrame = false;
    
    // background subtraction
    cv::Ptr<cv::BackgroundSubtractorMOG2> bsMOG2;
    cv::Mat fgMask;
    bool bgsubEnabled = false;

    // Loop until we hit ESC
    while (true)
    {
        epochtime = std::time(nullptr);

        // Convert to local time
        localtime_s(&timestruct, &epochtime);

        // Get the camera frame
        *captureStream >> *frame;

        // Ensure the frame is valid
        if (frame->empty())
            return -1;

        // If foreground detection is enabled perform the operations
        if (bgsubEnabled)
        {
            cv::Mat fgImage;
            cv::Mat bgImage = cv::imread("./beach.jpg", 1);

            // Run the MOG2 function and render it to the fgMask mat
            bsMOG2->apply(*frame, fgMask);

            // Mask our input image (onto a nice beach image!)
            bgImage.copyTo(fgImage);
            frame->copyTo(fgImage, fgMask);

            // Draw windows
            cv::imshow("Foreground mask", fgMask);
            cv::imshow("Foreground image", fgImage);
        }

        // Timestamp the image
        timestamp(frame);

        // Save a frame every 30 seconds
        if (saveFrames && (timestruct.tm_sec % savedFrameInterval) == 0)
        {
            if (!savedFrame) 
            {
                // Generate the filename
                std::string filename = "FRAME_" + std::to_string(timestruct.tm_mon + 1) + "_" + std::to_string(timestruct.tm_mday) + "_" + std::to_string(timestruct.tm_hour) + "." + std::to_string(timestruct.tm_min) + "." + std::to_string(timestruct.tm_sec) + ".jpg";
                
                // Save the frame
                cv::imwrite(filename, *frame);

                savedFrame = true;
            }
        }
        else
            savedFrame = false;

        // Upscale the image
        cv::Mat resizedFrame;
        cv::resize(*frame, resizedFrame, resizedFrame.size(), windowscale, windowscale, cv::INTER_CUBIC);

        // Draw the window
        cv::imshow("OpenCV Webcam", resizedFrame);

        // Input handling
        switch (cv::waitKey(10))
        {
        case 'h':   // bg subtraction initalize and enable
            bsMOG2 = cv::createBackgroundSubtractorMOG2();
            bgsubEnabled = true;
            break;
        case 27:    // Esc exit
            return 0;
            break;
        default:
            break;
        }
    }

    return 0;
}