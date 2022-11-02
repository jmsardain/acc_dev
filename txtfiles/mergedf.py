import pandas as pd


hits = pd.read_csv("Desktop/EEPP/Hough/txtfiles/hits.txt", sep=" ")
particles = pd.read_csv("Desktop/EEPP/Hough/txtfiles/particles.txt", sep=" ")
merge = pd.merge(hits, particles, on="event")

merge.to_csv('merge.txt', index=False)
