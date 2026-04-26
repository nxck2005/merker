import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("build/linux/x86_64/release/orbit.csv")

# position plot
fig, ax = plt.subplots(figsize=(8, 8))
ax.plot(df["xpos"], df["ypos"])
ax.scatter(0, 0, color="blue", s=100, label="Earth")
ax.set_aspect("equal")
ax.set_title("Simulated orbit (position)")
ax.set_xlabel("x (m)")
ax.set_ylabel("y (m)")
ax.legend()
fig.savefig("orbit_pos.png", dpi=150, bbox_inches="tight")
plt.close(fig)

# velocity plot
fig, ax = plt.subplots(figsize=(8, 8))
ax.plot(df["xvel"], df["yvel"])
ax.set_aspect("equal")
ax.set_title("Simulated orbit (velocity)")
ax.set_xlabel("vx (m/s)")
ax.set_ylabel("vy (m/s)")
fig.savefig("orbit_vel.png", dpi=150, bbox_inches="tight")
plt.close(fig)

print("saved orbit_pos.png and orbit_vel.png")
