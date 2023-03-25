from sklearn.datasets import load_breast_cancer
from sklearn.model_selection import train_test_split
from sklearn.naive_bayes import GaussianNB
from sklearn.metrics import accuracy_score
import numpy as np
import time


# Load dataset
for j in range(1,14):
    num=str(j)
    directory1="C:\\Users\\alexa\\Documents\\Alex\\3D_Geometry\\Apallaktiki\\Plane"+num
    directory2="\\10000"
    directory3="\\normals.txt"
    norm = open(directory1+directory2+directory3, "r")
    normal=norm.read()
    normal=normal.split("\n")
    normal=normal[:-1]
    if(j==1):
        normal_array=np.zeros((len(normal),3))
    for i in range(len(normal)):
        normal[i]=normal[i].split(",")
        normal[i][0]=np.double(normal[i][0])
        normal[i][1]=np.double(normal[i][1])
        normal[i][2]=np.double(normal[i][2])
        if(j==1):
            normal_array[i][0]=normal[i][0]
            normal_array[i][1]=normal[i][1]
            normal_array[i][2]=normal[i][2]
    norm.close()
    if(j!=1):
        normal_array=np.concatenate((normal_array,normal),axis=0)

    directory3="\\labels.txt"  

    label = open(directory1+directory2+directory3, "r")
    labels=label.read()
    labels=labels.split("\n")
    labels=labels[:-1]
    label.close()
    if(j==1):
        label_array=np.zeros(len(labels))
    temp_array =np.zeros(len(labels))
    for i in range(len(labels)):
        if(j==1):
            label_array[i]=int(labels[i])
        if(j!=1):
            temp_array[i]=int(labels[i])
    if(j==1):
        label_array=label_array.astype(np.int32)
    if(j!=1):
        temp_array=temp_array.astype(np.int32)
        label_array=np.concatenate((label_array,temp_array),axis=0)

    directory3="\\Gaussian.txt"
    curv = open(directory1+directory2+directory3, "r")
    curvature=curv.read()
    curvature=curvature.split("\n")
    curvature=curvature[:-1]
    curv.close()
    temp_curvature=np.zeros(len(curvature))
    if(j==1):
        curvature_array=np.zeros(len(curvature))
    for i in range(len(curvature)):
        if(j==1):
            curvature_array[i]=np.double(curvature[i])
        if(j!=1):
            temp_curvature[i]=np.double(curvature[i])
    if(j==1):
        curvature_array=curvature_array.astype(np.double)
    if(j!=1):
        curvature_array=np.concatenate((curvature_array,temp_curvature),axis=0)

data=np.zeros((13*len(curvature),4))
for i in range(len(data)):
    data[i][0]=curvature_array[i]
    data[i][1]=normal_array[i][0]
    data[i][2]=normal_array[i][1]
    data[i][3]=normal_array[i][2]

# Organize our data
label_names = ['wing','not wing']
feature_names = ['curvature','normal.x','normal.y','normal.z']
features = data

# Look at our data
print(label_names)
print(feature_names)

# Split our data
t1=time.time();
train, test, train_labels, test_labels = train_test_split(features,
                                                          label_array,
                                                          test_size=0.307692308,
                                                          random_state=42)

# Initialize our classifier
gnb = GaussianNB()

# Train our classifier
model = gnb.fit(train, train_labels)

# Make predictions
preds = gnb.predict(test)
print(preds)
t2=time.time()-t1
# Evaluate accuracy
print(accuracy_score(test_labels, preds))
print(t2)


    

