#include "InternalNode.hpp"
#include "LeafNode.hpp"
#include "RecordPtr.hpp"

//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if(!is_null(tree_ptr))
        this->load();
}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if(this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    TreePtr new_tree_ptr = NULL_PTR;
    TreePtr next_node_ptr = NULL_PTR;
    int ind = -1;
    int last = 0;
    for(int i=0;i<this->keys.size();i++){
        if(key <= this->keys[i]){
            next_node_ptr = this->tree_pointers[i];
            ind = i;
            break;
        }
    }
    if(next_node_ptr == NULL_PTR){
        next_node_ptr = this->tree_pointers[this->tree_pointers.size()-1];
        ind = this->keys.size()-1;
        last = 1;
    }
    TreeNode* next_node = TreeNode::tree_node_factory(next_node_ptr);
    TreePtr potential_split_node_ptr = next_node->insert_key(key, record_ptr);
    if(!is_null(potential_split_node_ptr)){
        int key = next_node->max();
        this->keys.insert(this->keys.begin()+ind+last, key);
        this->tree_pointers.insert(this->tree_pointers.begin()+ind+1+last, potential_split_node_ptr);
        this->size++;
        if(this->overflows()){
            auto new_internal_node = new InternalNode(NULL_PTR);
            for(int i=0;i<this->keys.size();i++){
                if(i < MIN_OCCUPANCY){
                    continue;
                }
                new_internal_node->keys.push_back(this->keys[i]);
            }
            int cnt = 0;
            for(int i=0;i<this->tree_pointers.size();i++){
                if(i < MIN_OCCUPANCY){
                    continue;
                }
                cnt++;
                new_internal_node->tree_pointers.push_back(this->tree_pointers[i]);
            }
            new_internal_node->size = cnt;
            this->size = MIN_OCCUPANCY;
            int new_key = this->keys[MIN_OCCUPANCY-1];
            this->keys.erase(this->keys.begin()+MIN_OCCUPANCY-1, this->keys.end());
            this->tree_pointers.erase(this->tree_pointers.begin()+MIN_OCCUPANCY, this->tree_pointers.end());
            new_tree_ptr = new_internal_node->tree_ptr;
            new_internal_node->dump();
        }
    }
    this->dump();
    return new_tree_ptr;
}

//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key) {
    TreePtr next_node_ptr = NULL_PTR;
    int ind = -1;
    for(int i = 0; i < this->keys.size(); i++) {
        if(key <= this->keys[i]) {
            next_node_ptr = this->tree_pointers[i];
            ind = i;
            break;
        }
    }
    if(next_node_ptr == NULL_PTR) {
        next_node_ptr = this->tree_pointers[this->tree_pointers.size() - 1];
        ind = this->keys.size();
    }
    TreeNode* next_node = TreeNode::tree_node_factory(next_node_ptr);
    next_node->delete_key(key);
    if(next_node->size < MIN_OCCUPANCY) {
        if(ind != 0) {
            // merge or redistribute with left child
            TreePtr left_node_ptr = this->tree_pointers[ind - 1];
            TreeNode* left_node = TreeNode::tree_node_factory(left_node_ptr);
            if(left_node->size == MIN_OCCUPANCY) {
                // redistribution cannot occur. Merge Instead
                TreePtr mergedNodePtr = InternalNode::merge(left_node, next_node);
                this->size--;
                this->keys.erase(this->keys.begin() + ind - 1);
                this->tree_pointers.erase(this->tree_pointers.begin() + ind);
                this->tree_pointers[ind - 1] = mergedNodePtr;
                this->keys[ind - 1] = TreeNode::tree_node_factory(mergedNodePtr)->max();
            } else {
                // redistribution can occur
                InternalNode::redistribute(left_node, next_node);
                this->keys[ind - 1] = TreeNode::tree_node_factory(left_node_ptr)->max();
            }
            delete left_node;
        } else if(ind != this->keys.size()) {
            // merge or redistribute with right child
            TreePtr right_node_ptr = this->tree_pointers[ind + 1];
            TreeNode* right_node = TreeNode::tree_node_factory(right_node_ptr);
            if(right_node->size == MIN_OCCUPANCY) {
                // redistribution cannot occur. Merge Instead
                TreePtr mergedNodePtr = InternalNode::merge(next_node, right_node);
                this->size--;
                this->keys.erase(this->keys.begin() + ind);
                this->tree_pointers.erase(this->tree_pointers.begin() + ind + 1);
                this->tree_pointers[ind] = mergedNodePtr;
                this->keys[ind] = TreeNode::tree_node_factory(mergedNodePtr)->max();
            } else {
                // redistribution can occur
                InternalNode::redistribute(next_node, right_node);
                this->keys[ind] = TreeNode::tree_node_factory(next_node_ptr)->max();
            }
            delete right_node;
        } else {
            cout << "ERROR: single child node. No one to redistribute or merge with !!";
        }
        delete next_node;
    }
    this->dump();
}

