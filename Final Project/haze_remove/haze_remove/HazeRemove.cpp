#include "HazeRemove.h"

using namespace std;

int compareMyType(const void * a, const void * b)
{
    if (((Pixel*)a)->val <  ((Pixel*)b)->val) return 1;
    if (((Pixel*)a)->val == ((Pixel*)b)->val) return 0;
    return -1;
}

const float HazeRemove::T0 = 0.1;

HazeRemove::HazeRemove(Mat img, int radius, float coffecient) :w0(coffecient)
{
    srcImg = img;
    darkChannelImg.create(srcImg.rows, srcImg.cols, CV_8UC1);
    transmissionMap.create(srcImg.rows, srcImg.cols, CV_32F);
    estimatedTransmissionMap.create(srcImg.rows, srcImg.cols, CV_32F);
    patchRadius = radius;
    for (int i = 0; i < 3; i++)
    {
        globalAtmosphericLight[i] = 0;
    }
    
}


HazeRemove::~HazeRemove()
{
}

void HazeRemove::getPatchPosAndSize(Rect &rect,int rows,int cols,int x,int y)
{
    int i = y;
    int j = x;
    if (j - patchRadius < 0)
    {
        rect.x = 0;
        rect.width = j + patchRadius;
        if (i - patchRadius < 0)
        {
            rect.y = 0;
            rect.height = i + patchRadius;
        }
        else{
            rect.y = i - patchRadius;
            rect.height = i + patchRadius >= rows ? patchRadius + rows - i : 2 * patchRadius + 1;
        }
    }
    else if (i - patchRadius <= 0)
    {
        rect.y = 0;
        rect.height = i + patchRadius;
        rect.x = j - patchRadius;
        rect.width = j + patchRadius >= cols ? patchRadius + cols - j : 2 * patchRadius + 1;
    }
    else if (i + patchRadius >= rows)
    {
        rect.y = i - patchRadius;
        rect.height = patchRadius + rows - i;
        rect.x = j - patchRadius;
        rect.width = j + patchRadius >= cols ? patchRadius + cols - j : 2 * patchRadius + 1;
    }
    else if (j + patchRadius >= cols)
    {
        rect.x = j - patchRadius;
        rect.width = j + patchRadius >= cols ? patchRadius + cols - j : 2 * patchRadius + 1;
        rect.y = i - patchRadius;
        rect.height = 2 * patchRadius + 1;
    }
    else{
        rect.x = j - patchRadius;
        rect.y = i - patchRadius;
        rect.height = 2 * patchRadius + 1;
        rect.width = 2 * patchRadius + 1;
    }
}

void HazeRemove::getDarkChannelPrior()
{
    int rows = srcImg.rows;
    int cols = srcImg.cols;
    int nchannels = srcImg.channels();
    Mat tmpMat=darkChannelImg.clone();
    
    if (srcImg.isContinuous())
    {
        cols = cols*rows;
        rows = 1;
    }
    for (int i = 0; i < rows; i++)
    {
        uchar* srcData = srcImg.ptr<uchar>(i);
        uchar* tmpData = tmpMat.ptr<uchar>(i);
        for (int j = 0; j < cols; j++)
        {
            *tmpData = *srcData;
            for (int p = 1; p < nchannels; p++)
            {
                if (*tmpData>*(srcData + p))
                    *tmpData = *(srcData + p);
            }
            tmpData++;
            srcData = srcData + nchannels;
        }
    }
    
    cols = tmpMat.cols;
    rows = tmpMat.rows;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Rect rect;
            getPatchPosAndSize(rect, rows, cols, j, i);
            
            Mat patchImg = tmpMat(rect);
            double minimum=0;
            minMaxLoc(patchImg,&minimum);

            for (int patch_y = rect.y; patch_y < rect.y+rect.height; patch_y++)
            {
                uchar* darkPriorData = darkChannelImg.ptr<uchar>(patch_y);
                for (int patch_x = rect.x; patch_x < rect.x+rect.width; patch_x++)
                    *(darkPriorData+patch_x) = (int)minimum;
            }
        }
    }
}

