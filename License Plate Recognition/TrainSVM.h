#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;

class TrainSVM
{
public:
	vector<float> rS;
	void train_model();
	char char_recog(Mat);
	int count_pixel(Mat, bool);
	Ptr<SVM> svm = SVM::create();
	const int number_of_class = 30;
	const int number_of_sample = 10;
	const int number_of_feature = 32;
	vector<string> list_file(string);
	vector<string> list_folder(string);
	vector<float> calculate_feature(Mat);
private:
};