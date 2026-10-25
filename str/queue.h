#ifndef QUEUE_H
#define QUEUE_H

#include <cstddef>
#include <iosfwd>
#include <string>

using namespace std;

class Queue {
public:
    Queue();
    ~Queue();

    Queue(const Queue& other);
    Queue& operator=(const Queue& other);

    void push(const string& value);
    string pop();
    const string& front() const;
    void clear();

    size_t size() const;
    bool empty() const;
    string read() const;

    void serialize_binary(ostream& out) const;
    void deserialize_binary(istream& in);
    void serialize_text(ostream& out) const;
    void deserialize_text(istream& in);

private:
    struct Node {
        string data;
        Node* next;
        explicit Node(string val, Node* nxt = nullptr);
    };
    Node* head;
    Node* tail;
    size_t sz;

    void copy_from(const Queue& other);
};

#endif
