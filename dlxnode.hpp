namespace DLX {

class Node;
class Column;

// One good way to implement Algorithm X is to represent each 1 in the
// matrix A as a data object x with five fields L[x], R[x], U[x], D[x], C[x]. Rows of the matrix
// are doubly linked as circular lists via the L and R fields (“left” and “right”); columns are
// doubly linked as circular lists via the U and D fields (“up” and “down”). Each column
// list also includes a special data object called its list header.
class Node {
   public:
    // pointer up into the column that the node is associated with
    Column* c;
    int val;
    // adjacent nodes
    Node *l, *r, *u, *d;

    Node(int val) {
        this->val = val;
        this->l = this;
        this->r = this;
        this->u = this;
        this->d = this;
    }
};

class Column {
   public:
    // pointers to the top and bottom nodes in the column
    Node *top_node, *bottom_node;
    // left and right columns
    Column *l, *r;
    // num nodes in the column
    int node_count;

    int id;
};

}  // namespace DLX