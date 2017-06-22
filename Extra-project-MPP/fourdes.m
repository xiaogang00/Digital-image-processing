clc;clear
I = imread('mapleleaf.tif'); 
subplot(2,4,1);imshow(I);title('The original image');
binimg = im2bw(I,0.7);
subplot(2,4,2);imshow(binimg);title('After thresholding');
%边界提取
boundary = bwboundaries(binimg,'noholes');
boundary = boundary{1};
[m,n] = size(binimg);
%compute Fourier descriptors
z = frdescp(boundary);
z600 = ifrdescp(z,600);
z600im = bound2im(z600,m,n);
subplot(2,4,3);imshow(z600im);title('600个点描绘子获得逆变换');
z300 = ifrdescp(z,300);
z300im = bound2im(z300,m,n);
subplot(2,4,4);imshow(z300im);title('300个点描绘子');
z150 = ifrdescp(z,150);
z150im = bound2im(z150,m,n);
subplot(2,4,5);imshow(z150im);title('150个点描绘子');
z50 = ifrdescp(z,50);
z50im = bound2im(z50,m,n);
subplot(2,4,6);imshow(z50im);title('50个点描绘子');
z20 = ifrdescp(z,20);
z20im = bound2im(z20,m,n);
subplot(2,4,7);imshow(z20im);title('20个点描绘子');
z12 = ifrdescp(z,12);
z12im = bound2im(z12,m,n);
subplot(2,4,8);imshow(z12im);title('12个点描绘子');

maxvalue = abs(max(z));
std_deviation = maxvalue / 10;
noise = imnoise(z,'gaussian',0,std_deviation);
z_noise = z + noise;

rand = randperm(length(z));
sample100_train = rand(1:100);
sample100_test = rand(101:200);
z_sample = []
for i=1:100
    z_sample_trainset(i) = z_noise(sample100_train(i));
    z_sample_testset(i) = z_noise(sample100_test(i));
end






