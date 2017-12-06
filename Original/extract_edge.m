function extract_edge(raw_binary,raw_image)
    
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
    figure;imshow(raw_image);
    hold on;
    for i=1:size(dot,1)
        plot(dot(i,1),dot(i,2),'go');
        plot(dot(i,1),dot(i,3),'go');
    end

end
    