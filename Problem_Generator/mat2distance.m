function mat2distance(str1,distance1,distance2)
% Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
% 
% Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.
fop = fopen(str1,'w');
M=size(distance1,1);   %%f1
N=size(distance1,2);
for m = 1:M
    for n = 1:N
        fprintf(fop, '%s ', mat2str( distance1(m,n) ) );
    end
    fprintf(fop, '\n');
end
fprintf(fop, '\n');
M=size(distance2,1);   %%f2
N=size(distance2,2);
%[M,N] = size(map);
for m = 1:M
    for n = 1:N
        fprintf(fop, '%s ', mat2str( distance2(m,n) ) );
    end
    fprintf(fop, '\n');
end
fclose(fop);
end