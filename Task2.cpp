#include <iostream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include <list>

using namespace std;

bool is_var_start_at_ind(string &s, string &v, int ind) {  // Check if name of var start with s[ind]
    for (int i = 0; i < v.size(); ++i) {
        if (s[ind + i] != v[i]) return false;
    }
    return true;
}

int extract_var_from_string(string &s, string &v) {  // Extract value of var from string
    for (int i = 0; i + v.size() < s.size(); ++i) {
        if (is_var_start_at_ind(s, v, i)) {
            // s[i] == v[0] ... s[i + v.size() - 1] == v[v.size() - 1]
            unsigned int ind = i + v.size() + 4;
            int result = 0;
            while (ind < s.size() && s[ind] != ';') {
                result = result * 10 + (s[ind++] - '0');
            }
            return result;
        }
    }
}

class Expression {  // Abstract class
protected:
    stack<Expression *> _allocated;

public:
    virtual ~Expression() {
        while (!_allocated.empty()) {
            delete _allocated.top();
            _allocated.pop();
        }
    }

    virtual Expression *derivative(string s) = 0;

    virtual void printWithoutN() = 0;

    void print() {  // For inheritance only
        printWithoutN();
        cout << '\n';
    }

    virtual int eval(string s) = 0;
};

class Number : public Expression {
private:
    int _n;

    void printWithoutN() override { cout << _n; }

public:
    explicit Number(const int &n = 0) : _n(n) {}

    Number *derivative(string s) override {
        _allocated.push(new Number{0});
        return (Number *) _allocated.top();
    }

    int eval(string s) override { return _n; }
};

class Variable : public Expression {
private:
    string _s;

    void printWithoutN() override { cout << _s; }

public:
    explicit Variable(string s) : _s(std::move(s)) {}

    Number *derivative(string s) override {
        _allocated.push(new Number{_s == s ? 1 : 0});
        return (Number *) _allocated.top();
    }

    int eval(string s) override { return extract_var_from_string(s, _s); }
};

class Add : public Expression {
private:
    Expression *_first;
    Expression *_second;

    void printWithoutN() override {
        cout << '(';
        _first->printWithoutN();
        cout << '+';
        _second->printWithoutN();
        cout << ')';
    }

public:
    Add(Expression *first, Expression *second) : _first(first), _second(second) {}

    Add *derivative(string s) override {
        _allocated.push(new Add{_first->derivative(s), _second->derivative(s)});
        return (Add *) _allocated.top();
    }

    int eval(string s) override { return _first->eval(s) + _second->eval(s); }
};

class Sub : public Expression {
private:
    Expression *_first;
    Expression *_second;

    void printWithoutN() override {
        cout << '(';
        _first->printWithoutN();
        cout << '-';
        _second->printWithoutN();
        cout << ')';
    }

public:
    Sub(Expression *first, Expression *second) : _first(first), _second(second) {}

    Sub *derivative(string s) override {
        _allocated.push(new Sub{_first->derivative(s), _second->derivative(s)});
        return (Sub *) _allocated.top();
    }

    int eval(string s) override { return _first->eval(s) - _second->eval(s); }
};

class Mul : public Expression {
private:
    Expression *_first;
    Expression *_second;

    void printWithoutN() override {
        cout << '(';
        _first->printWithoutN();
        cout << '*';
        _second->printWithoutN();
        cout << ')';
    }

public:
    Mul(Expression *first, Expression *second) : _first(first), _second(second) {}

    Add *derivative(string s) override {
        _allocated.push(new Add{
                new Mul{_first->derivative(s), _second},
                new Mul{_first, _second->derivative(s)}
        });
        return (Add *) _allocated.top();
    }

    int eval(string s) override { return _first->eval(s) - _second->eval(s); }
};

class Div : public Expression {
private:
    Expression *_first;
    Expression *_second;

    void printWithoutN() override {
        cout << '(';
        _first->printWithoutN();
        cout << '/';
        _second->printWithoutN();
        cout << ')';
    }

public:
    Div(Expression *first, Expression *second) : _first(first), _second(second) {}

    Div *derivative(string s) override {
        _allocated.push(new Div{
                new Sub{
                        new Mul{_first->derivative(s), _second},
                        new Mul{_first, _second->derivative(s)}
                },
                new Mul{_second, _second}
        });
        return (Div *) _allocated.top();
    }

    int eval(string s) override { return _first->eval(s) - _second->eval(s); }
};

void example() {
    Expression *e = new Add(new Number(3), new Mul(new Number(2), new Variable("x")));
    cout << "f(x) = ";
    e->print();

    Expression *dex = e->derivative("x");
    cout << "f'(x) = df(x)/dx = ";
    dex->print();

    Expression *dey = e->derivative("y");
    cout << "df(x)/dy = ";
    dey->print();
}

