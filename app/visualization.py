import numpy as np
import matplotlib
matplotlib.use('Agg')  # No GUI – faster for headless saving

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd
import os


a = pd.read_csv('./build/data.csv').to_numpy()  
x = a[:, ::2]                                   
y = a[:, 1::2]                                   

total_frames = x.shape[0]
print(f"Rendering {total_frames} frames...")

X = x.reshape(a.shape[0], a.shape[1]//2)
Y = y.reshape(a.shape[0], a.shape[1]//2)

fig, ax = plt.subplots(figsize=(8, 6))
ax.set_xlim(X.min() - 0.5, X.max() + 0.5)
ax.set_ylim(Y.min() - 0.5, Y.max() + 0.5)
ax.grid(True)
ax.set_aspect('equal')


scat = ax.scatter(X[0], Y[0], s=80)

def update(frame):
    scat.set_offsets(np.column_stack([X[frame], Y[frame]]))  # (n_points, 2)
    print(f'Curr Frame: {frame}')
    return scat,

# Ensure the background is drawn before animation (prevents white frames)
fig.canvas.draw()

ani = animation.FuncAnimation(
    fig, update, frames=total_frames, 
    interval=5,          # 50 fps
    blit=True,            # Speed boost
    repeat=False
)

# Create output folder
os.makedirs('./build', exist_ok=True)

# Save with ffmpeg (or pillow if ffmpeg not installed)
ani.save(
    './build/animation.mp4', 
    writer='ffmpeg',       # faster
    bitrate=1000,          # smaller file size
)
print("Animation saved successfully!")