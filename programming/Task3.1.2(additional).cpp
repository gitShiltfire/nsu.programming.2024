/*
 * version 1.4
 * Main task
 * --Additional task 1
 * Additional task 2
 */

#include <iostream>

using namespace std;

template<typename T>
class Vector {
    static const int default_size = 8;

    int size;
    int memory;
    T *arr;

    void resize() {
        int past_memory = memory;
        memory *= 2;
        size = 0;
        T *arr2 = new T[memory];
        for (int i = 0; i < memory; ++i)
            arr2[i] = T{};
        swap(arr, arr2);
        for (int i = 0; i < past_memory; ++i)
            add(arr2[i]);
        delete[] arr2;
    }

public:
    explicit Vector(const int n = 0, const T default_value = T{}) {
        memory = default_size;
        size = n;
        arr = new T[memory];
        for (int i = 0; i < memory; ++i)
            arr[i] = default_value;
    }

    ~Vector() { delete[] arr; }

    void add(const T e) {
        if (size + 1 >= memory) resize();
        arr[size++] = e;
    }

    [[nodiscard]] int getSize() const { return size; }

    T &operator[](int i) {
        return arr[i % size];
    }

    bool operator==(const Vector &other) const {
        if (size == other.size) {
            for (int i = 0; i < size; ++i) {
                if (arr[i] != other.arr[i]) return false;
            }
            return true;
        }
        return false;
    }

    void print(char end='\n') const {
        cout << '[';
        for (int i = 0; i + 1 < size; ++i)
            cout << arr[i] << ' ';
        if (size > 0) cout << arr[size - 1] << "]" << end;
    }

    [[nodiscard]] bool in(const T &e) const {
        for (int i = 0; i < size; ++i)
            if (arr[i] == e) return true;
        return false;
    }
};

template<typename T>
class MyIterator : public iterator<input_iterator_tag, T> {
    T **p;

public:
    explicit MyIterator(T **p) : p(p) {}

    MyIterator(const MyIterator &it) : p(it.p) {}

    bool operator!=(MyIterator const &other) const { return p != other.p; }

    bool operator==(MyIterator const &other) const { return p == other.p; }

    typename MyIterator::reference operator*() const { return **p; }

    MyIterator &operator++() {
        while ((*(++p))->null);
        return *this;
    }
};


template<typename K, typename V>
class MultiHashMap {
    static const int default_size = 8;
    constexpr static const double rehash_size = 0.3;

    struct Node {
        K key;
        Vector<V> *value;
        bool deleted;
        bool null;

        explicit Node(const K &k = K{}, const V &v = V{}, const bool &n = true) :
                key(k), deleted(false), null(n) {
            value = new Vector<V>;
            value->add(v);
        }

        ~Node() { delete value; }
    };

    Node **arr;
    int active_size;  // Size without deleted
    int memory;
    int general_size;  // Size with deleted

    void resize() {
        int past_memory = memory;
        memory *= 2;
        general_size = 0;
        active_size = 0;
        Node **arr2 = new Node *[memory + 1];
        for (int i = 0; i < memory + 1; ++i)
            arr2[i] = new Node;
        arr2[memory]->null = false;
        swap(arr, arr2);
        for (int i = 0; i < past_memory; ++i) {
            Node *t = arr2[i];
            if (!t->null && !t->deleted) {
                for (int j = 0; j < t->value->getSize(); ++j)
                    add(t->key, (*t->value)[j]);
            }
        }
        for (int i = 0; i < past_memory + 1; ++i)
            if (arr2[i])
                delete arr2[i];
        delete[] arr2;
    }

