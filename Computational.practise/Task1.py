"""
version: 1.3
Method Gauss
Determinant
Inverse matrix
Run-throw method
--Reflection method
"""


def simplify_number(n: float, accuracy=8, sign_space=True) -> str:
    m = int(n * 10 ** accuracy) / 10 ** accuracy
    if abs(m - int(m)) <= 10 ** (1 - accuracy):
        return str(int(m))
    if 1 - abs(m - int(m)) <= 10 ** (1 - accuracy):
        return str(int(m + 0.5))
    n = str(m)
    if n[-2:] == '.0':
        n = n[:-2]
    if sign_space and n[0] != '-':
        return ' ' + n
    return n


def print_matrix(A: list[list[float]], accuracy=8) -> None:  # Print beautiful matrix
    n = len(A)
    m = len(A[0])
    A = [[simplify_number(A[i][j], accuracy) for j in range(m)] for i in range(n)]
    len_of_element = [max([len(A[i][j]) for i in range(n)]) for j in range(m)]
    for i in range(n):
        print('⌈' if i == 0 else '⌊' if i + 1 == n else '|', end='')
        for j in range(m):
            print(A[i][j] + ' ' * (len_of_element[j] - len(A[i][j])), end='')
            if j + 1 != m:
                print('  ', end='')
        print('⌉' if i == 0 else '⌋' if i + 1 == n else '|')


def print_vector(m: list[float]) -> None:
    print('x = (' + '; '.join(map(lambda x: simplify_number(x, sign_space=False), m)) + ')')


def is_upper_triangular(m: list[list[float]]) -> bool:
    for i in range(len(m)):
        for j in range(i):
            if m[i][j] != 0:
                return False
    return True


def to_upper_triangular(A: list[list[float]]) -> list[list[float]]:  # Straight running method Gauss
    n = len(A)
    m = len(A[0])
    B = [A[i][:] for i in range(n)]
    for k in range(n - 1):
        ind_max = index_maximum(B, k)  # Swap rows with max element and first not zero
        c = B[ind_max][:]
        B[ind_max] = B[k]
        B[k] = c
        if B[ind_max] != B[k]:
            for j in range(m):
                B[ind_max][j] *= -1
        t = B[k][k]
        for i in range(k + 1, n):
            for j in range(m - 1, k - 1, -1):
                B[i][j] -= B[k][j] * B[i][k] / t
    return B


def det(m: list[list[float]]) -> str:  # Return determinant of matrix m
    if is_upper_triangular(m):
        res = 1
        for i in range(len(m)):
            res *= m[i][i]
        return simplify_number(res)
    return det(to_upper_triangular(m))


def index_maximum(m: list[list[float]], k: int) -> int:  # Return index of row with maximum element in kth column
    res = k
    maximum = abs(m[k][k])
    for i in range(k + 1, len(m)):
        if abs(m[i][k]) > maximum:
            res = i
            maximum = abs(m[i][k])
    return res


def inverse_matrix(A: list[list[float]]) -> list[list[float]]:
    n = len(A)
    m = [[A[i][j] if j < n else 1 if i + n == j else 0 for j in range(2 * n)] for i in range(n)]  # m = [A|E]
    for k in range(n):
        ind_max = index_maximum(m, k)  # Swap rows with max element and first not zero
        c = m[ind_max][:]
        m[ind_max] = m[k]
        m[k] = c
        t = m[k][k]
        for j in range(2 * n):
            m[k][j] /= t
        for i in range(n):
            if i != k:
                for j in range(2 * n - 1, k - 1, -1):
                    m[i][j] -= m[k][j] * m[i][k]
    return [line[n:] for line in m]


def the_gauss_method(A: list[list[float]], b: list[float]) -> list[float]:  # Gauss method with column maximum selection
    n = len(A)
    m = [[A[i][j] if j < n else b[i] for j in range(n + 1)] for i in range(n)]  # m = [A|b]
    m = to_upper_triangular(m)  # Straight of gauss method
    # Calculating x
    x = [0.0 for _ in range(n)]
    for i in range(n - 1, -1, -1):
        x[i] = (m[i][n] - sum([m[i][j] * x[j] for j in range(i + 1, n)])) / m[i][i]
    return x


def run_throw_method(A: list[list[float]], b: list[float]) -> list[float]:  # A - triple diagonal matrix
    n = len(A)
    m = [[A[i][j] if j < len(A) else b[i] for j in range(n + 1)] for i in range(n)]  # m = [A|b]
    for k in range(n - 1):
        m[k][n] /= m[k][k]
        for j in range(min(n - 1, k + 1), k - 1, -1):
            m[k][j] /= m[k][k]
        m[k + 1][n] -= m[k][n] * m[k + 1][k]
        for j in range(min(n - 1, k + 1), k - 1, -1):
            m[k + 1][j] -= m[k][j] * m[k + 1][k]
    for j in range(n, n - 3, -1):
        m[n - 1][j] /= m[n - 1][n - 1]
    # Calculating x
    x = [0.0 for _ in range(n)]
    x[n - 1] = m[n - 1][n]
    for i in range(n - 2, -1, -1):
        x[i] = (m[i][n] - m[i][i + 1] * x[i + 1]) / m[i][i]
    return x


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
        # print_vector(the_gauss_method(A, b))
        # print_matrix(inverse_matrix(A))
        # print(det(A))
        # print_vector(run_throw_method(A, b))
        # print_matrix(to_upper_triangular(A))
        # print(det(A))


def test():
    A = [[1, 0.42, 0.54, 0.66],
         [0.42, 1, 0.32, 0.44],
         [0.54, 0.32, 1, 0.22],
         [0.66, 0.44, 0.22, 1]]
    b = [0.3, 0.5, 0.7, 0.9]
    # print_vector(the_gauss_method(A, b))
    # print_matrix(inverse_matrix(A))
    print_vector(run_throw_method(A, b))


if __name__ == '__main__':
    main()
