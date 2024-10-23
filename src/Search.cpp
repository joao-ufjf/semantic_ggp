#include "Search.h"
#include <vector>

/**
* Construtores da função de busca
*   Recebem todos os parametros relativos a busca
*   Parameters
*       Parser - responsavel por avaliar os individuos
*       PopulationReplace - reponsavel pela substituição da população para a proxima geração
*       IndividuoBuilder - responsavel pela criação de individuos
*       Mutation - Operador de mutação
*       Crossover - Operador genetico de mutação
*       Selection - Operador de seleção de indivudos
**/
Search::Search()
{
    setParser(NULL);
    setPopulationReplace(NULL);
    setIndividuoBuilder(NULL);
    setMutation(NULL);
    setCrossover(NULL);
    setSelection(NULL);

   // omp_set_num_threads(conf->NUM_THREADS);

}


Search::Search(Parser* parser,PopulationReplacement * populationreplace, IndividuoBuilder * individuobuilder)
{
    setParser(parser);
    setPopulationReplace(populationreplace);
    setIndividuoBuilder(individuobuilder);

    setMutation(NULL);
    setCrossover(NULL);
    setSelection(NULL);

   // omp_set_num_threads(conf->NUM_THREADS);

}

/**
    Função privada que instancia e preenche a população inicial de individuos
**/
void Search::createsInitialPopulation(){
//    cout << "Create Initial Population..." << endl;
    pop = new Subject*[conf->popSize * 2];

   // #pragma omp parallel for num_threads(conf->NUM_THREADS)
    for(int i = 0; i < conf->popSize; i++){
        pop[i] = individuoBuilder->generateIndividuo();
    }
}
//Substitui todos os valores constantes por _c_
void Search::replaceAllConst(int initialIndex, int finalIndex){
    #pragma omp parallel for num_threads(conf->NUM_THREADS)
    for(int i = initialIndex; i < finalIndex; i++){
        if(!pop[i]->optimized){
            pop[i]->trees[0]->replaceAllConst();
        }
    }
}

/**
*
*   Realiza a busca atravez do processo evolutivo
*   @return void
**/
void Search::evolve(){
    auto beg = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();

    int tolerance_count = 0;
    cout << "Criar populacao inicial " << conf->popSize << endl;
    this->createsInitialPopulation();
    DifferencialEvolution* de = new DifferencialEvolution();
    EvolutionStrategie* es = new EvolutionStrategie();
    //initial population evaluation
//    if(conf->approach != 0)
//        replaceAllConst(0, conf->popSize);
    EvaluatePopulation(0, conf->popSize);

    stable_sort(pop, pop + conf->popSize, SortPopulationFitness);
    cout << "0 " << pop[0]->trees[0]->fitness << endl;

    //cin.get();
    Subject* best = NULL;

    int timeStamp = 100;
    int DECounter = 1;
    int maxEvaluations = 250;

    conf->optimizationEvaluations = 100000;
    vector<double> fitInTime;
    vector<double> timeInTime;
    fitInTime.push_back(pop[0]->fitness);
    timeInTime.push_back(0);

//    int raco_counter = maxEvaluations * 0.5;

    for(int it = 0; true; it++){
        /// Realiza cross e mut
        Operate();

        /// Avalia população
        EvaluatePopulation(conf->popSize, conf->popSize * 2);

        /// Ordena os criados pelo fitness
        stable_sort(pop + conf->popSize, pop + conf->popSize * 2, SortPopulationFitness);

        /// Repõe a população com os melhores da nova população
        replacer->Replace(pop);

        /// Ordena a nova população total
        stable_sort(pop, pop + conf->popSize, SortPopulationFitness);

        /// Descarta os não selecionados
        for(int i = conf->popSize; i < conf->popSize * 2; i++){
            delete pop[i];
        }

        end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - beg);

        /// Exibe o melhor da geração it
        for(int i = 0; i < 1; i++){
            cout << "it:" << it << " t: " << duration.count() << " n:" << conf->evaluations << " fit:" << pop[i]->fitness << " size:" << pop[i]->trees[0]->linearModel.size()  << " model:";
            pop[i]->print();
        }

        fitInTime.push_back(pop[0]->fitness);
        timeInTime.push_back(duration.count());

        if(conf->node_evaluations > 15 * pow(10, 6)){
            break;
        }
    }

    /// Avalia população para teste (teste e validação estão trocados)
    EvaluatePopulationTest(0, conf->popSize);
    stable_sort(pop, pop + conf->popSize, SortPopulationFitnessTest);

