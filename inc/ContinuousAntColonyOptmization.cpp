#include "ContinuousAntColonyOptmization.h"

ContinuousAntColonyOptmization::ContinuousAntColonyOptmization(){
    //ctor
//    this->nc_max = 50;
//    this->accuracy = accuracy;
//    this->dm = 2;
//    this->k = k;
//
//    this->Tau = NULL;
//    this->domain = NULL;
}

ContinuousAntColonyOptmization::~ContinuousAntColonyOptmization(){
    //dtor
//    delete [] this->xmax;
//    delete [] this->xmin;
//
//    delete [] this->initial;
}

void ContinuousAntColonyOptmization::last(Subject* s, Parser* p){
    this->nc_max = 50;
    this->accuracy = 0.001;
    this->dm = 2;
    this->k = 11;
    this->Tau = NULL;
    this->domain = NULL;
    this->rho = 0.1;
    this->nc = 0;
    this->best_nc = NULL;
    this->best = NULL;

    /*** Inicializando variáveis ***/
    this->ind = dynamic_cast<SimpleIndividuo*>(s);
    this->parser = p;

    /*** Recuperando constantes ***/
    this->n = ind->trees[0]->constantsSize;
    this->initial = new double[this->n];
    ind->trees[0]->Constants(initial, 0, ind->trees[0]->root);

    /*** Atualizando árvore ***/
    int total = 0;
    for(int i = 0; i < conf->numTree; i++){
        ind->trees[i]->replaceAllConst();
        total += ind->trees[i]->constantsSize;
    }

    if(total == 0){
        cout << "Não existem constantes" << endl;
        return;
    }




    //    STEP 1
    this->nc_max = 50;
    this->accuracy = 0.001;
    this->dm = 2;
    this->k = 11;
    /// Calcula os limites
    this->xmax = new double[this->n];
    this->xmin = new double[this->n];

    for(int i = 0; i < this->n; i++){
        this->xmax[i] = this->initial[i] + 1.0;
        this->xmin[i] = this->initial[i] - 1.0;
    }



    /***                                    ***/
    /***           LOOP PRINCIPAL           ***/
    /***                                    ***/
    Ant* best_global_ant = NULL;
    int t = 15;
    while(this->maxH() >= this->accuracy){
        //    STEP 2
        /// Calcula o valor de hi para vi
        this->h = new double[this->n];
        for(int i = 0; i < this->n; i++){
            this->h[i] = (this->xmax[i] - this->xmin[i])/(this->k - 1);
        }

        /// Calcula o domínio em k equivalentes
        this->domain = new double*[this->k];
        for(int i = 0; i < this->k; i++){
            this->domain[i] = new double[this->n];
            for(int j = 0; j < this->n; j++){
                this->domain[i][j] = this->xmin[j] + i * this->h[j];
            }
        }


        //    STEP 3
//        CONDIÇÃO DO LOOP

        //    STEP 4
        /// Inicia o contador interno
        this->nc = 0;

        /// Calcula quantidade de formigas
        this->m = this->k + this->dm;

        /// Inicia Tau
        this->Tau = new double*[this->k];
        for(int i = 0; i < this->k; i++){
            this->Tau[i] = new double[this->n];
            for(int j = 0; j < this->n; j++){
                this->Tau[i][j] = 1.0;
            }
        }

        /// Calcula Q
        double minF = INFINITY;
        for(int i = 0; i < 100; i++){
            Ant* ant = createAnt();
            updateTau(ant->route, ant->fitness);
            minF = min(minF, ant->fitness);
            delete ant;
        }

        /// Calcula Q
        this->Q = pow(10, int(log10(minF)) + 1);


        //    STEP 5
        Ant* best_global_ant = NULL;
        int t = 15;
        while(this->nc < this->nc_max){
            Ant* best_local_ant = NULL;
            this->nc++;

            for(int i = 0; i < this->m; i++){
                Ant* ant = createAnt();
                evaluateAnt(ant);

                /// Atualiza o melhor local
                if(best_local_ant == NULL){
                    best_local_ant = ant;
                }
                else if(best_local_ant->fitness > ant->fitness){
                    delete best_local_ant;
                    best_local_ant = ant;
                }
                else{
                    delete ant;
                }
            }

//            /// Atualiza Tau com melhor local
//            for(int j = 0; j < this->n; j++){
//                this->Tau[best_local_ant->route[j]][j] = (1 - this->rho) * this->Tau[best_local_ant->route[j]][j] + this->Q/best_local_ant->fitness;
//            }

        }

        //    STEP 6


        //    STEP 7


        //    STEP 8
    }
}