void HazeRemove::getTransmissionMap(bool isUseAverage)
{
    getGlobalAtmosphericLight(isUseAverage);
    const int rows = darkChannelImg.rows;
    const int cols = darkChannelImg.cols;
    const int nChannels = srcImg.channels();
    Mat tmpMat(rows, cols, CV_32F);
    
    //get the transmission map
    for (int i = 0; i < rows; i++)
    {
        uchar* pdata = srcImg.ptr<uchar>(i);
        float* tmpData = tmpMat.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            float BVal = (float)*(pdata + j*nChannels);
            float GVal = (float)*(pdata + j*nChannels + 1);
            float RVal = (float)*(pdata + j*nChannels + 2);
            float tmp = BVal / globalAtmosphericLight[0] < GVal / globalAtmosphericLight[1] ? (BVal / globalAtmosphericLight[0]) : (GVal / globalAtmosphericLight[1]);
            float minVal = tmp < RVal / globalAtmosphericLight[2] ? tmp : (RVal / globalAtmosphericLight[2]);
            *tmpData++ = minVal;
        }
    }
    
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            Rect rect;
            getPatchPosAndSize(rect, rows, cols, j, i);
            
            //find the minimum in patch
            Mat patchImg = tmpMat(rect);
            double minimum = 0;
            minMaxLoc(patchImg, &minimum);
            
            for (int patch_y = rect.y; patch_y < rect.y + rect.height; patch_y++)
            {
                float* transmissionData = transmissionMap.ptr<float>(patch_y);
                for (int patch_x = rect.x; patch_x < rect.x + rect.width; patch_x++)
                    *(transmissionData + patch_x) = (1 - w0*minimum <= T0) ? T0 : (1 - w0*minimum);

            }
        }
    }
}

void HazeRemove::getGlobalAtmosphericLight(bool isUseAverage)
{
	const float brightes_ratio = 0.001;
	int rows = darkChannelImg.rows;
	int cols = darkChannelImg.cols;

	int brightestPixelNum = (int)(brightes_ratio*rows*cols);
	int count = 0;
	vector<Pixel> pixelContainer;
	if (darkChannelImg.isContinuous())
	{
		cols = cols*rows;
		rows = 1;
	}
	for (int i = 0; i < rows; i++)
	{
		uchar* srcData = darkChannelImg.ptr<uchar>(i);
		for (int j = 0; j < cols; j++)
		{
			Pixel p;
			p.posX = j;
			p.posY = i;
			p.val = *(srcData + j);
			if (count < brightestPixelNum)
			{
				pixelContainer.push_back(p);
				count++;
			}
			else{
				qsort(&pixelContainer[0], pixelContainer.size(), sizeof(Pixel), compareMyType);
				if (p.val>pixelContainer[brightestPixelNum - 1].val)
					pixelContainer[brightestPixelNum - 1] = p;
			}
		}
	}

	float average[3] = { 0, 0, 0 };
	float max[3] = { 0, 0, 0 };
	int nChannels = srcImg.channels();
	for (int c = 0; c < nChannels; c++)
	{
		int sum = 0;
		for (int iter = 0; iter < brightestPixelNum; iter++)
		{
			int y = pixelContainer[iter].posY;
			uchar* fogImgData = srcImg.ptr<uchar>(y);
			if (isUseAverage)
			{
				sum += *(fogImgData + nChannels*y + c);
			}
			else{
				if (max[c] < *(fogImgData + nChannels*y + c))
					max[c] = *(fogImgData + nChannels*y + c);
			}
		}
		if (isUseAverage)
		{
			average[c] = (float)sum / (float)brightestPixelNum;
			globalAtmosphericLight[c] = average[c];
		}
		else
			globalAtmosphericLight[c] = max[c];
	}
}

void HazeRemove::getEstimatedTransmissionMap(bool isUseAverage)
{
    getGlobalAtmosphericLight(isUseAverage);
    const int rows = darkChannelImg.rows;
    const int cols = darkChannelImg.cols;

    for (int i = 0; i < rows; i++)
    {
        uchar* pdata = darkChannelImg.ptr<uchar>(i);
        float* transmissinData = estimatedTransmissionMap.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            float Val = (float)*(pdata + j);
            float maxValOfGlobalAtmosphericLight = (globalAtmosphericLight[0] < globalAtmosphericLight[1]) ? globalAtmosphericLight[1] : globalAtmosphericLight[0];
            maxValOfGlobalAtmosphericLight = (maxValOfGlobalAtmosphericLight < globalAtmosphericLight[2]) ? globalAtmosphericLight[2] : maxValOfGlobalAtmosphericLight;
            
            float normalizedVal = Val / maxValOfGlobalAtmosphericLight;
            *transmissinData++ = (1 - normalizedVal <= T0) ? T0 : (1 - normalizedVal);
        }
    }
}

