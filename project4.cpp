#include <vector>
#include <iostream>
#include <exception>
using namespace std;

/* ***************************** Exceptions ***************************** */
/***** Write your exception class here ******* SEE main function *****/
// Custom exception class for handling duplicate insertions
class DuplicateInsertion : public std::exception {
private:
    const char* message; // Message to be displayed

public:
    // Constructor with default error message
    DuplicateInsertion(const char* msg = "Error: Duplicate Insertion Detected") : message(msg) {}

    // Overriding the what() method to return the message
    virtual const char* what() const noexcept override {
        return message;
    }
};

// Custom exception class for handling cases where a value is not found
class NotFoundException : public std::exception {
private:
    const char* message; // Message to be displayed

public:
    // Constructor with default error message
    NotFoundException(const char* msg = "Error: Value not found") : message(msg) {}

    // Overriding the what() method to return the message
    virtual const char* what() const noexcept override {
        return message;
    }
};

/* ***************************** MTree Class ***************************** */
// Template class representing an M-ary search tree
template <class DT>
class MTree {
protected:
    int M; // Maximum number of children per node (M-way split)
    vector<DT> values; // Values stored in the node (M-1 values max)
    vector<MTree*> children; // Child nodes (M pointers to children)

public:
    // Constructor
    MTree(int maxChildren);

    // Destructor to clean up dynamically allocated memory
    ~MTree();

    // Check if the current node is a leaf
    bool is_leaf() const;

    // Insert a value into the tree
    void insertValue(const DT& value);

    // Split the node if it exceeds capacity
    void split_node();

    // Find the child node to traverse based on the value
    MTree* find_child(const DT& value);

    // Search for a value in the tree
    bool search(const DT& value);

    // Remove a value from the tree
    void remove(const DT& value);

    // Build the tree from a vector of input values
    void buildTree(const vector<DT>& input_values);

    // Collect all values from the tree in sorted order
    vector<DT> collect_values() const;

    // Find a value in the tree (same as search)
    bool find(DT& value);

    // Find the minimum value in the tree
    DT findMin() const;

    // Find the maximum value in the tree
    DT findMax() const;

    // Perform an in-order traversal and store values in a vector
    void inOrderTraversal(vector<DT>& result) const;

    // Binary search for a value in the current node's values
    int binarySearch(const DT& value) const;

    // Helper function for binary search to find insertion index
    int binarySearchHelper(const DT& value) const;
};

// Constructor
template <class DT>
MTree<DT>::MTree(int maxChildren): M(maxChildren) {}

// Check if the current node is a leaf
template <class DT>
bool MTree<DT>::is_leaf() const{
    return children.empty();
}

// Insert a value into the tree
template <class DT>
void MTree<DT>::insertValue(const DT& value){
    // Check for duplicate values
    if (search(value)){
        throw DuplicateInsertion();
    }

    // Find the correct position for insertion
    int index = binarySearchHelper(value);

    if (is_leaf()) {
        // Insert directly into the leaf if space is available
        values.insert(values.begin() + index, value);
    } else {
        // Recursive call to the appropriate child for insertion
        if (children[index] != nullptr) {
            children[index]->insertValue(value);
        }
    }

    // Split the node if it exceeds capacity
    if (values.size() >= M) {
        split_node();
    }

}

template <class DT>
void MTree<DT>::split_node(){
    // Find the middle value to promote
    int midIndex = values.size() / 2;
    DT elementToPromote = values[midIndex];

    // Create new child nodes for left and right partitions
    MTree<DT>* leftChild = new MTree(M);
    MTree<DT>* rightChild = new MTree(M);

    // Split values between the left and right children
    leftChild->values.insert(leftChild->values.end(), values.begin(), values.begin() + midIndex);
    rightChild->values.insert(rightChild->values.end(), values.begin() + midIndex + 1, values.end());

    // If the node has children, redistribute them
    if (!is_leaf()) {
        leftChild->children.insert(leftChild->children.end(), children.begin(), children.begin() + midIndex + 1);
        rightChild->children.insert(rightChild->children.end(), children.begin() + midIndex + 1, children.end());
    }

    // Clear current node and set promoted value
    values.clear();
    children.clear();
    values.push_back(elementToPromote);
    children.push_back(leftChild);
    children.push_back(rightChild);
}

template <class DT>
MTree<DT>* MTree<DT>::find_child(const DT& value){

    if (!search(value)) {
        throw NotFoundException();
    }
    int index = binarySearchHelper(value);
    return children[index];
}

