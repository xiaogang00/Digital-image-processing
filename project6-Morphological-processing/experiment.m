
img = imread('test.jpg');
srcimg = rgb2gray(img);
dilateimg = dilate(srcimg);
erodeimg = erode(srcimg);
gradientimg = dilateimg - erodeimg;
%膨胀处理之后的图像
figure(1);
imshow(dilateimg);

%腐蚀处理之后的图像
figure(2);
imshow(erodeimg);

%我们提取的形态学梯度
figure(3);
imshow(gradientimg);

%原始图像的灰度图
figure(4);
imshow(srcimg);