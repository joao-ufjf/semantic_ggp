#include "EvoStrat.h"

EvoStrat::EvoStrat()
{
    this->generator = new default_random_engine(rand());
}

EvoStrat::~EvoStrat()
{
    delete this->generator;
    delete [] this->dim;

    for(int i = 0; i < this->MI; i++){
        delete this->population[i];
    }

    delete [] this->population;

//
//    for(int i = 0; i < this->MI; i++){
//        for(int j = 0; j < conf->numTree; j++){
//            delete [] this->population[i][j];
//            delete [] this->populationStd[i][j];
//        }
//
//        delete [] this->population[i];
//        delete [] this->populationStd[i];
//    }
//
//    for(int i = 0; i < this->LAMBDA; i++){
//        for(int j = 0; j < conf->numTree; j++){
//            delete [] this->children[i][j];
//            delete [] this->childrenStd[i][j];
//        }
//
//        delete [] this->children[i];
//        delete [] this->childrenStd[i];
//    }
//
//    delete [] this->population;
//    delete [] this->populationStd;
//    delete [] this->children;
//    delete [] this->childrenStd;
}

bool EvoStrat::SortPopulationFitness(EvoStratInd* a, EvoStratInd* b){
    return a->fitness < b->fitness;
}

void EvoStrat::Optimize(Subject* s, Parser* parser){
    cout << "Início" << endl;

    if(s->optimized)
        return;

    /*** Inicializa os primeiros parâmetros ***/
    this->MI = 15;
    this->LAMBDA = 7 * this->MI;
    this->parser = parser;

    /*** Faz o casting do indivíduo ***/
    this->ind = dynamic_cast<Subject*>(s);

    /*** Inicializa vetores ***/
    /* Vetor com a quantidade de cosntantes em cada árvore */
    this->dim = new int[conf->numTree];
    for(int i = 0; i < conf->numTree; i++) {
        this->dim[i] = s->trees[i]->constantsSize;
    }

    /*** Cria populações ***/
    this->population    = new EvoStratInd*[this->MI + this->LAMBDA];

    for(int i = 0; i < this->MI + this->LAMBDA; i++){
        this->population[i] = new EvoStratInd(this->dim);
    }

    /*** Verifica se há cosntantes a serem otimizadas ***/
    int total = 0;
    for(int i = 0; i < conf->numTree; i++) {
        ind->trees[i]->Constants(this->population[0]->problem[i], 0, ind->trees[i]->root);
        ind->trees[i]->replaceAllConst();
        total += ind->trees[i]->constantsSize;
    }
    if(total == 0)
        return;

    /*** Calcula o desvio para cada constante ***/
    for (int i = 0; i < this->MI + this->LAMBDA; i++) {
        for(int j = 0; j < conf->numTree; j++){
            for(int k = 0; k < this->dim[j]; k++){
                this->population[i]->Std[j][k] = this->population[i]->problem[j][k] * 0.05; // atribui inicialmente esse valor para todos stds
            }
        }
    }

    /*** Calcula o fitness da população inicial ***/
    for(int i = 1; i < this->MI + this->LAMBDA; i++){
        mutate_problem(this->population[0], this->population[i]);
        mutate_strategy(this->population[i]);
    }

    for(int i = 0; i < this->MI + this->LAMBDA; i++){
        this->population[i]->fitness = this->Evaluate(this->population[i]);
    }

    int generations = 0;
    int budget = 10000;
    int best = 0;
    double bestFit = INFINITY;

    while(generations * this->LAMBDA < budget){
        stable_sort(this->population, this->population + this->MI + this->LAMBDA, SortPopulationFitness);

//        cout << generations << " => ";
//        for(int i = 0; i < this->MI; i++){
//            cout << this->population[i]->fitness << " ";
//        }
//        cout << " initial = " << this->ind->fitness << " best = " << bestFit << endl;

        for(int i = 0; i < this->LAMBDA; i++){
//            cout << i % this->MI << " gerando " << this->MI + i << endl;
            mutate_problem(this->population[i % this->MI], this->population[this->MI + i]);
            mutate_strategy(this->population[this->MI + i]);
            this->population[this->MI + i]->fitness = this->Evaluate(this->population[this->MI + i]);
        }
//        cin.get();

//        for(int i = 0; i < this->MI; i++){
//            cout << i << " gerando ";
//            for(int j = 0; j < this->LAMBDA/this->MI; j++){
//                cout << this->MI + (i * this->MI) + j << " ";
//                mutate_problem(this->population[i], this->population[this->MI + (i * this->MI) + j]);
//                mutate_strategy(this->population[this->MI + (i * this->MI) + j]);
//                this->population[this->MI + (i * this->MI) + j]->fitness = this->Evaluate(this->population[this->MI + (i * this->MI) + j]);
//            }
//            cout << endl;
//        }
//        cin.get();

        //For para avaliar todos filhos
        for(int i = 0; i < this->MI + this->LAMBDA; i++){
            if(this->population[i]->fitness <= bestFit){
                best = i;
                bestFit = this->population[i]->fitness;
            }
        }

        generations++;
//        cin.get();
    }

    for(int i = 0; i < conf->numTree; i++) {
        this->ind->constants[i] = new double[dim[i]];
        for(int j = 0; j < dim[i]; j++) {
            this->ind->constants[i][j] = population[best]->problem[i][j];
            grammar->addConstant(this->ind->constants[i][j]);
        }
    }

    this->replace_all_c(s, this->ind->constants);
    s->fitness = parser->Evaluate(s);
}

double EvoStrat::Evaluate(EvoStratInd* individual) {
    this->ind->constants = individual->problem;
    return parser->Evaluate(this->ind);
}

//Metodo para retornar o filho mutado. Como eh um problema sem limites nao precisa fazer a verificacao
void EvoStrat::mutate_problem(EvoStratInd* parent, EvoStratInd* child){
    //double** child;
    std::normal_distribution<double> distribution(0,1);
    double rands = distribution(*this->generator); // gaussian
//    Para todas as arvores do individuo
    for(int i = 0; i < conf->numTree; i++){
        for(int j = 0; j < dim[i]; j++){
            child->problem[i][j] = parent->problem[i][j] + parent->Std[i][j] * rands;
        }
    }
}

//Metodo para
void EvoStrat::mutate_strategy(EvoStratInd* child){
    //double** child;
    std::normal_distribution<double> distribution(0,1);
    double rands = distribution(*generator); // gaussian
    double tau = pow(sqrt(2.0*dim[0]),-1);
    double tau_p = sqrt(pow(sqrt(2.0*dim[0]),-1));
    //Para todas as arvores do individuo
    for(int i = 0; i < conf->numTree; i++){
        for(int j = 0; j < dim[i]; j++){
            child->Std[i][j] = exp(tau_p * rands + tau * rands);
        }
    }
}

void EvoStrat::replace_all_c(Subject* s, double** constants){
    for(int i = 0; i < conf->numTree; i++){
        s->trees[i]->replaceAllC(constants[i]);
        s->trees[i]->constantsSize = this->dim[i];
    }
}
