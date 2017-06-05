function [dstimg, eta, result] = Otsu(srcimg)
%该函数是用来实现Otsu全局阈值分割的
%输入为一幅图像srcimg
%输出dstimg为阈值分割之后的图像，eta是可分性度量，result是最佳的阈值
if length(size(srcimg))==3
    srcimg = rgb2gray(srcimg);
end
%首先统计直方图，并且做归一化
Number=imhist(srcimg);
S = sum(Number);
Number = Number/S;

%统计累积和以及累积均值
P1 = zeros(length(Number), 1);
M1 = zeros(length(Number), 1);
for k = 1:length(Number)
    P = 0;
    M = 0;
    for i = 1:k
        P = P + Number(i);
        M = M + (i-1)*Number(i);
    end
    P1(k) = P;
    M1(k) = M;
end
%计算全局灰度均值
mg = M1(length(Number));

%计算类间方差和全局方差
sigmaB = zeros(length(Number), 1);
sigmaG = 0;
for k = 1:length(Number)
    sigmaG = sigmaG + (k- 1 - mg)^2 * Number(i);
    sigmaB(k) = ((mg * P1(k) - M1(k))^2) / (P1(k) * (1-P1(k)));
end

%找出最大的k就是最后的最佳阈值
maxnum = max(sigmaB);
result = find(sigmaB == maxnum);
result = mean(result);

%计算可分性度量
eta = sigmaB(result) / sigmaG;

%进行阈值分割
[m ,n] = size(srcimg);
dstimg = zeros(m, n);
for i = 1:m
    for j = 1:n
        if srcimg(i,j) > result
            dstimg(i,j) = 1;
        else
            dstimg(i,j) = 0;
        end
    end
end

end



    
    