void ContinuousAntColonyOptmization::Optimize(Subject* s, Parser* p){
//    cout << "Começou o ACO" << endl;
    this->nc_max = 10;
    this->k = 11;
    this->t_base = 5;
    int evaluations = 1000;

    if(conf->approach == 101 || conf->approach == 201){
        this->nc_max = 10;
        this->k = 11;
        this->t_base = 5;
        evaluations = 100000;
    }
    else if (conf->approach == 100 || conf->approach == 200){
        this->nc_max = 4;
        this->k = 6;
        this->t_base = 2;
        int evaluations = 10000;
    }

    this->accuracy = 0.0001;
    this->dm = 2;
    this->rho = 0.1;
    this->nc = 0;
    this->Tau = NULL;
    this->domain = NULL;
    this->best_nc = NULL;
    this->best = NULL;

    /*** Inicializando variáveis ***/
    this->ind = dynamic_cast<SimpleIndividuo*>(s);
    this->parser = p;

    /*** Recuperando constantes ***/
    this->n = ind->trees[0]->constantsSize;
    this->initial = new double[this->n];
    ind->trees[0]->Constants(initial, 0, ind->trees[0]->root);

    /*** Atualizando árvore ***/
    int total = 0;
    for(int i = 0; i < conf->numTree; i++){
        ind->trees[i]->replaceAllConst();
        total += ind->trees[i]->constantsSize;
    }

    if(total == 0){
//        cout << "Não existem constantes" << endl;
        return;
    }

    /// Calcula os limites
    this->xmax = new double[this->n];
    this->xmin = new double[this->n];

    for(int i = 0; i < this->n; i++){
        this->xmax[i] = 1;
        this->xmin[i] = -1;
    }

    /// Calcula o valor de hi para vi
    this->h = new double[this->n];
    for(int i = 0; i < this->n; i++){
        this->h[i] = (this->xmax[i] - this->xmin[i])/(this->k - 1);
    }

    /// Calcula o domínio em k equivalentes
    this->domain = new double*[this->k];
    for(int i = 0; i < this->k; i++){
        this->domain[i] = new double[this->n];
        for(int j = 0; j < this->n; j++){
            this->domain[i][j] = this->xmin[j] + i * this->h[j];
        }
    }

    /// Calcula quantidade de formigas
    this->m = this->k + this->dm;

    /// Calcula Tau
    this->Tau = new double*[this->k];
    for(int i = 0; i < this->k; i++){
        this->Tau[i] = new double[this->n];
        for(int j = 0; j < this->n; j++){
            this->Tau[i][j] = 1.0;
        }
    }

    /// Calcula Q
    double minF = INFINITY;
    for(int i = 0; i < 100; i++){
        Ant* ant = createAnt();
        evaluateAnt(ant);
        updateTau(ant->route, ant->fitness);
        minF = min(minF, ant->fitness);
        delete ant;
    }
    this->Q = pow(10, int(log10(minF)) + 1);

//    cout << this->Q << " " << minF << endl;
//    cin.get();

//    printDomain();

    /***                                    ***/
    /***           LOOP PRINCIPAL           ***/
    /***                                    ***/
    Ant* best_global_ant = NULL;
    int t = this->t_base;
//    int evaluations = 5000;
//    while(this->maxH() >= this->accuracy){
    while(evaluations > 0){
        Ant* best_local_ant = NULL;
        this->nc = 0;

        while(this->nc < this->nc_max){
            this->nc++;

            for(int i = 0; i < this->m; i++){
                Ant* ant = createAnt();
                evaluateAnt(ant);
                evaluations--;
                conf->evaluations++;

                /// Atualiza o melhor local
                if(best_local_ant == NULL){
                    best_local_ant = ant;
                }
                else if(best_local_ant->fitness > ant->fitness){
                    delete best_local_ant;
                    best_local_ant = ant;
                }
                else{
                    delete ant;
                }
            }

            /// Atualiza Tau com melhor local
            for(int j = 0; j < this->n; j++){
                this->Tau[best_local_ant->route[j]][j] = (1 - this->rho) * this->Tau[best_local_ant->route[j]][j] + this->Q/best_local_ant->fitness;
            }

//            printTau();
//            cin.get();
        }

        /// Atualiza o melhor global
        if(best_global_ant == NULL){
            best_global_ant = best_local_ant;
            t = this->t_base;
//            cout << "First global better " << best_global_ant->fitness << endl;
        }
        else if(best_global_ant->fitness > best_local_ant->fitness){
            delete best_global_ant;
            best_global_ant = best_local_ant;
            t = this->t_base;
//            cout << "New global better " << best_global_ant->fitness << endl;
        }
        else{
            delete best_local_ant;
            t--;
//            cout << "No new global better " << best_global_ant->fitness << endl;
        }

        /// Se acontecerem t iterações sem melhora
        if(t <= 0){
            /// Reinicia tentativas para o novo k
            t = this->t_base;

//            printDomain();
//            printTau();

            /// Recalcula os limites
            int r_max;
            double f_max;
            for(int i = 0; i < this->n; i++){
                r_max = -1;
                f_max = -INFINITY;

                for(int j = 0; j < this->k; j++){
//                    cout << "f_max < this->Tau[j][i] = " << f_max << " " << this->Tau[j][i] << endl;
                    if(f_max < this->Tau[j][i]){
                        r_max = j;
                        f_max = this->Tau[j][i];
                    }
                }

//                cout << "Centro de Tau[" << i << "] = " << r_max << "[" << this->theta*this->k << " >= " << r_max << " >= " << (1 - this->theta)*this->k << "]" << endl;

                if(r_max <= this->theta*this->k || r_max >= (1 - this->theta)*this->k){
//                    cout << "Caso 1 na coluna " << i << endl;
//                     << this->xmin[r_max] << " " << (this->k/2 + this->d1) << " " << this->h[i] << endl
//                     << this->xmax[r_max] << " " << (this->k/2 + this->d1) << " " << this->h[i] << endl;
                    this->xmin[i] = this->domain[r_max][i] - (this->k/2 + this->d1) * this->h[i];
                    this->xmax[i] = this->domain[r_max][i] + (this->k/2 + this->d1) * this->h[i];
                }
//                if(r_max <= this->theta*this->k){
//                    cout << "Caso 0 na coluna " << i << endl;
////                     << this->xmin[r_max] << " " << (this->k/2 + this->d1) << " " << this->h[i] << endl
////                     << this->xmax[r_max] << " " << (this->k/2 + this->d1) << " " << this->h[i] << endl;
//                    this->xmin[i] = this->xmin[i] + (this->k/2 + this->d1) * this->h[i];
//                    this->xmax[i] = this->xmax[i] + (this->k/2 + this->d1) * this->h[i];
//                }
//                else if(r_max >= (1 - this->theta)*this->k){
//                    cout << "Caso 1 na coluna " << i << endl;
////                     << this->xmin[r_max] << " " << (this->k/2 + this->d1) << " " << this->h[i] << endl
////                     << this->xmax[r_max] << " " << (this->k/2 + this->d1) << " " << this->h[i] << endl;
//                    this->xmin[i] = this->xmin[i] - (this->k/2 + this->d1) * this->h[i];
//                    this->xmax[i] = this->xmax[i] - (this->k/2 + this->d1) * this->h[i];
//                }
                else if(this->theta*(this->k) <= r_max && r_max <= (1 - this->theta)*(this->k)){
//                    cout << "Caso 2" << endl;
                    this->xmin[i] = this->xmin[i] + (this->xmax[i] - this->xmin[i])*this->d2;
                    this->xmax[i] = this->xmax[i] - (this->xmax[i] - this->xmin[i])*this->d2;
                }
                else if(r_max <= this->theta*(this->k)){
//                    cout << "Caso 3" << endl;
                    this->xmax[i] = this->xmax[i] - (this->xmax[i] - this->xmin[i])*this->d3;
                }
                else if(r_max >= (1 - this->theta)*(this->k)){
//                    cout << "Caso 4" << endl;
                    this->xmin[i] = this->xmin[i] + (this->xmax[i] - this->xmin[i])*this->d3;
                }
            }
//            cin.get();

            /// Incrementa o tamanho dos domínios
            this->k += this->dm;

            /// Atualiza os h
            for(int i = 0; i < this->n; i++){
                this->h[i] = (this->xmax[i] - this->xmin[i])/(this->k - 1);
            }

            /// Atualiza os domínios
            for(int i = 0; i < (this->k - this->dm); i++){
                delete [] this->domain[i];
            }
            delete [] this->domain;
            // calcula domínio
            this->domain = new double*[this->k];
            for(int i = 0; i < this->k; i++){
                this->domain[i] = new double[this->n];
                for(int j = 0; j < this->n; j++){
                    this->domain[i][j] = this->xmin[j] + i * this->h[j];
                }
            }

//            printX();
//            printDomain();
//            cin.get();

            /// Reinicializando matrix de feromônios
            for(int i = 0; i < (this->k - this->dm); i++){
                delete [] this->Tau[i];
            }
            delete [] this->Tau;

            this->Tau = new double*[this->k];
            for(int i = 0; i < this->k; i++){
                this->Tau[i] = new double[this->n];
                for(int j = 0; j < this->n; j++){
                    this->Tau[i][j] = 1.0;
                }
            }

            /// Atualizando quantidade de formigas
            this->m = this->k + this->dm;

            /// Calcula Q
            double minF = INFINITY;
            for(int i = 0; i < 100; i++){
                Ant* ant = createAnt();
                evaluateAnt(ant);
                updateTau(ant->route, ant->fitness);
                minF = min(minF, ant->fitness);
                delete ant;
            }
            this->Q = pow(10, int(log10(minF)) + 1);

//            cin.get();
//            cout << "k = " << this->k << setw(10) << " m = " << this->m << setw(15) << " maxH = " << this->maxH() << " " << setw(20);
//            cout << fixed << setprecision (4) << "k = " << this->k << setw(10) << " m = " << this->m << setw(15) << " maxH = " << this->maxH() << " " << setw(20);
//            best_global_ant->print();
        }


//        printTau();
//        printDomain();
//        cout << evaluations << endl;
    }
//        cin.get();
//    printTau();
//    printDomain();
//    printX();

//    cout << "k = " << this->k << setw(10) << " m = " << this->m << setw(15) << " maxH = " << this->maxH() << " " << setw(20);
//    best_global_ant->print();

    evaluateAnt(best_global_ant);

    s->fitness = best_global_ant->fitness;
    s->optimized = true;
}

