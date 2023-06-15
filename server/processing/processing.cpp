
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <sys/wait.h>

#include "common_types.h"

void apply_processing(const char* file_path, header_t header, int unix_timestamp)
{

  char* output = NULL;

  asprintf(&output, "./server/%d_output.mp4", unix_timestamp);

  cv::VideoCapture cap(file_path);

  int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
  int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  double fps = cap.get(cv::CAP_PROP_FPS);

  cv::VideoWriter writer(
    output,
    cv::VideoWriter::fourcc('m','p','4','v'),
    fps,
    cv::Size(
      frame_width + (10 * header.upscale_multiplier) - (10 * header.downscale_multiplier),
      frame_height + (10 * header.upscale_multiplier) - (10 * header.downscale_multiplier)
    ),
    true
  );

  if (!cap.isOpened())
  {
    printf("Child[%d]: Error opening video file: %s", getpid(), file_path);
    return;
  }

  cv::Mat frame;

  while (cap.read(frame))
  {

    for (int i = 0; i < header.blur_multiplier; i++)
    {
      cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);
    }

    cv::resize(frame, frame, 
      cv::Size(
        frame_width + (10 * header.upscale_multiplier) - (10 * header.downscale_multiplier),
        frame_height + (10 * header.upscale_multiplier) - (10 * header.downscale_multiplier)
      ), 
      0, 0, cv::INTER_LINEAR
    );

    writer.write(frame);

  }

  cap.release();
  writer.release();

}