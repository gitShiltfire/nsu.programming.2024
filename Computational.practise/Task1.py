def print_matrix(m: list):
    print('⌈', *m[0], '⌉')
    for line in m[1:-1]:
        print('|', *line, '|')
    print('⌊', *m[-1], '⌋')


def det(m: list):
    if len(m) == 1:
        return m[0][0]
    res = 0
    for i in range(len(m)):
        res += m[0][i] * (-1) ** i * det([m[j][:i] + m[j][i + 1:] for j in range(1, len(m))])
    return res


def index_maximum(m: list, k: int):  # return index of row with maximum element in kth column
    res = k
    maximum = abs(m[k][k])
    for i in range(k + 1, len(m)):
        if abs(m[i][k]) > maximum:
            res = i
            maximum = abs(m[i][k])
    return res


def the_gauss_method(A: list, b: list):
    n = len(A)
    m = [[A[i][j] if j < len(A) else b[i] for j in range(len(A) + 1)] for i in range(len(A))]  # m = [A|b]
    # We convert the matrix A to the upper triangular form
    for k in range(n - 1):
        # Swap rows with max element and first not zero
        ind_max = index_maximum(m, k)
        c = m[ind_max][:]
        m[ind_max] = m[k]
        m[k] = c
        for i in range(k + 1, n):
            for j in range(n, k - 1, -1):
                m[i][j] = m[i][j] * m[k][k] / m[i][k] - m[k][j]
    # Calculating x
    x = [0 for _ in range(n)]
    for i in range(n - 1, -1, -1):
        x[i] = (m[i][n] - sum([m[i][j] * x[j] for j in range(i + 1, n)])) / m[i][i]
    # Print answer
    print(f'x = ({x[0]:.{8}f}', end='')
    for e in x[1:]:
        print(f' {e:.{8}f}', end='')
    print(')')


def main():
    with open('input.txt', 'r') as f:
        n = int(f.readline())  # Input size of matrix nxn
        A = [list(map(float, f.readline().split())) for _ in range(n)]  # Input matrix
        b = list(map(float, f.readline().split()))  # Input vector b
        if n != len(A) or n != len(b) or any([len(line) != n for line in A]):  # Incorrect size of A or b
            print('Incorrect size')
            return -1
        if det(A) == 0:  # Incorrect type of matrix
            print('This matrix is degenerate')
            return -1
        the_gauss_method(A, b)


def test():
    A = [[1, 0.42, 0.54, 0.66],
         [0.42, 1, 0.32, 0.44],
         [0.54, 0.32, 1, 0.22],
         [0.66, 0.44, 0.22, 1]]
    b = [0.3, 0.5, 0.7, 0.9]
    the_gauss_method(A, b)


if __name__ == '__main__':
    test()