void ContinuousAntColonyOptmization::setIndividual(Subject* s, Parser* parser){

    ///Step 1

    /*** Inicializando variáveis ***/
    this->ind = dynamic_cast<SimpleIndividuo*>(s);
    this->parser = parser;

    /*** Recuperando constantes ***/
    this->n = ind->trees[0]->constantsSize;
    this->initial = new double[this->n];
    ind->trees[0]->Constants(initial, 0, ind->trees[0]->root);

    /*** Atualizando árvore ***/
    int total = 0;
    for(int i = 0; i < conf->numTree; i++){
        ind->trees[i]->replaceAllConst();
        total += ind->trees[i]->constantsSize;
    }

    if(total == 0){
        cout << "Não existem constantes" << endl;
        return;
    }

    initialize();

//    evaluateAnt(this->best);
}

void ContinuousAntColonyOptmization::initialize(){
    /*** Inicializando acurácia ***/
    this->accuracy = 0.1;

    /*** Inicializando domínio ***/
    this->xmax = new double[this->n];
    this->xmin = new double[this->n];

    for(int i = 0; i < this->n; i++){
        this->xmax[i] = this->initial[i] + 1.0;
        this->xmin[i] = this->initial[i] - 1.0;
    }

    /*** Inicializando k ***/
    this->k = 11;

    /*** Inicializando rho ***/
    this->rho = 0.1;

    this->nc = 0;
    this->best_nc = NULL;
    this->best = NULL;

    this->h = new double[this->n];

    /// Divide the domain into k equivalent shares
    updateH();
    updateDomain(0);
    updateAntSize();
    initializeTau();
    updateQ();

    evolve();
}

