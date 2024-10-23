#include "SemanticSimpleCrossover.h"

SemanticSimpleCrossover::SemanticSimpleCrossover()
{
    //ctor
}

SemanticSimpleCrossover::~SemanticSimpleCrossover()
{
    //dtor
}


void SemanticSimpleCrossover::cross(Subject ** vec_individuos, int num_individuos) {
    //    Cross pode ocorrer em cada árvore
    if(conf->crossover_method == 0){
        for(int i = 1; i < num_individuos; i+= 2) {
            for( int j = 0 ; j < vec_individuos[i]->trees.size(); j++) {
                if(checkRate(conf->crossoverRate)) {
                    treeCrossover( vec_individuos[i-1]->trees[j], vec_individuos[i]->trees[j] );
                }
            }

            for( int j = 0 ; j < vec_individuos[i]->functions.size(); j++) {
                if(checkRate(conf->crossoverRate)) {
                    treeCrossover( vec_individuos[i-1]->functions[j], vec_individuos[i]->functions[j] );
                }
            }
        }
    }
    else{
    //    Cross só em uma árvore
        for(int i = 1; i < num_individuos; i+= 2) {
            if(checkRate(conf->crossoverRate)) {
                int t = rand() % (conf->numFunc + conf->numTree);

                if(t < conf->numTree){
                    treeCrossover(vec_individuos[i-1]->trees[t], vec_individuos[i]->trees[t]);
                }
                else{
                    t -= conf->numTree;
                    treeCrossover(vec_individuos[i-1]->functions[t], vec_individuos[i]->functions[t]);
                }
            }
        }
    }
}

// Compares two intervals according to starting times.
bool SemanticSimpleCrossover::compareSemantic(No* a, No* b){
    return (a->semantic_distance < b->semantic_distance);
}

// Compares two intervals according to starting times.
bool SemanticSimpleCrossover::compareSemanticDifference(No* a, No* b){
    return (a->semantic_difference > b->semantic_difference);
}

void SemanticSimpleCrossover::treeCrossover(Tree* a, Tree* b) {
    switch(conf->crossover_type){
        /// Seleciona um candidato com prioridade para o mais similar
        case 1: {
            RSC(a, b);
            break;
        }
        /// Seleciona o candidato mais similar
        case 2: {
            FVSC(a, b);
            break;
        }
        /// Seleciona o candidato mais similar
        case 3: {
            SAC(a, b);
            break;
        }
        /// Seleciona o candidato mais similar
        case 4: {
            SSC(a, b);
            break;
        }
        /// Seleciona o candidato mais similar
        case 5: {
            SCPS(a, b);
            break;
        }
    }
}

/*** faz 10 tentativas de troca de uma subárvores de A para as de B onde a < sd < b ***/
/// Semantic Crossover for Program Synthesis (Uy2010)
void SemanticSimpleCrossover::SCPS(Tree* a, Tree* b){
    No* n = a->subTree();
    No* m = NULL;

    vector<No*> semanticTargetedSubTrees = b->targetSubTrees(n);

    conf->node_evaluations = conf->node_evaluations + semanticTargetedSubTrees.size();

    sort(semanticTargetedSubTrees.begin(), semanticTargetedSubTrees.end(), compareSemanticDifference);

    if(semanticTargetedSubTrees.size() > 0){
        int c_diff = 0;

        while(c_diff < semanticTargetedSubTrees.size() &&
            semanticTargetedSubTrees.at(c_diff)->semantic_difference > 0){
            c_diff++;
        }

        if(c_diff > 0){
            sort(semanticTargetedSubTrees.begin(), semanticTargetedSubTrees.begin() + c_diff, compareSemantic);

            m = semanticTargetedSubTrees.at(0);
        }
        else{
            int semantic_size = n->semantic.size();

            for(No* x : semanticTargetedSubTrees){
                int diff = x->semantic_difference;

                if(diff > 0 && diff < semantic_size){
                    m = x;
                    break;
                }
            }

            if(m == NULL){
                int rand_int = rand() % semanticTargetedSubTrees.size();
                m = semanticTargetedSubTrees.at(rand_int);
            }
        }
    }

    if(n == NULL || m == NULL) {
        return;
    }

    swap(n->sons, m->sons);
    swap(n->father, m->father);

    a->update();
    b->update();
}

/// Tentativa do SSC
void SemanticSimpleCrossover::SSC_TRY(Tree* a, Tree* b, No*& n, No*& m){
    n = a->subTree();

//    conferir como ele tá pegando isso, se os candidatos a troca estão obedecendo o limite de altura
    vector<No*> semanticTargetedSubTrees = b->targetSubTrees(n);
    conf->node_evaluations = conf->node_evaluations + 1;

    if(semanticTargetedSubTrees.size() > 0){
        int rand_int = rand() % semanticTargetedSubTrees.size();
        No* o = semanticTargetedSubTrees.at(rand_int);

        if(o->semantic_distance >= conf->lower_bound_sensitivity &&
           o->semantic_distance <= conf->upper_bound_sensitivity){
            m = o;
        }
    }
}

