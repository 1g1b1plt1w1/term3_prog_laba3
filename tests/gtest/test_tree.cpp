#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "../../str/tree.h"

class TreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (int v : {50, 30, 70, 20, 40, 60, 80}) t.insert(v);
    }
    Tree t;
};

TEST_F(TreeTest, InsertAndMember) {
    EXPECT_EQ(t.size(), 7U);
    EXPECT_TRUE(t.is_member(40));
    EXPECT_TRUE(t.is_member(80));
    EXPECT_FALSE(t.is_member(45));
    EXPECT_FALSE(t.empty());
    EXPECT_EQ(t.height(), 3);
    EXPECT_TRUE(t.is_avl());
}

TEST_F(TreeTest, DuplicateInsert) {
    EXPECT_FALSE(t.insert(50));
    EXPECT_EQ(t.size(), 7U);
}

TEST_F(TreeTest, ReadSorted) {
    EXPECT_EQ(t.read(), " 20 30 40 50 60 70 80");
}

TEST_F(TreeTest, EraseLeaf) {
    EXPECT_TRUE(t.erase(20));
    EXPECT_FALSE(t.is_member(20));
    EXPECT_EQ(t.size(), 6U);
    EXPECT_TRUE(t.is_avl());
}

TEST_F(TreeTest, EraseOneChild) {
    t.erase(20);
    EXPECT_TRUE(t.erase(30));
    EXPECT_EQ(t.read(), " 40 50 60 70 80");
    EXPECT_TRUE(t.is_avl());
}

TEST_F(TreeTest, EraseTwoChildren) {
    EXPECT_TRUE(t.erase(50));
    EXPECT_FALSE(t.is_member(50));
    EXPECT_EQ(t.read(), " 20 30 40 60 70 80");
    EXPECT_TRUE(t.is_avl());
}

TEST_F(TreeTest, EraseMissing) {
    EXPECT_FALSE(t.erase(999));
    EXPECT_FALSE(t.erase(-5));
    EXPECT_EQ(t.size(), 7U);
}

TEST_F(TreeTest, EraseAll) {
    for (int v : {50, 30, 70, 20, 40, 60, 80}) {
        EXPECT_TRUE(t.erase(v));
        EXPECT_TRUE(t.is_avl());
    }
    EXPECT_TRUE(t.empty());
    EXPECT_EQ(t.height(), 0);
    EXPECT_EQ(t.read(), "");
}

TEST_F(TreeTest, EraseCausesRotation) {
    Tree x;
    for (int v : {10, 5, 20, 3, 7, 15, 30, 40}) x.insert(v);
    x.erase(5);
    x.erase(3);
    x.erase(7);
    EXPECT_TRUE(x.is_avl());
    EXPECT_EQ(x.read(), " 10 15 20 30 40");
    Tree y;
    for (int v : {20, 10, 30, 25, 40, 5, 15, 1}) y.insert(v);
    y.erase(40);
    y.erase(30);
    y.erase(25);
    EXPECT_TRUE(y.is_avl());
}

TEST_F(TreeTest, Clear) {
    t.clear();
    EXPECT_TRUE(t.empty());
    EXPECT_EQ(t.size(), 0U);
    EXPECT_FALSE(t.is_member(50));
    t.insert(1);
    EXPECT_EQ(t.read(), " 1");
}

TEST_F(TreeTest, StructureAndPrint) {
    string s = t.structure();
    EXPECT_NE(s.find("root = 50"), string::npos);
    EXPECT_NE(s.find("root->left = 30"), string::npos);
    EXPECT_NE(s.find("root->right->right = 80"), string::npos);
    stringstream ss;
    t.print(ss);
    EXPECT_EQ(ss.str(), s);
    Tree e;
    EXPECT_EQ(e.structure(), "");
}

TEST_F(TreeTest, CopyIsIndependent) {
    Tree c(t);
    c.insert(10);
    c.erase(50);
    EXPECT_EQ(t.read(), " 20 30 40 50 60 70 80");
    EXPECT_EQ(c.size(), 7U);
    EXPECT_TRUE(c.is_avl());
    Tree e;
    Tree ec(e);
    EXPECT_TRUE(ec.empty());
}

TEST_F(TreeTest, Assign) {
    Tree o;
    o.insert(1);
    o = t;
    o.insert(99);
    EXPECT_EQ(t.size(), 7U);
    EXPECT_EQ(o.size(), 8U);
    Tree& ref = t;
    t = ref;
    EXPECT_EQ(t.size(), 7U);
}

TEST_F(TreeTest, SerializeBinaryRoundTrip) {
    stringstream ss;
    t.serialize_binary(ss);
    Tree o;
    o.insert(1);
    o.deserialize_binary(ss);
    EXPECT_EQ(o.read(), t.read());
    EXPECT_EQ(o.structure(), t.structure());
}

TEST_F(TreeTest, SerializeTextRoundTrip) {
    stringstream ss;
    t.serialize_text(ss);
    Tree o;
    o.deserialize_text(ss);
    EXPECT_EQ(o.read(), t.read());
    EXPECT_EQ(o.structure(), t.structure());
}

class RotateTest : public ::testing::TestWithParam<vector<int>> {};

TEST_P(RotateTest, StaysBalanced) {
    Tree t;
    for (int v : GetParam()) t.insert(v);
    EXPECT_TRUE(t.is_avl());
    EXPECT_EQ(t.height(), 2);
    EXPECT_NE(t.structure().find("root = 2"), string::npos);
}

INSTANTIATE_TEST_SUITE_P(Rotations, RotateTest, ::testing::Values(
    vector<int>{1, 2, 3},
    vector<int>{3, 2, 1},
    vector<int>{1, 3, 2},
    vector<int>{3, 1, 2}));

TEST(Tree, SequentialInsertStaysBalanced) {
    Tree t;
    for (int i = 1; i <= 200; i++) t.insert(i);
    EXPECT_TRUE(t.is_avl());
    EXPECT_LE(t.height(), 9);
    for (int i = 1; i <= 200; i += 2) t.erase(i);
    EXPECT_TRUE(t.is_avl());
    EXPECT_EQ(t.size(), 100U);
}

TEST(Tree, EmptyTree) {
    Tree t;
    EXPECT_TRUE(t.empty());
    EXPECT_TRUE(t.is_avl());
    EXPECT_EQ(t.height(), 0);
    EXPECT_FALSE(t.is_member(1));
    EXPECT_FALSE(t.erase(1));
}
