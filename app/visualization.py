import numpy as np
import matplotlib
matplotlib.use('Agg') #should be faster, when no gui?

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd


a = pd.read_csv('./build/data.csv').to_numpy()  
x = a[:, ::2] # we get only x                                  
y = a[:, 1::2] # we get only y                             

total_frames = x.shape[0]
print(f"Rendering {total_frames} frames...")

X = x.reshape(a.shape[0], a.shape[1]//2)
Y = y.reshape(a.shape[0], a.shape[1]//2)

fig, ax = plt.subplots(figsize=(8, 6))
ax.set_xlim(-1, 1) #find how to get BOX Size from constants.h
ax.set_ylim(-1, 1)
ax.grid(True)
ax.set_aspect('equal')


scat = ax.scatter(X[0], Y[0], s=80) #initial plot

def update(frame):
    scat.set_offsets(np.column_stack([X[frame], Y[frame]])) # only updates if needed, faster
    print(f'Curr Frame: {frame}')
    return scat,


ani = animation.FuncAnimation(
    fig, update, frames=total_frames, 
    interval=20,         
    blit=True,  #faster?           
    repeat=False
)


ani.save(
    './build/animation1.mp4', 
    writer='ffmpeg',       # faster?
    bitrate=1000,          
)
print("Animation saved successfully!")