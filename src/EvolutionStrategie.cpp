#include "EvolutionStrategie.h"

// Organizar, tem de substituir c's? organizar pra tamanhos e dimensoes diferentes


/*A classical rule used to govern the amount of mutation
 (standard deviation used in mutation for continuous function optimization)
  was the 1/5 rule, where the ratio of successful mutations
  should be 1/5 of all mutations. If it is greater the variance is increased,
  otherwise if the ratio is is less, the variance is decreased.*/

void pprint(double** ind, int* dim) {
    for(int i = 0; i < conf->numTree; i++){
        cout << "t" << i << " = ";
        for(int j = 0; j < dim[i]; j++){
            cout << ind[i][j] << ", ";
        }
        cout << endl;
    }
}

void EvolutionStrategie::Optimize(Subject* s, Parser* parser){
    if(s->optimized)
        return;

//    if(conf->approach == 3 || conf->approach == 6 || conf->approach == 10 || conf->approach == 15 || conf->approach == 18 || conf->approach == 22)
//        tamLambda = 1;
//    else if(conf->approach == 4 || conf->approach == 7 || conf->approach == 11 || conf->approach == 16 || conf->approach == 19 || conf->approach == 23)
//        tamLambda = 2;
//    else if(conf->approach == 5 || conf->approach == 8 || conf->approach == 12 || conf->approach == 17 || conf->approach == 20 || conf->approach == 24)
//        tamLambda = 4;

    tamLambda = 7 * this->tamMi;

//    cout << s->fitness << endl;

    this->ind = dynamic_cast<Subject*>(s);
    double fitness = 0;
    double* fitnessChild = new double[tamLambda];
    dim = new int[conf->numTree];
    double** initialIndividual = new double*[conf->numTree];
    for(int i = 0; i < conf->numTree; i++) {
        dim[i] = s->trees[i]->constantsSize;
    }

    /*Inicializao da populacao 'pais'*/
    //Primeiro temos populacao de tamanho mi
    population = new double**[tamMi];
    populationStd = new double**[tamMi];
    //Depois para todos mi individuos temos numTree arvores
    for (int i = 0; i < tamMi; i++) {
        population[i] = new double*[conf->numTree];
        populationStd[i] = new double*[conf->numTree];
        //Entao teremos para todas as arvores a dimensao de valores
        for (int j = 0; j < conf->numTree; j++) {
            population[i][j] = new double[dim[j]];
            populationStd[i][j] = new double[dim[j]];
        }
    }

    int total = 0;
    for(int i = 0; i < conf->numTree; i++) {
        ind->trees[i]->Constants(population[0][i], 0, ind->trees[i]->root);
        ind->trees[i]->replaceAllConst();
        total += ind->trees[i]->constantsSize;
    }

//////    Vai otimizar?
    if(total == 0)
        return;
//    s->optimized = true;
    this->parser = parser;

    double *minValue = new double[conf->numTree];
    double *maxValue = new double[conf->numTree];
    for (int i = 0; i < tamMi; i++) {
        minValue[i] = maxValue[i] = population[0][0][0];
        for(int j = 0; j < conf->numTree; j++){
            for(int k = 0; k < dim[j]; k++){
                if(minValue[i] > population[i][j][k]){
                    minValue[i] = population[i][j][k];
                }
                else if(maxValue[i] < population[i][j][k]){
                    maxValue[i] = population[i][j][k];
                }
            }
        }
    }

    for (int i = 0; i < tamMi; i++) {
        for(int j = 0; j < conf->numTree; j++){
            for(int k = 0; k < dim[j]; k++){
                populationStd[i][j][k] = (maxValue[j] - minValue[j]) * 0.05; // atribui inicialmente esse valor para todos stds
            }
        }
    }

    /*Inicializacao da populacao 'filhos'*/
    //Primeiro temos populacao de tamanho tamLambda
    children = new double**[tamLambda];
    childrenStd = new double**[tamLambda];

    //Depois para todos lambda individuos temos numTree arvores
    for (int i = 0; i < tamLambda; i++) {
        children[i] = new double*[conf->numTree];
        childrenStd[i] = new double*[conf->numTree];
        //Entao teremos para todas as arvores a dimensao de valores
//    cout << "Antes" << endl;
        for (int j = 0; j < conf->numTree; j++) {
//            cout << dim[j] << endl;
            children[i][j] = new double[dim[j]];
            childrenStd[i][j] = new double[dim[j]];
        }

//        for (int j = 0; j < conf->numTree; j++) {
//            for (int k = 0; k < dim[j]; k++) {
//                cout << j << " " << k << " " << children[i][j][k] << endl;
////                cout << childrenStd[i][j][k] << endl;
//            }
//        }
//    cout << "Depois" << endl;
    }

    fitness = evaluate(population[0]);// Posicao 0 porque mi eh sempre 1, depois mudar

    int generations = 0;
    int budget = 10000;

    while (generations * tamLambda < budget){
//        conf->evaluations += tamLambda;
        for(int i = 0; i < tamLambda; i++){
            for(int j = 0; j < conf->numTree; j++){
                children[i] = mutate_problem(population[0], populationStd[0], j); //Os valores passados por parametro sao como os pais
                childrenStd[i] = mutate_strategy(populationStd[0], j);
            }
        }
        //For para avaliar todos filhos
        for(int i = 0; i < tamLambda; i++){
            fitnessChild[i] = evaluate(children[i]);
            if(fitness > fitnessChild[i]){//Entao passamos o filho como novo pai
                population[0] = children[i]; // Posicao 0 porque mi eh sempre 1, depois mudar
                populationStd[0] = childrenStd[i];
                fitness = fitnessChild[i];
//                for (int i = 0; i < dim[0]; i++) {
//                    cout << " POP: " << population[0][0][i];
//                }
//                cout << endl;
                cout << fitness << endl;
            }//Se nao, mantemos o pai
        }
        generations++;
        cout << generations << endl;
    }

    for(int i = 0; i < conf->numTree; i++) {
        ind->constants[i] = new double[dim[i]];
        for(int j = 0; j < dim[i]; j++) {
            ind->constants[i][j] = population[0][i][j];
            grammar->addConstant(ind->constants[i][j]);
        }
    }

    replace_all_c(s, ind->constants);
    s->fitness = parser->Evaluate(s);

//    cout << s->fitness << endl;
//    s->print();
//    cin.get();

    delete [] fitnessChild;
}

