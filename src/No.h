#ifndef NO_H
#define NO_H


#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <sstream>
#include <tuple>

using namespace std;

class No {
  public:
    No();
    No(double t, double v, string m, int h);
    virtual ~No();

    int replaceAllC(double* constants, int index);
    int replaceAllConst(int constantCount);
    void print(int format);
    void detailed(int format);
    void print();
    void addSon(No* n);
    void erase();
    void swap(No* m);
    No* clone();

    double type, value;
    string mask;
    int deep;
    int height;
    bool modificated;
    vector<double> semantic;
    double semantic_distance;
    double semantic_difference;


    vector<No*> sons;
    No* father;
    int index;
    void destrutor(No * p);

    double semanticDistance(No* n, int method);
    double semanticDifference(No* n);
    void printSemantic();

    void getLinearModel(vector<tuple<double, double> >* linearModel);

  protected:
  private:
};

#endif // NO_H
