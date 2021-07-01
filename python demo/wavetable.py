import numpy as np
from matplotlib import pyplot as plt



class Wavetable:

    def __init__(self, N_samples=256, f_s=44100):
        self.tables = {}
        self.current_sample = 0
        self.N_samples = N_samples
        self.f_s = f_s
        self.t_table_length = N_samples / f_s  # seconds
        self.freq(0)
        self.gen_tables()

#amount 0-1
#symm -1 - +1
    def fold(self, x, amount=0, symm=0):
        y = (x + symm) * (1+amount)
        if y > 2:
            y = y-2
        if y < -2:
            y = y+2

        if y < 1 and y > -1:
            return y
        elif y >= 1:
            return 2 - y
        elif y <= -1:
            return -2 - y

        return x


    def sine(self, i):
        return np.sin(i/self.N_samples * np.pi * 2)

    def tri(self, i):
        if i < self.N_samples/2:
            return -1 + i * 4 / self.N_samples
        else:
            return 3 - i * 4 / self.N_samples

    def square(self, i):
        if i < self.N_samples/2:
            return -1
        else:
            return +1

    def gen_samples(self, f):
        t = []
        for i in range(0, self.N_samples):
            t.append(f(i))
        return t

    def gen_tables(self):
        self.tables['sine'] = self.gen_samples(self.sine)
        self.tables['triangle'] = self.gen_samples(self.tri)
        self.tables['square'] = self.gen_samples(self.square)

    def freq(self, f):
        self.f = f
        self.sample_step = self.f * self.t_table_length

    def get_next_sample(self, type='sine', fold_amount=0, fold_symm=0):
        start_index = int(np.floor(self.current_sample))
        end_index = (start_index + 1) % self.N_samples
        frac = self.current_sample - start_index
        interpolated = self.tables[type][start_index] + frac * (self.tables[type][end_index] - self.tables[type][start_index])
        self.current_sample += self.sample_step
        self.current_sample %= self.N_samples
        return self.fold(interpolated, amount=fold_amount, symm=fold_symm)

    def reset_phase(self):
        self.current_sample = 0;


wtg1 = Wavetable()
wtg2 = Wavetable()
wtg1.freq(10000)
wtg2.freq(5000)

data1 = []
data2 = []
index = range(0, 50)
for j in index:
    data1.append(wtg1.get_next_sample(type = 'triangle', fold_amount=0))
    data2.append(wtg2.get_next_sample(type = 'sine',fold_amount=0, fold_symm=0))

    plt.stem(np.arange(0, len(data1), 1) / wtg1.f_s * 1000, data1)
    #plt.stem(np.arange(0, len(data2), 1) / wtg2.f_s * 1000, data2)
plt.show()
