#define BOOST_TEST_MODULE AdtBoostTests
#define BOOST_TEST_DYN_LINK
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../str/array.h"
#include "../../str/linked_list.h"
#include "../../str/one_linked_list.h"
#include "../../str/open_hash.h"
#include "../../str/queue.h"
#include "../../str/stack.h"
#include "../../str/tree.h"

using ListTypes = boost::mpl::list<OneLinked, Linked>;
using AllTypes = boost::mpl::list<Array, OneLinked, Linked, Stack, Queue, OpenHash, Tree>;

struct ListFixture {
    ListFixture() {
        for (const char* v : {"10", "20", "30", "40", "50"}) {
            one.push_back(v);
            two.push_back(v);
        }
    }
    OneLinked one;
    Linked two;
};

BOOST_AUTO_TEST_SUITE(Lists)

BOOST_AUTO_TEST_CASE_TEMPLATE(PushAndRead, T, ListTypes) {
    T l;
    BOOST_CHECK(l.empty());
    BOOST_CHECK_EQUAL(l.read(), "");
    BOOST_CHECK_EQUAL(l.read_reverse(), "");
    l.push_back("b");
    l.push_front("a");
    l.push_back("c");
    BOOST_CHECK_EQUAL(l.read(), " a b c");
    BOOST_CHECK_EQUAL(l.read_reverse(), " c b a");
    BOOST_CHECK_EQUAL(l.size(), 3U);
    BOOST_CHECK_EQUAL(l.find("b"), 1);
    BOOST_CHECK_EQUAL(l.find("zzz"), -1);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(PushAfterBefore, T, ListTypes) {
    T l;
    for (const char* v : {"10", "20", "30"}) l.push_back(v);
    l.push_after("30", "40");
    l.push_after("10", "15");
    l.push_before("10", "05");
    l.push_before("30", "25");
    BOOST_CHECK_EQUAL(l.read(), " 05 10 15 20 25 30 40");
    BOOST_CHECK_EQUAL(l.read_reverse(), " 40 30 25 20 15 10 05");
    BOOST_CHECK_THROW(l.push_after("zzz", "1"), invalid_argument);
    BOOST_CHECK_THROW(l.push_before("zzz", "1"), invalid_argument);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Pops, T, ListTypes) {
    T l;
    for (const char* v : {"10", "20", "30", "40", "50"}) l.push_back(v);
    BOOST_CHECK_EQUAL(l.pop_front(), "10");
    BOOST_CHECK_EQUAL(l.pop_back(), "50");
    BOOST_CHECK_EQUAL(l.pop_after("20"), "30");
    BOOST_CHECK_EQUAL(l.pop_before("40"), "20");
    BOOST_CHECK_EQUAL(l.read(), " 40");
    BOOST_CHECK_THROW(l.pop_after("40"), out_of_range);
    BOOST_CHECK_THROW(l.pop_before("40"), out_of_range);
    BOOST_CHECK_THROW(l.pop_after("zzz"), invalid_argument);
    BOOST_CHECK_THROW(l.pop_before("zzz"), invalid_argument);
    l.pop_back();
    BOOST_CHECK_THROW(l.pop_front(), out_of_range);
    BOOST_CHECK_THROW(l.pop_back(), out_of_range);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DelAndClear, T, ListTypes) {
    T l;
    for (const char* v : {"10", "20", "30", "40", "50"}) l.push_back(v);
    BOOST_CHECK(l.del_by_value("10"));
    BOOST_CHECK(l.del_by_value("50"));
    BOOST_CHECK(l.del_by_value("30"));
    BOOST_CHECK(!l.del_by_value("zzz"));
    BOOST_CHECK_EQUAL(l.read(), " 20 40");
    l.clear();
    BOOST_CHECK(l.empty());
    l.push_front("n");
    BOOST_CHECK_EQUAL(l.read(), " n");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(CopyAndAssign, T, ListTypes) {
    T l;
    l.push_back("a");
    l.push_back("b");
    T copy(l);
    copy.push_back("c");
    BOOST_CHECK_EQUAL(l.read(), " a b");
    BOOST_CHECK_EQUAL(copy.read(), " a b c");
    T other;
    other.push_back("x");
    other = l;
    BOOST_CHECK_EQUAL(other.read(), " a b");
    T& ref = l;
    l = ref;
    BOOST_CHECK_EQUAL(l.read(), " a b");
    other = T();
    BOOST_CHECK(other.empty());
    T empty_copy(other);
    BOOST_CHECK(empty_copy.empty());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(Serialization, T, ListTypes) {
    T l;
    l.push_back("a");
    l.push_back("two words");
    stringstream bin;
    l.serialize_binary(bin);
    T b;
    b.push_back("old");
    b.deserialize_binary(bin);
    BOOST_CHECK_EQUAL(b.read(), l.read());
    stringstream txt;
    l.serialize_text(txt);
    T t;
    t.deserialize_text(txt);
    BOOST_CHECK_EQUAL(t.read(), l.read());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(StackQueue)

BOOST_AUTO_TEST_CASE(StackLifo) {
    Stack s;
    s.push("a");
    s.push("b");
    BOOST_CHECK_EQUAL(s.top(), "b");
    BOOST_CHECK_EQUAL(s.read(), " b a");
    BOOST_CHECK_EQUAL(s.size(), 2U);
    BOOST_CHECK_EQUAL(s.pop(), "b");
    BOOST_CHECK_EQUAL(s.pop(), "a");
    BOOST_CHECK(s.empty());
    BOOST_CHECK_THROW(s.pop(), out_of_range);
    BOOST_CHECK_THROW(s.top(), out_of_range);
}

BOOST_AUTO_TEST_CASE(StackCopyAssignClear) {
    Stack s;
    s.push("a");
    s.push("b");
    Stack c(s);
    c.push("c");
    BOOST_CHECK_EQUAL(s.read(), " b a");
    Stack o;
    o.push("x");
    o = s;
    BOOST_CHECK_EQUAL(o.read(), " b a");
    Stack& ref = s;
    s = ref;
    s.clear();
    BOOST_CHECK(s.empty());
    Stack e;
    o = e;
    BOOST_CHECK(o.empty());
}

BOOST_AUTO_TEST_CASE(QueueFifo) {
    Queue q;
    q.push("a");
    q.push("b");
    q.push("c");
    BOOST_CHECK_EQUAL(q.front(), "a");
    BOOST_CHECK_EQUAL(q.read(), " a b c");
    BOOST_CHECK_EQUAL(q.pop(), "a");
    BOOST_CHECK_EQUAL(q.pop(), "b");
    q.push("d");
    BOOST_CHECK_EQUAL(q.pop(), "c");
    BOOST_CHECK_EQUAL(q.pop(), "d");
    BOOST_CHECK(q.empty());
    BOOST_CHECK_THROW(q.pop(), out_of_range);
    BOOST_CHECK_THROW(q.front(), out_of_range);
}

BOOST_AUTO_TEST_CASE(QueueCopyAssignClear) {
    Queue q;
    q.push("a");
    q.push("b");
    Queue c(q);
    c.push("c");
    BOOST_CHECK_EQUAL(q.read(), " a b");
    Queue o;
    o.push("x");
    o = q;
    BOOST_CHECK_EQUAL(o.read(), " a b");
    Queue& ref = q;
    q = ref;
    q.clear();
    BOOST_CHECK(q.empty());
    Queue e;
    o = e;
    BOOST_CHECK(o.empty());
}

BOOST_AUTO_TEST_CASE(StackQueueSerialization) {
    Stack s;
    s.push("a");
    s.push("two words");
    stringstream sb;
    s.serialize_binary(sb);
    Stack s2;
    s2.deserialize_binary(sb);
    BOOST_CHECK_EQUAL(s2.read(), s.read());
    stringstream st;
    s.serialize_text(st);
    Stack s3;
    s3.deserialize_text(st);
    BOOST_CHECK_EQUAL(s3.read(), s.read());

    Queue q;
    q.push("a");
    q.push("two words");
    stringstream qb;
    q.serialize_binary(qb);
    Queue q2;
    q2.deserialize_binary(qb);
    BOOST_CHECK_EQUAL(q2.read(), q.read());
    stringstream qt;
    q.serialize_text(qt);
    Queue q3;
    q3.deserialize_text(qt);
    BOOST_CHECK_EQUAL(q3.read(), q.read());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ArrayTests)

struct ArrayFixture {
    ArrayFixture() {
        arr.push_back("a");
        arr.push_back("b");
        arr.push_back("c");
    }
    Array arr;
};

BOOST_FIXTURE_TEST_CASE(Basics, ArrayFixture) {
    BOOST_CHECK_EQUAL(arr.length(), 3U);
    BOOST_CHECK_EQUAL(arr.get(2), "c");
    BOOST_CHECK_EQUAL(arr.read(), " a b c");
    BOOST_CHECK_THROW(arr.get(3), out_of_range);
    arr.set(1, "B");
    BOOST_CHECK_EQUAL(arr.read(), " a B c");
    BOOST_CHECK_THROW(arr.set(3, "x"), out_of_range);
}

BOOST_FIXTURE_TEST_CASE(InsertErase, ArrayFixture) {
    arr.insert(0, "x");
    arr.insert(2, "y");
    arr.insert(arr.length(), "z");
    BOOST_CHECK_EQUAL(arr.read(), " x a y b c z");
    BOOST_CHECK_THROW(arr.insert(arr.length() + 1, "bad"), out_of_range);
    arr.erase(0);
    arr.erase(arr.length() - 1);
    BOOST_CHECK_EQUAL(arr.read(), " a y b c");
    BOOST_CHECK_THROW(arr.erase(arr.length()), out_of_range);
    arr.clear();
    BOOST_CHECK(arr.empty());
}

BOOST_AUTO_TEST_CASE(Capacity) {
    Array a;
    BOOST_CHECK_EQUAL(a.capacity(), Array::MIN_CAP);
    for (size_t i = 0; i < Array::MIN_CAP + 1; i++) a.push_back("x");
    BOOST_CHECK_EQUAL(a.capacity(), Array::MIN_CAP * 2);
    BOOST_CHECK_EQUAL(Array(3).capacity(), Array::MIN_CAP);
    BOOST_CHECK_EQUAL(Array(100).capacity(), 100U);
    BOOST_CHECK_THROW(Array(Array::MAX_CAP + 1), length_error);
    Array big(Array::MAX_CAP);
    for (size_t i = 0; i < Array::MAX_CAP; i++) big.push_back("x");
    BOOST_CHECK_THROW(big.push_back("x"), length_error);
}

BOOST_FIXTURE_TEST_CASE(CopyAssign, ArrayFixture) {
    Array c(arr);
    c.set(0, "X");
    BOOST_CHECK_EQUAL(arr.read(), " a b c");
    Array o;
    o.push_back("q");
    o = arr;
    BOOST_CHECK_EQUAL(o.read(), " a b c");
    Array& ref = arr;
    arr = ref;
    BOOST_CHECK_EQUAL(arr.read(), " a b c");
}

BOOST_FIXTURE_TEST_CASE(Serialization, ArrayFixture) {
    stringstream bin;
    arr.serialize_binary(bin);
    Array b;
    b.deserialize_binary(bin);
    BOOST_CHECK_EQUAL(b.read(), arr.read());
    stringstream txt;
    arr.serialize_text(txt);
    Array t;
    t.deserialize_text(txt);
    BOOST_CHECK_EQUAL(t.read(), arr.read());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(HashTests)

BOOST_AUTO_TEST_CASE(InsertEraseContains) {
    OpenHash h;
    BOOST_CHECK(h.insert("a"));
    BOOST_CHECK(h.insert("b"));
    BOOST_CHECK(!h.insert("a"));
    BOOST_CHECK(h.contains("a"));
    BOOST_CHECK(!h.contains("zzz"));
    BOOST_CHECK_EQUAL(h.size(), 2U);
    BOOST_CHECK(h.find("a") >= 0);
    BOOST_CHECK_EQUAL(h.find("zzz"), -1);
    BOOST_CHECK(h.erase("a"));
    BOOST_CHECK(!h.erase("a"));
    BOOST_CHECK_EQUAL(h.find("a"), -1);
    BOOST_CHECK(h.insert("a"));
    h.clear();
    BOOST_CHECK(h.empty());
    BOOST_CHECK_EQUAL(h.read(), "");
}

BOOST_AUTO_TEST_CASE(RehashAndTombstones) {
    OpenHash h;
    for (int i = 0; i < 500; i++) h.insert("k" + to_string(i));
    BOOST_CHECK_EQUAL(h.size(), 500U);
    BOOST_CHECK(h.capacity() > OpenHash::MIN_CAP);
    for (int i = 0; i < 500; i += 2) h.erase("k" + to_string(i));
    for (int i = 0; i < 500; i++) BOOST_CHECK_EQUAL(h.contains("k" + to_string(i)), i % 2 == 1);
    OpenHash t;
    for (int i = 0; i < 50; i++) {
        t.insert("t" + to_string(i));
        t.erase("t" + to_string(i));
    }
    BOOST_CHECK(t.empty());
}

BOOST_AUTO_TEST_CASE(CapacityLimits) {
    BOOST_CHECK_EQUAL(OpenHash(20).capacity(), 32U);
    BOOST_CHECK_THROW(OpenHash(OpenHash::MAX_CAP + 1), length_error);
    OpenHash big(OpenHash::MAX_CAP);
    bool thrown = false;
    try {
        for (int i = 0; i < 60000; i++) big.insert("k" + to_string(i));
    } catch (const length_error&) {
        thrown = true;
    }
    BOOST_CHECK(thrown);
}

BOOST_AUTO_TEST_CASE(HashFunctions) {
    BOOST_CHECK_EQUAL(OpenHash::hash_djb2("abc"), OpenHash::hash_djb2("abc"));
    BOOST_CHECK(OpenHash::hash_djb2("abc") != OpenHash::hash_djb2("abd"));
    BOOST_CHECK_EQUAL(OpenHash::hash_fnv1a("abc"), OpenHash::hash_fnv1a("abc"));
    BOOST_CHECK(OpenHash::hash_fnv1a("abc") != OpenHash::hash_fnv1a("abd"));
}

BOOST_AUTO_TEST_CASE(CopyAssignSerialize) {
    OpenHash h;
    h.insert("a");
    h.insert("two words");
    OpenHash c(h);
    c.insert("d");
    BOOST_CHECK_EQUAL(h.size(), 2U);
    OpenHash o;
    o.insert("q");
    o = h;
    BOOST_CHECK(!o.contains("q"));
    OpenHash& ref = h;
    h = ref;
    stringstream bin;
    h.serialize_binary(bin);
    OpenHash b;
    b.deserialize_binary(bin);
    BOOST_CHECK_EQUAL(b.size(), 2U);
    BOOST_CHECK(b.contains("two words"));
    stringstream txt;
    h.serialize_text(txt);
    OpenHash t;
    t.deserialize_text(txt);
    BOOST_CHECK_EQUAL(t.size(), 2U);
    BOOST_CHECK(t.contains("a"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TreeTests)

struct TreeFixture {
    TreeFixture() {
        for (int v : {50, 30, 70, 20, 40, 60, 80}) t.insert(v);
    }
    Tree t;
};

BOOST_FIXTURE_TEST_CASE(InsertMember, TreeFixture) {
    BOOST_CHECK_EQUAL(t.size(), 7U);
    BOOST_CHECK(t.is_member(40));
    BOOST_CHECK(!t.is_member(45));
    BOOST_CHECK(!t.insert(50));
    BOOST_CHECK_EQUAL(t.height(), 3);
    BOOST_CHECK(t.is_avl());
    BOOST_CHECK_EQUAL(t.read(), " 20 30 40 50 60 70 80");
}

BOOST_FIXTURE_TEST_CASE(Erase, TreeFixture) {
    BOOST_CHECK(t.erase(20));
    BOOST_CHECK(t.erase(30));
    BOOST_CHECK(t.erase(50));
    BOOST_CHECK(!t.erase(999));
    BOOST_CHECK_EQUAL(t.read(), " 40 60 70 80");
    BOOST_CHECK(t.is_avl());
    for (int v : {40, 60, 70, 80}) BOOST_CHECK(t.erase(v));
    BOOST_CHECK(t.empty());
    BOOST_CHECK_EQUAL(t.height(), 0);
}

BOOST_AUTO_TEST_CASE(Rotations) {
    std::vector<std::vector<int>> cases = {{1, 2, 3}, {3, 2, 1}, {1, 3, 2}, {3, 1, 2}};
    for (const auto& c : cases) {
        Tree t;
        for (int v : c) t.insert(v);
        BOOST_CHECK(t.is_avl());
        BOOST_CHECK_EQUAL(t.height(), 2);
    }
    Tree big;
    for (int i = 1; i <= 200; i++) big.insert(i);
    BOOST_CHECK(big.is_avl());
    for (int i = 1; i <= 200; i += 2) big.erase(i);
    BOOST_CHECK(big.is_avl());
    BOOST_CHECK_EQUAL(big.size(), 100U);
}

BOOST_FIXTURE_TEST_CASE(StructureCopyClear, TreeFixture) {
    BOOST_CHECK(t.structure().find("root = 50") != string::npos);
    stringstream ss;
    t.print(ss);
    BOOST_CHECK_EQUAL(ss.str(), t.structure());
    Tree c(t);
    c.insert(10);
    BOOST_CHECK_EQUAL(t.size(), 7U);
    Tree o;
    o.insert(1);
    o = t;
    BOOST_CHECK_EQUAL(o.size(), 7U);
    Tree& ref = t;
    t = ref;
    t.clear();
    BOOST_CHECK(t.empty());
    BOOST_CHECK_EQUAL(t.structure(), "");
}

BOOST_FIXTURE_TEST_CASE(Serialization, TreeFixture) {
    stringstream bin;
    t.serialize_binary(bin);
    Tree b;
    b.deserialize_binary(bin);
    BOOST_CHECK_EQUAL(b.structure(), t.structure());
    stringstream txt;
    t.serialize_text(txt);
    Tree x;
    x.deserialize_text(txt);
    BOOST_CHECK_EQUAL(x.structure(), t.structure());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(SerializeErrors)

BOOST_AUTO_TEST_CASE_TEMPLATE(EmptyRoundTrip, T, AllTypes) {
    T a;
    stringstream bin;
    a.serialize_binary(bin);
    T b;
    b.deserialize_binary(bin);
    BOOST_CHECK(b.empty());
    stringstream txt;
    a.serialize_text(txt);
    T c;
    c.deserialize_text(txt);
    BOOST_CHECK(c.empty());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BadBinary, T, AllTypes) {
    T a;
    stringstream bad_tag("XXXXXXXXXXXX");
    BOOST_CHECK_THROW(a.deserialize_binary(bad_tag), runtime_error);
    stringstream ok;
    a.serialize_binary(ok);
    string s = ok.str();
    stringstream cut(s.substr(0, 6));
    BOOST_CHECK_THROW(a.deserialize_binary(cut), runtime_error);
    stringstream none;
    BOOST_CHECK_THROW(a.deserialize_binary(none), runtime_error);
    for (size_t i = 4; i < 12; i++) s[i] = '\xFF';
    stringstream huge(s);
    BOOST_CHECK_THROW(a.deserialize_binary(huge), runtime_error);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(BadText, T, AllTypes) {
    T a;
    stringstream bad_tag("Wrong 0\n");
    BOOST_CHECK_THROW(a.deserialize_text(bad_tag), runtime_error);
    stringstream none;
    BOOST_CHECK_THROW(a.deserialize_text(none), runtime_error);
    stringstream ok;
    a.serialize_text(ok);
    string head = ok.str().substr(0, ok.str().find(' '));
    stringstream huge(head + " 99999999999\n");
    BOOST_CHECK_THROW(a.deserialize_text(huge), runtime_error);
    stringstream truncated(head + " 3\n");
    BOOST_CHECK_THROW(a.deserialize_text(truncated), runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()
