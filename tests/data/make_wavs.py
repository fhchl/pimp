# %%
import numpy as np
import matplotlib.pyplot as pl
import soundfile as sf
import adafilt
# %%
np.random.seed(123)

samplerate = 1000 # Hertz
rectime = 1 # second
gain = 0.01
x = gain * np.random.randn(rectime * int(samplerate))
x[x > 1] = 1
x[x < -1] = -1


ws = [[0.], [0, 0.5]]

y = []
for i, w in enumerate(ws):
    y = adafilt.olafilt(w, x)
    sf.write(f'y_{i}.wav', y, samplerate, subtype='FLOAT');

sf.write('x.wav', x, samplerate, subtype='FLOAT');

# %%
