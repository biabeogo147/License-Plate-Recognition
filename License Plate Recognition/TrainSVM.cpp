#include "pch.h"
#include "TrainSVM.h"
#include "framework.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include <fstream>

using namespace cv::ml;
using namespace std;
using namespace cv;

int TrainSVM::count_pixel(Mat img, bool black_pixel = 1)
{
    int black = 0, white = 0;
    for (int i = 0; i < img.rows; ++i)
        for (int j = 0; j < img.cols; ++j)
        {
            if (img.at<uchar>(i, j) == 0)
                ++black;
            else
                ++white;
        }
    return (black_pixel ? black : white);
}

vector<float> TrainSVM::calculate_feature(Mat src)
{
    Mat gray, img;
    if (src.channels() == 3)
    {
        cvtColor(src, gray, COLOR_BGR2GRAY);
        adaptiveThreshold(gray, img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 5);

    }
    else
    {
        adaptiveThreshold(src, img, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 5);
    }

    vector<float> r;
    resize(img, img, Size(20, 20));

    int h = img.rows / 4;
    int w = img.cols / 4;
    int S = count_pixel(img);
    int T = img.cols * img.rows;
    for (int i = 0; i < img.rows; i += h)
    {
        for (int j = 0; j < img.cols; j += w)
        {
            Mat cell = img(Rect(i, j, h, w));
            int s = count_pixel(cell);
            float f = (float)s / S;
            r.push_back(f);
        }
    }

    for (int i = 0; i < 16; i += 4)
    {
        float f = r[i] + r[i + 1] + r[i + 2] + r[i + 3];
        r.push_back(f);
    }

    for (int i = 0; i < 4; ++i)
    {
        float f = r[i] + r[i + 4] + r[i + 8] + r[i + 12];
        r.push_back(f);
    }
    r.push_back(r[0] + r[5] + r[10] + r[15]);
    r.push_back(r[3] + r[6] + r[9] + r[12]);
    r.push_back(r[7] + r[10] + r[13] + r[16]);
    r.push_back(r[23] + r[24] + r[25] + r[26]);
    r.push_back(r[2] + r[7] + r[12] + r[17]);
    r.push_back(r[9] + r[11] + r[13] + r[15]);
    r.push_back(r[10] + r[15] + r[20] + r[25]);
    r.push_back(r[14] + r[16] + r[18] + r[20]);

    std::ofstream outFile("text.txt");
            for (int i = 0; i < r.size(); i += 2)
                outFile << r[i] << '\n';
            outFile.close();

    return r; // 32 dac trung
}

vector<string> TrainSVM::list_folder(string path)
{
    vector<string> folders;
    CFileFind finder;
    BOOL bWorking = finder.FindFile(CString(path.c_str()) + _T("\\*.*"));
    while (bWorking)
    {
        bWorking = finder.FindNextFile();
        if (finder.IsDots())
            continue;
        if (finder.IsDirectory())
        {
            string folderPath = CT2A(finder.GetFilePath().GetString());
            folders.push_back(folderPath);
        }
    }
    return folders;
}

vector<string> TrainSVM::list_file(string path)
{
    vector<string> files;
    CFileFind finder;
    BOOL bWorking = finder.FindFile(CString(path.c_str()) + _T("\\*.*"));
    while (bWorking)
    {
        bWorking = finder.FindNextFile();
        if (finder.IsDots() || finder.IsDirectory())
            continue;
        string filePath = CT2A(finder.GetFilePath().GetString());
        files.push_back(filePath);
    }
    return files;
}

char TrainSVM::char_recog(Mat img_character)
{
    char c = '*';
    if (img_character.empty()) //Load mo hinh svm truoc khi su dung ham nay
        return c;

    vector<float> feature = this->calculate_feature(img_character);
    Mat sample = Mat(1, number_of_feature, CV_32FC1);

    for (size_t i = 0; i < feature.size(); ++i)
        sample.at<float>(0, i) = feature[i];

    std::ofstream outFile("trainData_error.txt");
    try 
    {
        float r = this->svm->predict(sample);
        rS.push_back(r);

        int ri = (int)r;
        if (ri >= 0 && ri <= 9)
            c = char(ri + 48);
        if (ri >= 10 && ri < 18)
            c = char(ri + 55);
        if (ri >= 18 && ri < 22)
            c = char(ri + 55 + 2);
        if (ri == 22)
            c = 'P';
        if (ri == 23)
            c = 'S';
        if (ri >= 24 && ri < 27)
            c = char(ri + 60);
        if (ri >= 27 && ri < 30)
            c = char(ri + 61);
        return c;
    }
    catch (cv::Exception& e) 
    {
        outFile << e.what() << "\n";
    }
    outFile.close();
}


void TrainSVM::train_model()
{
    /*svm->setC(16);
    svm->setGamma(0.5);
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));

    Mat data = Mat(number_of_sample * number_of_class, number_of_feature, CV_32FC1);
    Mat label = Mat(number_of_sample * number_of_class, 1, CV_32SC1);
    int index = 0;

    vector<string> folders = list_folder("D:\\Image Processing\\Data");
    for (size_t i = 0; i < folders.size(); ++i)
    {
        vector<string> files = list_file(folders.at(i));
        string folder_path = folders.at(i);
        string label_folder = folder_path.substr(folder_path.length() - 1);
        for (size_t j = 0; j < files.size(); ++j)
        {
            Mat src = imread(files.at(j));
            if (src.empty())
                continue;
            vector<float> feature = calculate_feature(src);
            if (feature.size() < number_of_feature)
                continue;

            for (size_t t = 0; t < feature.size(); ++t)
                data.at<float>(index, t) = feature.at(t);
            label.at<float>(index, 0) = int(i);
            ++index;
        }
    }*/

    svm->setC(16);
    svm->setGamma(0.5);
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::RBF);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 1e-6));

    vector<string> folders = list_folder("D:\\Image Processing\\Data");
    std::sort(folders.begin(), folders.end());

    Mat src;
    int index = 0;
    Mat data = Mat(number_of_sample * number_of_class, number_of_feature, CV_32FC1);
    Mat label = Mat(number_of_sample * number_of_class, 1, CV_32SC1);

    for (size_t i = 0; i < folders.size(); ++i)
    {
        string folder_path = folders.at(i);
        vector<string> files = list_file(folders.at(i));
        //cout << "list folder" << folders.at(i) << endl;
        string label_folder = folder_path.substr(folder_path.length() - 1);
        for (size_t j = 0; j < files.size(); ++j)
        {
            src = imread(files.at(j));
            vector<float> feature = calculate_feature(src);
            
            std::ofstream outFile("text1.txt");

            for (size_t t = 0; t < feature.size(); ++t)
                outFile << (data.at<float>(index, t) = feature[t]) << '\n';
            label.at<int>(index, 0) = i;
            index++;

            outFile.close();
        }
    }


    Ptr<TrainData> trainData = TrainData::create(data, ROW_SAMPLE, label);

    std::ofstream outFile("text1.txt");
    
    for (int i = 0; i < data.rows; ++i)
    {
        for (int j = 0; j < data.cols; ++j)
            outFile << data.at<float>(i, j) << '\n';
        outFile << '\n';
    }

    outFile.close();

    this->svm->train(trainData);
    this->svm->save("D:\\Image Processing\\svm.xml");
}