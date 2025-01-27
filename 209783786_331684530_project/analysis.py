import sys
import numpy as np
from sklearn.metrics import silhouette_score
from sklearn.cluster import KMeans
import symnmf

def read_input(file_name):
    """Reads the input file and returns the data as a NumPy array."""
    try:
        with open(file_name, 'r') as f:
            data = [list(map(float, line.strip().split(','))) for line in f]
        return np.array(data)
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)

def perform_symnmf_clustering(X, k):
    """Performs clustering using SymNMF and returns the labels."""
    W = symnmf.norm(X)  # Compute normalized similarity matrix
    H_init = np.random.uniform(0, 2 * np.sqrt(np.mean(W) / k), (W.shape[0], k))
    H_final = symnmf.symnmf(W, H_init, k)
    labels = np.argmax(H_final, axis=1)
    return labels

def perform_kmeans_clustering(X, k):
    """Performs clustering using KMeans and returns the labels."""
    kmeans = KMeans(n_clusters=k, random_state=1234).fit(X)
    return kmeans.labels_

def calculate_silhouette_score(X, labels):
    """Calculates the silhouette score for the given labels."""
    return silhouette_score(X, labels)

def main():
    if len(sys.argv) != 3:
        print("An Error Has Occurred")
        sys.exit(1)

    try:
        k = int(sys.argv[1])
        file_name = sys.argv[2]
    except ValueError:
        print("An Error Has Occurred")
        sys.exit(1)

    X = read_input(file_name)

    # Perform SymNMF clustering
    symnmf_labels = perform_symnmf_clustering(X, k)
    symnmf_score = calculate_silhouette_score(X, symnmf_labels)

    # Perform KMeans clustering
    kmeans_labels = perform_kmeans_clustering(X, k)
    kmeans_score = calculate_silhouette_score(X, kmeans_labels)

    print(f"nmf: {symnmf_score:.4f}")
    print(f"kmeans: {kmeans_score:.4f}")

if __name__ == '__main__':
    main()
