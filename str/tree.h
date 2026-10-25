#ifndef TREE_H
#define TREE_H

#include <cstddef>
#include <iosfwd>
#include <string>

using namespace std;

class Tree {
public:
    Tree();
    ~Tree();

    Tree(const Tree& other);
    Tree& operator=(const Tree& other);

    bool insert(int data);
    bool erase(int data);
    bool is_member(int data) const;
    void clear();

    size_t size() const;
    bool empty() const;
    int height() const;
    bool is_avl() const;

    string read() const;
    string structure() const;
    void print(ostream& out) const;

    void serialize_binary(ostream& out) const;
    void deserialize_binary(istream& in);
    void serialize_text(ostream& out) const;
    void deserialize_text(istream& in);

private:
    struct Node {
        int data;
        int height;
        Node* left;
        Node* right;
        explicit Node(int val);
    };
    Node* root;
    size_t sz;

    static int get_height(const Node* node);
    static void update_height(Node* node);
    static int get_balance(const Node* node);
    static Node* left_rotate(Node* node);
    static Node* right_rotate(Node* node);
    static Node* make_balance(Node* node);
    static Node* find_max(Node* node);
    static Node* clone(const Node* node);
    static void destroy(Node* node);
    static bool check_avl(const Node* node, const int* lo, const int* hi, int& h);

    static Node* insert_node(Node* node, int data, bool& inserted);
    static Node* delete_node(Node* node, int data, bool& deleted);
    static void read_node(const Node* node, ostream& out);
    static void get_level_nodes(const Node* node, int target, int current,
                                const string& path, ostream& out);
    void level_order(ostream& out, bool binary) const;
};

#endif
