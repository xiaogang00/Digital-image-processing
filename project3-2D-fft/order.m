function output = order(input,n)
%在这里是用来实现位倒序的函数

half = n/2;
temp  = half;
temp1 = n-2;
for i = 1:temp1
    if i < temp
        t = input(i+1);
        input(i+1) = input(temp+1);
        input(temp+1) = t;
    end
    k = half;
    while(temp>=k)
        temp = temp-k;
        k = k/2;
    end
    temp = temp+k;
end

output = input;