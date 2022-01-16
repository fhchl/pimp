import os
import numpy as np
from scipy.io import wavfile
from scipy.signal import lfilter

# change directory to parent dir of this script
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# generate some random input
np.random.seed(123)
samplerate = 1000  # Hertz
rectime = 1  # second
gain = 0.01
x = gain * np.random.randn(rectime * int(samplerate))
x[x > 1] = 1
x[x < -1] = -1
wavfile.write('data/x.wav', samplerate, x.astype(np.float32))

# generate output of some sample filters
ws = [[0.5], [0, 0.5], [0, 0.5, -0.5], [0.1, 0.5, -0.5], [0.1, 0.5, -0.5, 1]]
for i, w in enumerate(ws):
    y = lfilter(w, 1, x)
    wavfile.write(f'data/y_{i}.wav', samplerate, y.astype(np.float32))
