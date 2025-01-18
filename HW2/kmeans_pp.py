import sys
import os
import math
import numpy as np
import mykmeanssp


EPSILON = 0.001
DEFAULT_ITERATIONS = 200

def processInput():
    if not 5 <= len(sys.argv) <= 6:
        print("An Error Has Occurred")
        sys.exit(1)
    

    
    if len(sys.argv) == 5:
        K, iter,eps,path1,path2 =  int(sys.argv[1]),DEFAULT_ITERATIONS,float(sys.argv[2]),sys.argv[3],sys.argv[4]
        noIter = 0
    else:
        K, iter,eps,path1,path2 =  int(sys.argv[1]),int(sys.argv[2]),float(sys.argv[3]),sys.argv[4],sys.argv[5]
       
    if not 1 < iter < 1000:
        print("Invalid maximum iteration!")
        sys.exit(1)
    if(not 1<K):
        print("Invalid number of clusters!")
        sys.exit(1)
    if eps < 0:
        print("Invalid epsilon!")
        sys.exit(1)
    
    if not ( os.path.isfile(path1) and (path1.endswith('.txt') or path1.endswith('.csv')) ) or not ( os.path.isfile(path2) and (path2.endswith('.txt') or path2.endswith('.csv')) ):
        print("An Error Has Occurred")
        sys.exit(1)

    vectors = processFiles(path1, path2)
    
    if not K<len(vectors):
        print("Invalid number of clusters!")
        sys.exit(1)
    
    return K, iter, eps, vectors, len(vectors)

def processFiles(file1, file2):
    #preform inner join based on the first column and sort the resulting by the first column
    vectors1 = []
    with open(file1, 'r') as file:
        for line in file:
            vectors1.append([float(x) for x in line.split(',')])
    #sort
    vectors1.sort(key=lambda x: x[0])
    vectors2 = []
    with open(file2, 'r') as file:
        for line in file:
            vectors2.append([float(x) for x in line.split(',')])
    #sort
    vectors2.sort(key=lambda x: x[0])
    #inner join

    if(len(vectors1) != len(vectors2)):
        print("An Error Has Occurred")
        sys.exit(1)
    
    for i in range(len(vectors1)):
        if vectors1[i][0] != vectors2[i][0]:
            print("An Error Has Occurred")
            sys.exit(1)
        vectors1[i] = vectors1[i][1:] + vectors2[i][1:]

    return np.array(vectors1)
    

    
def main():
    np.random.seed(1234)

    K, iter, eps, vectors, N = processInput()


    weights = np.ones(N)
    centroidIndexes = np.zeros(K, dtype=int)
    
    choice = np.random.choice(N)
    centroidIndexes[0] = choice
    weights[choice] = 0
    currentCentroidCount = 1

    while currentCentroidCount < K:
        for i in range(N):
            if weights[i] == 0:
                continue
            minDist = math.inf
            for centroid in range(currentCentroidCount):
                dist = np.linalg.norm(vectors[i] - vectors[centroidIndexes[centroid]])
                if dist < minDist:
                    minDist = dist
            weights[i] = minDist
        
        choice = np.random.choice(N, p=weights/weights.sum())
        centroidIndexes[currentCentroidCount] = choice
        weights[choice] = 0
        currentCentroidCount += 1
    
    print(','.join([str(index) for index in centroidIndexes]))
    # result = mykmeanssp.fit( vectors.tolist() , [vectors[index].tolist() for index in centroidIndexes] ,K, iter, eps)
    # for cluster in result:
    #     print(','.join(["{:.4f}".format(centroid) for centroid in cluster]))



if __name__=="__main__":
    main()