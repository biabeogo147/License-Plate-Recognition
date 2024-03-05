#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

#include "TrainSVM.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

class LprCore
{
public:
	void set_image(Mat);
	void set_svm_model(string);
	void do_process();
	vector<string> get_text_recognition();
	vector<Mat> get_plate_detection();
	Mat get_image();
	TrainSVM train_lpr;
private:
	bool done;
	bool ready;
	Mat image;
	vector<Mat> plates;
	vector<vector<Mat>> characters;
	vector<string> text_recognition;
	char character_recognition(Mat);
	bool plate_detection();
	bool plate_recognition();
	vector<Mat> debug;
};