    void rehash() {
        general_size = 0;
        active_size = 0;
        Node **arr2 = new Node *[memory + 1];
        for (int i = 0; i < memory + 1; ++i)
            arr2[i] = new Node;
        arr2[memory]->null = false;
        swap(arr, arr2);
        for (int i = 0; i < memory; ++i) {
            Node *t = arr2[i];
            if (!t->null && !t->deleted) {
                for (int j = 0; j < t->value->getSize(); ++j)
                    add(t->key, (*t->value)[j]);
            }
        }
        for (int i = 0; i < memory + 1; ++i)
            if (arr2[i])
                delete arr2[i];
        delete[] arr2;
    }

public:
    MultiHashMap() {
        memory = default_size;
        active_size = 0;
        general_size = 0;
        arr = new Node *[memory + 1];
        for (int i = 0; i < memory + 1; ++i)
            arr[i] = new Node;
        arr[memory]->null = false;
    }

    ~MultiHashMap() {
        for (int i = 0; i < memory + 1; ++i)
            if (arr[i])
                delete arr[i];
        delete[] arr;
    }

    bool add(const K &k, const V &v) {
        if (active_size + 1 > int(rehash_size * memory)) resize();
        else if (general_size > 2 * active_size) rehash();
        int h = hash<K>{}(k) % memory;
        int i = 0;
        int first_deleted = -1;
        while (!arr[h]->null && i < memory) {
            if (arr[h]->key == k && !arr[h]->deleted) {
                if (arr[h]->value->in(v)) return false;
                arr[h]->value->add(v);
                return true;
            }
            if (arr[h]->deleted && first_deleted == -1) first_deleted = h;
            h = (h + 1) % memory;
            ++i;
        }
        if (first_deleted == -1) {
            arr[h] = new Node(k, v, false);
            ++general_size;
        } else {
            arr[first_deleted]->key = k;
            arr[first_deleted]->deleted = false;
        }
        ++active_size;
        return true;
    }

    bool remove(const K &key) {
        int h = hash<K>{}(key) % memory;
        int i = 0;
        while (!arr[h]->null && i < memory) {
            if (arr[h]->key == key && !arr[h]->deleted) {
                arr[h]->deleted = true;
                --active_size;
                return true;
            }
            h = (h + 1) % memory;
            ++i;
        }
        return false;
    }

    Vector<V> operator[](const K &key) {
        int h = hash<K>{}(key) % memory;
        int i = 0;
        while (!arr[h]->null && i < memory) {
            if (arr[h]->key == key && !arr[h]->deleted)
                return arr[h]->value;
            h = (h + 1) % memory;
            ++i;
        }
        return nullptr;
    }

    void print() {
        for (const Node &it: *this) {
            cout << it.key << " | ";
            it.value->print(' ');
            cout << (it.deleted ? "(deleted)" : "") << endl;
        }
    }

    [[nodiscard]] int getSize() const { return active_size; }

    [[nodiscard]] int getUnique() const {
        Vector<V> v;
        for (int i = 0; i < memory; ++i) {
            if (!arr[i]->null && !arr[i]->deleted) {
                for (int q = 0; q < arr[i]->value->getSize(); ++q) {
                    bool found = false;
                    for (int j = 0; j < v.getSize(); ++j) {
                        if (v[j] == (*arr[i]->value)[q]) {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        v.add((*arr[i]->value)[q]);
                }
            }
        }
        return v.getSize();
    }

    MyIterator<Node> begin() {
        for (int i = 0; i < memory; ++i)
            if (!arr[i]->null && !arr[i]->deleted)
                return MyIterator<Node>(&arr[i]);
        return end();
    }

    MyIterator<Node> end() { return MyIterator<Node>(&arr[memory]); }

    [[nodiscard]] MyIterator<const Node> begin() const {
        for (int i = 0; i < memory; ++i)
            if (!arr[i]->null && !arr[i]->deleted)
                return MyIterator<const Node>(arr);
        return end();
    }

    [[nodiscard]] MyIterator<const Node> end() const { return MyIterator<const Node>(&arr[memory]); }
};


int main() {
    MultiHashMap<int, string> hashMap;
    hashMap.add(10, "1");
    hashMap.add(2, "2");
    hashMap.add(3, "3");
    hashMap.add(1, "4");
    hashMap.add(2, "5");
    hashMap.add(2, "6");
    hashMap.print();
    return 0;
}
