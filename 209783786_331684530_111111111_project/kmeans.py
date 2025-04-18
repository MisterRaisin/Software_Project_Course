import sys
import math


EPSILON = 1e-4
MAX_ITERATIONS = 300


def error_has_occured():
    """
    Default action to handle random error
    """
    print("An Error Has Occurred")
    sys.exit()

def get_euclidean_dist(vec1, vec2):
    """
    params: vec1, vec2 : lists of float values of length K
    """   
    # Validate vectors of same len
    if (len(vec1) != len(vec2)):
        error_has_occured()
        
    sum = 0
    for i in range(len(vec1)):
        sum += math.pow(vec1[i]-vec2[i], 2)
    
    return math.sqrt(sum)



def find_closest_cluster(centroid_lst, vector):
    """
    Returns index of closest cluster in centroid_lst
    """
    # Default to first cluster
    min_index = 0
    min_dist = get_euclidean_dist(centroid_lst[0], vector) 

    # Find closest cluster
    for i in range (1, len(centroid_lst)):
        dist = get_euclidean_dist(centroid_lst[i], vector)
        if dist < min_dist:
            min_index = i
            min_dist = dist
    
    return min_index




def check_convergence(old_centroid_lst, new_centroid_lst):
    """
    Checks if the centroid list has converged based on constant EPSILON.
    """
    for i in range(len(old_centroid_lst)):
        if (not get_euclidean_dist(old_centroid_lst[i], new_centroid_lst[i]) < EPSILON):
            return False
    return True



def get_centroids(cluster_lst, vector_len):
    """
    Go over every cluster and return the centroid
    """
    new_centroid_lst = []
    for cluster in cluster_lst:
        centroid = [0 for i in range(vector_len)]
        for vector in cluster:
            for i in range(vector_len):
                centroid[i] += vector[i]
        
        centroid = [centroid[i]/len(cluster) for i in range(vector_len)]

        new_centroid_lst.append(centroid)
    
    return new_centroid_lst
            

def k_means(data, K):

    # Format data to lists of vectors where each vector is a list of K float values
    vector_lst = data.split('\n')
    vector_lst = [[float(value) for value in vector.split(',')] for vector in list(filter(None,vector_lst))]
    vector_len = len(vector_lst[0])

    # Get first K elements to be default centroid values
    centroid_lst = vector_lst[:K]
    curr_iter = 0 # Iteration counter

    # Simulate a do-while loop
    while True:
        curr_iter += 1
        cluster_lst = [[] for _ in range(K)]
        labels = []  # to track cluster index for each vector

        for vector in vector_lst:
            index = find_closest_cluster(centroid_lst, vector)
            cluster_lst[index].append(vector)
            labels.append(index)

        new_centroid_list = get_centroids(cluster_lst, vector_len)

        if check_convergence(centroid_lst, new_centroid_list) or curr_iter == MAX_ITERATIONS:
            break

        centroid_lst = new_centroid_list

    return labels


def get_data(file_path):
    """
    Reads data from file given file_path
    """
    with open(file_path, 'r') as file:
        return file.read()

    error_has_occured()
