function dstimg = my_fft2shift(srcimg)
%在使用fft2算法之后，需要进行频谱的移动

[m,n] = size(srcimg);
dstimg = zeros(m,n);
%获取图像的尺寸并且创建新的图像

%在x方向上进行移动
for i = 1:m
    x = srcimg(i,:);
    temp1 = x;
    temp1(1:n/2) = x(n/2+1:n);
    temp1(n/2+1:n) = x(1:n/2);
    dstimg(i,:) = temp1;
end

%在y方向上进行移动
for j = 1:n
    x = dstimg(:,j);
    temp2 = x;
    temp2(1:n/2) = x(n/2+1:n);
    temp2(n/2+1:n) = x(1:n/2);
    dstimg(:,j) = temp2;
end