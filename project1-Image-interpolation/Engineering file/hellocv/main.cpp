#include <opencv.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

void Nearest_neighbor_interpolation(IplImage * image, IplImage * dst);
void bilinearity_interpolation(IplImage * image, IplImage * dst);
void bicubic_interpolation(IplImage * image, IplImage * dst);


int main(void)
{
	IplImage * img = cvLoadImage("timg2.jpg", 1);
	IplImage * dst = NULL;
	IplImage * dst_neighbor = NULL;
	IplImage * dst_bilinearity = NULL;
	IplImage * dst_bicubic = NULL;
	double fScale = 0.5;

	namedWindow("Origin", 1);
	cvShowImage("Origin", img);
	CvSize czSize;

	czSize.width = img->width * fScale;
	czSize.height = img->height * fScale;

	dst = cvCreateImage(czSize, img->depth, img->nChannels);
	cvResize(img, dst, CV_INTER_NN);
	namedWindow("Down sampling ", 1);
	cvShowImage("Down sampling ", dst);
	cvSaveImage("Down sampling.jpg", dst);

	czSize.width  = img->width;
	czSize.height = img->height;

	dst_neighbor = cvCreateImage(czSize, img->depth, img->nChannels);
	Nearest_neighbor_interpolation(dst, dst_neighbor);
	namedWindow("neighbor ", 1);
	cvShowImage("neighbor ", dst_neighbor);
	cvSaveImage("neighbor.jpg", dst_neighbor);

	dst_bilinearity = cvCreateImage(czSize, img->depth, img->nChannels);
	bilinearity_interpolation(dst, dst_bilinearity);
	namedWindow("bilinearity", 1);
	cvShowImage("bilinearity", dst_bilinearity);
	cvSaveImage("bilinearity.jpg", dst_bilinearity);

	dst_bicubic = cvCreateImage(czSize, img->depth, img->nChannels);
	bicubic_interpolation(dst, dst_bicubic);
	namedWindow("bicubic ", 1);
	cvShowImage("bicubic ", dst_bicubic);
	cvSaveImage("bicubic.jpg", dst_bicubic);


	cvWaitKey();
	system("pause");
	return 0;
}


void Nearest_neighbor_interpolation(IplImage * image, IplImage * dst)
{
	int image_width = image->width;
	int image_height = image->height;
	int dst_width = dst->width;
	int dst_height = dst->height;

	CvScalar s1;
	CvScalar s2;

	float scale_x = image_width *1.0 / dst_width;
	float scale_y = image_height *1.0 / dst_height;

	for (int i = 0; i < dst->height; ++i)
	{
		int point_x = cvFloor(i * scale_x);
		point_x = min(point_x, image->height - 1);

		for (int j = 0; j < dst->width; ++j)
		{
			int point_y = cvFloor(j * scale_y);
			point_y = min(point_y, image->width - 1);
			s1 = cvGet2D(image, point_x, point_y);
			s2 = cvGet2D(dst, i, j);
			s2.val[0] = s1.val[0];
			s2.val[1] = s1.val[1];
			s2.val[2] = s1.val[2];
			cvSet2D(dst, i, j, s2);

		}
	}
}


void bilinearity_interpolation(IplImage * image, IplImage * dst)
{
	int image_width = image->width;
	int image_height = image->height;
	int dst_width  = dst->width;
	int dst_height = dst->height;

	struct Point{
		float x;
		float y;
		float z;
	};

	//用四个邻近点去估计给定位置的灰度
    float scale_x = image_width  *1.0 / dst_width;
    float scale_y = image_height *1.0 / dst_height;

	Point point;
	Point point1;
	Point point2;

	for (int j = 0; j < dst_height ; j++)
	{
		for (int i = 0; i < dst_width ; i++)
		{
			point.x = i*scale_x;
			point.y = j*scale_y;

			int point_u = min((int)point.x, image_height -2);
			point_u = max(1, point_u);
			int point_v = min((int)point.y, image_width - 2);
			point_v = max(1, point_v);

			CvScalar s1, s2, s3, s4, s;
			s1 = cvGet2D(image, point_v, point_u);
			s2 = cvGet2D(image, point_v, point_u + 1);
			s3 = cvGet2D(image, point_v + 1, point_u);
			s4 = cvGet2D(image, point_v + 1, point_u + 1);

			point1.x = s1.val[0] * (1 - abs(point.x - point_u)) + s2.val[0] * (point.x - point_u);
			point1.y = s1.val[1] * (1 - abs(point.x - point_u)) + s2.val[1] * (point.x - point_u);
			point1.z = s1.val[2] * (1 - abs(point.x - point_u)) + s2.val[2] * (point.x - point_u);

			point2.x = s3.val[0] * (1 - abs(point.x - point_u)) + s4.val[0] * (point.x - point_u);
			point2.y = s3.val[1] * (1 - abs(point.x - point_u)) + s4.val[1] * (point.x - point_u);
			point2.z = s3.val[2] * (1 - abs(point.x - point_u)) + s4.val[2] * (point.x - point_u);

			s.val[0] = point1.x*(1 - abs(point.y - point_v)) + point2.x*(abs(point.y - point_v));
			s.val[1] = point1.y*(1 - abs(point.y - point_v)) + point2.y*(abs(point.y - point_v));
			s.val[2] = point1.z*(1 - abs(point.y - point_v)) + point2.z*(abs(point.y - point_v));
			cvSet2D(dst, j, i, s);

		}

	}

}


