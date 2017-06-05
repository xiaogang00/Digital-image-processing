function dstimg = my_fft2(srcimg)
%fft2的主函数，在使用fft2的时候，可以在两个方向上分别使用fft的算法
[m, n] = size(srcimg);

%由于DFT是具有可分性的，所以可以现在x方向调用fft的函数来进行实现
for x = 1:m
    srcimg(x,:) = my_fft(srcimg(x,:),n);
end

%再在y方向上进行调用
for y = 1:n
    srcimg(:,y) = my_fft(srcimg(:,y),m);
end

dstimg = srcimg;