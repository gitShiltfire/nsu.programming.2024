/*
 * version 1.3
 * HashMap
 * Main task out
 * Iterator
 * --Additional task 1
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
    explicit Vector(const int n = 0, T default_value = T{}) {
        memory = default_size;
        size = n;
        arr = new T[memory];
        for (int i = 0; i < memory; ++i)
            arr[i] = default_value;
    }

    ~Vector() { delete[] arr; }

    void add(T e) {
        if (size + 1 >= memory) resize();
        arr[size++] = e;
    }

    [[nodiscard]] int getSize() const { return size; }

    T &operator[](int i) {
        return arr[i % size];
    }

    void print() const {
        for (int i = 0; i + 1 < size; ++i)
            cout << arr[i] << ' ';
        if (size > 0) cout << arr[size - 1] << endl;
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
class HashMap {
    static const int default_size = 8;
    constexpr static const double rehash_size = 0.3;

    struct Node {
        K key;
        V value;
        bool deleted;
        bool null;

        explicit Node(const K &k = K{}, const V &v = V{}, const bool &n = true) :
                key(k), value(v), deleted(false), null(n) {}
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
            if (!arr2[i]->null && !arr2[i]->deleted)
                add(arr2[i]->key, arr2[i]->value);
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
            if (!arr2[i]->null && !arr2[i]->deleted)
                add(arr2[i]->key, arr2[i]->value);
        }
        for (int i = 0; i < memory + 1; ++i)
            if (arr2[i])
                delete arr2[i];
        delete[] arr2;
    }

public:
    HashMap() {
        memory = default_size;
        active_size = 0;
        general_size = 0;
        arr = new Node *[memory + 1];
        for (int i = 0; i < memory + 1; ++i)
            arr[i] = new Node;
        arr[memory]->null = false;
    }

    ~HashMap() {
        for (int i = 0; i < memory + 1; ++i)
            if (arr[i])
                delete arr[i];
        delete[] arr;
    }

    bool add(const K &key, const V &value) {
        if (active_size + 1 > int(rehash_size * memory)) resize();
        else if (general_size > 2 * active_size) rehash();
        int h = hash<K>{}(key) % memory;
        int i = 0;
        int first_deleted = -1;
        while (!arr[h]->null && i < memory) {
            if (arr[h]->key == key && !arr[h]->deleted) {
                if (arr[h]->value == value) return false;
                arr[h]->value = value;
                return true;
            }
            if (arr[h]->deleted && first_deleted == -1) first_deleted = h;
            h = (h + 1) % memory;
            ++i;
        }
        if (first_deleted == -1) {
            arr[h] = new Node(key, value, false);
            ++general_size;
        } else {
            arr[first_deleted]->key = key;
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

    V operator[](const K &key) {
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
            cout << it.key << ' ' << it.value << ' ' << (it.deleted ? "(deleted)" : "") << endl;
        }
    }

    [[nodiscard]] int getSize() const { return active_size; }

    [[nodiscard]] int getUnique() const {
        Vector<V> v;
        for (int i = 0; i < memory; ++i) {
            if (!arr[i]->null && !arr[i]->deleted) {
                bool found = false;
                for (int j = 0; j < v.getSize(); ++j) {
                    if (v[j] == arr[i]->value) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    v.add(arr[i]->value);
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
    char K, V;
    cin >> K >> V;
    HashMap<string, string> hashMap;
    int n;
    cin >> n;
    string act, k, v;
    for (int i = 0; i < n; ++i) {
        cin >> act >> k;
        if (act == "R") hashMap.remove(k);
        else {
            cin >> v;
            hashMap.add(k, v);
        }
    }
    cout << hashMap.getSize() << ' ' << hashMap.getUnique() << endl;
    // hashMap.print();
    return 0;
}
