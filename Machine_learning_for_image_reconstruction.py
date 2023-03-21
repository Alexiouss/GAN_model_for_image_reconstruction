import numpy as np
import matplotlib.pyplot as plt
import math
import scipy.io
from numpy import random


class NN():

    def __init__(self):
        return

    def ReLu(self, x):
        temp = np.where(x > 0, x, 0)
        return temp
    
    def ReLu_der(self, x):
        temp = np.where(x > 0, 1, 0)
        temp = np.reshape(temp, (len(temp), 1))
        return temp

    def Sigmoid(self, output):
        return 1 / (1 + np.exp(output))

    def Sigmoid_der(self,x):
        return -np.exp(x)/(np.power((1+np.exp(x)),2))

    def forward_pass(self,Z,A1,A2,B1,B2):
        W1 = np.dot(A1,Z)+B1
        Z1 = self.ReLu(W1)
        W2 = np.dot(A2,Z1)+B2
        X = self.Sigmoid(W2)
        return W1,Z1,W2,X

    def calc_fi_x(self,T,X,Xn,N):
        temp = np.reshape(Xn,(1,N))
        norm_power = np.sum(np.power((np.dot(T,X).T-temp),2))
        fi_x = np.log10(norm_power)
        return fi_x

    def calc_fi_x_der(self,T,X,Xn,N):
        temp = np.reshape(Xn,(1,N))
        num = 2*np.dot((np.dot(T,X).T-temp),T)
        den = np.sum(np.power((np.dot(T,X).T-temp),2))
        return num/den
    
    def calc_Jz(self,N,T,X,Xn,Z):
        fi_x=self.calc_fi_x(T,X,Xn,N)
        Jz = N*fi_x+np.sum(np.power(Z,2))
        return Jz

    def calc_Jz_der(self,N,u0,Z):
        return N*u0+2*Z

    def training(self,Xn,A1,A2,B1,B2,T,Z,N,rep):
        learnig_rate=0.001
        cost=[]
        l_adams=0.1
        for i in range(rep):
            Parameters=self.forward_pass(Z,A1,A2,B1,B2)
            W1=Parameters[0]
            W2=Parameters[2]
            X =Parameters[3]
           
            Jz = self.calc_Jz(N,T,X,Xn,Z)
            u2=self.calc_fi_x_der(T,X,Xn,N)
            v2=np.multiply(u2.T,self.Sigmoid_der(W2))
            u1 = np.dot(A2.T,v2)
            v1 = np.multiply(u1,self.ReLu_der(W1))
            u0 = np.dot(A1.T,v1)
            Jz_der = self.calc_Jz_der(N,u0,Z)
            if(i==0):
                power=np.power(Jz_der,2)
            else :
                power=(1-l_adams)*power+l_adams*np.power(Jz_der,2)

            Z= Z-learnig_rate*Jz_der/(np.sqrt(power+0.0000001))
            cost.append(Jz)
        return X,Jz,cost


