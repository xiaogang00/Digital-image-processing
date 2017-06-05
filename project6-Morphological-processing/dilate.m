function dstimg = dilate(srcimg)
if length(srcimg) == 3
    srcimg = rgb2gray(srcimg);
end
[m, n] = size(srcimg);
for i = 2:m-1
    for j = 2:n-1
        neighbor = [srcimg(i,j), srcimg(i,j-1), srcimg(i,j+1)...
                    srcimg(i+1,j), srcimg(i+1,j-1), srcimg(i+1,j+1)...
                    srcimg(i-1,j), srcimg(i-1,j-1), srcimg(i-1,j+1)];
        dstimg(i,j) = max(neighbor);
    end
end
end
        