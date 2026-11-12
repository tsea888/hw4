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
    void rebalance(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    void removeRebalance(AVLNode<Key,Value>* node, int balance);
    void rotateLeft(AVLNode<Key,Value>* node);
    void rotateRight(AVLNode<Key,Value>* node);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    const Key& k = new_item.first;
    const Value& v = new_item.second;

    AVLNode<Key, Value> *current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value> *parent = nullptr;

    while(current != nullptr){
      //check if key akready in tree
      if(current->getKey() == k){
        current->setValue(v);
        return;
      }

      //otherwise continue traversing
      parent = current;
      if(k < current->getKey()){
        current = current->getLeft();
      }
      else{
        current = current->getRight();
      }
    }

    AVLNode<Key, Value>* toInsert = new AVLNode<Key, Value>(k, v, parent);
    if(parent == nullptr){
      this->root_ = toInsert;
      return;
    }

    if(k<parent->getKey()){
      parent->setLeft(toInsert);
    }
    else{
      parent->setRight(toInsert);
    }

    if(parent->getBalance() != 0){
      parent->setBalance(0);
    }
    else{
      if(parent->getLeft() == toInsert){
        parent->setBalance(-1);
      }
      else{
        parent->setBalance(1);
      }

      rebalance(parent, toInsert);
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
    AVLNode<Key, Value>* node =  static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(node == nullptr){
      return;
    }

    if(node->getLeft() != nullptr && node->getRight() != nullptr){
      AVLNode<Key, Value>* pred =  static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
      nodeSwap(node, pred);
    }

    AVLNode<Key, Value>* parent = node->getParent();
    int balance = 0;
    if(parent != nullptr){
      if(parent->getLeft() == node){
        balance = 1;
      }
      else{
        balance = -1;
      }
    }
    
    AVLNode<Key, Value>* child;
    if(node->getLeft() != nullptr){
      child = node->getLeft();
    }
    else{
      child = node->getRight();
    }

    if(child!=nullptr){
      child->setParent(parent);
    }

    if(parent == nullptr){
      this->root_ = child;
    }
    else if(parent->getLeft() == node){
      parent->setLeft(child);
    }
    else{
      parent->setRight(child);
    }

    delete node;

    removeRebalance(parent, balance);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node){
  if(parent == nullptr){
    return;
  }
  AVLNode<Key, Value>* grand = parent->getParent();
  if(grand == nullptr){
    return;
  }
  if(grand->getLeft() == parent){
    //parent on left side, since extra child, then the grandparent also needs to be adjusted (-1 since LHS)
    grand->updateBalance(-1);

    if(grand->getBalance() == -2){
      if(parent->getLeft() == node){
        rotateRight(grand);
        parent->setBalance(0);
        grand->setBalance(0);
      }
      else{
        rotateLeft(parent);
        rotateRight(grand);

        if(node->getBalance() == -1){
          parent->setBalance(0);
          grand->setBalance(1);
        }
        else if(node->getBalance() == 0){
          parent->setBalance(0);
          grand->setBalance(0);
        }
        else{
          parent->setBalance(-1);
          grand->setBalance(0);
        }
        node->setBalance(0);
      }
    }
    else if(grand->getBalance() == -1){
      rebalance(grand, parent);
    }
    else{
      return;
    }
  }
  else{
    grand->updateBalance(1);
    if(grand->getBalance() == 0){
      return;
    } 
    else if(grand->getBalance() == 1){
      rebalance(grand, parent);
    }
    else{
      if(parent->getRight() == node){
        rotateLeft(grand);
        parent->setBalance(0);
        grand->setBalance(0);
      }
      else{
        rotateRight(parent);
        rotateLeft(grand);

        if(node->getBalance() == 1){
          parent->setBalance(0);
          grand->setBalance(-1);
        }
        else if(node->getBalance() == 0){
          parent->setBalance(0);
          grand->setBalance(0);
        }
        else{
          parent->setBalance(1);
          grand->setBalance(0);
        }
        node->setBalance(0);
      }
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeRebalance(AVLNode<Key,Value>* node, int balance){
  if(node == nullptr){
    return;
  }

  AVLNode<Key, Value>* parent = node->getParent();
  int difference = 0;
  if(parent != nullptr){
    if(parent->getLeft() == node){
      difference = 1;
    }
    else{
      difference = -1;
    }
  }

  int nDiff = node->getBalance() + balance;
  if(balance == -1){
    if(nDiff == -2){
      AVLNode<Key, Value>* child = node->getLeft();
      if(child->getBalance() == -1){
        rotateRight(node);
        node->setBalance(0);
        child->setBalance(0);
        removeRebalance(parent, difference);
      }
      else if (child->getBalance() == 0){
        rotateRight(node);
        node->setBalance(-1);
        child->setBalance(1);
      }
      else{
        AVLNode<Key, Value>* grand = child->getRight();
        int grandBalance = grand->getBalance();
        rotateLeft(child);
        rotateRight(node);

        if(grandBalance == 1){
          node->setBalance(0);
          child->setBalance(-1);
        }
        else if(grandBalance == 0){
          node->setBalance(0);
          child->setBalance(0);
        }
        else{
          node->setBalance(1);
          child->setBalance(0);
        }
        
        grand->setBalance(0);
        removeRebalance(parent, difference);
      }
    }
    else if(nDiff == -1){
      node->setBalance(-1);
    }
    else{
      node->setBalance(0);
      removeRebalance(parent, difference);
    }
  }

  else{
    if(nDiff == 2){
      AVLNode<Key, Value>* child = node->getRight();

      if(child->getBalance() == 1){
        rotateLeft(node);
        node->setBalance(0);
        child->setBalance(0);
        removeRebalance(parent, difference);
      } 
      else if(child->getBalance() == 0){
        rotateLeft(node);
        node->setBalance(1);
        child->setBalance(-1);
      }
      else{
        AVLNode<Key, Value>* grand = child->getLeft();
        int grandBalance = grand->getBalance();
        rotateRight(child);
        rotateLeft(node);

        if(grandBalance == -1){
          node->setBalance(0);
          child->setBalance(1);
        }
        else if(grandBalance == 0){
          node->setBalance(0);
          child->setBalance(0);
        }
        else{
          node->setBalance(-1);
          child->setBalance(0);
        }
        grand->setBalance(0);
        removeRebalance(parent, difference);
      }
    }
    else if(nDiff == 1){
      node->setBalance(1);
    }
    else{
      node->setBalance(0);
      removeRebalance(parent, difference);
    }
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft( AVLNode<Key,Value>* node){
  AVLNode<Key, Value>* current = node->getRight();
  AVLNode<Key, Value>* parent = node->getParent();

  current->setParent(parent);
  if(parent == nullptr){
    this->root_ = current;
  }
  else if(parent->getLeft() == node){
    parent->setLeft(current);
  }
  else{
    parent->setRight(current);
  }

  node->setRight(current->getLeft());
  if(current->getLeft() != nullptr){
    current->getLeft()->setParent(node);
  }

  current->setLeft(node);
  node->setParent(current);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight( AVLNode<Key,Value>* node){
  AVLNode<Key, Value>* current = node->getLeft();
  AVLNode<Key, Value>* parent = node->getParent();

  current->setParent(parent);
  if(parent == nullptr){
    this->root_ = current;
  }
  else if(parent->getLeft() == node){
    parent->setLeft(current);
  }
  else{
    parent->setRight(current);
  }

  node->setLeft(current->getRight());
  if(current->getRight() != nullptr){
    current->getRight()->setParent(node);
  }

  current->setRight(node);
  node->setParent(current);
}

#endif
