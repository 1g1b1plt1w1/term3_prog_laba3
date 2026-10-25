package ds

import (
	"strings"
	"testing"

	"github.com/stretchr/testify/assert"
)

func newFilledTree() *Tree {
	t := &Tree{}
	for _, v := range []int{50, 30, 70, 20, 40, 60, 80} {
		t.Insert(v)
	}
	return t
}

func TestTreeInsertMember(t *testing.T) {
	tr := newFilledTree()
	assert.Equal(t, 7, tr.Len())
	assert.True(t, tr.IsMember(40))
	assert.False(t, tr.IsMember(45))
	assert.False(t, tr.Insert(50))
	assert.Equal(t, 3, tr.Height())
	assert.True(t, tr.IsAVL())
	assert.Equal(t, " 20 30 40 50 60 70 80", tr.String())
	assert.Equal(t, []int{20, 30, 40, 50, 60, 70, 80}, tr.Items())
}

func TestTreeErase(t *testing.T) {
	tr := newFilledTree()
	assert.True(t, tr.Erase(20))
	assert.True(t, tr.Erase(30))
	assert.True(t, tr.Erase(50))
	assert.False(t, tr.Erase(999))
	assert.Equal(t, " 40 60 70 80", tr.String())
	assert.True(t, tr.IsAVL())
	for _, v := range []int{40, 60, 70, 80} {
		assert.True(t, tr.Erase(v))
	}
	assert.True(t, tr.Empty())
	assert.Equal(t, 0, tr.Height())
}

func TestTreeRotations(t *testing.T) {
	cases := [][]int{{1, 2, 3}, {3, 2, 1}, {1, 3, 2}, {3, 1, 2}}
	for _, c := range cases {
		tr := &Tree{}
		for _, v := range c {
			tr.Insert(v)
		}
		assert.True(t, tr.IsAVL())
		assert.Equal(t, 2, tr.Height())
		assert.Contains(t, tr.Structure(), "root = 2")
	}
	big := &Tree{}
	for i := 1; i <= 200; i++ {
		big.Insert(i)
	}
	assert.True(t, big.IsAVL())
	for i := 1; i <= 200; i += 2 {
		big.Erase(i)
	}
	assert.True(t, big.IsAVL())
	assert.Equal(t, 100, big.Len())
}

func TestTreeStructureCloneClear(t *testing.T) {
	tr := newFilledTree()
	assert.True(t, strings.Contains(tr.Structure(), "root = 50"))
	c := tr.Clone()
	c.Insert(10)
	assert.Equal(t, 7, tr.Len())
	tr.Clear()
	assert.True(t, tr.Empty())
	assert.Equal(t, "", tr.Structure())
}
