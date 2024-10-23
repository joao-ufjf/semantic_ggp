#ifndef CONTINUOUSANTCOLONYOPTMIZATION_H
#define CONTINUOUSANTCOLONYOPTMIZATION_H

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
#include "RK4LSIndividuo.h"
#include "Parser.h"
#include "SimpleIndividuo.h"
#include "../inc/Ant.h"

#define M_PI 3.14159265358979323846

class ContinuousAntColonyOptmization{
    public:
        ContinuousAntColonyOptmization();
        virtual ~ContinuousAntColonyOptmization();

        void setIndividual(Subject* s, Parser* parser);

        void initialize();
        void evolve();

        double maxH();

        void printTau();
        void printDomain();
        void printX();

        void updateTau(int* route, double f);
        void updateK(int increment);
        void updateH();
        void updateDomain(int dm);
        void updateAntSize();
        void updateQ();

        Ant* createAnt();
        void evaluateAnt(Ant* ant);

        void initializeTau();

        bool hStopCriteria();

        void updateX();

        void Optimize(Subject* s, Parser* p);
        void last(Subject* s, Parser* p);

        /***  Variables  ***/

        SimpleIndividuo* ind;
        double* initial;
        Parser* parser;

        int nc_max;
        int nc;
        int t_base;
        double accuracy;
        double dm; // pequeno número inteiro pra fazer m ser pouco maior que k
        int k; //divisão do domínio
        int oldK;
        int m; //tamanho da ant
        int oldM;

        double* xmax;
        double* xmin;
        int n; // quantidade de constantes
        double* h; // tamanho do passo no domínio
        double** domain; // o domínio

        double** Tau; // matriz de feromônios
        double** p; // matriz de probabilidades
        double Q;

        double rho; // evaporação < 1

        Ant* best;
        Ant* best_nc;

        double theta = 0.2;
        double d1 = 1.25;
        double d2 = 0.05;
        double d3 = 0.10;

    protected:

    private:
};

#endif // CONTINUOUSANTCOLONYOPTMIZATION_H