template <class DT>
bool MTree<DT>::search(const DT& value){
    // Check if the value is in the current node's values
    int foundIndex = binarySearch(value);
    if (foundIndex >= 0) {
        return true;
    }
    // Check if it's a leaf node; if so, the value isn't present
    else if (is_leaf()) {
        return false;
    }
    else{
        // Determine the appropriate child index
        int index = binarySearchHelper(value);
        
        // Check that index is within bounds before accessing children
        if (index < 0 || index >= children.size()) {
            return false;
        }
        
        // Recursively search in the appropriate child
        return children[index]->search(value);
    }
    
}
template <class DT>
bool MTree<DT>::find(DT& value){
    return search(value);
}

template <class DT>
void MTree<DT>::remove(const DT& value){

    if (!search(value)) {
        throw NotFoundException();
    }

    int index = binarySearch(value);    

    if (is_leaf()) {
        if (index >= 0 && values[index] == value) {
            values.erase(values.begin() + index);
            return;
        }
        else {
            cerr << "Error: Value " << value << " not found in leaf node values." << endl;
        }
    }
    else {
        // Internal node handling
        if (index >= 0) {
            if (index < values.size() - 1){
                DT minValue = children[index + 1]->findMin();
                values[index] = minValue;
                children[index + 1]->remove(minValue);
            }
            else{
                DT maxValue = children[index]->findMax();
                values[index] = maxValue;
                children[index]->remove(maxValue);
            }
            
            // Rebalance the tree
            vector<DT> tree = collect_values();
            buildTree(tree);
            return;
        }
        else {
            int childIndex = binarySearchHelper(value);
            children[childIndex]->remove(value);
        }
    }

}

template <class DT>
DT MTree<DT>::findMin() const {
    // Start at the current node
    const MTree<DT>* currentNode = this;
    
    // Traverse down to the leftmost leaf
    while (!currentNode->is_leaf()) {
        currentNode = currentNode->children.front();
    }
    
    // // Return the first value in the leftmost leaf node
    if (currentNode && !currentNode->values.empty()) {
        return currentNode->values.front();
    } 
    else {
        throw std::runtime_error("Tree structure error in findMin.");
    }
}
template <class DT>
DT MTree<DT>::findMax() const {
    // Start at the current node
    const MTree<DT>* currentNode = this;
    
    // Traverse down to the leftmost leaf
    while (!currentNode->is_leaf()) {
        currentNode = currentNode->children.back();
    }
    
    // // Return the first value in the leftmost leaf node
    if (currentNode && !currentNode->values.empty()) {
        return currentNode->values.back();
    } 
    else {
        throw std::runtime_error("Tree structure error in findMax.");
    }
}

template <class DT>
void MTree<DT>::buildTree(const vector<DT>& inputValues){
    values.clear();
    for (int i = 0; i < children.size(); i++){
        delete children[i];
        children[i] = nullptr;
    }
    children.clear();

    for (int i = 0; i < inputValues.size(); i++) {
        insertValue(inputValues[i]);  // Inserts each value into the tree
    }
}

template <class DT>
vector<DT> MTree<DT>::collect_values() const{
    // Passes resulting vector to inorder traversal to gather values in sorted order
    vector<DT> result;

    inOrderTraversal(result);
    
    return result;
}

template <class DT>
void MTree<DT>::inOrderTraversal(vector<DT>& result) const{

    for (int i = 0; i < values.size(); i++){
        // If node is not a leaf node, then recursively call inordertraversal on the left most child
        if (!is_leaf()){
            children[i]->inOrderTraversal(result);
        }

        // Add value to resulting vector
        result.push_back(values[i]);
    }
    
    // does inorder traversal for the rightmost children
    if (!is_leaf() && children.size() > values.size()){
        children.back()->inOrderTraversal(result);
    }
}

template <class DT>
int MTree<DT>::binarySearch(const DT& value) const {
    int left = 0;
    int right = values.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (values[mid] == value) {
            return mid;  // Value found at mid index
        } 
        else if (values[mid] < value) {
            left = mid + 1;  // Search in the right half
        } 
        else {
            right = mid - 1;  // Search in the left half
        }
    }
    
    // Value not found; return the position where it can be inserted to keep sorted order
    return -1;
}
template <class DT>
int MTree<DT>::binarySearchHelper(const DT& value) const {
    // cout << "Binary search for value: " << value << endl;

    int left = 0, right = values.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (values[mid] == value) {
            return mid; // Value found
        }
        if (values[mid] < value) {
            left = mid + 1; // Search in the right half
        } 
        else {
            right = mid - 1; // Search in the left half
        }
    }

    return left; // Insertion index
}

// Destructor to delete child nodes
template <class DT>
MTree<DT>::~MTree() {
    for (auto child : children) {
        delete child;
        child = nullptr;
    }
}

