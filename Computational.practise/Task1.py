"""
version: 1.5
Method Gauss
Determinant
Inverse matrix
Run-throw method
--Reflection method
"""


def simplify_number(n: float, accuracy=8, sign_space=False) -> str:
    m = str(int(n * 10 ** accuracy) / 10 ** accuracy)
    if '.' in m and '9' in m:
        a = m.rindex('9')
        b = a
        while b > 1 and m[b - 1] == '9':
            b -= 1
        if 2 * (a - b + 1) >= accuracy:
            if m[b - 1] == '.':
                m = str(int(m[:b - 1]) + 1)
            else:
                m = m[:b - 1] + str(int(m[b - 1]) + 1)
    if abs(int(float(m)) - float(m)) < 10 ** (-accuracy // 4):
        m = str(int(float(m)))
    if sign_space:
        return m if m[0] == '-' else ' ' + m
    return m


def print_matrix(A: list[list[float]], accuracy=8) -> None:  # Print beautiful matrix
    n = len(A)
    m = len(A[0])
    A = [[simplify_number(A[i][j], accuracy, sign_space=any([A[i][j] < 0 for i in range(n)])) for j in range(m)]
         for i in range(n)]
    len_of_element = [max([len(A[i][j]) for i in range(n)]) for j in range(m)]
    for i in range(n):
        print('⌈' if i == 0 else '⌊' if i + 1 == n else '|', end='')
        for j in range(m):
            print(A[i][j] + ' ' * (len_of_element[j] - len(A[i][j])), end='')
            if j + 1 != m:
                print('  ', end='')
        print('⌉' if i == 0 else '⌋' if i + 1 == n else '|')


def print_vector(m: list[float]) -> None:
    print('x = (' + '; '.join(map(simplify_number, m)) + ')')


def is_upper_triangular(m: list[list[float]]) -> bool:
    for i in range(len(m)):
        for j in range(i):
            if m[i][j] != 0:
                return False
    return True


def to_upper_triangular(A: list[list[float]]) -> list[list[float]]:  # Straight running method Gauss
    n = len(A)
    m = len(A[0])
    A = [A[i][:] for i in range(n)]
    for k in range(n - 1):
        ind_max = k + A[k:].index(max(A[k:], key=lambda x: x[k]))  # Swap rows with max element and first not zero
        c = A[ind_max][:]
        A[ind_max] = A[k]
        A[k] = c
        if A[ind_max] != A[k]:
            for j in range(m):
                A[ind_max][j] *= -1
        t = A[k][k]
        for i in range(k + 1, n):
            for j in range(m - 1, k - 1, -1):
                A[i][j] -= A[k][j] * A[i][k] / t
    return A


def det(m: list[list[float]]) -> str:  # Return determinant of matrix m
    if is_upper_triangular(m):
        res = 1
        for i in range(len(m)):
            res *= m[i][i]
        return simplify_number(res)
    return det(to_upper_triangular(m))


def inverse_matrix(m: list[list[float]]) -> list[list[float]]:
    n = len(m)
    m = [[m[i][j] if j < n else 1 if i + n == j else 0 for j in range(2 * n)] for i in range(n)]  # m = [A|E]
    for k in range(n):
        ind_max = k + m[k:].index(max(m[k:], key=lambda x: x[k]))  # Swap rows with max element and first not zero
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
    A = [[A[i][j] if j < n else b[i] for j in range(n + 1)] for i in range(n)]  # m = [A|b]
    A = to_upper_triangular(A)  # Straight of gauss method
    # Calculating x
    x = [0.0 for _ in range(n)]
    for i in range(n - 1, -1, -1):
        x[i] = (A[i][n] - sum([A[i][j] * x[j] for j in range(i + 1, n)])) / A[i][i]
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


def transpose(m: list[list[float]]) -> list[list[float]]:
    return [[m[j][i] for j in range(len(m))] for i in range(len(m))]


def norm_vector(v: list[float]) -> float:  # The norm 2
    return sum(map(lambda x: x * x, v)) ** 0.5


def scalar_vectors(a: list[float], b: list[float]) -> float:
    if len(a) != len(b):
        raise ValueError('Incorrect size of vector for scalar')
    return sum([a[i] * b[i] for i in range(len(a))])


def reflection_method(m: list[list[float]]) -> list[list[float]]:
    n = len(m)
    m = [line[:] for line in m]
    for k in range(n - 1):
        Ak = [[m[i][j] for j in range(k, n)] for i in range(k, n)]
        # ind_max = transpose(Ak).index(max(transpose(Ak), key=norm_vector))
        # Soon continued
    return m


def main():
    try:
        with open('input.txt', 'r') as f:
            n = int(f.readline())  # Input size of matrix nxn
            A = [list(map(float, f.readline().split())) for _ in range(n)]  # Input matrix
            b = list(map(float, f.readline().split()))  # Input vector b
            if n != len(A) or n != len(b) or any([len(line) != n for line in A]):  # Incorrect size of A or b
                raise ValueError('Incorrect size')
            if det(A) == 0:  # Incorrect type of matrix
                raise ValueError('This matrix is degenerate')
            # print_vector(the_gauss_method(A, b))
            # print_matrix(inverse_matrix(A))
            # print(det(A))
            # print_vector(run_throw_method(A, b))
            # reflection_method(A)
    except Exception as e:
        print(str(e.__class__)[8:-2] + ': ' + str(e))


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
