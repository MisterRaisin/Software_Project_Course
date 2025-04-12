import sys
import numpy as np
from sklearn.metrics import silhouette_score
import symnmf

from kmeans import k_means, get_data


def read_input(file_name):
    """Reads the input file and returns the data as a NumPy array. Params: file_name - name of the file to read. Ret: NumPy array."""
    try:
        txt = np.loadtxt(file_name, delimiter=",", dtype=float)
        if len(txt) == 0:
            raise ValueError
        return txt
    except FileNotFoundError:
        raise ValueError


def perform_symnmf_clustering(X, k):
    """Performs clustering using SymNMF and returns the labels."""
    W = symnmf.norm(X)  # Compute normalized similarity matrix
    H_init = np.random.uniform(0, 2 * np.sqrt(np.mean(W) / k), (W.shape[0], k))
    H_final = symnmf.symnmf(W, H_init)
    labels = np.argmax(H_final, axis=1)
    return labels


def main():
    try:
        k = int(sys.argv[1]) # No input validation is required
        file_name = sys.argv[2]
        X = read_input(file_name)

        if not 1 < k < X.shape[0]:
            raise ValueError
        
        np.random.seed(1234)

        # Perform SymNMF clustering
        symnmf_labels = perform_symnmf_clustering(X, k)
        symnmf_score = silhouette_score(X, symnmf_labels)

        X_not_np = get_data(file_name)
        # Perform KMeans clustering
        kmeans_labels = k_means(X_not_np, k)
        kmeans_score = silhouette_score(X, kmeans_labels)

        print(f"nmf: {symnmf_score:.4f}")
        print(f"kmeans: {kmeans_score:.4f}")

    except ValueError:
        print("An Error Has Occurred")
        sys.exit(1) 

if __name__ == '__main__':
    main()
