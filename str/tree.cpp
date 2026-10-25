#include "tree.h"

#include <algorithm>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <utility>
#include <vector>

#include "serialize.h"

using namespace std;

Tree::Node::Node(int val) : data(val), height(1), left(nullptr), right(nullptr) {}

Tree::Tree() : root(nullptr), sz(0) {}

Tree::~Tree() {
    destroy(root);
}

Tree::Tree(const Tree& other) : root(clone(other.root)), sz(other.sz) {}

Tree& Tree::operator=(const Tree& other) {
    if (this == &other) return *this;
    Node* copy = clone(other.root);
    destroy(root);
    root = copy;
    sz = other.sz;
    return *this;
}

int Tree::get_height(const Node* node) {
    return node ? node->height : 0;
}

void Tree::update_height(Node* node) {
    node->height = 1 + max(get_height(node->left), get_height(node->right));
}

int Tree::get_balance(const Node* node) {
    return get_height(node->left) - get_height(node->right);
}

Tree::Node* Tree::left_rotate(Node* node) {
    Node* r = node->right;
    node->right = r->left;
    r->left = node;
    update_height(node);
    update_height(r);
    return r;
}

Tree::Node* Tree::right_rotate(Node* node) {
    Node* l = node->left;
    node->left = l->right;
    l->right = node;
    update_height(node);
    update_height(l);
    return l;
}

Tree::Node* Tree::make_balance(Node* node) {
    const int balance = get_balance(node);
    if (balance > 1) {
        if (get_balance(node->left) < 0) node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    if (balance < -1) {
        if (get_balance(node->right) > 0) node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    return node;
}

Tree::Node* Tree::insert_node(Node* node, int data, bool& inserted) {
    if (!node) {
        inserted = true;
        return new Node(data);
    }
    if (data < node->data) node->left = insert_node(node->left, data, inserted);
    else if (data > node->data) node->right = insert_node(node->right, data, inserted);
    else return node;

    update_height(node);
    return make_balance(node);
}

Tree::Node* Tree::find_max(Node* node) {
    while (node->right) node = node->right;
    return node;
}

Tree::Node* Tree::delete_node(Node* node, int data, bool& deleted) {
    if (!node) return nullptr;
    if (data < node->data) {
        node->left = delete_node(node->left, data, deleted);
    } else if (data > node->data) {
        node->right = delete_node(node->right, data, deleted);
    } else {
        if (!node->left || !node->right) {
            Node* child = node->left ? node->left : node->right;
            delete node;
            deleted = true;
            return child;
        }
        const Node* pred = find_max(node->left);
        node->data = pred->data;
        node->left = delete_node(node->left, pred->data, deleted);
    }
    update_height(node);
    return make_balance(node);
}

Tree::Node* Tree::clone(const Node* node) {
    if (!node) return nullptr;
    Node* copy = new Node(node->data);
    copy->height = node->height;
    copy->left = clone(node->left);
    copy->right = clone(node->right);
    return copy;
}

void Tree::destroy(Node* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

bool Tree::insert(int data) {
    bool inserted = false;
    root = insert_node(root, data, inserted);
    if (inserted) sz++;
    return inserted;
}

bool Tree::erase(int data) {
    bool deleted = false;
    root = delete_node(root, data, deleted);
    if (deleted) sz--;
    return deleted;
}

bool Tree::is_member(int data) const {
    const Node* node = root;
    while (node && node->data != data) {
        node = (data < node->data) ? node->left : node->right;
    }
    return node != nullptr;
}

void Tree::clear() {
    destroy(root);
    root = nullptr;
    sz = 0;
}

size_t Tree::size() const {
    return sz;
}

bool Tree::empty() const {
    return root == nullptr;
}

int Tree::height() const {
    return get_height(root);
}

bool Tree::check_avl(const Node* node, const int* lo, const int* hi, int& h) {
    if (!node) {
        h = 0;
        return true;
    }
    if ((lo && node->data <= *lo) || (hi && node->data >= *hi)) return false;
    int lh = 0;
    int rh = 0;
    if (!check_avl(node->left, lo, &node->data, lh)) return false;
    if (!check_avl(node->right, &node->data, hi, rh)) return false;
    h = 1 + max(lh, rh);
    return abs(lh - rh) <= 1 && node->height == h;
}

bool Tree::is_avl() const {
    int h = 0;
    return check_avl(root, nullptr, nullptr, h);
}

void Tree::read_node(const Node* node, ostream& out) {
    if (!node) return;
    read_node(node->left, out);
    out << " " << node->data;
    read_node(node->right, out);
}

string Tree::read() const {
    stringstream ss;
    read_node(root, ss);
    return ss.str();
}

void Tree::get_level_nodes(const Node* node, int target, int current,
                           const string& path, ostream& out) {
    if (!node) return;
    if (current == target) {
        out << path << " = " << node->data << "\n";
        return;
    }
    get_level_nodes(node->left, target, current + 1, path + "->left", out);
    get_level_nodes(node->right, target, current + 1, path + "->right", out);
}

string Tree::structure() const {
    stringstream ss;
    for (int d = 0; d < height(); d++) {
        get_level_nodes(root, d, 0, "root", ss);
    }
    return ss.str();
}

void Tree::print(ostream& out) const {
    out << structure();
}

void Tree::level_order(ostream& out, bool binary) const {
    vector<const Node*> level;
    if (root) level.push_back(root);
    while (!level.empty()) {
        vector<const Node*> next;
        for (const Node* node : level) {
            if (binary) ser::write_int(out, node->data);
            else ser::write_text_int(out, node->data);
            if (node->left) next.push_back(node->left);
            if (node->right) next.push_back(node->right);
        }
        level = move(next);
    }
}

void Tree::serialize_binary(ostream& out) const {
    ser::write_tag(out, "TREE");
    ser::write_size(out, sz);
    level_order(out, true);
}

void Tree::deserialize_binary(istream& in) {
    ser::read_tag(in, "TREE");
    const size_t n = ser::read_size(in);
    Tree tmp;
    for (size_t i = 0; i < n; i++) {
        tmp.insert(ser::read_int(in));
    }
    *this = tmp;
}

void Tree::serialize_text(ostream& out) const {
    ser::write_text_header(out, "Tree", sz);
    level_order(out, false);
}

void Tree::deserialize_text(istream& in) {
    const size_t n = ser::read_text_header(in, "Tree");
    Tree tmp;
    for (size_t i = 0; i < n; i++) {
        tmp.insert(ser::read_text_int(in));
    }
    *this = tmp;
}
