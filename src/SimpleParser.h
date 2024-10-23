#ifndef SIMPLEPARSER_H
#define SIMPLEPARSER_H

#include "Configures.h"
#include "Parser.h"
#include "SimpleIndividuo.h"
#include "DifferencialEvolution.h"
#include "EvolutionStrategie.h"
#include <vector>
#include <stack>
#include <tuple>
#include <cmath>
#include <sstream>

class SimpleParser : public Parser {
  public:
    SimpleParser();
    virtual string nameParser();
    virtual double Evaluate(Subject* s);
    virtual void Optimize(Subject* s);
    virtual double Operate(int opType, int opValue, double a, double b = NULL, double c = NULL);
    virtual void Normalize();
    virtual double Normalize(double value, int var);

    DifferencialEvolution* diffEvo;
    EvolutionStrategie* evoStrat;

    virtual void setDataSet(double ** x,int tam) {
        tamDataset = tam;
        dataset = x;
    }

    virtual void printResult(Subject * s) {}

    ~SimpleParser();
    double** dataset;
    double** norm_dataset;
    int tamDataset;

    void resetStringStreams();
    stringstream integer;
    stringstream decimal;

  protected:
  private:
    double AuxEvaluate(Subject* s, int model, double* dat);
};

#endif // SIMPLEPARSER_H
