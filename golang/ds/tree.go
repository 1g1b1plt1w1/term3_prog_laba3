package ds

import (
	"bufio"
	"fmt"
	"io"
	"strconv"
	"strings"
)

type treeNode struct {
	data        int
	height      int
	left, right *treeNode
}

type Tree struct {
	root *treeNode
	size int
}

func height(n *treeNode) int {
	if n == nil {
		return 0
	}
	return n.height
}

func updateHeight(n *treeNode) {
	n.height = 1 + max(height(n.left), height(n.right))
}

func balance(n *treeNode) int { return height(n.left) - height(n.right) }

func rotateLeft(n *treeNode) *treeNode {
	r := n.right
	n.right = r.left
	r.left = n
	updateHeight(n)
	updateHeight(r)
	return r
}

func rotateRight(n *treeNode) *treeNode {
	l := n.left
	n.left = l.right
	l.right = n
	updateHeight(n)
	updateHeight(l)
	return l
}

func rebalance(n *treeNode) *treeNode {
	switch b := balance(n); {
	case b > 1:
		if balance(n.left) < 0 {
			n.left = rotateLeft(n.left)
		}
		return rotateRight(n)
	case b < -1:
		if balance(n.right) > 0 {
			n.right = rotateRight(n.right)
		}
		return rotateLeft(n)
	}
	return n
}

func insertNode(n *treeNode, v int, inserted *bool) *treeNode {
	if n == nil {
		*inserted = true
		return &treeNode{data: v, height: 1}
	}
	switch {
	case v < n.data:
		n.left = insertNode(n.left, v, inserted)
	case v > n.data:
		n.right = insertNode(n.right, v, inserted)
	default:
		return n
	}
	updateHeight(n)
	return rebalance(n)
}

func deleteNode(n *treeNode, v int, deleted *bool) *treeNode {
	if n == nil {
		return nil
	}
	switch {
	case v < n.data:
		n.left = deleteNode(n.left, v, deleted)
	case v > n.data:
		n.right = deleteNode(n.right, v, deleted)
	default:
		if n.left == nil || n.right == nil {
			*deleted = true
			if n.left != nil {
				return n.left
			}
			return n.right
		}
		pred := n.left
		for pred.right != nil {
			pred = pred.right
		}
		n.data = pred.data
		n.left = deleteNode(n.left, pred.data, deleted)
	}
	updateHeight(n)
	return rebalance(n)
}

func (t *Tree) Insert(v int) bool {
	inserted := false
	t.root = insertNode(t.root, v, &inserted)
	if inserted {
		t.size++
	}
	return inserted
}

func (t *Tree) Erase(v int) bool {
	deleted := false
	t.root = deleteNode(t.root, v, &deleted)
	if deleted {
		t.size--
	}
	return deleted
}

func (t *Tree) IsMember(v int) bool {
	n := t.root
	for n != nil && n.data != v {
		if v < n.data {
			n = n.left
		} else {
			n = n.right
		}
	}
	return n != nil
}

func (t *Tree) Clear() { *t = Tree{} }

func (t *Tree) Len() int { return t.size }

func (t *Tree) Empty() bool { return t.root == nil }

func (t *Tree) Height() int { return height(t.root) }

func (t *Tree) IsAVL() bool {
	_, ok := checkAVL(t.root, nil, nil)
	return ok
}

func checkAVL(n *treeNode, lo, hi *int) (int, bool) {
	if n == nil {
		return 0, true
	}
	if (lo != nil && n.data <= *lo) || (hi != nil && n.data >= *hi) {
		return 0, false
	}
	lh, okL := checkAVL(n.left, lo, &n.data)
	rh, okR := checkAVL(n.right, &n.data, hi)
	h := 1 + max(lh, rh)
	return h, okL && okR && lh-rh <= 1 && rh-lh <= 1 && n.height == h
}

func (t *Tree) Items() []int {
	out := make([]int, 0, t.size)
	var walk func(n *treeNode)
	walk = func(n *treeNode) {
		if n == nil {
			return
		}
		walk(n.left)
		out = append(out, n.data)
		walk(n.right)
	}
	walk(t.root)
	return out
}

func (t *Tree) String() string {
	var sb strings.Builder
	for _, v := range t.Items() {
		sb.WriteString(" ")
		sb.WriteString(strconv.Itoa(v))
	}
	return sb.String()
}

func (t *Tree) Structure() string {
	var sb strings.Builder
	type item struct {
		node *treeNode
		path string
	}
	level := []item{}
	if t.root != nil {
		level = append(level, item{t.root, "root"})
	}
	for len(level) > 0 {
		var next []item
		for _, it := range level {
			fmt.Fprintf(&sb, "%s = %d\n", it.path, it.node.data)
			if it.node.left != nil {
				next = append(next, item{it.node.left, it.path + "->left"})
			}
			if it.node.right != nil {
				next = append(next, item{it.node.right, it.path + "->right"})
			}
		}
		level = next
	}
	return sb.String()
}

func (t *Tree) levelOrder() []int {
	out := make([]int, 0, t.size)
	level := []*treeNode{}
	if t.root != nil {
		level = append(level, t.root)
	}
	for len(level) > 0 {
		var next []*treeNode
		for _, n := range level {
			out = append(out, n.data)
			if n.left != nil {
				next = append(next, n.left)
			}
			if n.right != nil {
				next = append(next, n.right)
			}
		}
		level = next
	}
	return out
}

func cloneNode(n *treeNode) *treeNode {
	if n == nil {
		return nil
	}
	return &treeNode{data: n.data, height: n.height, left: cloneNode(n.left), right: cloneNode(n.right)}
}

func (t *Tree) Clone() *Tree { return &Tree{root: cloneNode(t.root), size: t.size} }

func (t *Tree) replace(values []int) {
	t.Clear()
	for _, v := range values {
		t.Insert(v)
	}
}

func (t *Tree) SerializeBinary(w io.Writer) error {
	if err := writeTag(w, "TREE"); err != nil {
		return err
	}
	if err := writeSize(w, t.size); err != nil {
		return err
	}
	for _, v := range t.levelOrder() {
		if err := writeInt(w, v); err != nil {
			return err
		}
	}
	return nil
}

func (t *Tree) DeserializeBinary(r io.Reader) error {
	if err := readTag(r, "TREE"); err != nil {
		return err
	}
	n, err := readSize(r)
	if err != nil {
		return err
	}
	values := make([]int, 0, n)
	for i := 0; i < n; i++ {
		v, err := readInt(r)
		if err != nil {
			return err
		}
		values = append(values, v)
	}
	t.replace(values)
	return nil
}

func (t *Tree) SerializeText(w io.Writer) error {
	if err := writeTextHeader(w, "Tree", t.size); err != nil {
		return err
	}
	for _, v := range t.levelOrder() {
		if err := writeTextInt(w, v); err != nil {
			return err
		}
	}
	return nil
}

func (t *Tree) DeserializeText(r *bufio.Reader) error {
	n, err := readTextHeader(r, "Tree")
	if err != nil {
		return err
	}
	values := make([]int, 0, n)
	for i := 0; i < n; i++ {
		v, err := readTextInt(r)
		if err != nil {
			return err
		}
		values = append(values, v)
	}
	t.replace(values)
	return nil
}
