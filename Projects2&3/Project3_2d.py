import numpy as np
import matplotlib.pyplot as plt
import scipy.io
import random

data33=scipy.io.loadmat('data33.mat')
points=data33['X']
x_coor=points[0,:]
y_coor=points[1,:]
x_coor=np.reshape(x_coor,(200,1))
y_coor=np.reshape(y_coor,(200,1))
points=np.concatenate((x_coor,y_coor),axis=1)
z1=np.random.uniform(-2,2,2)
z2=np.random.uniform(-2,2,2)
print(z1)
print(z2)
while True:
    error=0
    cluster1=[]
    cluster2=[]
    for i in range (200):
        dst1 = np.power(z1[0]-points[i,0],2)+np.power(z1[1]-points[i,1],2)
        dst2 = np.power(z2[0]-points[i,0],2)+np.power(z2[1]-points[i,1],2)
        if dst1<dst2:
            cluster1.append(i)
        else:
            cluster2.append(i)
    z1_t=z1
    z2_t=z2
    z1_x=0
    z1_y=0
    for i in cluster1:
        if i >=100:
            error+=1
        z1_x+=points[i,0]
        z1_y+=points[i,1]
    z1_x=z1_x/len(cluster1)
    z1_y=z1_y/len(cluster1)

    z1=np.array((z1_x,z1_y))
    
    z2_x=0
    z2_y=0
    for i in cluster2:
        if i < 100:
            error+=1
        z2_x+=points[i,0]
        z2_y+=points[i,1]
    z2_x=z2_x/len(cluster2)
    z2_y=z2_y/len(cluster2)
    z2=np.array((z2_x,z2_y))
    error=error/200

    if error>=0.5:
        error=1-error
        z1,z2=z2,z1
        z1_t,z2_t=z2_t,z1_t
    print(error)
    if([z1_t[0],z1_t[1]]==[z1[0],z1[1]] and [z2_t[0],z2_t[1]]==[z2[0],z2[1]]):
        break
    

plt.scatter(points[0:99,0],points[0:99,1],s=20,color='blue')
plt.scatter(points[100:199,0],points[100:199,1],s=20,color='red')
for i in cluster1:
    plt.scatter(points[i,0],points[i,1],s=10,color='blue')
for i in cluster2:
    plt.scatter(points[i,0],points[i,1],s=10,color='red')
plt.scatter(z1[0],z1[1],color='yellow')
plt.scatter(z2[0],z2[1],color='green')
plt.show()
