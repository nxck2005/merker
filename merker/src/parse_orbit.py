import matplotlib

matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
import pandas as pd
from mpl_toolkits.mplot3d import Axes3D

df = pd.read_csv("build/linux/x86_64/release/orbit.csv")

# position plot 3D
fig = plt.figure(figsize=(8, 8))
ax = fig.add_subplot(111, projection="3d")
ax.plot(df["xpos"], df["ypos"], df["zpos"])
ax.scatter(0, 0, 0, color="blue", s=1000, label="Earth")
ax.set_title("Simulated orbit (position)")
ax.set_xlabel("x (m)")
ax.set_ylabel("y (m)")
ax.set_zlabel("z (m)")
ax.legend()
fig.savefig("orbit_pos.png", dpi=150, bbox_inches="tight")
# plt.show()
plt.close(fig)

# velocity plot 3D
fig = plt.figure(figsize=(8, 8))
ax = fig.add_subplot(111, projection="3d")
ax.plot(df["xvel"], df["yvel"], df["zvel"])
ax.set_title("Simulated orbit (velocity)")
ax.set_xlabel("vx (m/s)")
ax.set_ylabel("vy (m/s)")
ax.set_zlabel("vz (m/s)")
fig.savefig("orbit_vel.png", dpi=150, bbox_inches="tight")
# plt.show()
plt.close(fig)

print("saved orbit_pos.png and orbit_vel.png")
