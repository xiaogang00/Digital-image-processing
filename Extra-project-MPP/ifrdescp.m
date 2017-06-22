%IFRDESCP computes inverse Frouier descriptors
function s = ifrdescp(z,nd)
np = length(z);
if nargin == 1 
    nd = np;
end
if np/2 ~=round(np/2)
    error('length(z) must be an evenn integer.')
elseif nd/2 ~= round(nd/2)
    error('nd must be an even integer.')
end
 x = 0:(np - 1);
 m = ((-1) .^ x)';
 d = (np-nd)/2;
 z(1:d) = 0;
 z(np - d +1:np) = 0;
 zz = ifft(z);
 s(:,1) = real(zz);
 s(:,2) = imag(zz);
 s(:,1) = m.*s(:,1);
 s(:,2) = m.*s(:,2);
 
 