void ContinuousAntColonyOptmization::evolve(){
    int para = 0;
    int t = 15;

    // até completar o critério de parada
    while(this->maxH() >= this->accuracy){
        this->nc = 0;

        int bestAnt;
        double bestFitness;

        while(this->nc < this->nc_max){
            this->nc++;

//            printTau();
//            updateH();
//            updateDomain(0);
//            updateAntSize();
//            updateQ();

            bestAnt = 0;
            bestFitness = INFINITY;

            /// cria as formigas
//            Ant** ants = new Ant*[this->m];
            for(int i = 0; i < this->m; i++){
                Ant* ant = createAnt();
                evaluateAnt(ant);

                if(best_nc == NULL){
                    this->best_nc = new Ant(ant);
                    bestAnt = i;
                }
                else if(best_nc->fitness > ant->fitness){
                    delete this->best_nc;
                    best_nc = new Ant(ant);
                    bestAnt = i;
                }

//                ants[i] = ant;
            }

            /// atualiza Tau
            updateTau(this->best_nc->route, this->best_nc->fitness);

            /// mata as formigas
//            delete [] ants;
        }

        /// atualiza best global
        if(this->best == NULL){
            this->best = new Ant(this->best_nc);
            t = 10;
        }
        else if(this->best->fitness > bestFitness){
            delete this->best;
            this->best = new Ant(this->best_nc);
            t = 10;
        }
        else{
            t--;
        }

        if(t == 0){
            /// atualiza os xmax e xmin
            updateX();
            /// Atualiza valor de k
            updateK(1);
            /// Reseta o Tau
            initializeTau();
            /// Atualiza valor de h
            updateH();
            /// Atualiza os domínios
            updateDomain(this->dm); /// LEMBRAR QUE TEM QUE TIRAR O -1 DALI
            /// Atualiza a quantidade de morfigas
            updateAntSize();
//            /// Atualiza valor de Q
//            updateQ();
            /// Reinicia t
            t = 10;
        }

        cout << "k = " << this->k << " t = " << t << " maxH = " << this->maxH() << " ";
        this->best->print();
    }
}