//    /***
//     * Calcula SUM(Norma 1(TREES))
//     ***/
//    conf->tree_fit_method = 0;
//    conf->model_fit_method = 0;
//    conf->normalized_data = 1;

    /// Avalia população para validação (teste e validação estão trocados)
    pop[0]->fitnessValidation = parserValidation->Evaluate(pop[0]);

    cout << pop[0]->fitness << " " << pop[0]->fitnessTest << " " << pop[0]->fitnessValidation << endl;
    pop[0]->print();

    cout << "it:" << maxEvaluations/conf->popSize << " n:" << conf->evaluations << " fit:" << pop[0]->fitnessValidation << " size:" << pop[0]->trees[0]->linearModel.size()  << " model:";
    pop[0]->print();

    /// Printa o modelo para plot
    std::fstream plot_file;
    plot_file.open(
            conf->output +
            "dump/" +
            conf->fname +
            "_" +
            std::to_string(conf->crossover_type) +
            "_" +
            std::to_string(conf->distance_method) +
            "_" +
            std::to_string(conf->lower_bound_sensitivity) +
            "_" +
            std::to_string(conf->upper_bound_sensitivity) +
            "_" +
            std::to_string(conf->seed) +
            ".plot",
        std::ios_base::app);
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(plot_file.rdbuf()); //redirect std::cout to out.txt!
    parser->printResult(pop[0]);
    plot_file.close();
    std::cout.rdbuf(coutbuf); //reset to standard output again

    /// Printa o resultado no csv
    std::ofstream final_pop_file;
    final_pop_file.open(conf->output + "res.csv", std::ios_base::app);
//    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(final_pop_file.rdbuf()); //redirect std::cout to out.txt!
//    cout << "fitness,model" << endl;
//    for(int i = 0; i < conf->popSize; i++){
    for(int i = 0; i < 1; i++){
        cout
            << conf->seed << ";"
            << conf->fname << ";"
            << conf->crossover_type << ";"
            << conf->distance_method << ";"
            << conf->lower_bound_sensitivity << ";"
            << conf->upper_bound_sensitivity << ";"
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count() << ";"
            << pop[i]->fitness << ";"
            << pop[i]->fitnessTest << ";"
            << pop[i]->fitnessValidation << ";";

        pop[i]->print();
//        cin.get();
    }
    final_pop_file.close();
    std::cout.rdbuf(coutbuf); //reset to standard output again

    /// Printa o histórico
    std::ofstream hist_file;
    hist_file.open(conf->output +
            "dump/" +
            conf->fname +
            "_" +
            std::to_string(conf->crossover_type) +
            "_" +
            std::to_string(conf->distance_method) +
            "_" +
            std::to_string(conf->lower_bound_sensitivity) +
            "_" +
            std::to_string(conf->upper_bound_sensitivity) +
            "_" +
            std::to_string(conf->seed) +
            ".hist", std::ios_base::app);
//    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(hist_file.rdbuf()); //redirect std::cout to out.txt!
//    cout << "fitness,model" << endl;
//    for(int i = 0; i < conf->popSize; i++){
    for(int i = 0; i < fitInTime.size(); i++){
        cout << fitInTime.at(i) << ";" << timeInTime.at(i) << endl;
    }
    hist_file.close();
    std::cout.rdbuf(coutbuf); //reset to standard output again

    return;
}

void Search::Replace(){
    stable_sort(pop, pop + conf->popSize * 2, SortPopulationFitness);
}

