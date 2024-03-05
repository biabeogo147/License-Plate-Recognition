#include "pch.h"
#include "LprCore.h"
#include "framework.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

void LprCore::set_image(Mat src)
{
	image = src;
	plates.clear();
	characters.clear();
	train_lpr.rS.clear();
	text_recognition.clear();
}

Mat LprCore::get_image()
{
	return image;
}

void LprCore::do_process()
{
	if (this->plate_detection() && this->plate_recognition())
		done = true;
}

vector<string> LprCore::get_text_recognition()
{
	if (done)
		return this->text_recognition;
}

vector<Mat> LprCore::get_plate_detection()
{
	if (done)
		return this->plates;
}

void LprCore::set_svm_model(string file_name)
{
	this->train_lpr.svm = Algorithm::load<SVM>(file_name);
	ready = true;
}

char LprCore::character_recognition(Mat img_character)
{
	imshow("debug", img_character);
	waitKey(0);
	return (ready ? train_lpr.char_recog(img_character) : 'O');
}

bool LprCore::plate_recognition()
{
	for (size_t i = 0; i < characters.size(); i++)
	{
		string result;
		for (size_t j = 0; j < characters.at(i).size(); ++j)
		{
			char cs = character_recognition(characters.at(i).at(j));
			result.push_back(cs);
		}
		text_recognition.push_back(result);
	}
	return 1;

	/*if (plates.size() <= 0) return 0;
	//for (size_t i = 0; i < characters.size(); ++i)
	//{
		string result;
		for (size_t j = 0; j < characters.at(0).size(); ++j)
		{
			char cs = character_recognition(characters.at(0).at(j));
			result.push_back(cs);
		}
		this->text_recognition.push_back(result);
	//}

	std::ofstream outFile("text.txt");
	for (int i = 0; i < train_lpr.rS.size(); i += 1)
		outFile << train_lpr.rS[i] << '\n';
	outFile.close();*/
}

bool LprCore::plate_detection()
{
	Mat gray, binary;
	vector<Mat> draw_character;
	void sort_character(vector<Mat>&);

	cvtColor(image, gray, COLOR_BGR2GRAY);
	adaptiveThreshold(gray, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 5);

	Mat or_binary = binary.clone();
	Mat element = getStructuringElement(MORPH_RECT, cv::Size(3, 3));

	erode(binary, binary, element);
	dilate(binary, binary, element);

	vector<Vec4i> hierarchy;
	vector<vector<cv::Point> > contours;
	findContours(binary, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	if (contours.size() <= 0) return 0;

	for (size_t i = 0; i < contours.size(); ++i)
	{
		Rect r = boundingRect(contours.at(i));
		if (r.width > image.cols / 2 || r.height > image.cols / 2 || r.width < 120 || r.height < 20
			|| (double)r.width / r.height > 4.5 || (double)r.width / r.height < 3.5)
			continue;
		vector<Vec4i> sub_hierarchy;
		Mat sub_binary = or_binary(r);
		Mat _plate = sub_binary.clone();
		vector<vector<cv::Point> > sub_contours;
		findContours(sub_binary, sub_contours, sub_hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		if (sub_contours.size() < 8) continue;

		int count = 0;
		vector<Mat> c;
		Mat sub_image = image(r);
		vector<Rect> r_characters;
		for (size_t j = 0; j < sub_contours.size(); ++j)
		{
			Rect sub_r = boundingRect(sub_contours.at(j));
			if (sub_r.height > r.height / 2 && sub_r.width < r.width / 8 && sub_r.width > 5 && r.width > 15 && sub_r.x > 5)
			{
				Mat cj = _plate(sub_r);
				double ratio = (double)train_lpr.count_pixel(cj, 1) / (cj.cols * cj.rows);
				if (ratio > 0.2 && ratio < 0.7)
				{
					r_characters.push_back(sub_r);
					rectangle(sub_image, sub_r, Scalar(0, 0, 255), 2, 8, 0);
				}
			}
		}
		if (r_characters.size() >= 7)
		{
			for (int i = 0; i < r_characters.size() - 1; ++i)
			{
				for (int j = i + 1; j < r_characters.size(); ++j)
				{
					Rect temp;
					if (r_characters.at(j).x < r_characters.at(i).x)
					{
						temp = r_characters.at(j);
						r_characters.at(j) = r_characters.at(i);
						r_characters.at(i) = temp;
					}
				}
			}
			for (int i = 0; i < r_characters.size(); ++i)
			{
				Mat cj = _plate(r_characters.at(i));
				c.push_back(cj);
			}
			characters.push_back(c);
			sub_binary = or_binary(r);
			plates.push_back(_plate);
			draw_character.push_back(sub_image);
		}
		rectangle(image, r, Scalar(0, 255, 0), 2, 8, 0);
	}

	if (draw_character.empty())
		return 0;

	return 1;

	// Plate recoginatinon
	

	/*Mat gray, binary;
	cvtColor(image, gray, COLOR_RGB2GRAY);
	adaptiveThreshold(gray, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 55, 5);

	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours;
	findContours(binary, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	if (contours.size() <= 0) 
		return 0;

	bool firstT = 0;
	for (size_t i = 0; i < contours.size(); ++i)
	{
		Rect r = boundingRect(contours.at(i));

		if (r.height > r.width || r.height < 50 || r.width < 50)
			continue;

		Mat sub_image = image(r);
		Mat sub_binary = binary(r);
		vector<Vec4i> sub_hierarchy;
		vector<vector<Point>> sub_contours;
		findContours(sub_binary, sub_contours, sub_hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

		vector<Rect> r_characters;
		for (size_t j = 0; j < sub_contours.size(); ++j)
		{
			Rect sub_r = boundingRect(sub_contours.at(j));
			if (sub_r.height > r.height / 3 && sub_r.width < r.width / 4 && sub_r.width >= 20 && sub_r.height >= 20)
			{
				r_characters.push_back(sub_r);
				//rectangle(sub_image, sub_r, Scalar(0, 0, 255), 2, 8, 0);
			}
		}

		if (r_characters.size() > 7)
		{
			vector<Mat> addChar;
			for (auto val : r_characters)
				addChar.push_back(sub_image(val));
			characters.push_back(addChar);
			plates.push_back(sub_image);

			if (!firstT)
			{
				Mat mask = Mat::zeros(image.size(), CV_8UC3);
				drawContours(mask, contours, i, Scalar(0, 255, 255), 1);
				imshow("in", binary);
				imshow("out", mask);
				firstT = 1;
				waitKey(0);
			}

			//this->train_lpr.rS.push_back(r.height);
			//this->train_lpr.rS.push_back(r.width);
		}

		/*std::ofstream outFile("text.txt");
		for (int i = 0; i < train_lpr.rS.size(); i += 2)
			outFile << train_lpr.rS[i] << ' ' << train_lpr.rS[i + 1] << '\n';
		outFile << minI << ' ' << r.height << ' ' << r.width;
		outFile.close();
	}
	
	return plates.size() > 0;*/
}

