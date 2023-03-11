#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void leftRotate(AVLNode<Key, Value>* node);
    void rightRotate(AVLNode<Key, Value>* node);
    int getHeight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if (BinarySearchTree<Key, Value>::empty()){
        BinarySearchTree<Key, Value>::root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    AVLNode<Key, Value>* currNode = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::root_);
    while (currNode != nullptr){
        if (new_item.first == currNode->getKey()){ /* No duplicate keys in a BST. */
            currNode->setValue(new_item.second);
            return;
        } else if (new_item.first < currNode->getKey()){ /* key must go in left subtree */
            if (currNode->getLeft() == nullptr){
                AVLNode<Key, Value>* newLChild = new AVLNode<Key, Value>(new_item.first, new_item.second, currNode);
                currNode->setLeft(newLChild);
                currNode = newLChild;
                break;
            } else {
                currNode = currNode->getLeft();
            }
        } else { /* key must go in right subtree */
            if (currNode->getRight() == nullptr){
                AVLNode<Key, Value>* newRChild = new AVLNode<Key, Value>(new_item.first, new_item.second, currNode);
                currNode->setRight(newRChild);
                currNode = newRChild;
                break;
            } else {
                currNode = currNode->getRight();
            }
        }
    }
    if (currNode->getParent()->getBalance() == -1 || currNode->getParent()->getBalance() == 1) {
        currNode->getParent()->setBalance(0);
        return;
    }
    else if (currNode->getParent()->getBalance() == 0){
        currNode->getParent()->setBalance(getHeight(currNode->getParent()->getRight()) - getHeight(currNode->getParent()->getLeft()));
        AVLTree::insertFix(currNode->getParent(), currNode);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node)
{
    if (parent != nullptr && parent->getParent() != nullptr){
        AVLNode<Key, Value>* grandParent = parent->getParent();
        if (grandParent->getLeft() == parent){         // P is left child of g
            grandParent->updateBalance(-1);
            if (grandParent->getBalance() == 0) return;
            else if (grandParent->getBalance() == -1) insertFix(grandParent, parent);
            else if (grandParent->getBalance() == -2){
                if ((grandParent->getLeft() == parent && parent->getLeft() == node) || (grandParent->getRight() == parent && parent->getRight() == node)){ // Zig-zig
                    AVLTree<Key, Value>::rightRotate(grandParent);
                    parent->setBalance(0);
                    grandParent->setBalance(0);
                } else if ((grandParent->getLeft() == parent && parent->getRight() == node) || (grandParent->getRight() == parent && parent->getLeft() == node)) { // Zig-Zag
                    AVLTree<Key, Value>::leftRotate(parent);
                    AVLTree<Key, Value>::rightRotate(grandParent);
                    if (node->getBalance() == -1) {
                        parent->setBalance(0);
                        grandParent->setBalance(1);
                        node->setBalance(0);
                    } else if (node->getBalance() == 0) {
                        parent->setBalance(0);
                        grandParent->setBalance(0);
                        node->setBalance(0);
                    } else if (node->getBalance() == 1) {
                        parent->setBalance(-1);
                        grandParent->setBalance(0);
                        node->setBalance(0);
                    }
                }
            }
        } else if (grandParent->getRight() == parent){         // P is right child of g
            grandParent->updateBalance(1);
            if (grandParent->getBalance() == 0) return;
            else if (grandParent->getBalance() == 1) insertFix(grandParent, parent);
            else if (grandParent->getBalance() == 2){
                if ((grandParent->getLeft() == parent && parent->getLeft() == node) || (grandParent->getRight() == parent && parent->getRight() == node)){ // Zig-zig
                    AVLTree<Key, Value>::leftRotate(grandParent);
                    parent->setBalance(0);
                    grandParent->setBalance(0);
                } else if ((grandParent->getLeft() == parent && parent->getRight() == node) || (grandParent->getRight() == parent && parent->getLeft() == node)) { // Zig-Zag
                    AVLTree<Key, Value>::rightRotate(parent);
                    AVLTree<Key, Value>::leftRotate(grandParent);
                    if (node->getBalance() == 1) {
                        parent->setBalance(0);
                        grandParent->setBalance(-1);
                        node->setBalance(0);
                    } else if (node->getBalance() == 0) {
                        parent->setBalance(0);
                        grandParent->setBalance(0);
                        node->setBalance(0);
                    } else if (node->getBalance() == -1) {
                        parent->setBalance(1);
                        grandParent->setBalance(0);
                        node->setBalance(0);
                    }
                }
            }
        }
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::internalFind(key));
    int diff = 0;
    if (!BinarySearchTree<Key,Value>::empty() && nodeToRemove != nullptr){
        if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) { // Node has 2 children
            nodeSwap(nodeToRemove, static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(nodeToRemove))); // Swap values
        }
        if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() == nullptr) { // Node has left child only
            if (nodeToRemove->getParent() != nullptr){ // Node is not the root
                (nodeToRemove->getLeft())->setParent(nodeToRemove->getParent()); // Promote left child
                if ((nodeToRemove->getParent())->getLeft() == nodeToRemove){ // Node is a parent's left child
                    (nodeToRemove->getParent())->setLeft(nodeToRemove->getLeft());
                    diff = 1;
                } else if ((nodeToRemove->getParent())->getRight() == nodeToRemove){ // Node is a parent's right child
                    (nodeToRemove->getParent())->setRight(nodeToRemove->getLeft());
                    diff = -1;
                }
            } else { // Node is the root
                BinarySearchTree<Key,Value>::root_ = nodeToRemove->getLeft();
                (nodeToRemove->getLeft())->setParent(nullptr);
            }
        } else if (nodeToRemove->getLeft() == nullptr && nodeToRemove->getRight() != nullptr) { // Node has right child only
            if (nodeToRemove->getParent() != nullptr){ // Node is not the root
                (nodeToRemove->getRight())->setParent(nodeToRemove->getParent()); // Promote right child
                if ((nodeToRemove->getParent())->getLeft() == nodeToRemove){ // Node is a parent's left child
                    (nodeToRemove->getParent())->setLeft(nodeToRemove->getRight());
                    diff = 1;
                } else if ((nodeToRemove->getParent())->getRight() == nodeToRemove){ // Node is a parent's right child
                    (nodeToRemove->getParent())->setRight(nodeToRemove->getRight());
                    diff = -1;
                }
            } else { // Node is the root
                BinarySearchTree<Key,Value>::root_ = nodeToRemove->getRight();
                (nodeToRemove->getRight())->setParent(nullptr);
            }
        } else {
            if (nodeToRemove == BinarySearchTree<Key,Value>::root_) {
                delete nodeToRemove;
                BinarySearchTree<Key,Value>::root_ = nullptr;
                return;
            } else if (nodeToRemove->getParent() != nullptr) {
                if (nodeToRemove->getParent()->getLeft() == nodeToRemove) {
                    diff = 1;
                    nodeToRemove->getParent()->setLeft(nullptr);
                } else if (nodeToRemove->getParent()->getRight() == nodeToRemove) {
                    diff = -1;
                    nodeToRemove->getParent()->setRight(nullptr);
                }
            }
        }
        AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(nodeToRemove->getParent());
        delete nodeToRemove;
        AVLTree::removeFix(parent, diff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    int ndiff = 0;
    if (node != nullptr){
        AVLNode<Key, Value>* parent = node->getParent();
        if (parent != nullptr){
            if (parent->getLeft() == node){ // Node is a left child
                ndiff = 1;
            } else if (parent->getRight() == node){ // Node is a right child
                ndiff = -1;
            }
        }
        if (diff == -1) {
            if (node->getBalance() + diff == -2) {
                AVLNode<Key, Value> *c = node->getLeft();
                if (c->getBalance() == -1) { // Zig-zig
                    rightRotate(node);
                    node->setBalance(0);
                    c->setBalance(0);
                    removeFix(parent, ndiff);
                } else if (c->getBalance() == 0) { // Zig-zig
                    rightRotate(node);
                    node->setBalance(-1);
                    c->setBalance(1);
                    return;
                } else if (c->getBalance() == 1) { // Zig-Zag
                    AVLNode<Key, Value> *g = c->getRight();
                    leftRotate(c);
                    rightRotate(node);
                    if (g->getBalance() == 1) {
                        node->setBalance(0);
                        c->setBalance(-1);
                        g->setBalance(0);
                    } else if (g->getBalance() == 0) {
                        node->setBalance(0);
                        c->setBalance(0);
                        g->setBalance(0);
                    } else if (g->getBalance() == -1) {
                        node->setBalance(1);
                        c->setBalance(0);
                        g->setBalance(0);
                    }
                    removeFix(parent, ndiff);
                }
            } else if (node->getBalance() + diff == -1) {
                node->setBalance(-1);
                return;
            } else if (node->getBalance() + diff == 0) {
                node->setBalance(0);
                removeFix(parent, ndiff);
            }
        } else if (diff == 1){
            if (node->getBalance() + diff == 2) {
                AVLNode<Key, Value> *c = node->getRight();
                if (c->getBalance() == 1) { // Zig-zig
                    leftRotate(node);
                    node->setBalance(0);
                    c->setBalance(0);
                    removeFix(parent, ndiff);
                } else if (c->getBalance() == 0) { // Zig-zig
                    leftRotate(node);
                    node->setBalance(1);
                    c->setBalance(-1);
                    return;
                } else if (c->getBalance() == -1) { // Zig-Zag
                    AVLNode<Key, Value> *g = c->getLeft();
                    rightRotate(c);
                    leftRotate(node);
                    if (g->getBalance() == -1) {
                        node->setBalance(0);
                        c->setBalance(1);
                        g->setBalance(0);
                    } else if (g->getBalance() == 0) {
                        node->setBalance(0);
                        c->setBalance(0);
                        g->setBalance(0);
                    } else if (g->getBalance() == 1) {
                        node->setBalance(-1);
                        c->setBalance(0);
                        g->setBalance(0);
                    }
                    removeFix(parent, ndiff);
                }
            } else if (node->getBalance() + diff == 1) {
                node->setBalance(1);
                return;
            } else if (node->getBalance() + diff == 0) {
                node->setBalance(0);
                removeFix(parent, ndiff);
            }
        }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::leftRotate(AVLNode<Key, Value>* node)
{
    if (node != nullptr){
        AVLNode<Key, Value>* x = node;
        AVLNode<Key, Value>* y = x->getRight();
//        AVLNode<Key, Value>* a = x->getLeft();
        AVLNode<Key, Value>* b = y->getLeft();
//        AVLNode<Key, Value>* z = y->getRight();
        if (x == BinarySearchTree<Key, Value>::root_){
            BinarySearchTree<Key, Value>::root_ = y;
        }
        y->setParent(x->getParent());
        if (x->getParent() != nullptr){
            if ((x->getParent())->getLeft() == x){ // Node is a parent's left child
                (x->getParent())->setLeft(y);
            } else if ((x->getParent())->getRight() == x){ // Node is a parent's right child
                (x->getParent())->setRight(y);
            }
        }
        y->setLeft(x);
        x->setRight(b);
        x->setParent(y);
        if (b != nullptr) b->setParent(x);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rightRotate(AVLNode<Key, Value>* node)
{
    if (node != nullptr){
        AVLNode<Key, Value>* z = node;
//        AVLNode<Key, Value>* d = z->getRight();
        AVLNode<Key, Value>* y = z->getLeft();
        AVLNode<Key, Value>* c = y->getRight();
//        AVLNode<Key, Value>* x = y->getLeft();
        if (z == BinarySearchTree<Key, Value>::root_){
            BinarySearchTree<Key, Value>::root_ = y;
        }
        y->setParent(z->getParent());
        if (z->getParent() != nullptr){
            if ((z->getParent())->getLeft() == z){ // Node is a parent's left child
                (z->getParent())->setLeft(y);
            } else if ((z->getParent())->getRight() == z){ // Node is a parent's right child
                (z->getParent())->setRight(y);
            }
        }
        y->setRight(z);
        z->setLeft(c);
        z->setParent(y);
        if (c != nullptr) c->setParent(z);
    }
}

template<class Key, class Value>
int AVLTree<Key, Value>::getHeight(AVLNode<Key, Value>* node)
{
    if (node == nullptr){
        return 0;
    }
    int rootL = getHeight(node->getLeft());
    int rootR = getHeight(node->getRight());
    return std::max(rootL, rootR) + 1;
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
