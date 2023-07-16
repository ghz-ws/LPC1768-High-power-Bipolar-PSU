import serial
import time
import numpy
inst=serial.Serial("COM21",115200)
Vset=[-5,5,5,5]          ##V unit
Iset=[100,100,100,100]  ##mA unit
EN=[1,1,1,1]
wait=0.1    ##read wait time. sec unit

buf=f'{Vset[0]*1000:+06}'+f'{Iset[0]:03}'+f'{EN[0]:01}'+f'{Vset[1]*1000:+06}'+f'{Iset[1]:03}'+f'{EN[1]:01}'+f'{Vset[2]*1000:+06}'+f'{Iset[2]:03}'+f'{EN[2]:01}'+f'{Vset[3]*1000:+06}'+f'{Iset[3]:03}'+f'{EN[3]:01}'
inst.write(buf.encode())
time.sleep(wait)
vm=numpy.zeros((4))
im=numpy.zeros((4))
for i in range(4):
    buf=inst.read(6)
    temp=float(buf)
    vm[i]=temp/1000
    buf=inst.read(4)
    im[i]=float(buf)
buf=inst.read(8)
print('Ch1 Vset=',Vset[0],'V, Iset=',Iset[0],'mA, VM=',vm[0],'V, IM=',im[0],'mA\nCh2 Vset=',Vset[1],'V, Iset=',Iset[1],'mA, VM=',vm[1],'V, IM=',im[1],'mA\nCh3 Vset=',Vset[2],'V, Iset=',Iset[2],'mA, VM=',vm[2],'V, IM=',im[2],'mA\nCh4 Vset=',Vset[3],'V, Iset=',Iset[3],'mA, VM=',vm[3],'V, IM=',im[3],'mA\nStatus=',buf)