import numpy as np
import matplotlib.pyplot as plt
import scipy.io
import random
from mpl_toolkits.mplot3d import Axes3D

data32 = scipy.io.loadmat('data32.mat')
circles = data32['circles']
stars = data32['stars']
l=0.01
h=0.01

ai=np.zeros(int(stars.size/2))
bi=np.zeros(int(circles.size/2))
sumtotal=[]
for k in range(50):
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
print(sumtotal[-1])

point_size=1000
points=np.zeros((point_size,2))
for i in range(point_size):
    x_new=np.random.uniform(-1,1,1)
    y_new=np.random.uniform(-1,1,1)
    points[i][0]=x_new
    points[i][1]=y_new

colors=['blue','red']

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
        plt.plot(points[i][0],points[i][1],color='blue', marker="o", markersize=2)
    elif f_hat>0:
        plt.plot(points[i][0],points[i][1],color='red', marker="o", markersize=2)
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
        plt.plot(circles[i][0],circles[i][1],color='blue', marker="o", markersize=5)
    else:
        plt.plot(circles[i][0],circles[i][1],color='red', marker="o", markersize=5)

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
        plt.plot(stars[i][0],stars[i][1],color='red', marker="o", markersize=5)
    else:
        plt.plot(stars[i][0],stars[i][1],color='blue', marker="o", markersize=5)


plt.plot(circles[:,0],circles[:,1],color="blue")
plt.plot(stars[:,0],stars[:,1],color="red")

x = np.linspace(-1.1,1.1,50)
y=np.power(x,2)
y=np.reshape(y,(50,1))
plt.plot(x,y)

plt.show()
plt.plot(sumtotal)
plt.show()
