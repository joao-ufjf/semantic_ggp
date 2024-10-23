#include <iostream>
#include "Database.h"
#include "Grammar.h"
#include "No.h"
#include "Configures.h"
#include "Search.h"
#include "MultiSearch.h"
#include "Configures.h"
#include "SimpleParser.h"
#include "SemanticSimpleParser.h"
#include "SimpleCrossover.h"
#include "SemanticSimpleCrossover.h"
#include "SemanticSimpleMutation.h"
#include "SimpleFunctionalEquationsParser.h"
#include "SimpleClassifierParser.h"
#include "AdolfoClassifierParser.h"
#include "../inc/ParserDerivadas.h"
#include "../inc/LeastSquareParser.h"
#include "../inc/LSDerivadasParser.h"
#include "IndividuoBuilder.h"
#include "LeastSquareIndividuoBuilder.h"
#include "../inc/RK4LSDerivadasParser.h"
#include "../inc/RK4LSIndividuoBuilder.h"
#include "Subject.h"
#include "RK4sParser.h"
#include "RK4_Stoichiometry.h"
#include "../inc/RK4AGParser.h"
#include "RK4LSDParser.h"
#include <sstream>

#include <random>
//#include "DifferencialEvolution.h"

#include "../inc/ContinuousAntColonyOptmization.h"

using namespace std;

int func(double sl, double sw, double pl, double pw){
    if(3 >= pl){
        return 0;
    }
    else{
        if(pl >= 5){
            return 2;
        }
        else{
            return 1;
        }
    }
}

#define simpleParser
//#define RK4Parser
int main(int argc, char** argv){
/**********************************************************
1) seed	                     1
2) grammar                   input/circadian5/grammar_lv1
3) data                      input/circadian5/data
4) groups                    input/circadian5/groups
5) approach                  0
6) output                    output/
7) max depth                 8
8) nº of auxiliar functions  0
9) crossover method          0
10) normalized data          1
11) fit do modelo            0
12) fit da árvore            0
13) max evaluations          500
**********************************************************/

    cout << "Hello GP-ufjf!" << endl;

//    cout << exp(39502102.81) << endl;
//    cin.get();

    /// set parametros
    conf = new Configures();
    conf->seed                    = atoi(argv[1]);
	srand(conf->seed);
	/// Loading database and grammar
	data                          = new Database(argv[3]);
	data->loadGroup(argv[4]);
	grammar                       = new Grammar(argv[2]);
	fgrammar                      = new Grammar(argv[2]);
    conf->approach                = atoi(argv[5]);
    conf->output                  = argv[6];
    conf->MAXDEEP                 = atoi(argv[7]);
    conf->numFunc                 = atoi(argv[8]);
    conf->crossover_method        = atoi(argv[9]);
    conf->normalized_data         = atoi(argv[10]);
    conf->model_fit_method        = atoi(argv[11]);
    conf->tree_fit_method         = atoi(argv[12]);
    conf->popSize                 = atoi(argv[13]);
    conf->crossover_type          = atoi(argv[14]);
    conf->mutation_type           = atoi(argv[15]);
    conf->lower_bound_sensitivity = atof(argv[16]);
    conf->upper_bound_sensitivity = atof(argv[17]);
    conf->distance_method         = atof(argv[18]);
    conf->fname                   = argv[19];

    cout.precision(10);

    conf->generations = 100;
    conf->elitism = 0.002; /// 1/500
    conf->crossoverRate = 0.9;
    conf->mutationRate = 0.1;
    conf->NUM_THREADS = 1;

    conf->numVar = data->variables;
    conf->numTree = data->prediction; // seta o numero de variaveis a serem preditas. dependente do problema a ser tratado
    conf->numFunc = data->functions = atoi(argv[8]); // seta o numero de variaveis a serem preditas. dependente do problema a ser tratado

    IndividuoBuilder * individuoBuilder = NULL;
    SemanticSimpleParser * parser = new SemanticSimpleParser();
    SemanticSimpleParser * parserTest = new SemanticSimpleParser();
    SemanticSimpleParser * parserValidation = new SemanticSimpleParser();
    individuoBuilder = new SimpleIndividuoBuilder();

    parser->setDataSet(data->training,data->totalTraining);
    parser->Normalize();
    parserTest->setDataSet(data->test,data->totalTest);
    parserTest->Normalize();
    parserValidation->setDataSet(data->validation,data->totalValidation);
    parserValidation->Normalize();

    Search* s = new Search(parser, NULL, individuoBuilder);
    s->setParserTest(parserTest);
    s->setParserValidation(parserValidation);

    if(conf->crossover_type != 0){
        s->setCrossover(new SemanticSimpleCrossover());
    }

    if(conf->mutation_type != 0){
        s->setMutation(new SemanticSimpleMutation(parser));
    }

    s->stepByStep = false;
    s->evolve();

    return 0;
}
