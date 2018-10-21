from keras import backend as K
from keras import optimizers
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten, Conv2D, BatchNormalization, MaxPooling2D


batch_size = 16

input_shape = (8, 7, 10)
model = Sequential()
model.add(Conv2D(32, (8, 7), padding='same', activation='linear', data_format="channels_last", input_shape=input_shape))
model.add(Conv2D(16, (8, 7), padding='same', activation='linear', data_format="channels_last"))
model.add(Conv2D(8, (8, 7), padding='same', activation='linear', data_format="channels_last"))
"""
for layer_num in range(1):
    model.add(Conv2D(64, (8, 7), padding='same', activation='linear', data_format="channels_last"))
    model.add(Conv2D(64, (8, 7), padding='same', activation='linear', data_format="channels_last"))
    model.add(Dropout(.75))
    model.add(Conv2D(64, (8, 7), padding='same', activation='linear', data_format="channels_last"))
    model.add(Conv2D(64, (8, 7), padding='same', activation='linear', data_format="channels_last"))
    model.add(Dropout(.75))
    model.add(BatchNormalization())
"""
model.add(Flatten())
model.add(Dense(32, kernel_initializer='normal', activation='linear'))
#model.add(Dense(1, kernel_initializer='normal', activation='sigmoid'))
model.add(Dense(1, kernel_initializer='normal', activation='relu'))

import numpy as np

def wrangle_board(board):
    rows = list([(board & 0b1111111) >> (7 * i)] for i in range(8))
    unpacked = [[b * 32 for b in a[1:]] for a in np.unpackbits(np.array(rows, dtype=np.uint8), axis=1)]
    reshaped = np.reshape(unpacked, (-1, 7))
    return reshaped

def parse_int(i):
    return int(i, base=16)

def scale_y(i):
    return (i * 1000) // 10
    #return 0 if i < .25 else 1 if i < .5 else 2 if i < .75 else 3
    #return np.tanh(2*i - 1)
    

def wrangle(record):
    record = record.split()
    pieces, teams, cars, knights, rooks, bishops, pawns = map(parse_int, record[:7])
    win_prob = float(record[7])
    return np.array([
        np.moveaxis([
            wrangle_board(~teams & cars),
            wrangle_board(~teams & knights),
            wrangle_board(~teams & rooks),
            wrangle_board(~teams & bishops),
            wrangle_board(~teams & pawns),
            wrangle_board(teams & cars),
            wrangle_board(teams & knights),
            wrangle_board(teams & rooks),
            wrangle_board(teams & bishops),
            wrangle_board(teams & pawns),
        ], 0, 2),
        [scale_y(win_prob)]
    ])

import glob

def generate_records(path, batch_size):
    while 1:
        f = open(path, 'r')
        lines = tuple(f)
        filesize = len(lines)
        n_entries = 0
        while n_entries < (filesize - batch_size):
            xs, ys = zip(*map(wrangle, lines[n_entries : n_entries + batch_size]))
            n_entries += batch_size
            yield np.array(xs), np.array(ys)
        f.close()
    

def lossyloss(y_true, y_pred):
    diff = y_true - y_pred
    return diff**4 / 1000

sgd = optimizers.SGD(lr=.02, clipvalue=.5)
model.compile(optimizer='nadam', loss='mean_squared_error', metrics=['accuracy'])
model.summary()
data_gen = generate_records('alldata.txt', batch_size)
sample_x, sample_y = next(data_gen)
model.fit_generator(data_gen, epochs=30, steps_per_epoch=40, verbose=1)
#score = model.evaluate_generator(data_gen, steps=25)
#print(score)

res = model.predict(sample_x, batch_size=batch_size)
print([(res[i][0], sample_y[i][0]) for i in range(len(sample_y))])

#import time

model.save('models/model1.h5')
with open("models/model1.json", "w") as f:
    f.write(model.to_json())

