function lipTracking(rawFrame)

%% Pre-process frame
% Resize frame to reduce execution time
resize = 1;

if resize == 0
    resizedFrame = rawFrame;
elseif resize == 1
    resizedFrame = imresize(rawFrame, [240 320]);
end

% Convert pixel values into doubles to avoid unexpected casting  
frame = im2double(resizedFrame);


%% Identify pixels related to lips 
outputImage = lipExtraction(frame);
subplot(1,2,1);
imshow(outputImage);


%% Lips segmentation
lipLine = edgeLocalization(outputImage,frame);

subplot(1,2,2);
hold on;
imshow(frame);
plot(lipLine(:,1), lipLine(:,2), '-go');
hold off;

end






% cMax = 0;
% cMin = 0;
% 
% [numRow, numCol, ~] = size(frame);
% 
% frameTrans = zeros(numRow, numCol, 'double');
% 
% 
% for row = 1 : numRow
%     for col = 1 : numCol
%         frameRGB = [ frame(row, col, 1) , frame(row, col, 2) , frame(row, col, 3)  ];
%         C = 0.2 * frameRGB(1) - 0.6 * frameRGB(2) + 0.3*frameRGB(3);
%         frameTrans(row, col) = C;
%         
%         cMax = max(C, cMax);
%         cMin = min(C, cMin);
%     end
% end
% 
% for row = 1 : numRow
%     for col = 1 : numCol
%         frameTrans(row, col) = ((frameTrans(row, col) - cMin) / (cMax - cMin))^2;    
%     end
% end
% 
% subplot(1,3,2);
% imshow(frameTrans);
% 
% % Find optimal grey threshold value to classify image pixel as black or white
% level = graythresh(frameTrans);         % Relies on Otsu's grey histogram method
% % level = 0.4;
% frameBW = im2bw(frameTrans, level);
% 
% subplot(1,3,3);
% imshow(frameBW);

        