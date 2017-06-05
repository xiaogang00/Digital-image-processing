srcimg = imread('test.jpg');
%添加噪声，并且使用算术均值滤波器进行平滑
%[dstimg,  noiseImage] = ColorMeanFilter(srcimg);

%添加噪声，并且使用几何均值滤波器进行平滑
[dstimg,  noiseImage] = ColorgeoMeanFilter(srcimg);
