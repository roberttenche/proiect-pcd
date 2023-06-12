#include "blur.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>

void apply_blur(const char* file_path)
{
    cv::VideoCapture cap(file_path);

    // Check if the video file was opened successfully
    if (!cap.isOpened()) {
      std::cout << "Error opening video file" << std::endl;
      return ;
    }

    // Read and display frames from the video file
    cv::Mat frame;
    while (cap.read(frame)) {

      cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);

      cv::imshow("Video", frame);

      // Exit if the 'q' key is pressed
      if (cv::waitKey(1) == 'q')
          break;
    }

    // Release the video file capture object
    cap.release();

    // Close the display window
    cv::destroyAllWindows();
}