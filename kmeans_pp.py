import pandas as pd
import numpy as np
import sys
from math import floor
import kmeansmodule as ksm

SEED = 1234
SEPARATOR = ","
GENERAL_ERROR_MSG = "An Error Has Occurred"
DEFAULT_ITER = 300
FILE_FORMAT = ".txt", ".csv"

def build_point_df_from_files(filepath1, filepath2 = None):
    '''
    Receives 1 or 2 txt/csv files with fragments of the input points.
    Connects both files into a single dataframe with the complete points, and returns said dataframe.
    '''
    points_frag1 = pd.read_csv(filepath1, sep = SEPARATOR, header = None)
    points_frag1.set_index(0, inplace=True)
    int_row_indexes = {float(ind) : ind for ind in range(points_frag1.shape[0])}
    points_frag1.rename(index=int_row_indexes, inplace=True)
    frag1_col_num = points_frag1.shape[1]
    if(filepath2 is not None):
        points_frag2 = pd.read_csv(filepath2, sep = SEPARATOR, header = None)
        points_frag2.set_index(0, inplace=True)
        points_frag2.rename(index=int_row_indexes, inplace=True)
        points_frag2.columns = [col + frag1_col_num for col in points_frag2.columns]
        points = points_frag1.join(points_frag2, sort = True)
    else:
        return points_frag1
    return points
    

def choose_move_initial_centroids(points, k):
    '''
    Implementing the kmeans++ initialization.
    Chosen centroids will be moved to the first k rows of the points dataframe, and a new dataframe will be returned.
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

        original_indices.append(int(random_index))
        rows_to_replace.append(random_index)
        
    # Builds the new dataframe, so that the indexing of the rows will stay consistent (i.e. 1,2,3,...,n-1,n) and the centroids will be in rows 0,...,k-1.
    new_points = pd.DataFrame()
    cnt_idx_set = set(rows_to_replace.keys())
    for orig_ind in rows_to_replace:
        new_points.iloc[len(new_points)] = points.iloc[orig_ind]
    for i in range(points_num):
        if i not in rows_to_replace:
            new_points.iloc[len(new_points)] = points.iloc[i]
    
    return new_points, original_indices


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
        print("Invalid number of clusters!")
        exit(1)
    try: # Check iter
        iter = float(iter)
        if(floor(iter) != iter or not 1 < iter < 1000):
            raise ValueError
    except:
        print("Invalid maximum iteration!")
        exit(1)
    try: # Check epsilon
        eps = float(eps)
        if(eps < 0):
            raise ValueError
    except:
        print("Invalid epsilon!")
        exit(1)


def main():
    len_argv = len(sys.argv)
    new_argv = sys.argv
    if (len_argv not in [4, 5, 6]): # including file's name, so its actually 4,5 args
        print(GENERAL_ERROR_MSG)
        return
    
    # if args.len = 5, pad 2nd arg (iter) with DEFAULT_ITER so it'll be 5 anyway
    if (len_argv in [4,5]):
        # TODO is needed: SUPPORT IN ONLY ONE FILE; add last arg = NULL
        if not any(fmt in new_argv[-2] for fmt in FILE_FORMAT):
            new_argv.append(None)
            if (len(new_argv) == 5): # if there's also no iter arg,
                new_argv.insert(2, DEFAULT_ITER)
        else:
            new_argv.insert(2, DEFAULT_ITER)
    
    FILEPATH_1 = new_argv[4]
    FILEPATH_2 = new_argv[5]
    # TODO _ sanity checks including try-except for each of the 5 arguments according to PDF's chart

    data_frame = build_point_df_from_files(FILEPATH_1, FILEPATH_2)
    check_legality(new_argv[1], data_frame.shape[0], new_argv[2], new_argv[3])
    clusters_num = int(new_argv[1])
    iter = int(new_argv[2])
    eps = float(new_argv[3])

    data_frame, initial_centroids_indices = choose_move_initial_centroids(data_frame, clusters_num)
    points_to_send = table_to_transferred(data_frame)
    clusters_to_send = table_to_transferred(data_frame.iloc[:clusters_num])
    dimension = data_frame.shape[1]
    
    # EXPECTED FOR MODULE: &clusters_num, &epsilon, &iter, &dimension, &transferred, &transferred_clusters
    our_beloved = ksm.kmeans_capi(clusters_num, eps, iter, dimension, points_to_send, clusters_to_send)
    print_list_without_brackets(initial_centroids_indices)
    print(transferred_to_printable_but_formatted_to_4(our_beloved))
    print('') # they want another empty line(?)


if __name__ == "__main__":
    main()