void ContinuousAntColonyOptmization::updateX(){
    for(int i = 0; i < this->n; i++){
        int r_max = 0;
        int f_max = -INFINITY;

        for(int j = 0; j < this->k; j++){
            cout << this->k << " " << j << " " << i << " " << Tau[j][i] << endl;
            if(f_max < this->Tau[j][i]){
                r_max = j;
                f_max = this->Tau[j][i];
            }
        }

        if(r_max <= this->theta*this->k || r_max >= (1 - this->theta)*this->k){
//            cout << "Atualizando 1 r" << i  << endl;
            this->xmin[r_max] = this->xmin[r_max] - (this->k/2 + this->d1) * this->h[r_max];
            this->xmax[r_max] = this->xmax[r_max] + (this->k/2 + this->d1) * this->h[r_max];
        }
        else if(this->theta*(this->k + 1) <= r_max && r_max <= (1 - this->theta)*(this->k + 1)){
//            cout << "Atualizando 2 r" << i  << endl;
            this->xmin[r_max] = this->xmin[r_max] + (this->xmax[r_max] - this->xmin[r_max])*this->d2;
            this->xmax[r_max] = this->xmax[r_max] - (this->xmax[r_max] - this->xmin[r_max])*this->d2;
        }
        else if(r_max <= this->theta*(this->k + 1)){
//            cout << "Atualizando 3 r" << i  << endl;
            this->xmax[r_max] = this->xmax[r_max] - (this->xmax[r_max] - this->xmin[r_max])*this->d3;
        }
        else if(r_max >= (1 - this->theta)*(this->k + 1)){
//            cout << "Atualizando 3 r" << i  << endl;
            this->xmin[r_max] = this->xmin[r_max] + (this->xmax[r_max] - this->xmin[r_max])*this->d3;
        }
    }
}

Ant* ContinuousAntColonyOptmization::createAnt(){
    Ant* ant = new Ant(this->n);
    ant->constants = new double[this->n];
    ant->route = new int[this->n];

    for(int v = 0; v < this->n; v++){
        double sumP = 0;

        for(int k = 0; k < this->k; k++){
            sumP += this->Tau[k][v];
        }

        double p = ((double) rand() / (RAND_MAX));
        int xk = 0;


        for(int k = 0; k < this->k; k++){
            p -= this->Tau[k][v]/sumP;

            if(p <= 0)
                break;

            xk++;
        }

        ant->route[v] = xk;
        ant->constants[v] = this->domain[xk][v];
    }

    return ant;
}

void ContinuousAntColonyOptmization::evaluateAnt(Ant* ant){
    if(this->ind->constants == NULL){
        this->ind->constants = new double*[conf->numTree];
        for(int i = 0; i < 1; i++) {
            this->ind->constants[i] = new double[this->n];
        }
    }

    for(int i = 0; i < 1; i++) {
        for(int j = 0; j < this->n; j++) {
            this->ind->constants[i][j] = ant->constants[j];
        }
    }

    ant->fitness = parser->Evaluate(this->ind);
}

double ContinuousAntColonyOptmization::maxH(){
    double m = this->h[0];
    for(int i = 1; i < this->n; i++){
        m = max(m, this->h[i]);
    }

    return m;
}

