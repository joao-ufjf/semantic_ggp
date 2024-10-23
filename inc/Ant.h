#ifndef ANT_H
#define ANT_H

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include "vector"
#include <time.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std;

class Ant{
    public:
        Ant(int n);
        Ant(Ant* ant);
        ~Ant();

        void print();

        double* constants;
        int* route;
        int n;
        double fitness;

    protected:

    private:
};

#endif // ANT_H