/*** faz 10 tentativas de troca de uma subárvores de A para as de B onde a < sd < b ***/
/// Semantic Similarity-based Crossover (Uy2010)
void SemanticSimpleCrossover::SSC(Tree* a, Tree* b){
    No* n = NULL;
    No* m = NULL;

    for(int i = 0; i < conf->max_tries; i++){
        SSC_TRY(a, b, n, m);

        if(m != NULL) {
            break;
        }
    }

    if(m == NULL) {
        return;
    }

    swap(n->sons, m->sons);
    swap(n->father, m->father);

    a->update();
    b->update();
}

/*** faz 10 tentativas de troca de uma subárvores de A para as de B onde a < sd < b ***/
/// Semantic Awere Crossover (Uy2010)
void SemanticSimpleCrossover::SAC(Tree* a, Tree* b){
    No* n = a->subTree();
    No* m = NULL;

    vector<No*> semanticTargetedSubTrees = b->targetSubTrees(n);

//    sort(semanticTargetedSubTrees.begin(), semanticTargetedSubTrees.end(), compareSemantic);
    if(semanticTargetedSubTrees.size() > 0){
        int rand_int = rand() % semanticTargetedSubTrees.size();
        conf->node_evaluations = conf->node_evaluations + 1;

        /// SE SÃO SEMANTICAMENTE EQUIVALENTES
        /// Scenario 1
        /// Abs(Value_On_Random_Set(P1)-Value_On_Random_Set(P2))<∑
        if(semanticTargetedSubTrees.at(rand_int)->semantic_distance <= conf->upper_bound_sensitivity){
            m = semanticTargetedSubTrees.at(rand_int);
//            cout << rand_int;
//            cin.get();
        }
    }

    if(n == NULL || m == NULL) {
        return;
    }

    swap(n->sons, m->sons);
    swap(n->father, m->father);

    a->update();
    b->update();
}

/*** faz a troca entre os mais similares com distancia maior que lower_bound_sensitivity ***/
/// First Viable Semantic Crossover
void SemanticSimpleCrossover::FVSC(Tree* a, Tree* b){
    No* n = a->subTree();
    No* m = NULL;

    vector<No*> semanticTargetedSubTrees = b->targetSubTrees(n);

    sort(semanticTargetedSubTrees.begin(), semanticTargetedSubTrees.end(), compareSemantic);

    for (int i = 0; i < semanticTargetedSubTrees.size(); ++i) {
        conf->node_evaluations = conf->node_evaluations + 1;
        if(semanticTargetedSubTrees.at(i)->semantic_distance >= conf->lower_bound_sensitivity &&
           semanticTargetedSubTrees.at(i)->semantic_distance <= conf->upper_bound_sensitivity){
            m = semanticTargetedSubTrees.at(i);
            break;
        }
    }

    if(n == NULL || m == NULL) {
        return;
    }

    swap(n->sons, m->sons);
    swap(n->father, m->father);

    a->update();
    b->update();
}

/*** Roleta com prioridade para o mais próximo ***/
/// Roullete Semantic Crossover
void SemanticSimpleCrossover::RSC(Tree* a, Tree* b){
    No* n = a->subTree();
    No* m = NULL;

    vector<No*> semanticTargetedSubTrees = b->targetSubTrees(n);
    vector<No*> candidates;

    sort(semanticTargetedSubTrees.begin(), semanticTargetedSubTrees.end(), compareSemantic);
    conf->node_evaluations = conf->node_evaluations + semanticTargetedSubTrees.size();

    double totalInverseDistances = 0.0;
    double inverseDistances[semanticTargetedSubTrees.size()];

    for (int i = 0; i < semanticTargetedSubTrees.size(); ++i) {
        double distancia = semanticTargetedSubTrees.at(i)->semantic_distance;

        if(distancia >= conf->lower_bound_sensitivity){
            candidates.push_back(semanticTargetedSubTrees.at(i));
            totalInverseDistances += 1.0 / distancia;
            inverseDistances[i] = 1.0 / distancia;
        }
    }

    // Normalizando as probabilidades
    for (int i = 0; i < candidates.size(); ++i) {
        inverseDistances[i] /= totalInverseDistances;
    }

    // Criando um gerador de números aleatórios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Sorteando um ponto com base nas probabilidades inversamente proporcionais
    double randomValue = dis(gen);
    double acumulado = 0.0;

    for (int i = 0; i < candidates.size(); ++i) {
        acumulado += inverseDistances[i];
        if (randomValue <= acumulado) {
            m = candidates.at(i);
            break;
        }
    }

    if(n == NULL || m == NULL) {
        return;
    }

    swap(n->sons, m->sons);
    swap(n->father, m->father);

    a->update();
    b->update();

    candidates.clear();
}

bool SemanticSimpleCrossover::checkRate(double rate) {
    return (rand()%100 < rate*100);
}
