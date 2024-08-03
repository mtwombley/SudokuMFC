#pragma once

// An implementation of Donald Knuth's Dancing Links algorithm based on the example from Dimitris Boutzounis on github

struct Node
{
  Node* left  = nullptr;
  Node* right = nullptr;
  Node* up    = nullptr;
  Node* down  = nullptr;
  Node* head  = nullptr;

  wchar_t strValue[20];

  int size = -1;  //used for Column header
  unsigned int candidate;
  unsigned int rowCounter;
  unsigned int columnCounter;
  int columnIndex;
  int rowIndex;

  // Find the column with the fewest nodes
  Node* ChooseColumn()
  {
    Node* c = right;
    int s = c->size;
    for (Node* j = c->right; j != head; j = j->right)
    {
      if (j->size < s)
      {
        c = j;
        s = j->size;
      }
    }
    return c;
  }

  // Cover the column
  void Cover()
  {
    left->right = right;
    right->left = left;
    for ( Node* i = down; i != this; i = i->down ) {
      for ( Node* j = i->right; j != i; j = j->right )
      {
        j->down->up = j->up;
        j->up->down = j->down;
        j->head->size--;
      }
    }
  }

  // Uncover the column
  void Uncover()
  {
    for (Node* i = up; i != this; i = i->up)
    {
      for (Node* j = i->left; j != i; j = j->left)
      {
        j->head->size++;
        j->down->up = j;
        j->up->down = j;
      }
    }
    left->right = this;
    right->left = this;
  }
};