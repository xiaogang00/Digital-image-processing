function [out, p2] = GLCM(image) 
%image是我们的输入图像
%out是我们根据灰度共生矩阵提取出来的图像的描述子，主要包括6个描述子:
%最大概率，相关，对比度，一致性，同质性，熵
%p2返回的是我们计算出来的进行归一化之后的灰度共生矩阵

%在这里灰度图像的级数可以到256，故而灰度共生矩阵的大小为256 * 256
gray=256; 
if length(size(image)) == 3
    image = rgb2gray(image);
end
[R,C]=size(image);  
 
p1=zeros(gray); 
%p1是灰度共生矩阵的计算结果
%在这里位置算子Q描述的是面对像素左右两边的像素

for M=1:R 
    for N=1:(C-1) 
        p1(image(M,N)+1,image(M,N+1)+1)= p1(image(M,N)+1,image(M,N+1)+1)+1; 
        p1(image(M,N+1)+1,image(M,N)+1)= p1(image(M,N+1)+1,image(M,N)+1)+1; 
    end 
end 

p2=p1; 
%显示灰度共生矩阵
figure(1);
imshow(p2); 
 
%将灰度共生矩阵归一化
p2=double(p2)/(M*N); 
 
%一致性
f1=p2.^2; 
f1=sum(f1(:)); 
 
% 计算相关度f2 
mr = 0;
mc = 0;
sigmar = 0;
sigmac = 0;
for i = 1:256
    mr = mr + i * sum(p2(i,:));
end
for j = 1:256
    mc = mc + j * sum(p2(:,j));
end
for i = 1:256
    sigmar = sigmar + (i-mr)^2 * sum(p2(i,:));
end
for j = 1:256
    sigmac = sigmac + (j-mc)^2 * sum(p2(:,j));
end

f2 = 0;
for i = 1:256
    for j = 1:256
        f2 = f2 + ((i - mr) * (j - mc) * p2(i,j))...
            / (sqrt(sigmar) * sqrt(sigmac));
    end
end

%计算熵f3 
pp=(p2.*log2(p2+eps)); 
f3=-sum(pp(:)); 
  
%同质性计算f4
f4=0; 
for k=1:gray 
    for j=1:gray 
        f4=f4+p2(k,j)/(1+abs(j-k)); 
    end 
end 
 
%计算对比度f5
f5=0; 
for k=2:2*gray 
    for i=1:k-1 
        j=k-i; 
        if j<=gray && i<gray 
        f5=f5+(i-j)^2*p2(i,j); 
        end 
    end 
end 
 
%计算最大概率f6
f6 = max(max(p2));
 
out(1,1)=f1; 
out(1,2)=f2; 
out(1,3)=f3; 
out(1,4)=f4; 
out(1,5)=f5; 
out(1,6)=f6; 
end