Mat HazeRemove::getHazeRemoveImg(Mat transmissionMat)
{
    int rows = srcImg.rows;
    int cols = srcImg.cols;
    Mat hazeRemoveImg(srcImg.rows, srcImg.cols, CV_32FC3);
    const int nchannels = srcImg.channels();
    
    //if the image is continus, transform it into 1-dimension to make the operation faster
    if (srcImg.isContinuous())
    {
        cols = cols*rows;
        rows = 1;
    }
    for (int i = 0; i < rows; i++)
    {
        uchar* srcData = srcImg.ptr<uchar>(i);
        float* dstData = hazeRemoveImg.ptr<float>(i);
        float* transData = transmissionMat.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            float TVal = *(transData + j);
            for (int c = 0; c < nchannels; c++)
            {
                *(dstData + nchannels*j + c) = (*(srcData + nchannels*j + c) - globalAtmosphericLight[c]) / (TVal*255.0) + globalAtmosphericLight[c]/255.0;
            }
        }
    }
    return hazeRemoveImg;
}

Mat HazeRemove::guidedFilter(cv::Mat I, cv::Mat p, int r, double eps)
{

     //guidance image
     //filtering input image
     //local window radius
     //regularization parameter: eps
    
    cv::Mat _I;
    I.convertTo(_I, CV_32FC1);
    I = _I;
    
    cv::Mat _p;
    p.convertTo(_p, CV_32FC1);
    p = _p;
    
    //[height, width] = size(I);
    int height = I.rows;
    int width = I.cols;
    
    //N = boxfilter(ones(height, width), r); % the size of each local patch; N=(2r+1)^2 except for boundary pixels.
    cv::Mat N;
    cv::boxFilter(cv::Mat::ones(height, width, I.type()), N, CV_32FC1, cv::Size(r, r));
    
    //mean_I = boxfilter(I, r) ./ N;
    cv::Mat mean_I;
    cv::boxFilter(I, mean_I, CV_32FC1, cv::Size(r, r));
    
    //mean_p = boxfilter(p, r) ./ N;
    cv::Mat mean_p;
    cv::boxFilter(p, mean_p, CV_32FC1, cv::Size(r, r));
    
    //mean_Ip = boxfilter(I.*p, r) ./ N;
    cv::Mat mean_Ip;
    cv::boxFilter(I.mul(p), mean_Ip, CV_32FC1, cv::Size(r, r));
    
    //cov_Ip = mean_Ip - mean_I .* mean_p; % this is the covariance of (I, p) in each local patch.
    cv::Mat cov_Ip = mean_Ip - mean_I.mul(mean_p);
    
    //mean_II = boxfilter(I.*I, r) ./ N;
    cv::Mat mean_II;
    cv::boxFilter(I.mul(I), mean_II, CV_32FC1, cv::Size(r, r));
    
    //var_I = mean_II - mean_I .* mean_I;
    cv::Mat var_I = mean_II - mean_I.mul(mean_I);
    
    //a = cov_Ip ./ (var_I + eps); % Eqn. (5) in the paper;	
    cv::Mat a = cov_Ip / (var_I + eps);
    
    //b = mean_p - a .* mean_I; % Eqn. (6) in the paper;
    cv::Mat b = mean_p - a.mul(mean_I);
    
    //mean_a = boxfilter(a, r) ./ N;
    cv::Mat mean_a;
    cv::boxFilter(a, mean_a, CV_32FC1, cv::Size(r, r));
    mean_a = mean_a / N;
    
    //mean_b = boxfilter(b, r) ./ N;
    cv::Mat mean_b;
    cv::boxFilter(b, mean_b, CV_32FC1, cv::Size(r, r));
    mean_b = mean_b / N;
    
    //q = mean_a .* I + mean_b; % Eqn. (8) in the paper;
    guidedfilterTransmissionMap = mean_a.mul(I) + mean_b;
    //q.copyTo();
    return guidedfilterTransmissionMap;
}
