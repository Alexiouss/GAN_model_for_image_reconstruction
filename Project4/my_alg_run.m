clear;

samples=input("Give number of sampling points: Choose between\n5000\n10000\n15000\n20000\n");
while ((samples~=5000) && (samples~=10000) && (samples~=15000) && (samples~=20000))
    samples=input("Please choose between one of\n5000\n10000\n15000\n20000\n");
end
training_size=input("Choose how many models will be used for training: Number must be between 1-12\n");
while(training_size<1 || training_size>12)
    training_size=input("Please choose a number between 1-12\n");
end
N1=strcat('Models for training:',int2str(training_size))
N2=strcat('Models for testing:',int2str(13-training_size))
tic;
[error,My_labels,Test_labels]=my_alg(samples,training_size);
accuracy=1-error;
execution_time=toc
My_wings=0;
for i=1:(13-training_size)*samples
    if (My_labels(i)==1) 
        My_wings=My_wings+1;
    end
end
True_wings=0;
for i=1:(13-training_size)*samples
    if Test_labels(i)==1
        True_wings=True_wings+1;
    end
end
correct_wings=0;
for i=1:(13-training_size)*samples
    if (My_labels(i)==1 && Test_labels(i)==1) 
        correct_wings=correct_wings+1;
    end
end
if(My_wings>True_wings)
    wing_accuracy=correct_wings/My_wings;
else
    wing_accuracy=correct_wings/True_wings;
end
disp(strcat(num2str(wing_accuracy),"%"));

