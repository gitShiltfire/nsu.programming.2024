#include <iostream>

using namespace std;

class MatrixElement {  // For additional task
public:
    int *_data;

    explicit MatrixElement(int *n) { _data = n; }

    MatrixElement &operator=(int n) {
        *_data = n;
        return *this;
    }

    // In the likeness of an int
    bool operator==(int n) const { *_data == n; }

    bool operator>(int n) const { *_data > n; }

    void operator+=(int n) const { *_data += n; }

    void operator-=(int n) const { *_data -= n; }

    void operator*=(int n) const { *_data *= n; }

    void operator/=(int n) const { *_data /= n; }

    int operator+(int n) const { return *_data + n; }

    int operator-(int n) const { return *_data - n; }

    int operator*(int n) const { return *_data * n; }

    int operator/(int n) const { return *_data / n; }
};

class MatrixLine {  // For additional task
private:
    int **_data;

public:
    explicit MatrixLine(const int n, int **&a) {
        _data = new int *[n];
        for (int i = 0; i < n; ++i) _data[i] = a[i];
    }

    ~MatrixLine() { delete[] _data; }

    MatrixElement operator[](int i) const {
        return MatrixElement(_data[i]);
    }
};


class Matrix {
private:
    int _n;
    int **_data;

public:
    // Constructors
    // Without variable(default) or diagonal matrix with identical elements
    explicit Matrix(const int &n = 0, const int &k = 1) {
        _n = n;
        _data = new int *[_n];
        for (int i = 0; i < _n; ++i) {
            _data[i] = new int[_n];
            for (int j = 0; j < _n; ++j) _data[i][j] = 0;
        }
        for (int i = 0; i < _n; ++i) _data[i][i] = k;
    }

    Matrix(const int &n, int *&D, const int &dLen) {  // With dimension and diagonal elements
        if (n != dLen) throw invalid_argument("Incorrect number of diagonal elements");
        _n = n;
        _data = new int *[_n];
        for (int i = 0; i < _n; ++i) {
            _data[i] = new int[_n];
            for (int j = 0; j < _n; ++j) _data[i][j] = 0;
        }
        for (int i = 0; i < _n; ++i) _data[i][i] = D[i];
    }

    Matrix(const int &n, int **&D, const int &dLen = -1) {
        if (n != dLen && dLen != -1) throw invalid_argument("Incorrect number of diagonal elements");
        _n = n;
        _data = new int *[_n];
        for (int i = 0; i < _n; ++i) {
            _data[i] = new int[_n];
            for (int j = 0; j < _n; ++j) _data[i][j] = D[i][j];
        }
    }

    [[nodiscard]] int getN() const { return _n; }

    [[nodiscard]] int **getData() const { return _data; }

    // Overloading operators
    Matrix operator+(const Matrix &m) {
        if (_n != m.getN()) throw invalid_argument("Incorrect sizes of matrices by 'operator+'");
        int **mData = m.getData(), **newData = new int *[_n];
        for (int i = 0; i < _n; ++i) {
            newData[i] = new int[_n];
            for (int j = 0; j < _n; ++j) newData[i][j] = _data[i][j] + mData[i][j];
        }
        return Matrix{_n, newData};
    }

    Matrix operator-(const Matrix &m) {
        if (_n != m.getN()) throw invalid_argument("Incorrect sizes of matrices by 'operator-'");
        int **mData = m.getData(), **newData = new int *[_n];
        for (int i = 0; i < _n; ++i) {
            newData[i] = new int[_n];
            for (int j = 0; j < _n; ++j) newData[i][j] = _data[i][j] - mData[i][j];
        }
        return Matrix{_n, newData};
    }

    Matrix operator*(const Matrix &m) {
        if (_n != m.getN()) throw invalid_argument("Incorrect sizes of matrices by 'operator*'");
        int **mData = m.getData(), **newData = new int *[_n];
        for (int i = 0; i < _n; ++i) {
            newData[i] = new int[_n];
            for (int j = 0; j < _n; ++j) {
                for (int k = 0; k < _n; ++k) {
                    newData[i][j] += _data[i][k] * mData[k][j];
                }
            }
        }
        return Matrix{_n, newData};
    }