/**
    Parameters
        int initialIndex - indice inicial a ser avaliado
        int finalIndex - indice final a pop sera avaliada até o finalIndex -1
    Percorre o vetor pop (população) avaliando os individuos do indice initialIndex até finalIndex
    @return void
**/
void Search::EvaluatePopulation(int initialIndex, int finalIndex, int optimizeRange){
    #pragma omp parallel for num_threads(conf->NUM_THREADS)
    for(int i = initialIndex; i < finalIndex; i++){
        if(i < optimizeRange && !pop[i]->optimized){
            Subject* ind_ = pop[i]->clone();
            parser->Evaluate(ind_);
//            cout << "Antes do otimizador " << ind_->fitness << endl;
            parser->Optimize(ind_);
            ind_->fitness = parser->Evaluate(ind_);
//            cout << "Depois do otimizador" << ind_->fitness << endl;
//            cin.get();
            if(ind_->fitness < pop[i]->fitness){
//                cout << pop[i]->fitness << " " << ind_->fitness << endl;
                swap(pop[i], ind_);
                delete ind_;
//                pop[i]->print();
//                cin.get();
            }
        }
        else{
            pop[i]->fitness = parser->Evaluate(pop[i]);
        }
        conf->evaluations++;
//        cout << "   " << i << " " << pop[i]->fitness << endl;
    }

//    #pragma omp parallel for num_threads(conf->NUM_THREADS)
//    for(int i = initialIndex; i < finalIndex; i++){
//        parser->Optimize(pop[i]);
//        pop[i]->fitness = parser->Evaluate(pop[i]);
//        conf->evaluations++;
////        cout << "   " << i << " " << pop[i]->fitness << endl;
//    }
}

void Search::EvaluatePopulationValidation(int initialIndex, int finalIndex, int optimizeRange){
    #pragma omp parallel for num_threads(conf->NUM_THREADS)
    for(int i = initialIndex; i < finalIndex; i++){
        if(i < optimizeRange && !pop[i]->optimized){
            Subject* ind_ = pop[i]->clone();
//            ind_->trees[0]->replaceAllConst();
            parserValidation->Optimize(ind_);
            ind_->fitnessValidation = parserValidation->Evaluate(ind_);
            if(ind_->fitness < pop[i]->fitness){
                cout << ind_->fitnessValidation << " " << pop[i]->fitnessValidation << endl;
//                cin.get();
                swap(pop[i], ind_);
                delete ind_;
            }
        }
        else{
            pop[i]->fitnessValidation = parserValidation->Evaluate(pop[i]);
        }
        conf->evaluations++;
//        cout << "   " << i << " " << pop[i]->fitnessValidation << endl;
    }
}



void Search::EvaluatePopulationTest(int initialIndex, int finalIndex, int optimizeRange){
    #pragma omp parallel for num_threads(conf->NUM_THREADS)
    for(int i = initialIndex; i < finalIndex; i++){
        if(i < optimizeRange && !pop[i]->optimized){
            Subject* ind_ = pop[i]->clone();
//            ind_->trees[0]->replaceAllConst();
            parserTest->Optimize(ind_);
            ind_->fitnessTest = parserTest->Evaluate(ind_);
            if(ind_->fitnessTest < pop[i]->fitnessTest){
                cout << ind_->fitnessTest << " " << pop[i]->fitnessTest << endl;
//                cin.get();
                swap(pop[i], ind_);
                delete ind_;
            }
        }
        else{
            pop[i]->fitnessTest = parserTest->Evaluate(pop[i]);
        }
        conf->evaluations++;
//        cout << "   " << i << " " << pop[i]->fitnessTest << endl;
    }
}

/**
    Aplica os operadores genéticos de mutação e cruzamento
    @return void
**/
void Search::Operate(){
    int num_individuos = seletor->tam_selection;
    Subject* selecionados[num_individuos];

    //#pragma omp parallel for private(selecionados)
    for(int i = conf->popSize; i < conf->popSize * 2; i += num_individuos){
        for(int j = 0; j < num_individuos; j++){
            selecionados[j] = seletor->selects(pop)->clone();
            pop[i+j] = selecionados[j];
        }

        crossoverOperator->cross(selecionados,num_individuos);

        for(int j = 0; j < num_individuos; j++){
            mutationOperator->mutate(selecionados[j]);
        }

        for(int j = 0; j < num_individuos; j++){
            selecionados[j] = NULL;
        }
    }
}


