function [error,My_labels,Test_Labels]=my_alg(samples,train_size)
FileName_1 = 'Gaussian.txt';
FileName_2 = 'normals.txt';
FileName_3 = 'labels.txt';
for i=1:train_size
    Plane=int2str(i);
    FolderName = 'C:\Users\alexa\Documents\Alex\3D_Geometry\Apallaktiki\Plane';
    FolderName = strcat(FolderName,Plane,'\',int2str(samples));
    File1 = fullfile(FolderName, FileName_1);
    File2 = fullfile(FolderName,FileName_2);
    File3 = fullfile(FolderName,FileName_3);
    Gaussian=load(File1);
    Normals=load(File2);
    Labels=load(File3);
    if i==1
        Concatenated_Gauss=Gaussian;
        Concatenated_Normals=Normals;
        Concatenated_Labels=Labels;
    else
        Concatenated_Gauss=[Concatenated_Gauss;Gaussian];
        Concatenated_Normals=[Concatenated_Normals;Normals];
        Concatenated_Labels = [Concatenated_Labels;Labels];
    end
end
Gaussian_wings=[];
Normals_wings=[];
Normals_wings2=[];
angle=0;
n=train_size*samples;
for i=1:n
    if(Concatenated_Labels(i)==1 && Concatenated_Gauss(i)<=255)
        v1=[Concatenated_Normals(i,1);Concatenated_Normals(i,2);Concatenated_Normals(i,3)];
        v2=[0;0;1];
        v3=[1;0;0];
        Gaussian_wings=[Gaussian_wings;Concatenated_Gauss(i)];
        angle=rad2deg(acos(dot(v2, v1) / (norm(v1) * norm(v2))));
        Normals_wings=[Normals_wings;v1(2)];
    end
end
mean_gauss_wings=mean(Gaussian_wings);
min_gauss_wings=min(Gaussian_wings);
max_gauss_wings=max(Gaussian_wings);
min_normals_wings=min(Normals_wings);
max_normals_wings=max(Normals_wings);
mean_normals_wings=mean(Normals_wings);
for i=train_size+1:13
    Plane=int2str(i);
    FolderName = 'C:\Users\alexa\Documents\Alex\3D_Geometry\Apallaktiki\Plane';
    FolderName = strcat(FolderName,Plane,'\',int2str(samples));
    File1 = fullfile(FolderName, FileName_1);
    File2 = fullfile(FolderName,FileName_2);
    File3 = fullfile(FolderName,FileName_3);
    Gaussian=load(File1);
    Normals=load(File2);
    Labels=load(File3);
    if i==train_size+1
        Test_Gauss=Gaussian;
        Test_Normals=Normals;
        Test_Labels=Labels;
    else
        Test_Gauss=[Test_Gauss;Gaussian];
        Test_Normals=[Test_Normals;Normals];
        Test_Labels = [Test_Labels;Labels];
    end
end
n=(13-train_size)*samples;
My_labels=zeros(n,1);
test_angle=[];
for i=1:n
    v1=[Test_Normals(i,1),Test_Normals(i,2),Test_Normals(i,3)];
    v2=[0;0;1];
    v3=[1;0;0];
    angle=rad2deg(acos(dot(v2, v1) / (norm(v1) * norm(v2))));
    test_angle=[test_angle;angle];
    if((Test_Gauss(i)>=(mean_gauss_wings-(3*mean_gauss_wings)) && Test_Gauss(i)<=(mean_gauss_wings+(3*mean_gauss_wings))) ...
            && (v1(2)<=-1+abs(mean_normals_wings) || v1(2)>1-abs(mean_normals_wings)))
        My_labels(i)=1;
    end
end
error=0;
for i=1:n
    if(My_labels(i)~=Test_Labels(i))
        error=error+1;
    end
end
error=error/n;
end