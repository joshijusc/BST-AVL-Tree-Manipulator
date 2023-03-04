#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int processLeaves(Node* root, bool& l);

bool equalPaths(Node * root)
{
    // Add your code below
    bool lengthsEqual = true;
    processLeaves(root, lengthsEqual);
    return lengthsEqual;
}

int processLeaves(Node* root, bool& l){
    if (root == nullptr){
        return 0;
    }
    int rootL = processLeaves(root->left, l);
    int rootR = processLeaves(root->right, l);
    if (root->left != nullptr && root->right != nullptr){
        if (rootL != rootR){
            l = false;
            return -1;
        }
        return ++rootR;
    }
    else if (root->left != nullptr && root->right == nullptr){
        return rootL + 1;
    } else if (root->left == nullptr && root->right != nullptr){
        return rootR + 1;
    }
}