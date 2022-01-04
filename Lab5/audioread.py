import numpy as np
from scipy.io import wavfile
from numpy import asarray
from numpy import savetxt

samplerate, data = wavfile.read('original.wav')
print(samplerate)
print(data.shape)
print(data[0,:].shape)
dataX = data[:,0]
np.reshape(dataX, dataX.shape[0])
print(dataX.shape)
savetxt('data.csv', (dataX), delimiter=',')