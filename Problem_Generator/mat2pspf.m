function mat2pspf(str1,pop,popobj)
% Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
% 
% Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.
fop = fopen(str1,'w');
M=size(pop,1);   %%Pop
N=size(pop,2);
for m = 1:M
    for n = 1:N
        fprintf(fop, '%s ', mat2str( pop(m,n) ) );
    end
    fprintf(fop, '\n');
end
fprintf(fop,'0');
fprintf(fop,'\n');
M=size(popobj,1);   %%Popobj
N=size(popobj,2);
%[M,N] = size(map);
for m = 1:M
    for n = 1:N
        fprintf(fop, '%s ', mat2str( popobj(m,n) ) );
    end
    fprintf(fop, '\n');
end
fclose(fop);
end