#ifndef TREE_H
#define TREE_H

#include "No.h"
#include "Configures.h"
#include <iostream>
#include <vector>
#include <tuple>
#include <random>

class Tree {
  public:
    Tree();
    Tree(Grammar* gg);
    Tree(No* n);
    Tree(Grammar* gg, No* n);
    virtual ~Tree();
    No* root;
    vector<No*> nonTerminals;
    No* targetedNonTerminal;
    vector<No*> targetedNonTerminals;
    vector<tuple<double,double> > linearModel;
    double fitness;

    int height;
    int terminals;

//        double * leastSquare = NULL;
//        int leastSquareSize;

    // DE
    int constantsSize;
    double* constants;

    No* subTree();
    No* targetSubTree(No* n);
    No* getSubTree(int i);
    void replaceAllC(double* constants);
    void replaceAllConst();
    void nonTerminalCount(No* n);
    static bool compareSemantic(No* a, No* b);
    void targetedNonTerminalCount(No* n, No* m);
    void update();
    void update(No* n);
    Tree * clone();
    No* clone(No * n);
    void print();
    void printInfix();
    int Constants(double* initial, int pos, No* n);

    vector<No*> semanticTargetSubTree(No* n);
    vector<No*> targetSubTrees(No* n);

  protected:
  private:
};

#endif // TREE_H
