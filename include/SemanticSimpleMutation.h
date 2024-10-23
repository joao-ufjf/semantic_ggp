#ifndef SEMANTICSIMPLEMUTATION_H
#define SEMANTICSIMPLEMUTATION_H

#include <SemanticSimpleParser.h>
#include <SimpleIndividuo.h>
#include <Tree.h>
#include <Mutation.h>

class Candidate {
    public:
        double distance;
        SimpleIndividuo* ind;

        Candidate(double d, SimpleIndividuo* i){
            distance = d;
            ind = i;
        };

        virtual ~Candidate(){
            delete ind;
        }
};

class SemanticSimpleMutation : public Mutation
{
    public:
        SemanticSimpleMutation(SemanticSimpleParser * parser);
        virtual void mutate(Subject * individuo);
        virtual ~SemanticSimpleMutation();
        static bool compareSemantic(Candidate* a, Candidate* b);

    protected:

    private:
        SemanticSimpleParser* parser;

        void auxMutate(Tree* t);
        bool checkRate(double rate) {
            return (rand()%100 < rate*100);
        }
};

#endif // SEMANTICSIMPLEMUTATION_H
