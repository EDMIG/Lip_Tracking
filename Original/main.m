% This program is used to extract the lip for TTS system
% the input is image file name stored in the working directory
%Update March 18,2016: no need to worry about the resolution of the raw image

% img = imread('Snip20160318_2.png');
img = imread('Snip20160318_3.png');
outputImage = extract_mouth(img);
figure;imshow(outputImage);

%% providing dot-marked output image
extract_edge(outputImage,img);