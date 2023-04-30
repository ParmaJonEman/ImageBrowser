// browser.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

static vector<string> list_directory(const char* dirname);
static int parseParameters(int argc, char** argv, String* directory, int* rows, int* cols);
static int showImages(vector<string> vectorOfFiles, int rows, int columns);
static Mat scaleImage(Mat sourceImage, int rows, int columns);
static int waitForKeyCommand(int* index);

