#ifndef EVOSTRAT_H
#define EVOSTRAT_H

#define _USE_MATH_DEFINES // for C++
#include <cmath>
#include "vector"
#include <time.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <random>
#include "RK4LSIndividuo.h"
#include "Parser.h"
#include "SimpleIndividuo.h"
#include "EvoStratInd.h"

class EvoStrat
{
    public:
        EvoStrat();
        virtual ~EvoStrat();

        void Optimize(Subject* s, Parser* parser);
        double Evaluate(EvoStratInd* individual);

        void mutate_problem(EvoStratInd* parent, EvoStratInd* child);
        void mutate_strategy(EvoStratInd* child);
        void replace_all_c(Subject* s, double** constants);

        int LAMBDA;
        int MI;

        Subject* ind;
        Parser* parser;

        std::default_random_engine* generator;

        int* dim;

        EvoStratInd** population;
        EvoStratInd** children;

    protected:

    private:
        static bool SortPopulationFitness(EvoStratInd* a, EvoStratInd* b);
};

#endif // EVOSTRAT_H