///<sumary>
/// Metodo para substituir todos os _c_ do individuo por seus respectivos valores ajustados
///</sumary>
void EvolutionStrategie::replace_all_c(Subject* s, double** constants){
    int cont = 0;
    for(int i = 0; i < conf->numTree; i++){
//        cout << dim[i] << " = ";
        s->trees[i]->replaceAllC(constants[i]);
//        cout << endl;
        s->trees[i]->constantsSize = dim[i];
//        cout << i << " ====> ";
//        for(int j = 0; j < dim[i]; j++){
//            cout << constants[i][j] << " ";
//        }
//        cout << endl;
    }
//    cin.get();
}

//Metodo para retornar o filho mutado. Como eh um problema sem limites nao precisa fazer a verificacao
double** EvolutionStrategie::mutate_problem(double** parentProblem, double** parentStrategy, int dimension_index){
    //double** child;
    std::normal_distribution<double> distribution(0,1);
    double rands = distribution(*generator); // gaussian
    child = new double*[conf->numTree];
    //Para todas as arvores do individuo
    for(int i = 0; i < conf->numTree; i++){
        child[i] = new double[dim[dimension_index]];
        for(int j = 0; j < dim[dimension_index]; j++){
            child[i][j] = parentProblem[i][j] + parentStrategy[i][j] * rands;
        }
    }
    return child;
}

//Metodo para
double** EvolutionStrategie::mutate_strategy(double** parentStrategy, int dimension_index){
    //double** child;
    std::normal_distribution<double> distribution(0,1);
    double rands = distribution(*generator); // gaussian
    double tau = pow(sqrt(2.0*dim[dimension_index]),-1);
    double tau_p = sqrt(pow(sqrt(2.0*dim[dimension_index]),-1));
    childStd = new double*[conf->numTree];
    //Para todas as arvores do individuo
    for(int i = 0; i < conf->numTree; i++){
        childStd[i] = new double[dim[dimension_index]];
        for(int j = 0; j < dim[dimension_index]; j++){
            childStd[i][j] = exp(tau_p * rands + tau * rands);
        }
    }
    return childStd;
}

/// <summary>
/// Metodo para realizar o calculo de aptidao de uma populacao e armazenar no vetor fitness
/// </summary>
double EvolutionStrategie::evaluate(double** population) {
        ind->constants = population;
        return parser->Evaluate(ind);
}

void EvolutionStrategie::replace_all_c(Subject* s, double* constants){
    int cont = 0;
    s->trees[0]->replaceAllC(constants);
    s->trees[0]->constantsSize = 0;
}

EvolutionStrategie::EvolutionStrategie()
{
    generator = new default_random_engine(rand());
    //ctor
}

EvolutionStrategie::~EvolutionStrategie()
{

    for (int i = 0; i < tamMi; i++) {
        for(int j =0; j < conf->numTree; j++) {
            delete [] population[i][j];
            delete [] populationStd[i][j];
        }
        delete [] population[i];
        delete [] populationStd[i];
    }
    delete [] population;
    delete [] populationStd;

    for (int i = 0; i < tamLambda; i++) {
        for(int j =0; j < conf->numTree; j++) {
            delete [] children[i][j];
            delete [] childrenStd[i][j];
        }
        delete [] children[i];
        delete [] childrenStd[i];
    }
    delete [] children;
    delete [] childrenStd;
    delete generator;
    for (int i = 0; i < conf->numTree; i++) {
        delete [] child[i];
        delete [] childStd[i];
    }
    delete [] child;
    delete [] childStd;
    //dtor
}
