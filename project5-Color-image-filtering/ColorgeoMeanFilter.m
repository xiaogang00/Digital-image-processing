function [dstimg,  noiseImage] = ColorgeoMeanFilter(srcimg)

  noiseImage = Noise(srcimg, 'salt & pepper');
  %noiseImage = Noise(srcimg, 'gaussian');
  figure(1);
  imshow(noiseImage);
  title('ÎÛÈ¾Í¼Ïñ');
 
  dstimg = srcimg;
  for count = 1:3
      dstimg(:,:,count) = geometricMeans(noiseImage(:,:,count), 3);
  end
  
  figure(2);
  imshow(uint8(dstimg));
  title('Æ½»¬ºóÍ¼Ïñ');
end