/**
    Set Parameters
**/
void Search::setParser(Parser * parser){
  if(parser==NULL){
        SimpleParser * avaliador  = new SimpleParser();
        avaliador->setDataSet(data->training,data->totalTraining);
        this->parser = avaliador;
    }else{
        this->parser = parser;
    }
}

void Search::setPopulationReplace(PopulationReplacement * popReplace){
    if(popReplace == NULL){
        this->replacer = new SimpleReplace();
    }else{
        this->replacer = popReplace;
    }
}

void Search::setIndividuoBuilder(IndividuoBuilder * indBuilder){
    if(indBuilder == NULL){
        this->individuoBuilder = new SimpleIndividuoBuilder();
    }else{
        this->individuoBuilder = indBuilder;
    }
}

void Search::setMutation(Mutation * opMutation){
    if(opMutation==NULL){
        mutationOperator = new SimpleMutation();
    }else{
        mutationOperator = opMutation;
    }
}

void Search::setCrossover(Crossover * opCrossover){
    if(opCrossover==NULL){
        crossoverOperator = new SimpleCrossover();
    }else{
        crossoverOperator = opCrossover;
    }
}

void Search::setSelection(Selection * seletor){
    if(seletor==NULL){
        this->seletor = new TournamentSelection();
    }else{
        this->seletor = seletor;
    }
}

/** Print Functions **/
void Search::printParameters(){
    cout << endl;
    cout << "************* PARAMETERS **************" << endl;
    cout << "*   GENERATIONS    " << conf->generations << endl;
    cout << "*   POP SIZE       " << conf->popSize << endl;
    cout << "*   ELITISM        " << conf->elitism << endl;
    cout << "*   MAXDEEP        " << conf->MAXDEEP << endl;
    cout << "*   CROSSOVER RATE " << conf->crossoverRate << endl;
    cout << "*   MUTATION RATE  " << conf->mutationRate << endl;
    cout << "*   INDIVIDUO      " << individuoBuilder->nameIndividuo() << endl;
    cout << "*   PARSER         " << parser->nameParser() << endl;
    cout << "***************************************" << endl << endl;
}

void Search::printResult(){
    cout << "--------------Results--------------" << endl;
    pop[0]->print();
//    cout <<  "SERIE TREINO" << endl;
    parser->printResult(pop[0]);
//    cout <<  "\nSERIE VALIDAÇÃO" << endl;
    parserValidation->printResult(pop[0]);
//    cout <<  "\n\nSERIE TESTE" << endl;
    parserTest->printResult(pop[0]);
}

void Search::printBestIndividuo(){

        pop[0]->print();
//        cout << " (" << pop[0]->fitness << ") " << endl;
//        for(int m = 0; m < conf->numTree; m++){
//            cout << "   f" << m << "() = (" << pop[0]->trees[m]->fitness << ") ";
//            pop[0]->trees[m]->root->print();
////            cout << "leastSquare=[";
////            for(int j = 0; j < pop[0]->trees[m]->leastSquareSize; j++)
////                cout << pop[0]->trees[m]->leastSquare[j] << ", ";
////            cout << "]";
//        }
        cout << endl;
}

 bool Search::SortPopulationFitness(Subject* a, Subject* b){
     //cout << "FITNESS: " << a->trees[0]->fitness << " - " << b->trees[0]->fitness << endl;
     return a->fitness < b->fitness;
 }

 bool Search::SortPopulationFitnessTest(Subject* a, Subject* b){
    return a->fitnessTest < b->fitnessTest;
 }

 bool Search::SortPopulationFitnessValidation(Subject* a, Subject* b){
    return a->fitnessValidation < b->fitnessValidation;
 }

Search::~Search()
{
    //dtor
}
