function [y] = my_fft(x,n)
%fft主函数,实现一维的fft函数
%首先需要计算级数
m = log2(n);  

%将输入进行倒位序的处理
x = order(x,n);

%下面的是依照在数字信号处理dsp中的公式进行编写的
for l = 1:m
    b = 2^(l-1);
    for t = 0:b-1
        %有b个不同的系数
        p = t*(2^(m-l));
        for k = t:2^l:n-1
            %各蝶形结依次相距2^L点
            
            %以下是主要进行蝶形计算的部分
            temp  = x(k+1) + x(k+1+b) * exp(-j*2*pi*p/n);
            temp1 = x(k+1) - x(k+1+b) * exp(-j*2*pi*p/n);
            x(k+1)   = temp;
            x(k+b+1) = temp1;
        end
    end
end
y = x;

