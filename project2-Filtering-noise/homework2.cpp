#include <opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;


//产生的是高斯型变量，为后面图像产生噪声
double generateGaussianNoise(double mu, double sigma)
{
	const double epsilon = numeric_limits<double>::min();
	static double z0, z1;
	static bool flag = false;

	flag = !flag;
	//如果flag为假，构造高斯随机变量  
	if (!flag)
		return z1*sigma + mu;
	double u1, u2;

	do
	{
		u1 = rand()*(1.0 / RAND_MAX);
		u2 = rand()*(1.0 / RAND_MAX);
	} while (u1 <= epsilon);

	//flag为真构造高斯随机变量X  
	z0 = sqrt(-2.0*log(u1))*cos(2 * CV_PI * u2);
	z1 = sqrt(-2.0*log(u1))*sin(2 * CV_PI * u2);
	return z1*sigma + mu;
}


//为图像添加高斯噪声  
void AddGaussianNoise(Mat& srcImage)
{
	
	int channels = srcImage.channels();   
	//获取图像的通道数目
	int nRows = srcImage.rows;    
	//获取图像的行数  
	int nCols = srcImage.cols*channels;   
	//获取图像的列数  


	//判断图像的连续性，若连续，我们相当于只需要遍历一个一维数组
	if (srcImage.isContinuous())   
	{
		//矩阵化为向量
		nCols *= nRows;
		nRows = 1;
	}

	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{  
			//添加高斯噪声  
			int val = srcImage.ptr<uchar>(i)[j] + generateGaussianNoise(0.1, 0.8) * 32;
			if (val < 0)
				val = 0;

			if (val > 255)
				val = 255;
			//但是不能超出灰度范围

			srcImage.ptr<uchar>(i)[j] = (uchar)val;
		}
	}
}


//添加椒盐噪声
void salt(Mat& image, int n) 
{
	int temp;
	for (int k = 0; k<n; k++) 
	{
		int i = rand() % image.cols;
		int j = rand() % image.rows;
		temp = rand() % 2;
		if (temp == 0)
			image.at<uchar>(j, i) = 255;
		else
			image.at<uchar>(j, i) = 0;
	}
}

//均值滤波的实现，srcImage是有噪声的图片，size是滤波器的尺寸
void meanFilter(Mat& srcImage, int size)
{
	Size m_Size = srcImage.size();
	//延展图像,以进行后面的滤波,需要添零
	Mat temimage = Mat::zeros(m_Size.height + size, m_Size.width + size, srcImage.type());
	srcImage.copyTo(temimage(Range(size / 2, m_Size.height + size / 2), Range(size / 2, m_Size.width + size / 2)));


	int rows = temimage.rows, cols = temimage.cols;
	int start = size /2 ;

	for (int m = start; m <rows - start; m++) 
	{
		for (int n = start; n < cols - start; n++) 
		{
			int sum = 0;
			//模板实现滤波
			for (int i = -start + m; i <= start + m; i++)
			{
				for (int j = -start + n; j <= start + n; j++) 
				{
					sum += temimage.at<uchar>(i, j);
				}
			}
			//求平均
			temimage.at<uchar>(m, n) = uchar(sum / (size * size));
		}
	}
	temimage(Range(size / 2, m_Size.height + size / 2), Range(size / 2, m_Size.width + size / 2)).copyTo(srcImage);
}


//中值滤波的实现，srcImage是有噪声的图片，size是滤波器的尺寸
void medeanFilter(Mat& srcImage, int size) 
{
	Size m_Size = srcImage.size();
	//延展图像,以进行后面的滤波,需要添零
	Mat temimage = Mat::zeros(m_Size.height + size, m_Size.width + size, srcImage.type());
	srcImage.copyTo(temimage(Range(size / 2, m_Size.height + size / 2), Range(size / 2, m_Size.width + size / 2)));

	int rows = temimage.rows, cols = temimage.cols;
	int start = size / 2;

	for (int m = start; m <rows - start; m++) 
	{
		for (int n = start; n < cols - start; n++) 
		{
			vector<uchar> model;
			for (int i = -start + m; i <= start + m; i++) 
			{
				for (int j = -start + n; j <= start + n; j++) 
				{
					model.push_back(temimage.at<uchar>(i, j));
				}
			}

			sort(model.begin(), model.end());     
			//采用快速排序进行中值的选择，选出中值作为新像素的值
			temimage.at<uchar>(m, n) = model[size * size / 2];
		}
	}
	temimage(Range(size / 2, m_Size.height + size / 2), Range(size / 2, m_Size.width + size / 2)).copyTo(srcImage);
}

int main(void)
{
	Mat image;
	Mat dst;
	image = imread("ocean.bmp", 1);
	//读取图片
	cvtColor(image, image, CV_BGR2GRAY);
	image.convertTo(dst, CV_8UC1);
	//在这里将图片转换到灰度图，以便于后面的滤波操作，主要体现算法思路

	//原始图像
	namedWindow("Origin image", WINDOW_AUTOSIZE);
	imshow("Origin image", dst);
	imwrite("Origin.jpg", dst);

	//高斯噪声的图片
	//AddGaussianNoise(dst);

	//为图像添加椒盐噪声
	salt(dst, 3000);
	namedWindow("Noisy image", WINDOW_AUTOSIZE); 
	imshow("Noisy image", dst); 
	imwrite("salt_noise.jpg", dst);
	//imwrite("gaussian_noise.jpg", dst);


	//对有噪声的图像进行均值滤波，这里采用3 * 3的模板
	Mat image1;
	dst.copyTo(image1);
	meanFilter(image1, 3);
	namedWindow("meanFilter", WINDOW_AUTOSIZE);
	imshow("meanFilter", image1);
	imwrite("meanFilter.jpg", image1);
	//imwrite("meanFilter_gaussian.jpg", image1);

	//对有噪声的图像进行中值滤波，这里采用3 * 3的模板
	Mat image2;
	dst.copyTo(image2);
	medeanFilter(image2, 3);
	namedWindow("medeanFilter", WINDOW_AUTOSIZE);
	imshow("medeanFilter", image2);
	imwrite("medeanFilter.jpg", image2);
	//imwrite("medeanFilter_gaussian.jpg", image2);

	waitKey(0);
	return 0;

}