    bool operator==(Matrix &m) {
        if (_n != m.getN()) throw invalid_argument("Incorrect sizes of matrices by operator '=='");
        int **mData = m.getData();
        for (int i = 0; i < _n; ++i) {
            for (int j = 0; j < _n; ++j) {
                if (_data[i][j] != mData[i][j]) return false;
            }
        }
        return true;
    }

    Matrix operator()(const int &a, const int &b) {  // Returning minor
        int **newData = new int *[_n - 1];
        for (int i = 0; i < _n; ++i) {
            newData[i] = new int[_n - 1];
            if (i + 1 == a) continue;
            for (int j = 0; j < _n; ++j) {
                if (j + 1 == b) continue;
                newData[(i + 1 < a ? i : i - 1)][(j + 1 < b ? j : j - 1)] = _data[i][j];
            }
        }
        return Matrix{_n - 1, newData};
    }

    // Additional task
    MatrixLine operator[](const int &i) {
        int **result = new int *[_n];
        for (int j = 0; j < _n; ++j) result[j] = &_data[i][j];
        return MatrixLine{_n, result};
    }

    MatrixLine operator()(const int &i) {
        int **result = new int *[_n];
        for (int j = 0; j < _n; ++j) result[j] = &_data[j][i];
        return MatrixLine{_n, result};
    }

    // Functions
    Matrix transpose() {
        int **newData = new int *[_n];
        for (int i = 0; i < _n; ++i) {
            newData[i] = new int[_n];
            for (int j = 0; j < _n; ++j) newData[i][j] = _data[j][i];
        }
        return Matrix{_n, newData};
    }

    void printData() {  // print _data of matrix
        for (int i = 0; i < _n; ++i) {
            for (int j = 0; j < _n; ++j) cout << _data[i][j] << ' ';
            cout << '\n';
        }
    }
};


void additional_task() {
    cout << "Additional task\n";
    int **data = new int *[4];
    for (int i = 0; i < 4; ++i) data[i] = new int[4];
    int d[4][4] = {{1, 1, 1, 1},
                   {1, 1, 1, 1},
                   {1, 1, 1, 1},
                   {1, 1, 1, 1}};
    /*int d[4][4] = {{1, 2, 3, 4},
                   {1, 3, 1, 3},
                   {2, 4, 2, 4},
                   {7, 5, 8, 0}};*/
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) data[i][j] = d[i][j];
    }
    Matrix m = Matrix(4, data);

    cout << "\nFor rows\n";
    auto a = m[0];
    m.printData();
    cout << "Second element of first row = 9\n";
    a[1] = 9;
    cout << "Result of changing:\n";
    m.printData();

    cout << "\nFor columns\n";
    auto b = m(0);
    m.printData();
    cout << "Second element of first column = 9\n";
    b[1] = 9;
    cout << "Result of it:\n";
    m.printData();
}

void main_task() {
    try {
        int N, k;
        cin >> N >> k;
        Matrix A, B, C, D, K{N, k};

        for (int i = 0; i < 4; ++i) {
            int **data = new int *[N];
            for (int j = 0; j < N; ++j) {
                data[j] = new int[N];
                for (int q = 0; q < N; ++q) {
                    int u;
                    cin >> u;
                    data[j][q] = u;
                }
            }
            if (i == 0) A = Matrix{N, data, N};
            else if (i == 1) B = Matrix{N, data, N};
            else if (i == 2) C = Matrix{N, data, N};
            else D = Matrix{N, data, N};
        }

        ((A + B * C.transpose() + K) * D.transpose()).printData();

    } catch (invalid_argument &e) {
        cerr << e.what() << endl;
    }
}

int main() {
    main_task();
    return 0;
}
