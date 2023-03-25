function [error,My_labels,Concatenated_Labels,mean_normals_wings]=my_alg2(i,samples)
FileName_1 = 'Gaussian.txt';
FileName_2 = 'normals.txt';
FileName_3 = 'labels.txt';
Plane=int2str(i);
FolderName = 'C:\Users\alexa\Documents\Alex\3D_Geometry\Apallaktiki\Plane';
FolderName = strcat(FolderName,Plane,'\',int2str(samples));
File1 = fullfile(FolderName, FileName_1);
File2 = fullfile(FolderName,FileName_2);
File3 = fullfile(FolderName,FileName_3);
Gaussian=load(File1);
Normals=load(File2);
Labels=load(File3);
Concatenated_Gauss=Gaussian;
Concatenated_Normals=Normals;
Concatenated_Labels=Labels;
Gaussian_wings=[];
Normals_wings=[];
Normals_wings2=[];
angle=0;
angle_x=0;
n=samples;
for i=1:n
    if(Concatenated_Labels(i)==1 && Concatenated_Gauss(i)<=255)
        v1=[Concatenated_Normals(i,1);Concatenated_Normals(i,2);Concatenated_Normals(i,3)];
        v2=[0;0;1];
        v3=[1;0;0];
        Gaussian_wings=[Gaussian_wings;Concatenated_Gauss(i)];
        angle=rad2deg(acos(dot(v2, v1) / (norm(v1) * norm(v2))));
        angle_x=rad2deg(acos(dot(v3, v1) / (norm(v1) * norm(v3))));
        Normals_wings=[Normals_wings;v1(2)];
        Normals_wings2=[Normals_wings2;angle_x];
    end
end
mean_gauss_wings=mean(Gaussian_wings);
min_gauss_wings=min(Gaussian_wings);
max_gauss_wings=max(Gaussian_wings);
min_normals_wings=min(Normals_wings);
max_normals_wings=max(Normals_wings);
mean_normals_wings=mean(Normals_wings);
mean_angle_x=mean(Normals_wings2);
n=samples;
My_labels=zeros(n,1);
test_angle=[];
for i=1:n
    v1=[Concatenated_Normals(i,1),Concatenated_Normals(i,2),Concatenated_Normals(i,3)];
    v2=[0;0;1];
    v3=[1;0;0];
    angle=rad2deg(acos(dot(v2, v1) / (norm(v1) * norm(v2))));
    angle_x=rad2deg(acos(dot(v3, v1) / (norm(v1) * norm(v3))));
    test_angle=[test_angle;angle];
    if((Concatenated_Gauss(i)>=(mean_gauss_wings-(3*abs(mean_gauss_wings))) && Concatenated_Gauss(i)<=(mean_gauss_wings+(3*abs(mean_gauss_wings)))) ...
            && (v1(2)<=-1+abs(mean_normals_wings) || v1(2)>1-abs(mean_normals_wings)))
        My_labels(i)=1;
    end
end
error=0;
for i=1:n
    if(My_labels(i)~=Concatenated_Labels(i))
        error=error+1;
    end
end
error=error/n;
end