import numpy as np
from scipy.io.wavfile import write
import csv

with open('echo.csv', newline='') as csvfile:
    data = list(csv.reader(csvfile))

wav_data = np.array(data[0])

float_array = wav_data.astype(float) * 10000

write("echo.wav", 48000, float_array.astype(np.int16))