function lipFrame = lipExtraction(rawFrame)
%lipExtraction Extract the lips from an RGB image and returns a Black&White image 
%   Detailed explanation goes here

    %% Reformat input frame
    resize = 1;
    
    if resize == 0
        resizedFrame = rawFrame;
    elseif resize == 1
        resizedFrame = imresize(rawFrame, [240 320]);
    end
    
    frame = im2double(resizedFrame);
       
    %% Transform RGB to B&W image
    bwFrame = log( frame(:,:,1) ./ (frame(:,:,2) + 1.0e-8) );

    mid3 = bwFrame(:);
    mid4 = sort(mid3);
    greyThres = mid4(end-floor(length(mid4)*0.1));
    
    bwFrame( bwFrame <= greyThres ) = 0;
    bwFrame( bwFrame > greyThres ) = 1;

    figure;imshow(bwFrame);
    
    %% Filter out non-lip connected objects
    [numRows, numCols] = size(bwFrame);
    lipFrame = zeros(numRows, numCols);
    
    cc = bwconncomp(bwFrame);
    labeled = labelmatrix(cc);
    index = zeros(1, cc.NumObjects);
    
    for i = 1:cc.NumObjects
        index(i) = length(cc.PixelIdxList{i});
    end
    
    [b, maxIndex] = max(index);
    lipFrame(find(labeled == maxIndex)) = 1;
    
    [x,y] = find(bwFrame == 1);
    
    thresholdLowX = numRows/4;
    thresholdHighX = numRows*3/4;
    thresholdLowY = min(y);
    thresholdHighY = max(y);
    
    for i = 1:cc.NumObjects
        [x,y] = find(labeled == i);
        midX = mode(x);
        midY = mean(y);
        if (midX < thresholdLowX || midX > thresholdHighX || midY < thresholdLowY || midY>thresholdHighY || length(y)<10000 )
            continue
        else
            lipFrame(find(labeled == i)) = 1;
        end
            
    end
%     lipFrame = lipFrame.* bwFrame;
    
    figure; imshow(lipFrame);
end