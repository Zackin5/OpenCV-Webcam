#pragma once
#include "opencv2\opencv.hpp"
#include <iostream>
#include <ctime>
#include <string>

int main(int argc, char* argv[]);

// Handle commandline arguments
void handleArgs(int argc, char* argv[]);

// Timestamp an image
void timestamp(cv::Mat * image_to_stamp);

// Draw window update logic
int updateWindow(cv::VideoCapture * capture_stream);