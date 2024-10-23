#ifndef RK4_STOICHIOMETRY_H
#define RK4_STOICHIOMETRY_H
#include "SimpleParser.h"
#include "RK4LSIndividuo.h"
#include "DifferencialEvolution.h"
#include <string>
#include <sstream>
#include <fstream>

class RK4_Stoichiometry : public SimpleParser {
    public:
        RK4_Stoichiometry();
        virtual double Evaluate(Subject* s);
        virtual void Optimize(Subject* s);
        virtual string nameParser();
        virtual void setDataSet(double ** x,int tam);
        virtual ~RK4_Stoichiometry();
        void printData();
        void printResult(Subject* s);

        void load_matrix(string sarq);
        double** s_matrix;
        int n_species;
        int n_reactions;

    protected:

    private:
        /*Ajustar tamanho do passo*/
        DifferencialEvolution* diffEvo;

        double AuxSEvaluate(vector<tuple<double,double> > linearModel, int model, double* dat, double* increments);
        double AuxEvaluate(Subject* s, int model, double* dat, double* increments);
        double* AuxEvaluateFunctions(Subject* s, int model, double* dat, double* increments);
        double** RKEvaluate(Subject* s, double* initial, int vars, double h, int iterations);
        double ** RKEvaluateVariable(Subject* s, double** matrix, int vars, double h, int iterations);
};

#endif // RK4_STOICHIOMETRY_H
