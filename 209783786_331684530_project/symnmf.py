import sys
import numpy as np
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

def output_matrix(matrix):
    """Outputs the matrix in the required format."""
    try:
        for row in matrix:
            print(','.join(f"{val:.4f}" for val in row))
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)

def initialize_H(W, k):
    """Initializes the matrix H for SymNMF."""
    np.random.seed(1234)
    m = np.mean(W)
    return np.random.uniform(0, 2 * np.sqrt(m / k), (W.shape[0], k))

def main():
    if len(sys.argv) != 4:
        print("An Error Has Occurred")
        sys.exit(1)

    try:
        k = int(sys.argv[1])
        goal = sys.argv[2]
        file_name = sys.argv[3]
    except ValueError:
        print("An Error Has Occurred")
        sys.exit(1)

    X = read_input(file_name)

    if goal == 'sym':
        W = symnmf.sym(X)
        output_matrix(W)

    elif goal == 'ddg':
        D = symnmf.ddg(X)
        output_matrix(D)

    elif goal == 'norm':
        W_norm = symnmf.norm(X)
        output_matrix(W_norm)

    elif goal == 'symnmf':
        W = symnmf.norm(X)  # Normalize similarity matrix first
        H_init = initialize_H(W, k)
        H_final = symnmf.symnmf(W, H_init, k)
        output_matrix(H_final)

    else:
        print("An Error Has Occurred")
        sys.exit(1)

if __name__ == '__main__':
    main()
