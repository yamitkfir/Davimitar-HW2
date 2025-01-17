import pandas as pd
import numpy as np
import sys

# FILEPATH_1 = "/home/developer/sp/Davimitar-HW2/tests/class_tests/input_1_db_1.txt"
# FILEPATH_2 = "/home/developer/sp/Davimitar-HW2/tests/class_tests/input_1_db_2.txt"
SEED = 1234
SEPARATOR = ","
GENERAL_ERROR_MSG = "An Error Has Occurred"
DEFAULT_ITER = 300

def build_point_df_from_files(filepath1, filepath2 = None):
    '''
    Receives 1 or 2 txt/csv files with fragments of the input points.
    Connects both files into a single dataframe with the complete points, and returns said dataframe.
    '''
    points_frag1 = pd.read_csv(filepath1, sep = SEPARATOR, header = None)
    frag1_col_num = points_frag1.shape[1]
    if(filepath2 is not None):
        points_frag2 = pd.read_csv(filepath2, sep = SEPARATOR, header = None)
        points_frag2.columns = [col + frag1_col_num for col in points_frag2.columns]
        points = points_frag1.join(points_frag2, sort = True)
    else:
        return points_frag1
    return points
    

def choose_move_initial_centroids(points, k):
    '''
    Implementing the kmeans++ initialization.
    Chosen centroids will be moved inplace to the first k rows of the points dataframe.
    Function will return a list of the original indices of the chosen centroids.
    '''
    np.random.seed(SEED)
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


def table_to_transferred(original_indices):
    '''
    Input: Pandas DataFrame.
    Output: List of lists representing the points/vectors in the DataFrame.
    '''
    return original_indices.values.tolist()
    # TODONE DAVID - decide mindfully on a format for strings and convert in a way that can be unpacked later*/


def transferred_to_printable(transferred):
    '''
    Input is a list of lists representing the clusters sent back from module after packing.
    Returns printable string (not here, printed in main)
    '''
    return '\n'.join([','.join(map(str, sublist)) for sublist in transferred])
    # TODONE DAVID - the vectors came back packed. Only get them in printing structure */


def main():
    len_argv = len(sys.argv)
    new_argv = sys.argv
    if (len_argv not in [5, 6]): # including file's name, so its actually 4,5 args
        print(GENERAL_ERROR_MSG)
        return
    
    # if args.len = 5, pad 2nd arg (iter) with DEFAULT_ITER so it'll be 5 anyway
    if (len_argv == 5):
        new_argv.insert(2, DEFAULT_ITER)
    clusters_num = int(new_argv[1])
    iter = int(new_argv[2])
    eps = float(new_argv[3])
    FILEPATH_1 = new_argv[4]
    FILEPATH_2 = new_argv[5]
    # TODO DAVID sanity checks including try-except for each of the 5 arguments according to PDF's chart
    
    # df = build_point_df_from_files(FILEPATH_1, FILEPATH_2)
    data_frame = build_point_df_from_files(FILEPATH_1, FILEPATH_2)
    choose_move_initial_centroids(data_frame, clusters_num)
    points_to_send = table_to_transferred(data_frame)
    clusters_to_send = table_to_transferred(data_frame.iloc[:clusters_num])
    dimension = data_frame.shape[1]

    # TODO YAMIT EXPECTED FOR MODULE: 
    # &clusters_num, &epsilon, &iter, &dimension, &transferred, &transferred_clusters

    # Py_BuildValue(...) returns a PyObject


if __name__ == "__main__":
    main()