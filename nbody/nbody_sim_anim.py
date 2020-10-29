import numpy as np
import pandas as pd
import sys
from matplotlib import pyplot as plt
import matplotlib.animation as animation
plt.style.use('seaborn-pastel')

filename = "./nbody.csv"
try: 
     hist = pd.read_csv(filename)
except IOError as e:
    print(e)
    sys.exit()

outname = "nbody_"
niter = max(hist["iteration"])
ran = (-40, 40)
fps = 15
inter = 100
writer = animation.PillowWriter(fps=fps)
xlist = [hist[hist["iteration"] == i]["x"].tolist() for i in range(niter)]
ylist = [hist[hist["iteration"] == i]["y"].tolist() for i in range(niter)]
refxlist = [hist[hist["iteration"] == i]["reference_x"].tolist() for i in range(niter)]
refylist = [hist[hist["iteration"] == i]["reference_y"].tolist() for i in range(niter)]
approxxlist = [hist[hist["iteration"] == i]["approx_x"].tolist() for i in range(niter)]
approxylist = [hist[hist["iteration"] == i]["approx_y"].tolist() for i in range(niter)]

fig = plt.figure(figsize=(8, 8), dpi=100)
ax = plt.axes(xlim=ran, ylim=ran)
point, = ax.plot([], [], ".", markersize=4, alpha=.5)

def init_ref():
    point.set_data(refxlist[0], refylist[0])
    return point,

def animate_ref(i):
    x = refxlist[i]
    y = refylist[i]
    point.set_data(x, y)
    return point,

print("Generating animation of reference simulation.\n")
ref = animation.FuncAnimation(fig, animate_ref, init_func=init_ref, frames=niter, interval=inter, blit=True)
ref.save(outname+'_ref.gif', writer=writer)

# def init():
#     point.set_data(xlist[0], ylist[0])
#     return point,
# def animate(i):
#     x = xlist[i]
#     y = ylist[i]
#     point.set_data(x, y)
#     return point,

# print("Generating animation of simulation using tree code with distance threshold 0.0.\n")
# theta0 = animation.FuncAnimation(fig, animate, init_func=init, frames=niter, interval=inter, blit=True)
# theta0.save(outname+'_tree_theta0.gif', writer=writer)

def init_approx():
    point.set_data(approxxlist[0], approxylist[0])
    return point,

def animate_approx(i):
    x = approxxlist[i]
    y = approxylist[i]
    point.set_data(x, y)
    return point,

print("Generating animation of simulation using tree code with distance threshold 1.0.\n")
theta1 = animation.FuncAnimation(fig, animate_approx, init_func=init_approx,frames=niter, interval=inter, blit=True)
theta1.save(outname+'_tree_theta1.gif', writer=writer)