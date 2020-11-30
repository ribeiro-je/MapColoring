#ifndef VIZ_H
#define VIZ_H

#include <opencv2/core/mat.hpp>

using cv::Mat;
using cv::Vec3b;

int viz_run(int argc, char **argv);
int viz_show(Mat view);

#endif
