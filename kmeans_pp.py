import pandas as pd
import numpy as np
import sys
from math import floor
import kmeansmodule as ksm

SEED = 1234
SEPARATOR = ","
GENERAL_ERROR_MSG = "An Error Has Occurred"
EPS_MSG = "Invalid epsilon!"
ITER_MSG = "Invalid maximum iteration!"
CLUSTER_MSG = "Invalid number of clusters!"
DEFAULT_ITER = 300
FILE_FORMAT = ".txt", ".csv"

def build_point_df_from_files(filepath1, filepath2 = None):
    '''
    Receives 1 or 2 txt/csv files with fragments of the input points.
    Connects both files into a single dataframe with the complete points, and returns said dataframe.
    '''
    points_frag1 = pd.read_csv(filepath1, sep = SEPARATOR, header = None)
    points_frag1.set_index(0, inplace = True)
    int_row_indexes = {float(ind) : ind for ind in range(points_frag1.shape[0])}
    points_frag1.rename(index = int_row_indexes, inplace = True)
    frag1_col_num = points_frag1.shape[1]
    if(filepath2 is not None):
        points_frag2 = pd.read_csv(filepath2, sep = SEPARATOR, header = None)
        points_frag2.set_index(0, inplace = True)
        points_frag2.rename(index = int_row_indexes, inplace = True)
        points_frag2.columns = [col + frag1_col_num for col in points_frag2.columns]
        points = points_frag1.join(points_frag2, sort = True)
    else:
        return points_frag1
    return points
    

def choose_initial_centroids(points, k):
    '''
    Implementing the kmeans++ initialization.
    Function will return a dataframe of the chosen centroids, as well as a list of their original indices.
    '''
    np.random.seed(SEED)
    serial_num_to_index = {} # For n points, holds the serial numbers 0,...,n-1 and assigns a serial number to every actual point index in "points".
    i = 0
    for ind, point in points.iterrows():
        serial_num_to_index[i] = int(ind)
        i = i + 1
    points_num = len(serial_num_to_index)
    zeros_reseter = [0.0 for i in range(points_num)]

    centroid_indices = [] # The returned centroid indices.
    cnt_sernum_set = set() # A set of the returned centroid serial numbers inside serial_num_to_index dict.
    centroids = pd.DataFrame(data={i:[float(i)] for i in range(np.shape(points)[1])}) # A dataframe of the centroids to be returned.

    for i in range(k):
        if i == 0:
            random_index_sernum = np.random.choice(points_num) # First centroid is chosen randomly. 
            centroids.loc[0] = [coord for coord in points.loc[serial_num_to_index[random_index_sernum]]] 

        else:
            # Calculates the distance of each point to the nearest centroid.
            distances = pd.array(zeros_reseter)
            for sernum in serial_num_to_index.keys():
                ind = serial_num_to_index[sernum]
                if (sernum not in cnt_sernum_set): # Iterates over all non-chosen points and calculates distances from centroids.
                    distances[sernum] = np.min([np.linalg.norm(points.loc[cent_ind] - points.loc[ind]) for cent_ind in centroid_indices])
            # Chooses the next centroid with probability proportional to the distances.
            probabilities = distances / np.sum(distances, axis=0)
            random_index_sernum = np.random.choice(points_num, p=probabilities)
            centroids.loc[i] = [coord for coord in points.loc[serial_num_to_index[random_index_sernum]]] 

        chosen_cnt_idx = serial_num_to_index[random_index_sernum]
        cnt_sernum_set.add(chosen_cnt_idx)
        centroid_indices.append(int(chosen_cnt_idx))
    
    return centroids, centroid_indices


def table_to_transferred(original_indices):
    '''
    Input: Pandas DataFrame.
    Output: List of lists representing the points/vectors in the DataFrame.
    '''
    return original_indices.values.tolist()


def transferred_to_printable_but_formatted_to_4(transferred):
    '''
    Input is a list of lists representing the clusters sent back from module after packing.
    Returns printable string with floats formatted to 4 digits after the dot.
    '''
    return '\n'.join([SEPARATOR.join(f"{x:.4f}" for x in sublist) for sublist in transferred])


def transferred_to_printable(transferred):
    '''
    Input is a list of lists representing the clusters sent back from module after packing.
    Returns printable string (not here, printed in main)
    '''
    return ''.join([SEPARATOR.join(map(str, sublist)) for sublist in transferred])


def print_list_without_brackets(lst):
    '''
    Input: List of elements.
    Output: Prints the list elements separated by commas without brackets.
    '''
    print(SEPARATOR.join(map(str, lst)))


def check_legality(k, N, iter, eps):
    '''
    Input: k = Number of wanted centroids. N = number of given points.
    iter = Number of wanted iterations. eps = Wanted epsilon.
    Checks the legality of k, iter, eps according to the instructions.
    Prints error message and terminates if at least one is illegal.
    '''
    try: # Check k
        k = float(k) # If we got here, k is indeed a float.
        if(floor(k) != k or not 1 < k < N): # k is not an integer, or isn't in the legal range.
            raise ValueError
    except:
        print(CLUSTER_MSG)
        exit(1)
    try: # Check iter
        iter = float(iter)
        if(floor(iter) != iter or not 1 < iter < 1000):
            raise ValueError
    except:
        print(ITER_MSG)
        exit(1)
    try: # Check epsilon
        eps = float(eps)
        if(eps < 0):
            raise ValueError
    except:
        print(EPS_MSG)
        exit(1)


def main():
    len_argv = len(sys.argv)
    new_argv = sys.argv
    if (len_argv not in [5, 6]): # including file's name, so its actually 4,5 args
        print(GENERAL_ERROR_MSG)
        return
    
    ''' if args.len = 5, pad 2nd arg (iter) with DEFAULT_ITER so it'll be 5 anyway
    SUPPORT IN ONLY ONE FILE; add last arg = NULL '''
    # if (len_argv in [4,5]): 
    #     if not any(fmt in new_argv[-2] for fmt in FILE_FORMAT):
    #         new_argv.append(None)
    #         if (len(new_argv) == 5): # if there's also no iter arg,
    #             new_argv.insert(2, DEFAULT_ITER)
    #     else:
    #         new_argv.insert(2, DEFAULT_ITER)
        # if args.len = 5, pad 2nd arg (iter) with DEFAULT_ITER so it'll be 5 anyway
    if (len_argv == 5):
            new_argv.insert(2, DEFAULT_ITER)
    
    FILEPATH_1 = new_argv[4]
    FILEPATH_2 = new_argv[5]

    points_df = build_point_df_from_files(FILEPATH_1, FILEPATH_2)
    check_legality(new_argv[1], points_df.shape[0], new_argv[2], new_argv[3])
    clusters_num = int(new_argv[1])
    iter = int(new_argv[2])
    eps = float(new_argv[3])

    initial_centroids_df, initial_centroids_indices = choose_initial_centroids(points_df, clusters_num)
    points_to_send = table_to_transferred(points_df)
    clusters_to_send = table_to_transferred(initial_centroids_df)
    dimension = points_df.shape[1]
    # EXPECTED FOR MODULE: &clusters_num, &epsilon, &iter, &dimension, &transferred, &transferred_clusters
    our_beloved = ksm.kmeans_capi(clusters_num, eps, iter, dimension, points_to_send, clusters_to_send)
    print_list_without_brackets(initial_centroids_indices)
    print(transferred_to_printable_but_formatted_to_4(our_beloved))


if __name__ == "__main__":
    main()