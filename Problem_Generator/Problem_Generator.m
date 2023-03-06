
% Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
% 
% Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.

% M=2 ,kD=10;
clc;
clear;
%% parmeter
kD=10;
M=2;
k=5;%k'(length of Sa and Sb)
pm=5;% parameter for multi-modality
Pc=0.9;%parameter for confliction among objectives

%%  Generating EmC
if input('Do you have Emc?')==1
    % having Emc
    strr='a280';
    strl=['.\problem\',strr,'.mat'];
    load(strl);
    [n,~]=size(distance);
else
    % don't have Emc
    n=0;
    ditance=0;
end
for i=1:n
    for j=1:i
        for kk=1:M
            cost{1,kk}=distance;
        end
    end
end

%% Generating EmD
% Algorithm 1 Generating EmD (1-13)
for i=1:kD-1
    for j=i+1:kD
        r=rand;
        if r<Pc
            list=rand(1,M);
            sumlist=sum(list);
            for kk=1:M
                cost{1,kk}(n+i,n+j)=list(1,kk)/sumlist;
                cost{1,kk}(n+j,n+i)=cost{1,kk}(n+i,n+j);
            end
        else
            for kk=1:M
                cost{1,kk}(n+i,n+j)=(2/M)*rand;
                cost{1,kk}(n+j,n+i)=cost{1,kk}(n+i,n+j);
            end
        end
    end
end
% Algorithm 1 Generating EmD (14-21)
for i=1:pm
    Sa=randperm(kD-1,k)+1;
    Sb=Sa;
    randomorder=randperm(k-2,2)+1;
    Sb(1,randomorder(1))=Sa(1,randomorder(2));
    Sb(1,randomorder(2))=Sa(1,randomorder(1));
    for j=1:k-1
        for kk=1:M
            cost{1,kk}(n+Sb(1,j),n+Sb(1,j+1))=cost{1,kk}(n+Sa(1,j),n+Sa(1,j+1));
            cost{1,kk}(n+Sb(1,j+1),n+Sb(1,j))=cost{1,kk}(n+Sb(1,j),n+Sb(1,j+1));
        end
    end
end
% Algorithm 1 Generating EmD (22) calculate the scaling factor ps
if n>0
    % having Emc
    cost{1,1}(logical(eye(size(cost{1,1},1))))=inf;
    liminC=sum(min(cost{1,1}(1:n,1:n)));
    cost{1,1}(logical(eye(size(cost{1,1},1))))=0;
    liminD=0;
    for i=1:M
        cost{1,i}(logical(eye(size(cost{1,i},1))))=inf;
        liminD=liminD+sum(min(cost{1,i}(n+1:n+kD,n+1:n+kD)));
        cost{1,i}(logical(eye(size(cost{1,i},1))))=0;
    end
    Ps=ceil(liminC/n*M*kD/liminD);
else
    Ps=1000000;
end
% Algorithm 1 Generating EmD (23)
for i=1:M
    cost{1,i}(n+1:n+kD,n+1:n+kD)=ceil(cost{1,i}(n+1:n+kD,n+1:n+kD)*Ps);
    % cost{1,i}(n+1:n+kD,n+1:n+kD)=cost{1,i}(n+1:n+kD,n+1:n+kD)*Ps;
end
%% Generating EmC-D
if n>0
    % Algorithm 2 Generating EmC-D (1)
    lmax=0;
    for i=1:M
        lmax=max(lmax,max(max(cost{1,i})));
    end
    % Algorithm 2 Generating EmC-D (6-9)
    for i=1:n
        for j=n+1:n+kD
            for kk=1:M
                cost{1,kk}(i,j)=3*lmax+randperm(lmax-1,1)+1;
                cost{1,kk}(j,i)=cost{1,kk}(i,j);
            end
        end
    end
    % Algorithm 2 Generating EmC-D (2)(choose c1,c2)
    if size(ps,2)>1
        %% if EmC have multi-solution
        temp=zeros(n,n);
        for kk=1:size(ps,2)
            for i=1:n-1
                temp(ps(i,kk),ps(i+1,kk))=temp(ps(i,kk),ps(i+1,kk))+1;
                temp(ps(i+1,kk),ps(i,kk))=temp(ps(i,kk),ps(i+1,kk));
            end
            temp(ps(1,kk),ps(n,kk))=temp(ps(1,kk),ps(n,kk))+1;
            temp(ps(n,kk),ps(1,kk))=temp(ps(1,kk),ps(n,kk));
        end
        t=1;
        for i=1:n-1
            for j=i+1:n
                if temp(i,j)==1
                    choose(t,:)=[i,j];
                    t=t+1;
                end
            end
        end
        index=randperm(size(choose,1),1);
        c1=choose(index,1);
        c2=choose(index,2);
        for kk=1:size(ps,2)
            indexc1=find(ps(kk,:)==c1);
            if ps(kk,mod(indexc1,n)+1)==c2 || (indexc1==1 && ps(kk,n)==c2) ||(indexc1~=1 && ps(kk,indexc1-1)==c2)
                ps=ps(kk,:);
                break;
            end
        end
    else
        index=randperm(n,1);
        c1=ps(index,1);
        c2=ps(mod(index,n)+1,1);
    end
    % Algorithm 2 Generating EmC-D (3)(choose d1,d2)
    d1d2=randperm(kD,2);
    d1=d1d2(1,1)+n;
    d2=d1d2(1,2)+n;
    longpoint(1,:)=[c1,c2];
    longpoint(2,:)=[d1,d2];
    % Algorithm 2 Generating EmC-D (5)
    for k=1:M
        cost{1,k}(d1,c1)=randperm(ceil(2*Ps/M),1);
        cost{1,k}(c1,d1)=cost{1,k}(d1,c1);
        cost{1,k}(d2,c2)=randperm(ceil(2*Ps/M),1);
        cost{1,k}(c2,d2)=cost{1,k}(d2,c2);
    end
end
%% Obtaining Pareto optimal solutions£¨find the true PF and PS£©
load([num2str(kD),'.mat'])
if n>0
    index=find(pop(:,1)==longpoint(2,1)-n);
    index1=find(pop(index,kD)==longpoint(2,2)-n);
    t=pop(index(index1),:);
else
    index=find(pop(:,1)==1);
    t=pop(index,:);
end
s=zeros(size(t,1),M);
for j=1:size(t,1)
    for i=1:kD-1
        for k=1:M
            s(j,k)=s(j,k)+cost{1,k}(n+t(j,i),n+t(j,i+1));
        end
    end
end
if n==0
    for j=1:size(t,1)
        for k=1:M
            s(j,k)=s(j,k)+cost{1,k}(n+t(j,kD),n+t(j,1));
        end
    end
end
[FrontNo,MaxFNo] = NDSort(s,100);
[~,f1_c]=find(FrontNo==1);
adaptation2=s(f1_c,:);
pop=t(f1_c,:)+n;
if n>0
    % connect 
    pf1=0;
    for i=1:n-1
        pf1=pf1+cost{1,1}(ps(i),ps(i+1));
    end
    pf1=pf1+cost{1,1}(ps(i+1),ps(1));%%this value should be equal to the value of tsplib providing
    s=adaptation2+pf1;
    for k=1:M
        s(:,k)=s(:,k)-cost{1,k}(longpoint(1,1),longpoint(1,2))+cost{1,k}(longpoint(1,1),longpoint(2,1))+cost{1,k}(longpoint(1,2),longpoint(2,2));
    end
    [FrontNo,MaxFNo] = NDSort(s,100);
    
    MaxFNo
    
    [~,f1_c]=find(FrontNo==1);
    adaptation2=s(f1_c,:);
    Pop1=pop(f1_c,1:kD);
    index=find(ps==longpoint(1,1));
    Pop=[repmat(ps(1:index)',size(Pop1,1),1),Pop1,repmat(ps(index+1:end)',size(Pop1,1),1)];
else
    [~,pop]=ring([pop,pop(:,1)]);
    Pop=pop(:,1:kD);
end
%% check Pareto optimal solutions
Popobj=zeros(size(Pop,1),M);
for j=1:size(Pop,1)
    for i=1:n+kD-1
        for k=1:M
            Popobj(j,k)=Popobj(j,k)+cost{1,k}(Pop(j,i),Pop(j,i+1));
        end
    end
    for k=1:M
        Popobj(j,k)=Popobj(j,k)+cost{1,k}(Pop(j,n+kD),Pop(j,1));
    end
end
unique(adaptation2,'rows')==unique(Popobj,'rows')

[Popobj,index]=sortrows(Popobj);
Pop=Pop(index,:);



[m,~]=size(Popobj);
PF=unique(Popobj,'rows');
plot(PF(:,1),PF(:,2),'ro');
[n,~]=size(PF);
ufo=0;
A=zeros(n,M+1);
for i=1:n
    t=0;
    for j=1:m
        f=1;
        for k=1:M
            if Popobj(j,k)~=PF(i,k)
                f=0;
            end
        end
        if f==1
            t=t+1;
        end
    end
    for k=1:M
        A(i,k)=PF(i,k);
    end
    A(i,M+1)=t;
    ufo=ufo+t;
end


