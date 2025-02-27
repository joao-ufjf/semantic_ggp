#ifndef MULTISEARCH_H
#define MULTISEARCH_H

#include "Subject.h"
#include "Grammar.h"
#include "Tree.h"
#include "Configures.h"
#include "Parser.h"
#include "SimpleParser.h"
#include <cmath>
#include <algorithm>
#include <exception>
#include <fstream>
#include "Selection.h"
#include "MultiobjectiveTournamentSelection.h"
#include "PopulationReplacement.h"
#include "SimpleReplace.h"
#include "IndividuoBuilder.h"
#include "SimpleIndividuoBuilder.h"
#include "DifferencialEvolution.h"
#include "EvolutionStrategie.h"

#include "Mutation.h"
#include "SimpleMutation.h"

#include "Crossover.h"
#include "SimpleCrossover.h"

# include <omp.h>

class MultiSearch{
    public:
        Subject** pop;
        Subject** tmpPop;
        bool stepByStep = false;
        /**
            Tolerância de otimização. Quando o fitness não está melhorando
            pelo menos {tolerance} para duas iterações consecutivas a população é
            considera adaptada.
        **/
        double tolerance = 0.0001;// quando o fitnesse é menor que tolerance, o individuo está adaptado

        IndividuoBuilder * individuoBuilder;
        Selection * seletor;
        Parser * parser;
        Parser * parserTraining;
        Parser * parserTest;
        Parser * parserValidation;
        PopulationReplacement * replacer;
        Mutation * mutationOperator;
        Crossover * crossoverOperator;

        MultiSearch();
        MultiSearch(Parser * parser = NULL, PopulationReplacement * populationreplacement = NULL,IndividuoBuilder * individuobuilder = NULL);

        void Operate();
        void Replace();
        void evolve();
        void evolvePG();
        virtual ~MultiSearch();

        /** Multiobjective functions **/

        int Tournament(int a, int b);
        void CalcRank(int size);
        bool Dominate(Subject* a, Subject* b);
        static bool SortPopulationRank(Subject* a, Subject* b);
        static bool SortPopulationCrowding(Subject* a, Subject* b);
        static bool SortPopulationComplexity(Subject* a, Subject* b);
        void crowdingDistanceFitness(int i, int f);
        void crowdingDistanceComplexity(int i, int f);

        /** Print functions **/
        void printResult();
        void printParameters();
        void printBestIndividuo();

        /**Set GP Parameters*/
        void setCrossover(Crossover * opCrossover);
        void setMutation(Mutation * opMutation);
        void setIndividuoBuilder(IndividuoBuilder * indBuilder);
        void setPopulationReplace(PopulationReplacement * popReplace);
        void setParser(Parser * parser);
        void setSelection(Selection * seletor);

        void setParserTest(Parser * p) {
            parserTest = p;
        }
        void setParserValidation(Parser * p) {
            parserValidation = p;
        }

      protected:

      private:
        void replaceAllConst(int initialIndex, int finalIndex );
        void EvaluatePopulation(int initialIndex, int finalIndex, bool optimize = false);
        void EvaluatePopulationValidation(int initialIndex, int finalIndex, int optimizeRange = 0);
        void DetailedEvaluatePopulation(Subject* s);
        static bool SortPopulationFitness(Subject* a, Subject* b);
        static bool SortPopulationFitnessValidation(Subject* a, Subject* b);
        void createsInitialPopulation();
};

#endif // MULTISEARCH_H
