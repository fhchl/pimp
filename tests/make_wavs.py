# %%
import numpy as np
import matplotlib.pyplot as pl
import soundfile as sf
import adafilt
import measuretf.signals
# %%
np.random.seed(123)

samplerate = 1000 # Hertz
rectime = 1 # second
gain = 0.01
x = gain * np.random.randn(rectime * int(samplerate))
x[x > 1] = 1
x[x < -1] = -1


ws = [[0.5], [0, 0.5], [0, 0.5, -0.5], [0.1, 0.5, -0.5]]

y = []
for i, w in enumerate(ws):
    y = adafilt.olafilt(w, x)
    sf.write(f'data/y_{i}.wav', y, samplerate, subtype='FLOAT');

sf.write('data/x.wav', x, samplerate, subtype='FLOAT');

# %%
sr = 1000
sweep = measuretf.signals.exponential_sweep(
    1, sr, tfade=0, f_start=None, f_end=None, maxamp=0.5, post_silence=0.5
)
sf.write("data/sweep.wav", sweep, sr, subtype="FLOAT")
# %%
w = [0, 0.5, -0.5]
y = adafilt.olafilt(w, sweep)
sf.write("data/sweep_w.wav", y, sr, subtype="FLOAT")