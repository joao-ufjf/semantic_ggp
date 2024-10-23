#ifndef SEMANTICSIMPLEPARSER_H
#define SEMANTICSIMPLEPARSER_H

#include "SimpleParser.h"
#include "DifferencialEvolution.h"
#include "EvolutionStrategie.h"
#include "EvoStrat.h"
#include <algorithm>

class SemanticSimpleParser : public SimpleParser
{
    public:
        SemanticSimpleParser();
        virtual ~SemanticSimpleParser();
        double Evaluate(Subject* s);
        string nameParser();

        void AuxEvaluateTree(Subject * s);

    protected:

    private:

        void Aux2EvaluateTree(No* node, vector<tuple<double, double> >* partial);
        vector<tuple<double, double> > Aux3EvaluateTree(No* node);
};

#endif // SEMANTICSIMPLEPARSER_H
