clear;clc;
%读入测试的图像
image = imread('Test.bmp');
%显示原始的图像
figure(1);
imshow(image);
image = double(image);

temp = image - 128;
[m,n] = size(temp);
%首先进行fft2的计算
DFT2 = my_fft2(temp);
%为了方便显示，必须要进行移位
%DFT2SHIFT = DFT2;
DFT2SHIFT = my_fft2shift(DFT2);

%画出幅度谱
figure(2);
imshow(abs(DFT2SHIFT),[]);
%为使得幅度谱的信息更加清晰，可以画出其对数表示
figure(3);
imshow(log10(1+abs(DFT2SHIFT)),[]);
%画出相位谱
figure(4);
imshow(angle(DFT2SHIFT),[]);