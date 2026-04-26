import matplotlib.pyplot as plt
import pandas as pd

df = pd.read_csv("build/linux/x86_64/release/orbit.csv")

plt.figure(figsize=(8, 8))
plt.plot(df["xpos"], df["ypos"])
plt.scatter(0, 0, color="blue", s=100, label="Earth")  # Earth at origin
plt.axis("equal")  # important — prevents ellipse looking squashed
plt.title("Simulated orbit (pos)")
plt.legend()
plt.show()
