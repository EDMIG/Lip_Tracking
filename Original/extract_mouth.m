%Only keep mouth by using point connection method
function newImage = findMouth(rawImageInput)
    mid2 = double(rawImageInput);
    Imgdat = log((mid2(:,:,1))./(mid2(:,:,2)+1.0e-8));
     %  Imgdat(Imgdat<0)=0;   Imgdat(Imgdat>1)=1;
    
    mid3 = Imgdat(:);
    mid4 = sort(mid3);
    th1 = mid4(end-floor(length(mid4)*0.14)); %to increase threshold low the number, and vise versa. default 0.1 (for picture 2:use 0.1 is good; picture 3: use 0.14 is good)
    th2 = mid4(end-floor(length(mid4)*0.1));
    Imgdat(Imgdat<th1)=0;  Imgdat(Imgdat>th2)=th2;    
    Imgdat = (Imgdat-min(min(Imgdat)))/(max(max(Imgdat))-min(min(Imgdat)));  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
%     Imgdat = findMouth(Imgdat);

%     figure;imshow(Imgdat);
    newImage = [];
    I = Imgdat;
    cc = bwconncomp(I);
    labeled = labelmatrix(cc);
    index = [];
    for i = 1:cc.NumObjects
    index(i) = length(cc.PixelIdxList{i});
    end
    [b,maxIndex] = max(index);
    [m,n] = size(I);

    newImage = zeros(m,n);
    newImage(find(labeled == maxIndex)) = 1;
    [x,y] = find(newImage == 1);
    
    thresholdLowX = m/4;
    thresholdHighX = m*3/4;
    thresholdLowY = min(y);
    thresholdHighY = max(y);
    
%     threshold = m/3;
    for i = 1:cc.NumObjects
        [x,y] = find(labeled == i);
        midX = mode(x);
        midY = mean(y);
        if (midX < thresholdLowX || midX > thresholdHighX || midY < thresholdLowY || midY>thresholdHighY || length(y)<0.85*b )
            continue
        else
            newImage(find(labeled == i)) = 1;
        end
            
    end
    newImage = newImage.*Imgdat;
end