//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for(int i = 0; i < this->size - 1; i++){
        if(min_key <= this->keys[i]){
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

void InternalNode::redistribute(TreeNode* left, TreeNode* right) {
    // we always need to transfer only one element.
    if(left->size > MIN_OCCUPANCY) {
        if(left->node_type == LEAF) {
            // both left and right must be leaf
            LeafNode* left_leaf = new LeafNode(left->tree_ptr);
            LeafNode* right_leaf = new LeafNode(right->tree_ptr);
            left_leaf->size--;
            right_leaf->size++;
            auto last_pair = *(left_leaf->data_pointers.rbegin());
            left_leaf->data_pointers.erase(last_pair.first);
            right_leaf->data_pointers[last_pair.first] = (last_pair.second);
            left_leaf->dump();
            right_leaf->dump();
            delete left_leaf;
            delete right_leaf;
        } else {
            // both left and right must be internal
            InternalNode* left_internal = new InternalNode(left->tree_ptr);
            InternalNode* right_internal = new InternalNode(right->tree_ptr);
            int maxi = left_internal->max();
            left_internal->keys.pop_back();
            TreePtr lastPtr = left_internal->tree_pointers.back();
            left_internal->tree_pointers.pop_back();
            right_internal->keys.insert(right_internal->keys.begin(), maxi);
            right_internal->tree_pointers.insert(right_internal->tree_pointers.begin(), lastPtr);
            left_internal->dump();
            right_internal->dump();
            delete left_internal;
            delete right_internal;
        }
    } else {
        if(left->node_type == LEAF) {
            // both left and right must be leaf
            LeafNode* left_leaf = new LeafNode(left->tree_ptr);
            LeafNode* right_leaf = new LeafNode(right->tree_ptr);
            left_leaf->size++;
            right_leaf->size--;
            auto first_pair = *(right_leaf->data_pointers.begin());
            right_leaf->data_pointers.erase(first_pair.first);
            left_leaf->data_pointers[first_pair.first] = (first_pair.second);
            left_leaf->dump();
            right_leaf->dump();
            delete left_leaf;
            delete right_leaf;
        } else {
            // both left and right must be internal
            InternalNode* left_internal = new InternalNode(left->tree_ptr);
            InternalNode* right_internal = new InternalNode(right->tree_ptr);
            int maxi = left_internal->max();
            right_internal->keys.erase(right_internal->keys.begin());
            TreePtr firstPtr = right_internal->tree_pointers.front();
            right_internal->tree_pointers.erase(right_internal->tree_pointers.begin());
            left_internal->keys.push_back(maxi);
            right_internal->tree_pointers.push_back(firstPtr);
            left_internal->dump();
            right_internal->dump();
            delete left_internal;
            delete right_internal;
        }
    }
}

TreePtr InternalNode::merge(TreeNode* left, TreeNode* right) {
    if(left->node_type == LEAF) {
        // both left and right must be leaf
        LeafNode* left_leaf = new LeafNode(left->tree_ptr);
        LeafNode* right_leaf = new LeafNode(right->tree_ptr);
        for(auto rec: right_leaf->data_pointers){
            left_leaf->data_pointers[rec.first] = rec.second;
            left_leaf->size++;
        }
        left_leaf->next_leaf_ptr = right_leaf->next_leaf_ptr;
        left_leaf->dump();
        delete left_leaf;
        delete right_leaf;
    } else {
        // both left and right must be internal
        InternalNode* left_internal = new InternalNode(left->tree_ptr);
        InternalNode* right_internal = new InternalNode(right->tree_ptr);
        left_internal->keys.push_back(left_internal->max());
        for(Key key: right_internal->keys) {
            left_internal->keys.push_back(key);
        }
        for(TreePtr treePtr: right_internal->tree_pointers) {
            left_internal->size++;
            left_internal->tree_pointers.push_back(treePtr);
        }
        left_internal->dump();
        delete left_internal;
        delete right_internal;
    }
    return left->tree_ptr;
}
//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for(int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for(int i = 0; i < this->size; i++){
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for(int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if(i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i-1]) + " < x";
        } else {
            link += to_string(this->keys[i-1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    for(int i = 0; i < this->size - 1; i++){
        if(&os == &cout)
            os << "\nP" << i+1 << ": ";
        os << this->tree_pointers[i] << " ";
        if(&os == &cout)
            os << "\nK" << i+1 << ": ";
        os << this->keys[i] << " ";
    }
    if(&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream& InternalNode::read(istream& is){
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for(int i = 0; i < this->size - 1; i++){
        if(&is == &cin)
            cout << "P" << i+1 << ": ";
        is >> this->tree_pointers[i];
        if(&is == &cin)
            cout << "K" << i+1 << ": ";
        is >> this->keys[i];
    }
    if(&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
