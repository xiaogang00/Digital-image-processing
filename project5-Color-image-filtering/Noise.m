function  noiseImage = Noise(srcimg, type)

   noiseImage = srcimg;
   noiseImageR=imnoise(srcimg(:,:,1),type,0.02);
   noiseImageG=imnoise(srcimg(:,:,2),type,0.02);
   noiseImageB=imnoise(srcimg(:,:,3),type,0.02);
   figure(3);
   subplot(1,3,1);
   noiseImage(:,:,1) = noiseImageR;
   imshow(noiseImageR);
   title('Noise image¡ªR map ');
   subplot(1,3,2);
   noiseImage(:,:,2) = noiseImageG;
   imshow(noiseImageG);
   title('Noise image¡ªG map ');
   
   subplot(1,3,3);
   noiseImage(:,:,3) = noiseImageB;
   imshow(noiseImageB);
   title('Noise image¡ªB map ');
   
end