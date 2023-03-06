function [n,Pop]=ring(pop)
% Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
% 
% Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.
[px,py]=size(pop);
choose=zeros(px,1);
id=ones(px,1);
t=1;
for i=1:px
    if choose(i)==0
        choose(i)=1;
        id(i)=t;
        %         u=pop(i,py:-1:1);
        %         index=find(sum(abs(pop-repmat(u,px,1)),2)==0);
        %         if size(index,1)>0
        %             id(index)=t;
        %             choose(index)=1;
        %         end;
        for ii=1:py-1
            if ii==1
                u=pop(i,py:-1:1);
            else
                u=pop(i,ii:py);
                u=[u,pop(i,2:ii)];
                index=find(sum(abs(pop-repmat(u,px,1)),2)==0);
                if size(index,1)>0
                    id(index)=t;
                    choose(index)=1;
                end;
                u=u(py:-1:1);
            end
            index=find(sum(abs(pop-repmat(u,px,1)),2)==0);
            if size(index,1)>0
                id(index)=t;
                choose(index)=1;
            end;
        end
        
        index1=find(sum(abs(pop-repmat(pop(i,:),px,1)),2)==0);
        if size(index1,1)>1
            id(index1)=t;
            choose(index1)=1;
        end;
        t=t+1;
    end
end
n=t-1;
for i=1:n
    index=find(id==i);
    Pop(i,:)=pop(index(1),:);
end

% end