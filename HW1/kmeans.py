import sys
import math


EPSILON = 0.001
DEFAULT_ITERATIONS = 200


# Read data from file
# get as args K, iterations, data. Example command: 
# python3 kmeans.py 3 100 input_data.txt
# Recommended using split()
# Can use builtin packages
# Print output using print()


# TODO:
# Validate inputs (k, iters)
# format to 4 decimal places (use: '%4f')


def get_euclidean_dist(vec1, vec2):
    """
    params: vec1, vec2 : lists of float values of length K
    """   
    sum = 0
    # Validate vectors of same len
    if (len(vec1) != len(vec2)):
        print("ERROR")
        print(vec1)
        print(vec2)
        raise error
    
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
                




def k_means(K, iters, data):

    # Format data to lists of vectors where each vector is a list of K float values
    vector_lst = data.split('\n')
    vector_lst = [[float(value) for value in vector.split(',')] for vector in list(filter(None,vector_lst))]
    vector_len = len(vector_lst[0])

    # Get first K elements to be default centroid values
    centroid_lst = vector_lst[:K]
    curr_iter = 0 # Iteration counter

    # Simulate a do-while loop
    while (True):
        curr_iter += 1
        cluster_lst = [[] for i in range(K)]
        
        # Assign every vector to the closest cluster
        for vector in vector_lst:
            index = find_closest_cluster(centroid_lst, vector)
            cluster_lst[index].append(vector)

        # Update the centroids
        new_centroid_list = get_centroids(cluster_lst, vector_len)


        # If finished converging or reached maximum iterations, exit
        if check_convergence(centroid_lst, new_centroid_list) or curr_iter == iters:
            centroid_lst = new_centroid_list
            break

        centroid_lst = new_centroid_list

    return centroid_lst





def validate_input(K, N, iter = None):
    if (not 1 < K < N):
        return False, "Invalid number of clusters!"
    if (iter and not 1 < iter < 1000):
        return False, "Invalid maximum iteration!"
    return True, None


def get_data(file_path):
    with open(file_path, 'r') as file:
        return file.read()
    

def format_output(output):
    new_output = []
    for centroid in output:
        for i in range(len(centroid)):
            centroid[i] = "%.4f" % centroid[i]
        new_output.append(','.join(centroid))
    return new_output
            


if __name__=="__main__":
    args = sys.argv
    
    if (len(args) == 3):
        K, iter, file_path = int(args[1]), DEFAULT_ITERATIONS, args[2]
    else:
        K, iter, file_path = int(args[1]), int(args[2]), args[3]

    print(K, iter, file_path)

    data = get_data(file_path)

    valid, error = validate_input(K, len(data.split('\n')), iter)
    if (not valid):
        print(error)
        exit()

    output = format_output(k_means(K, iter, data))

    for line in output:
        print(line)


    
    

