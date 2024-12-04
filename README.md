This project aims to implement a multiway search tree (M-Tree), a data structure designed to
store and efficiently manage sorted data while maintaining balanced tree properties. The M-Tree
provides efficient insertion, deletion, and search operations by splitting a sorted array recursively and
using the split values for tree navigation. The M-Tree uses recursive splitting of arrays and places the
split values in internal (non-leaf) nodes, while the leaf nodes store actual values in sorted order.
Additionally, the implemention of a rebuild operation to maintain balance in the tree after deletions.

Search trees, particularly the M-Tree, are not just theoretical constructs. They enable efficient data
retrieval and modification in many real-world applications, such as database indexing, file systems,
and memory management. The M-Tree enhances traditional binary search trees by allowing each
node to hold multiple values and up to M child pointers, resulting in more compact trees with fewer
levels.