void bicubic_interpolation(IplImage * image, IplImage * dst)
{
	int image_width = image->width;
	int image_height = image->height;
	int dst_width = dst->width;
	int dst_height = dst->height;

	CvScalar s1;
	CvScalar s2;

	float scale_x = image_width *1.0 / dst_width;
	float scale_y = image_height *1.0 / dst_height;


	for (int j = 0; j < dst_width; ++j)
	{
		float y = (float)(j* scale_y);
		int point_y = cvFloor(y);
		point_y = min(point_y, image_width - 3);
		point_y = max(1, point_y);

		float a = -0.5;//BiCubic基函数
		float dis_y[4];
		dis_y[0] = 1 + abs(y - point_y);
		dis_y[1] = abs(y - point_y);
		dis_y[2] = 1 - abs(y - point_y);
		dis_y[3] = 2 - abs(y - point_y);
		float coeff_y[4];
		coeff_y[0] = a*abs(dis_y[0] * dis_y[0] * dis_y[0]) - 5 * a*dis_y[0] * dis_y[0] + 8 * a*abs(dis_y[0]) - 4 * a;
		coeff_y[1] = (a + 2)*abs(dis_y[1] * dis_y[1] * dis_y[1]) - (a + 3)*dis_y[1] * dis_y[1] + 1;
		coeff_y[2] = (a + 2)*abs(dis_y[2] * dis_y[2] * dis_y[2]) - (a + 3)*dis_y[2] * dis_y[2] + 1;
		//coeff_y[3] = a*abs(dis_y[3] * dis_y[3] * dis_y[3]) - 5 * a*dis_y[3] * dis_y[3] + 8 * a*abs(dis_y[3]) - 4 * a;
		coeff_y[3] = 1 - coeff_y[0] - coeff_y[1] - coeff_y[2];


		for (int i = 0; i < dst_height; ++i)
		{
			float x = (float)(i * scale_x);
			int point_x = cvFloor(x);
			point_x = min(point_x, image_width - 3);
			point_x = max(1, point_x);

			float dis_x[4];
			dis_x[0] = 1 + abs(x - point_x);
			dis_x[1] = abs(x - point_x);
			dis_x[2] = 1 - abs(x - point_x);
			dis_x[3] = 2 - abs(x - point_x);
			float coeff_x[4];
			coeff_x[0] = a*abs(dis_x[0] * dis_x[0] * dis_x[0]) - 5 * a*dis_x[0] * dis_x[0] + 8 * a * abs(dis_x[0]) - 4 * a;
			coeff_x[1] = (a + 2)*abs(dis_x[1] * dis_x[1] * dis_x[1]) - (a + 3)*dis_x[1] * dis_x[1] + 1;
			coeff_x[2] = (a + 2)*abs(dis_x[2] * dis_x[2] * dis_x[2]) - (a + 3)*dis_x[2] * dis_x[2] + 1;
			//coeff_x[3] = a*abs(dis_x[3] * dis_x[3] * dis_x[3]) - 5 * a*dis_x[3] * dis_x[3] + 8 * a*abs(dis_x[3]) - 4 * a;
			coeff_x[3] = 1 - coeff_x[0] - coeff_x[1] - coeff_x[2];


			CvScalar  s1, s2, s3, s4, s5, s6, s7, s8, s9;
			CvScalar  s10, s11, s12, s13, s14, s15, s16, s;
			s1 = cvGet2D(image, point_x - 1, point_y - 1);
			s2 = cvGet2D(image, point_x, point_y - 1);
			s3 = cvGet2D(image, point_x + 1, point_y - 1);
			s4 = cvGet2D(image, point_x + 2, point_y - 1);
			s5 = cvGet2D(image, point_x - 1, point_y);
			s6 = cvGet2D(image, point_x, point_y);
			s7 = cvGet2D(image, point_x + 1, point_y);
			s8 = cvGet2D(image, point_x + 2, point_y);
			s9 = cvGet2D(image, point_x - 1, point_y + 1);
			s10 = cvGet2D(image, point_x, point_y + 1);
			s11 = cvGet2D(image, point_x + 1, point_y + 1);
			s12 = cvGet2D(image, point_x + 2, point_y + 1);
			s13 = cvGet2D(image, point_x - 1, point_y + 2);
			s14 = cvGet2D(image, point_x, point_y + 2);
			s15 = cvGet2D(image, point_x + 1, point_y + 2);
			s16 = cvGet2D(image, point_x + 2, point_y + 2);
			s = cvGet2D(dst, i, j);

			for (int k = 0; k < 3; ++k)
			{
				s.val[k] = s1.val[k] * coeff_x[0] * coeff_y[0] + s2.val[k] * coeff_x[0] * coeff_y[1] +
					s3.val[k] * coeff_x[0] * coeff_y[2] + s4.val[k] * coeff_x[0] * coeff_y[3] +
					s5.val[k] * coeff_x[1] * coeff_y[0] + s6.val[k] * coeff_x[1] * coeff_y[1] +
					s7.val[k] * coeff_x[1] * coeff_y[2] + s8.val[k] * coeff_x[1] * coeff_y[3] +
					s9.val[k] * coeff_x[2] * coeff_y[0] + s10.val[k] * coeff_x[2] * coeff_y[1] +
					s11.val[k] * coeff_x[2] * coeff_y[2] + s12.val[k] * coeff_x[2] * coeff_y[3] +
					s13.val[k] * coeff_x[3] * coeff_y[0] + s14.val[k] * coeff_x[3] * coeff_y[1] +
					s15.val[k] * coeff_x[3] * coeff_y[2] + s16.val[k] * coeff_x[3] * coeff_y[3];
			}
			cvSet2D(dst, i, j, s);
		}
	}
}
