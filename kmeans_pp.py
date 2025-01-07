import pandas as pd
import numpy as np

def choose_move_initial_centroids(points, k):
    '''
    Implementing the kmeans++ initialization.
    Chosen centroids will be moved inplace to the first k rows of the points dataframe.
    Function will return a list of the original indices of the chosen centroids.
    '''
    np.random.seed(1234)
    points_num = np.shape(points)[0]
    original_indices = []

    for i in range(k):
        if i == 0:
            random_index = np.random.choice(range(points_num)) # First centroid is chosen randomly.  

        else:
            # Calculates the distance of each point to the nearest centroid.
            distances = np.zeros(points_num - i)
            for ind, point in points.iloc[i:].iterrows(): # Iterates over all non-chosen points.
                distances[ind - i] = np.min(np.linalg.norm(points.iloc[:i] - point, axis=1))
            # Chooses the next centroid with probability proportional to the distances.
            probabilities = distances / np.sum(distances)
            random_index = np.random.choice(points_num - i, p=probabilities)
            random_index += i # Adjusts the index from the distances dataframe to the original one.

        original_indices.append(random_index)
        points.iloc[i], points.iloc[random_index] = points.iloc[random_index], points.iloc[i]
        # Swaps the indices of the swapped points, so the distance calculations will still work as planned.
        points.rename(index={random_index: i, i: random_index}, inplace=True)
        
    return original_indices


def main():
    # TODO

if __name__ == "__main__":
    main()