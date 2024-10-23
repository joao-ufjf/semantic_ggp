#ifndef CONFIGURES_INCLUDED
#define CONFIGURES_INCLUDED
#include <stdlib.h>
#include <time.h>       /* time */
#include <omp.h>
#include <string>

#include "Grammar.h"

#define NONTERMINAL -1
#define ERROR -2


#define simpleParser


class Configures {
  public:
    int MAXDEEP;
    int popSize;
    int generations;
    int leastSquare = 0;
    int mono;
    double peso0;
    double peso1;
    double elitism;
    double mutationRate;
    double crossoverRate;
    int numVar;
    int numTree;
    int numFunc;
    int NUM_THREADS = 1;
    int memorySize;
    int cicles;
    int seed;

    int tree_fit_method;
    int model_fit_method;
    int normalized_data;
    int crossover_method;
    int maxEvaluations;

    double ffunction;

    int optimizationEvaluations;
    int approach;

    int semantic;
    int crossover_type;
    int mutation_type;

    int evaluations = 0;
    int node_evaluations = 0;
    bool optimize = false;

    int deuRuimOLS = 0;
    int totalLS = 0;

    int complexity = 0; //0=high 1=terminals

    double* transportVector;

    std::string output;

    std::string history;

    short constant,variable,bynaryArithmeticOperators,unaryArithmeticOperators,
          binaryLogicalOperators,unaryLogicalOperators,comparisonOperators, leastSquareOperator,
          programOperators, adjustedCoeficient, declare_variable, use_variable, hill_cube;

    double wall_timer;

    /// ***  Semantics *** ///
    double upper_bound_sensitivity = 0.5;
    double lower_bound_sensitivity = 10e-02;
    double max_tries = 20;
    double n_cases = 0; // msm do treinamento
    int distance_method = 0;
    std::string fname = "";

    Configures();

};

extern Configures* conf;

#endif // CONFIGURES_INCLUDED

