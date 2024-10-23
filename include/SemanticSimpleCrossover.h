#ifndef SEMANTICSIMPLECROSSOVER_H
#define SEMANTICSIMPLECROSSOVER_H

#include "Crossover.h"

#include <random>

class SemanticSimpleCrossover : public Crossover
{
    public:
        SemanticSimpleCrossover();
        virtual ~SemanticSimpleCrossover();

        virtual void cross(Subject ** vec_individuos, int num_individuos);

    protected:

    private:

        static bool compareSemantic(No* a, No* b);
        static bool compareSemanticDifference(No* a, No* b);
        void treeCrossover(Tree* a, Tree* b);
        bool checkRate(double rate);
        void RSC(Tree* a, Tree* b);
        void FVSC(Tree* a, Tree* b);
        void SAC(Tree* a, Tree* b);
        void SSC(Tree* a, Tree* b);
        void SSC_TRY(Tree* a, Tree* b, No*& n, No*& m);
        void SCPS(Tree* a, Tree* b);
};

#endif // SEMANTICSIMPLECROSSOVER_H