/* ***************************** Main Function ***************************** */
int main() {
    //cout << "Start of Main" << endl;
    int n; // number of numbers in the initial sorted array
    int MValue;
    int numCommands;
    char command;
    int value;

    //read n numbers from the input and add them to the vector mySortedValues
    cin >> n;
    vector<int> mySortedValues;

    for (int i = 0; i < n; i++){
        cin >> value;
        mySortedValues.push_back(value);
    }
    
    //Get the M value
    cin >> MValue;
    MTree<int>* myTree = new MTree<int>(MValue);

    //Build the tree
    (*myTree).buildTree(mySortedValues);
    
    cin >> numCommands; // Read the number of commands

    /************** Read each command Process ***************/
    for (int i = 0; i < numCommands; i++) {
        cin >> command; // Read the command type
        
        switch (command) {
            case 'I': { // Insert
            cin >> value;
            try {
                (*myTree).insertValue(value);
                cout << "The value = " << value << " has been inserted." << endl;
            }
            catch (DuplicateInsertion& e) {
                cout << "The value = " << value << " already in the tree." <<  endl;
            }
            break;
            }
            case 'R': { // Remove
                cin >> value;
                try {
                    (*myTree).remove(value);
                    cout << "The value = " << value << " has been removed." << endl;
                }
                catch (NotFoundException& e) {
                    cout << "The value = " << value << " not found." << endl;
                }
                break;
            }
            case 'F': { // Find
                cin >> value;
                if ((*myTree).find(value)){
                    cout << "The element with value = " << value << " was found." << endl;
                }
                else{
                    cout << "The element with value = " << value << " not found.";
                    cout << endl;
                }
                break;
            }
            case 'B': { // Rebuild tree
                vector<int> myValues = (*myTree).collect_values();
                (*myTree).buildTree (myValues);
                cout << "The tree has been rebuilt." << endl;
                break;
            }

            default: {
                cout << "Invalid command!" << endl;
            }
        }
    }

    mySortedValues.clear();

    mySortedValues = myTree->collect_values();

    cout << "Final list: ";
    for (int i = 0; i < mySortedValues.size(); i++) {
        cout << mySortedValues[i] << " ";
        
        // Print a newline after every 20 values
        if ((i + 1) % 20 == 0) {
            cout << endl;
        }
    }

    delete myTree;
    return 0;

}

/*

One prompt that I have asked ChatGPT a bunch this project is I'm encountering a problem with this function. What is 
happening here and what can I do to debug it? Each time I encountered an issue, whether it was unexpected behavior in 
a recursive method, a null pointer error, or structural issues during insertion or deletion, this prompt allowed me to 
break down the problem systematically. Chat pointed out that my recursive calls weren’t handling the base case properly, 
and it suggested adding a check to make sure the function returned nullptr if no child was found. This sounded reasonable, 
so I implemented it, and it did fix the infinite recursion issue I was running into. But when I started testing more, I 
noticed it still wasn’t working in some edge cases, like when the tree only had a single node. At that point, I had to go 
back in and rework parts of the logic myself. While the initial suggestion wasn’t a perfect fix, it helped me understand the 
root of the problem better. By breaking down the issue and giving me a direction to start in, Chat made it a lot easier for 
me to debug and ultimately improve the function on my own.

Another example is when I was struggling with the remove method. Chat had suggested using many method to rebalnce the tree 
with redistribute and mergeSibling functions. Initially, this sounded like a great idea, but after some time, the output of 
the code took longer and longer to output, at minimum 10 minutes for input 3 and a minute or two for the first input. This 
initially confused me. I did not know where to go from here and asked Chat for help. Chat suggested somethings that did not 
really make sense at the time and after I had investigated further, it was actually harming me. I realized that the reason 
it was becoming so slow was due to maultiple stacks being called, unnecessary amounts of rebalancing. I asked Chat if it was 
possible to change the balancing factors to not be recursive and that helped a little bit. Only after I realized that I had 
been unnecessarly rebalancing nodes that didn't need it, did the code start to output at a reasonable speed.


During the development process, I encountered several issues. One notable challenge was related to the remove method, which 
was not behaving as expected when trying to balance the tree. This issue became more pronounced when unnecessary rebalancing 
operations were introduced, which slowed down the performance significantly. To resolve this, I initially tried ChatGPT’s suggestion 
of adding more complex balancing methods, but this made things worse. After further investigation, I realized that the issue was due 
to calling too many unnecessary rebalancing functions. I then modified the balancing logic to ensure that it was only triggered when 
absolutely necessary, which improved the performance significantly.

Another issue I ran into was with memory allocation. Specifically, I encountered a situation where the destructor was not being called 
correctly, leading to memory leaks. This issue was resolved by revisiting the destructor method and ensuring that all dynamically 
allocated memory was properly freed when nodes were removed or the tree was destroyed.


For verification, I used the input and output files to check accuracy. Thorughout the development of the project, I would also use 
numerous debugging lines to find incorrect splitting, insertion, deleteion, and other functions of the tree. I also learned that you 
can compare text files in VS Code recently and I used that to compare my output verses the output files given in canvas. 

*/