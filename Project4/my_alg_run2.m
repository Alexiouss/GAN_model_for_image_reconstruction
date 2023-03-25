clear;
plane=input("Give plane\n");
samples=input("Give samples\n");
[error,My_labels,Labels,mm]=my_alg2(plane,samples);
accuracy=1-error;
My_wings=0;
for i=1:samples
    if (My_labels(i)==1) 
        My_wings=My_wings+1;
    end
end
True_wings=0;
for i=1:samples
    if Labels(i)==1
        True_wings=True_wings+1;
    end
end
correct_wings=0;
for i=1:samples
    if (My_labels(i)==1 && Labels(i)==1) 
        correct_wings=correct_wings+1;
    end
end
if(My_wings>True_wings || plane==1 || plane==2)
    wing_accuracy=correct_wings/My_wings;
else
    wing_accuracy=correct_wings/True_wings;
end
disp("The accuracy for the recognition of the wing is:")
disp(strcat(num2str(wing_accuracy),"%"));
disp("The accuracy for the whole model is:")
disp(strcat(num2str(accuracy),"%"));

