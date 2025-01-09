import pandas as pd
import numpy as np

def build_point_df_from_files(filepath1, filepath2=None):
    '''
    Receives 1 or 2 txt/csv files with fragments of the input points.
    Connects both files into a single dataframe with the complete points, and returns said dataframe.
    '''
    points_frag1 = pd.read_csv(filepath1, sep=",", header=None)
    frag1_col_num = points_frag1.shape[1]
    if(filepath2 is not None):
        points_frag2 = pd.read_csv(filepath2, sep=",", header=None)
        points_frag2.columns = [col + frag1_col_num for col in points_frag2.columns]
        points = points_frag1.join(points_frag2, sort=True)
    else:
        return points_frag1
    return points
    

def choose_move_initial_centroids(points, k):
    '''
    Implementing the kmeans++ initialization.
    Chosen centroids will be moved inplace to the first k rows of the points dataframe.
    Function will return a list of the original indices of the chosen centroids.
    '''
    np.random.seed(1234)
    points_num = np.shape(points)[0]
    original_indices = []
    rows_to_replace = {}

    for i in range(k):
        if i == 0:
            random_index = np.random.choice(points_num) # First centroid is chosen randomly.  

        else:
            # Calculates the distance of each point to the nearest centroid.
            distances = np.zeros(points_num)
            for ind, point in points.iterrows(): # Iterates over all non-chosen points.
                if (ind not in original_indices):
                    distances[ind] = np.min([np.linalg.norm(points.iloc[cent_ind] - point) for cent_ind in original_indices])
            # Chooses the next centroid with probability proportional to the distances.
            probabilities = distances / np.sum(distances)
            random_index = np.random.choice(points_num, p=probabilities)
            #random_index += i # Adjusts the index from the distances dataframe to the original one.

        original_indices.append(int(random_index))
        rows_to_replace[random_index] = i
        
    # Replaces the values in the rows, so that the indexing of the rows will stay consistent (i.e. 1,2,3,...,n-1,n) and the centroids will be in rows 0,...,k-1.
    for orig_ind in rows_to_replace:
        new_ind = rows_to_replace[orig_ind]
        tmp_coords = [coord for coord in points.iloc[new_ind]]
        points.iloc[new_ind] = [coord for coord in points.iloc[orig_ind]]
        points.iloc[orig_ind] = tmp_coords
    return original_indices


def main():
    #df = build_point_df_from_files("/home/developer/sp/Davimitar-HW2/tests/class_tests/input_1_db_1.txt", "/home/developer/sp/Davimitar-HW2/tests/class_tests/input_1_db_2.txt")
    df = build_point_df_from_files("/home/developer/sp/Davimitar-HW2/tests/class_tests/input_1.txt")
    print(choose_move_initial_centroids(df, 3))
    print(df)

if __name__ == "__main__":
    main()