Expression *extract_expression(string s, stack<Expression *> &allocated) {
    int cnt = 0;
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == '(') ++cnt;
        else if (s[i] == ')') --cnt;
        else if (cnt == 0) {  // If there are only numbers and variables left
            if ('0' <= s[i] && s[i] <= '9') {  // Number
                int result = s[i] - '0';
                for (int j = i + 1; j < s.size(); ++j) {
                    result = result * 10 + (s[j] - '0');
                }
                allocated.push(new Number{result});
            } else {  // Variable
                string result{s[i]};
                for (int j = i + 1; j < s.size(); ++j) {
                    result += s[j];
                }
                allocated.push(new Variable{result});
            }
            return allocated.top();
        } else if (cnt == 1) {  // If the first Monomial is over. We found the last operation
            while (s[i] != '+' && s[i] != '-' && s[i] != '*' && s[i] != '/') ++i;
            char sign = s[i];
            string first = s.substr(1, i - 1), second = s.substr(i + 1, s.size() - i - 2);
            if (sign == '+')
                allocated.push(new Add{extract_expression(first, allocated), extract_expression(second, allocated)});
            else if (sign == '-')
                allocated.push(new Sub{extract_expression(first, allocated), extract_expression(second, allocated)});
            else if (sign == '*')
                allocated.push(new Mul{extract_expression(first, allocated), extract_expression(second, allocated)});
            else if (sign == '/')
                allocated.push(new Div{extract_expression(first, allocated), extract_expression(second, allocated)});
            return allocated.top();
        }
    }
}

void deallocating(stack<Expression *> &allocated) {
    while (!allocated.empty()) {
        delete allocated.top();
        allocated.pop();
    }
}

void main_task() {
    stack<Expression *> allocated;
    string s;
    cin >> s;

    Expression *e = extract_expression(s, allocated);
    Expression *de = e->derivative("x");
    de->print();

    deallocating(allocated);
}

bool isOperation(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }

bool isDigit(char c) { return '0' <= c && c <= '9'; }

bool isCharOfVar(char c) { return !isOperation(c) && !isDigit(c) && c != '(' && c != ')'; }

enum MonomialTypes {
    number, variable, operation
};

string adding_brackets(const string &s) {
    struct Monomial {
        string value;
        double priority;
        MonomialTypes type;
        int unique_priority;

        Monomial(string v, MonomialTypes t, double p = -1, int u = 0) : value(std::move(v)), type(t), priority(p) {}
    };
    // Converting an expression into a convenient form
    vector<Monomial> monomials;
    int priority = 0;
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == '(') ++priority;
        else if (s[i] == ')') --priority;
        else if (isOperation(s[i])) {
            monomials.emplace_back(string{s[i]}, operation, priority + (s[i] == '*' || s[i] == '/' ? 0.5 : 0));
        } else if (isDigit(s[i])) {
            int n = s[i] - '0';
            for (; i + 1 < s.size() && '0' <= s[i + 1] && s[i + 1] <= '9'; ++i) n = n * 10 + (s[i + 1] - '0');
            monomials.emplace_back(to_string(n), number);
        } else {  // Variable
            int i_start = i;
            for (; i + 1 < s.size() && isCharOfVar(s[i + 1]); ++i);
            monomials.emplace_back(s.substr(i_start, i - i_start + 1), variable);
        }
    }
    // We define the order of operations
    int cnt_operations = 0;
    for (const Monomial &x: monomials) {
        if (x.type == operation) ++cnt_operations;
    }
    int cnt = cnt_operations;
    double max_priority = 0;
    for (const Monomial &x: monomials) { max_priority = max(x.priority, max_priority); }
    for (double cur_priority = max_priority; cur_priority >= 0; cur_priority -= 0.5) {
        for (Monomial &e: monomials) {
            if (e.type == operation && e.priority == cur_priority) {
                e.unique_priority = cnt--;
            }
        }
    }
    // Now we add parentheses to our expression
    list<pair<string, int>> l;  // <value, unique_priority>
    for (Monomial &x: monomials) {
        l.emplace_back(x.value, (x.type == operation ? x.unique_priority : -1));
    }
    for (int i = cnt_operations; i > 0; --i) {
        auto t = l.begin(), prev = l.begin();
        while (t->second != i) prev = t++;  // Finding required operation
        auto operation = t++;  // After this line, t - next of operation
        l.insert(t, make_pair("(" + prev->first + operation->first + t->first + ")", -1));
        l.erase(prev);
        l.erase(operation);
        l.erase(t);
    }
    return l.back().first;  // l.size() == 1
}

int main() {
    stack<Expression *> allocated;
    string s;
    cin >> s;
    s = adding_brackets(s);

    Expression *e = extract_expression(s, allocated);
    Expression *de = e->derivative("x");
    de->print();

    deallocating(allocated);
    return 0;
}
