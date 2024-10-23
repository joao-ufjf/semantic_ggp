#ifndef EVOSTRATIND_H
#define EVOSTRATIND_H

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include "vector"
#include <time.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include "RK4LSIndividuo.h"
#include "Parser.h"
#include "SimpleIndividuo.h"

class EvoStratInd
{
    public:
        EvoStratInd(int* dim);
        virtual ~EvoStratInd();

        int* dim;

        double fitness;
        double** problem;
        double** Std;

        void print();
    protected:

    private:
};

#endif // EVOSTRATIND_H
