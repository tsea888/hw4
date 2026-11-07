#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int findMaxHeight(Node* root);
int findMinHeight(Node* root);

bool equalPaths(Node * root)
{
  //if this is true, assuming height = n, that means every path is at most AND at least height n
  //therefore, every path is equal to height n, so the paths are equal length.
  return findMaxHeight(root) == findMinHeight(root); 
}

int findMaxHeight(Node* root){
  if(root == nullptr){ return 0; }
  int left = findMaxHeight(root->left);
  int right = findMaxHeight(root->right);

  int max;
  if(left > right){
    max = left;
  }
  else{
    max = right;
  }

  return 1+max;
}

int findMinHeight(Node* root){
  if(root == nullptr){ return 0; }
  //need to check if the child even exists (if it doesn't, without this check, it will assume that the side 
  // with no child is the min height)
  if(root->left == nullptr){
    //no left child, look for right
    return 1 + findMinHeight(root->right);
  }
  if(root->right == nullptr){
    //no right child, look for left
    return 1 + findMinHeight(root->left);
  }

  int left = findMinHeight(root->left);
  int right = findMinHeight(root->right);

  int min;
  if(left < right){
    min = left;
  }
  else{
    min = right;
  }

  return 1 + min;
}