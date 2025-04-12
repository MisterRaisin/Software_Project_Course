import sys
import numpy as np
import symnmf


def read_input(file_name):
    """Reads the input file and returns the data as a NumPy array. Params: file_name - name of the file to read. Ret: NumPy array."""
    try:
        txt = np.loadtxt(file_name, delimiter=",", dtype=np.float64)
        if len(txt) == 0:
            raise ValueError
        return txt
    except FileNotFoundError:
        raise ValueError

def output_matrix(matrix):
    """Print the resulting matrix. Params: matrix - matrix to print. Ret: None."""
    for row in matrix:
        print(",".join(f"{x:.4f}" for x in row))

def initialize_H(W, k):
    """Initializes the matrix H for SymNMF. Params: W - similarity matrix, k - number of clusters. Ret: initialized
    matrix H."""
    np.random.seed(1234)
    m = np.mean(W)
    return np.random.uniform(0, 2 * np.sqrt(m / k), (W.shape[0], k))

def main():
    """Main function. Reads the input, performs the requested operation and prints the result. Params: CMD args. Ret: None."""
    try:
        if len(sys.argv) != 4:
            raise ValueError
        
        k = int(sys.argv[1])
        goal = sys.argv[2]
        file_name = sys.argv[3]
    
        X = read_input(file_name)
        if k >= X.shape[0]:
            raise ValueError
        
        if goal == 'sym':
            A = symnmf.sym(X)
            output_matrix(A)
        elif goal == 'ddg':
            D = symnmf.ddg(X)
            output_matrix(D)
        elif goal == 'norm':
            W_norm = symnmf.norm(X)
            output_matrix(W_norm)
        elif goal == 'symnmf':
            if not k > 1:
                raise ValueError
            W = symnmf.norm(X)  # Normalize similarity matrix first
            H_init = initialize_H(W, k)
            H_final = symnmf.symnmf(W, H_init)
            output_matrix(H_final)
        else:
            raise ValueError
        
    except ValueError:
        print("An Error Has Occurred")
        sys.exit(1)

if __name__ == '__main__':
    main()
