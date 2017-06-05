function dstimg = arithmeticMeans(srcimg, n)
%srcimg为原始的图像， n为均值滤波器模板的大小
%该函数对图像进行算术均值滤波操作
[rows, cols] = size(srcimg);
dstimg = zeros(rows, cols);

for i = ceil(n / 2): rows - floor(n / 2)
    for j = ceil(n / 2) : cols - floor(n / 2)
        count = 0;
        for mm = i - floor(n/2):i + floor(n/2)
            for nn = j - floor(n/2): j + floor(n/2)
                count  = count + srcimg(mm, nn);
            end
        end
        dstimg(i, j) = count / (n * n);
    end
end


