import numpy as np
import matplotlib.pyplot as plt
import scipy.io
import random
from mpl_toolkits.mplot3d import Axes3D

data32 = scipy.io.loadmat('data32.mat')
circles = data32['circles']
stars = data32['stars']
x_coor_c=circles[:,0]
y_coor_c=circles[:,1]
z_coor_c=np.zeros(int(circles.size/2))
for i in range(z_coor_c.size):
    z_coor_c[i]=np.power(x_coor_c[i],2)

x_coor_s=stars[:,0]
y_coor_s=stars[:,1]
z_coor_s=np.zeros(int(stars.size/2))
for i in range(z_coor_s.size):
    z_coor_s[i]=np.power(x_coor_s[i],2)


l=0.001
m=0.01
h=0.01

ai=np.zeros(int(stars.size/2))
bi=np.zeros(int(circles.size/2))
sumtotal=[]
for k in range(100):
    sumstars=0
    for i in range(int(stars.size/2)):
        f_hat=0
        for j in range(ai.size):
            K=np.exp(-(np.power(stars[i][0]-stars[j][0],2)+np.power(stars[i][1]-stars[j][1],2))/h)
            f_hat+=ai[j]*K
        for j in range(bi.size):
            K=np.exp(-(np.power(stars[i][0]-circles[j][0],2)+np.power(stars[i][1]-circles[j][1],2))/h)
            f_hat+=bi[j]*K
        if np.sign(f_hat)!=1:
            ai[i]+=1
        sumstars+=np.power(1-f_hat,2)
    sumcircles=0
    for i in range(int(circles.size/2)):
        f_hat=0
        for j in range(ai.size):
            K=np.exp(-(np.power(circles[i][0]-stars[j][0],2)+np.power(circles[i][1]-stars[j][1],2))/h)
            f_hat+=ai[j]*K
        for j in range(bi.size):
            K=np.exp(-(np.power(circles[i][0]-circles[j][0],2)+np.power(circles[i][1]-circles[j][1],2))/h)
            f_hat+=bi[j]*K
        if np.sign(f_hat)!=-1:
            bi[i]-=1
        sumcircles+=np.power(1+f_hat,2)
    sumnorm=0
    for i in range(ai.size):
        for j in range(ai.size):
            K=np.exp(-(np.power(stars[i][0]-stars[j][0],2)+np.power(stars[i][1]-stars[j][1],2))/h)
            sumnorm+=ai[i]*ai[j]*K

    for i in range(ai.size):
        for j in range(bi.size):
            K=np.exp(-(np.power(stars[i][0]-circles[j][0],2)+np.power(stars[i][1]-circles[j][1],2))/h)
            sumnorm+=2*ai[i]*bi[j]*K

    for i in range(bi.size):
        for j in range(bi.size):
            K=np.exp(-(np.power(circles[i][0]-circles[j][0],2)+np.power(circles[i][1]-circles[j][1],2))/h)
            sumnorm+=bi[i]*bi[j]*K

    sumnorm=l*sumnorm
    sumtotal.append((sumstars+sumcircles+sumnorm))

point_size=50
points=np.zeros((point_size,3))
for i in range(point_size):
    x_new=np.random.uniform(-1,1,1)
    y_new=np.random.uniform(-1,1,1)
    z_new=np.power(x_new,2)
    points[i][0]=x_new
    points[i][1]=y_new
    points[i][2]=z_new

colors=['blue','red']

fig = plt.figure()
fig.set_size_inches(10, 10)

ax = fig.add_subplot(111, projection='3d')

for i in range(point_size):
    index = random.randint(0,1)
    f_hat=0
    for j in range(ai.size):
        K=np.exp(-(np.power(points[i][0]-stars[j][0],2)+np.power(points[i][1]-stars[j][1],2))/h)
        f_hat+=ai[j]*K
    for j in range(bi.size):
        K=np.exp(-(np.power(points[i][0]-circles[j][0],2)+np.power(points[i][1]-circles[j][1],2))/h)
        f_hat+=bi[j]*K
    if f_hat<0:
        ax.scatter(points[i][0],points[i][1],points[i][2],s=20,color='blue')
    elif f_hat>0:
        ax.scatter(points[i][0],points[i][1],points[i][2],s=20,color='red')
    elif f_hat==0:
        color1=colors[index]
        plt.plot(points[i][0],points[i][1],color=color1, marker="o", markersize=2)

for i in range(int(circles.size/2)):
    index = random.randint(0,1)
    f_hat=0
    for j in range(ai.size):
        K=np.exp(-(np.power(circles[i][0]-stars[j][0],2)+np.power(circles[i][1]-stars[j][1],2))/h)
        f_hat+=ai[j]*K
    for j in range(bi.size):
        K=np.exp(-(np.power(circles[i][0]-circles[j][0],2)+np.power(circles[i][1]-circles[j][1],2))/h)
        f_hat+=bi[j]*K
    if f_hat<0:
        ax.scatter(x_coor_c[i],y_coor_c[i],z_coor_c[i],s=20,color='blue')
    else:
        ax.scatter(x_coor_c[i],y_coor_c[i],x_coor_c[i],s=20,color='red')

for i in range(int(circles.size/2)):
    index = random.randint(0,1)
    f_hat=0
    for j in range(ai.size):
        K=np.exp(-(np.power(stars[i][0]-stars[j][0],2)+np.power(stars[i][1]-stars[j][1],2))/h)
        f_hat+=ai[j]*K
    for j in range(bi.size):
        K=np.exp(-(np.power(stars[i][0]-circles[j][0],2)+np.power(stars[i][1]-circles[j][1],2))/h)
        f_hat+=bi[j]*K
    if f_hat>0:
        ax.scatter(x_coor_s[i],y_coor_s[i],z_coor_s[i],s=20,color='red')
    else:
        ax.scatter(x_coor_s[i],y_coor_s[i],z_coor_s[i],s=20,color='blue')

x = np.linspace(-1.5,1.5,100)
y=x
y=np.reshape(y,(100,1))
z=x
z=np.reshape(z,(100,1))
x,y=np.meshgrid(x,y)
ax.plot_surface(x,y,z)
plt.show()
