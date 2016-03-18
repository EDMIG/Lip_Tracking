function lipLine = edgeLocalization(raw_binary,raw_image)
    
    numPoints = 15;
    I = zeros(size(raw_binary));
    [m,n] = size(I);
    gap = floor(n/numPoints);
    dot = [];
    for i = 1:gap:n
        mid = find(raw_binary(:,i) == 1);
        if (length(mid) ~= 0)
            dot = [dot;i,min(mid),max(mid)];
        end
    end
    
    lipLine = [ dot(:,1) , dot(:,2) ;
                flip(dot(:,1)) , flip(dot(:,3)) 
                dot(1,1) , dot(1,2)];
    
%     figure;imshow(raw_image);
%     hold on;
%     
%     plot(lipLine(:,1), lipLine(:,2), '-go');
%            
%     plot(dot(:,1),dot(:,2),'-go');
%     plot(dot(:,1),dot(:,3),'-go');
    
%     for i=1:size(dot,1)
%         plot(dot(i,1),dot(i,2),'-go');
%         plot(dot(i,1),dot(i,3),'-go');
%     end

end
    