void ContinuousAntColonyOptmization::printTau(){
    cout << "Tau: " << endl;
    if(this->Tau == NULL)
        cout << " NULL" << endl;
    else
        for(int i = 0; i < this->k; i++){
            cout << "(" << i << ") ";
            for(int j = 0; j < this->n; j++){
                cout << " | " << this->Tau[i][j];
            }
            cout << endl;
        }
}

void ContinuousAntColonyOptmization::printX(){
    cout << "Xmin: " ;
    if(this->xmin == NULL)
        cout << " NULL" << endl;
    else{
        cout << "[";
        for(int i = 0; i < this->n; i++){
                cout << this->xmin[i] << ", ";
        }
        cout << "]" << endl;
    }
    cout << "Xmax: " ;
    if(this->xmax == NULL)
        cout << " NULL" << endl;
    else{
        cout << "[";
        for(int i = 0; i < this->n; i++){
                cout << this->xmax[i] << ", ";
        }
        cout << "]" << endl;
    }
}

void ContinuousAntColonyOptmization::printDomain(){
    cout << "domain: " << endl;
    if(this->domain == NULL)
        cout << " NULL" << endl;
    else
        for(int i = 0; i < this->k; i++){
            cout << i << " [" << this->domain[i] << "] | ";
            for(int j = 0; j < this->n; j++){
                cout << this->domain[i][j] << " | ";
            }
            cout << endl;
        }
}

void ContinuousAntColonyOptmization::updateTau(int* route, double f){
//    printTau();
//    for(int j = 0; j < this->n; j++)
//        cout << this->Tau[0] << " => ";
//    cout << endl;

    for(int j = 0; j < this->n; j++){
        int r = route[j];
//        cout << "Começa " << j << " " << r << " " << this->Tau[r][j] << endl;
        this->Tau[r][j] = (1 - this->rho) * this->Tau[r][j] + this->Q/f;
    }
//    cout << "Termina" << endl;
}

void ContinuousAntColonyOptmization::updateK(int increment){
    // incrementa k
    this->k += increment;
}

void ContinuousAntColonyOptmization::updateH(){
    // atribui o valor de hi para vi
    for(int i = 0; i < this->n; i++){
        this->h[i] = (this->xmax[i] - this->xmin[i])/(this->k - 1);
    }
}

/// LEMBRAR QUE TEM QUE TIRAR O -1 DALI
void ContinuousAntColonyOptmization::updateDomain(int dm){
    // se existe domínio antigo, apaga
    if(this->domain != NULL){
        for(int i = 0; i < (this->k - dm); i++){
            delete [] this->domain[i];
        }
        delete [] this->domain;
    }

    // calcula domínio
    this->domain = new double*[this->k];
    for(int i = 0; i < this->k; i++){
        this->domain[i] = new double[this->n];
        for(int j = 0; j < this->n; j++){
            this->domain[i][j] = this->xmin[j] + i * this->h[j];
        }
    }
}

void ContinuousAntColonyOptmization::updateAntSize(){
    this->m = this->k + this->dm;
}

void ContinuousAntColonyOptmization::initializeTau(){
    /*** Inicializando matrix de feromônios ***/
    if(this->Tau != NULL){
        for(int i = 0; i < this->k - 1; i++){
            delete [] this->Tau[i];
        }
        delete [] this->Tau;
    }

    this->Tau = new double*[this->k];
    for(int i = 0; i < this->k; i++){
        this->Tau[i] = new double[this->n];
        for(int j = 0; j < this->n; j++){
            this->Tau[i][j] = 1.0;
        }
    }
}

void ContinuousAntColonyOptmization::updateQ(){
    /*** Calcula Q inicial ***/
    double minF = INFINITY;
    // 10 tentativas
    for(int i = 0; i < 20; i++){

        // gera uma rota
        int* route = new int[this->n];
        double** constants = new double*[1];
        constants[0] = new double[this->n];
        for(int j = 0; j < this->n; j++){
            route[j] = rand() % this->k;
            constants[0][j] = this->domain[route[j]][j];
        }

        // avalia a rota
        this->ind->constants = constants;
        double fit = this->parser->Evaluate(this->ind);
        minF = min(minF, fit);

//        printTau();
//        for(int j = 0; j < this->n; j++)
//            cout << route[j] << " => ";
        updateTau(route, fit);
//        printTau();

        delete [] route;
    }

    this->Q = pow(10, int(log10(minF)) + 1);
}

bool ContinuousAntColonyOptmization::hStopCriteria(){
    return this->maxH() < this->accuracy;
}


