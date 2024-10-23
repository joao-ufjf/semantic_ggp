#include "Subject.h"

Subject::Subject() {
    //cout << "subject set fit = 0 \n";
    fitness = INFINITY;
    fitnessTest = INFINITY;
    fitnessValidation = INFINITY;
    optimized = false;
    complexity = 0;
    crowdingDistance = 0;
}

void Subject::addTree(Tree* t) {
    trees.push_back(t);
    complexity += t->linearModel.size();
}

void Subject::addFunction(Tree* t) {
    functions.push_back(t);
//    complexity += t->linearModel.size();
}

Subject::~Subject() {
//   cout << "Destrutor Subject" << endl;
//   cin.get();
    for(Tree* t : trees)
        delete t;
    trees.clear();

    for(Tree* t : functions)
        delete t;
    functions.clear();
    //
}