while True:
    print("Πατήστε 1 για το πρώτο ερώτημα, 2 για το δεύτερο και 3 για το τρίτο.\nΠατήστε 0 για έξοδο")
    Nn=NN()
    data21 = scipy.io.loadmat('data21.mat')
    A1 = data21['A_1']
    B1 = data21['B_1']
    A2 = data21['A_2']
    B2 = data21['B_2']
    choice=input()
    if choice=='0':
        break
    if choice=='1':
        N=10
        Z=np.random.randn(N,100)
        X = Nn.forward_pass(Z,A1,A2,B1,B2)
        X=X[-1].T
        for i in range(1,101):
            X_2D = np.reshape(X[i-1],(28,28))
            plt.subplot(10,10,i)
            plt.axis('off')
            plt.imshow(X_2D.T)
        plt.show()


    if choice=='2':
        
        data22 = scipy.io.loadmat('data22.mat')
        Xi = data22['X_i']
        Xn = data22['X_n']
        print("Διαλέξτε αριθμό φωτογραφίας μεταξύ 1-4")
        im_number=int(input())
        while True:
            if im_number<1 or im_number>4:
                print("Λάθος αριθμός παρακαλώ εισάγεται αριθμό από 1-4")
                im_number=int(input())
            if im_number>=1 and im_number<=4:
                break

        print('Διαλέξτε ποσότητα πληροφορίας που θέλετε να κρατήσετε\nΑποδεκτές τιμές 500,400,350,300')
        N=int(input())
        if N==500:
            rep=2000
        elif N==400:
            rep=3000
        elif N==350:
            rep=4000
        elif N==300:
            rep=5000
        else:
            rep=5000
        Set=10
        Xn=Xn.T
        Xi=Xi.T
        I=np.eye(N,dtype=float)
        zeros=np.zeros((N,784-N))
        T=np.concatenate((I,zeros),axis=1)
        Xn_0=np.dot(T,Xn[im_number-1])
        Jz=[]
        for i in range(20):
            Z=np.random.randn(Set,1)
            X = Nn.training(Xn_0,A1,A2,B1,B2,T,Z,N,rep)
            if i ==0:
                Z_all=Z
                X_all=X[0]
                costs=[X[2]]
            else:
                Z_all=np.concatenate((Z_all,Z),axis=1)
                X_all=np.concatenate((X_all,X[0]),axis=1)
                costs=np.concatenate((costs,[X[2]]),axis=0)
            Jz.append(X[1])
        minn=Jz[0]
        min_index=0
        for i in range(1,len(Jz)):
            if(Jz[i]<minn):
                minn=Jz[i]
                min_index=i
        print(Z_all.T[min_index])
        print(Jz[min_index])
        x = np.linspace(1, rep,rep )
        plt.plot(x, costs[min_index])
        plt.show()
        X_2D = np.reshape(Xi[im_number-1],(28,28))
        plt.subplot(1,3,1)
        plt.axis('off')
        plt.imshow(X_2D.T)

        zero_pad=np.zeros(784-N)
        Xn_0_pad=np.concatenate((Xn_0,zero_pad))
        X_2D = np.reshape(Xn_0_pad,(28,28))
        plt.subplot(1,3,2)
        plt.axis('off')
        plt.imshow(X_2D.T)

        X_2D = np.reshape(X_all.T[min_index],(28,28))
        plt.subplot(1,3,3)
        plt.axis('off')
        plt.imshow(X_2D.T)
        
        plt.show()

    if choice=='3':
        
        N=49
        Set=10
        data23 = scipy.io.loadmat('data23.mat')
        Xi = data23['X_i']
        Xn = data23['X_n']
        print("Διαλέξτε αριθμό φωτογραφίας μεταξύ 1-4")
        im_number=int(input())
        while True:
            if im_number<1 or im_number>4:
                print("Λάθος αριθμός παρακαλώ εισάγεται αριθμό από 1-4")
                im_number=int(input())
            if im_number>=1 and im_number<=4:
                break
        Xi=Xi.T
        Xn=Xn.T
        T=np.zeros((49,784))
        k=0
        
        for i in range(49):
            if(i%7==0 and i!=0):
                k+=112
            for j in range(4):
                for t in range(4):
                    T[i][((28*j)+(t+4*(i%7)))+k]=1/16
        
        Jz=[]
        rep=5000
        for i in range(20):
            Z=np.random.randn(Set,1)
            X = Nn.training(Xn[im_number-1],A1,A2,B1,B2,T,Z,N,rep)
            if i ==0:
                Z_all=Z
                X_all=X[0]
                costs=[X[2]]
            else:
                Z_all=np.concatenate((Z_all,Z),axis=1)
                X_all=np.concatenate((X_all,X[0]),axis=1)
                costs=np.concatenate((costs,[X[2]]),axis=0)
            Jz.append(X[1])
        minn=Jz[0]
        min_index=0
        for i in range(1,len(Jz)):
            if(Jz[i]<minn):
                minn=Jz[i]
                min_index=i
        print(Jz[min_index])
        print(min_index)
        print(min(costs[min_index]))
        print(Z_all.T[min_index])
        x = np.linspace(1, rep,rep )
        plt.plot(x, costs[min_index])
        plt.show()

        X_2D = np.reshape(Xi[im_number-1],(28,28))
        plt.subplot(1,3,1)
        plt.axis('off')
        plt.imshow(X_2D.T)

        X_2D = np.reshape(Xn[im_number-1],(7,7))
        plt.subplot(1,3,2)
        plt.axis('off')
        plt.imshow(X_2D.T)


        X_2D = np.reshape(X[0],(28,28))

        plt.subplot(1,3,3)
        plt.axis('off')
        plt.imshow(X_2D.T)
        plt.show()
