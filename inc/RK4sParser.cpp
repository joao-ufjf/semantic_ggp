#include "RK4sParser.h"

RK4sParser::RK4sParser()
{
    //ctor
    diffEvo = new DifferencialEvolution();
    evoStrat = new EvolutionStrategie();
//    es = new EvoStrat();
}

string RK4sParser::nameParser(){
    return "RK4sParser";
}

void RK4sParser::setDataSet(double ** x,int tam){
    tamDataset = tam;
    dataset = x;
}

void RK4sParser::printResult(Subject* s){
    if(tamDataset == 0)
        return;
    RK4LSIndividuo * s1 = dynamic_cast<RK4LSIndividuo*>(s);
    double fit = 0;
    double ** serieRK = RKEvaluateVariable(s1,dataset,data->prediction + data->variables,dataset[1][0] - dataset[0][0],tamDataset);
//    double ** serieRK = RKEvaluate(s1,dataset[0],data->prediction + data->variables,dataset[1][0] - dataset[0][0],tamDataset);


    for(int i = 0 ; i < tamDataset; i++){
         for(int j = 0 ; j < data->prediction + data->variables; j++){
            cout << serieRK[i][j] << " ";
//            if(j!=data->prediction + data->variables-1){
//                cout << " ";
//            }
         }

         for(int j = 1 ; j < data->prediction + data->variables; j++){
            cout << dataset[i][j] ;
            if(j!=data->prediction + data->variables-1){
                cout << " ";
            }
         }

         cout << endl;
    }

}

void RK4sParser::Optimize(Subject* s){
//    diffEvo = new DifferencialEvolution();
//    diffEvo->Optimize(s, this);
//    evoStrat->Optimize(s, this);
    EvoStrat* es = new EvoStrat();
    es->Optimize(s, this);
    delete es;
//    delete diffEvo;
}

double RK4sParser::Evaluate(Subject* s){
//    if(s->constants != NULL){
//        cout << "NÃO É NULL" << endl;
//    }
//    else{
//        cout << "É NULL" << endl;
//    }

    AuxEvaluateTree(s);

    RK4LSIndividuo * s1 = dynamic_cast<RK4LSIndividuo*>(s);
    s1->constants = s->constants;

//    if(s1->constants != NULL){
//        cout << "1 NÃO É NULL" << endl;
//    }
//    else{
//        cout << "1 É NULL" << endl;
//    }

    double fit = 0;
    /// cada passo começa nos dados originais
    double ** serieRK = RKEvaluateVariable(s1,dataset,data->prediction + data->variables,dataset[1][0] - dataset[0][0],tamDataset);
    /// cada passo começa nos dados obtidos
//    double ** serieRK = RKEvaluate(s1,dataset[0],data->prediction + data->variables,dataset[1][0] - dataset[0][0],tamDataset);

    if(s1->rk4 != NULL){
        for(int i = 0; i < s1->rk4Size; i++)
            delete [] s1->rk4[i];
        delete [] s1->rk4;
    }
    s1->rk4 = serieRK;
    s1->rk4Size = tamDataset;

    //s1->fitness  = 0;
    for(int m = 0; m < conf->numTree; m++){
        s1->trees[m]->fitness = 0;
        for(int j = 0; j < tamDataset; j++){
            double a, b;

            a = dataset[j][data->variables + m];
            b = serieRK[j][data->variables + m];

            /***
            * 0 = Raw
            * 1 = Normalizado
            ***/

            if(conf->normalized_data == 1){
                a = Normalize(a, data->variables + m);
                b = Normalize(b, data->variables + m);
            }

            /***
            * 0 = Norma 1
            * 1 = Norma 2
            * 2 = MSE
            ***/

//            cout << "m = " << m << " a=" << a << " b=" << b << " abs=" << abs(a - b) << " acc=" << s1->trees[m]->fitness << endl;

            if(conf->tree_fit_method == 0){
                s1->trees[m]->fitness += double(abs(a - b));
            }
            else if(conf->tree_fit_method == 1){
                s1->trees[m]->fitness += double(pow(a - b, 2));
            }
            else{
                s1->trees[m]->fitness += double(pow(a - b, 2)/tamDataset);
            }

            if(isinf(b) || isnan(b)){
                s1->trees[m]->fitness = INFINITY;
//                cout << " AAH " << endl;
                break;
            }
        }

        if(std::isinf(s1->trees[m]->fitness) || std::isnan(s1->trees[m]->fitness)){
            s1->trees[m]->fitness = INFINITY;
//            cout << " BBH " << endl;
            return INFINITY;
        }

        /***
        * 0 = Soma o valor de cada árvore
        * 1 = Soma a média do valor de cada árvore
        * 2 = Soma o maior valor detre as árvores
        ***/

        if(conf->model_fit_method == 0){
            fit += s1->trees[m]->fitness;
        }
        else if(conf->model_fit_method == 1){
            fit += s1->trees[m]->fitness/double(conf->numTree);
        }
        else{
            fit = max(s1->trees[m]->fitness, fit);
        }

//        cout << "m=" << m << " " << s1->trees[m]->fitness << " = " << fit << endl;
    }

    return fit;
}

double ** RK4sParser::RKEvaluateVariable(Subject* s, double** matrix, int vars, double h, int iterations){

    double k[vars][4];
    double** r = new double*[iterations];
    for(int i = 0; i < iterations; i++)
        r[i] = new double[vars];

    for(int i = 0; i < vars; i++){
        r[0][i] = matrix[0][i];
    }

    //assuming initial[0] is x and initial[1 ... vars - 1] is f0(x) ... fn(x)

    for(int i = 0; i < iterations - 1; i++){
        //NÃO USAR H DO CABEÇALHO
        double h = matrix[i+1][0] - matrix[i][0];

        double* increments = new double[vars];
        r[i + 1][0] = matrix[i][0] + h;

        // k0
        increments[0] = 0;
        for(int j = 1; j < vars; j++)
            increments[j] = 0;
        for(int j = 1; j < vars; j++) // s: modelos j:especific model r[i]:variables increments:vector
            k[j][0] = h * AuxEvaluate(s, j, matrix[i], increments);
            // trocar matrix[i] por r[i] quando quiser fazer a série antiga

        // k1
        increments[0] = h/2.0;
        for(int j = 1; j < vars; j++)
            increments[j] = k[j][0]/2.0;
        for(int j = 1; j < vars; j++)
            k[j][1] = h * AuxEvaluate(s, j, matrix[i], increments);

        // k2
        increments[0] = h/2.0;
        for(int j = 1; j < vars; j++)
            increments[j] = k[j][1]/2.0;
        for(int j = 1; j < vars; j++)
            k[j][2] = h * AuxEvaluate(s, j, matrix[i], increments);

        // k3
        increments[0] = h;
        for(int j = 1; j < vars; j++)
            increments[j] = k[j][2];
        for(int j = 1; j < vars; j++)
            k[j][3] = h * AuxEvaluate(s, j, matrix[i], increments);

        delete [] increments;


        for(int j = 1; j < vars; j++){
            r[i + 1][j] = r[i][j] + ((k[j][0] + 2*(k[j][1] + k[j][2]) + k[j][3]))/6.0;
        }

    }

    return r;
}



double** RK4sParser::RKEvaluate(Subject* s, double* initial, int vars, double h, int iterations){
//    cout << "h = " << h << endl;
//    for(int i = 0; i < vars - 1; i++){
//        cout << "f" << i << " = ";
//        s->trees[i]->root->print();
//        cout << endl;
//    }

//    double k[vars][4];
    double k[vars][4];
    double** r = new double*[iterations];
    for(int i = 0; i < iterations; i++)
        r[i] = new double[vars];

    for(int i = 0; i < vars; i++){
        r[0][i] = initial[i];
//        cout << r[0][i] << " ";
    }
//    cout << endl;

    //assuming initial[0] is x and initial[1 ... vars - 1] is f0(x) ... fn(x)

    for(int i = 0; i < iterations - 1; i++){
        double* increments = new double[vars];
        r[i + 1][0] = r[i][0] + h;

        // k0
        increments[0] = 0;
        for(int j = 1; j < vars; j++)
            increments[j] = 0;
        for(int j = 1; j < vars; j++) // s: modelos j:especific model r[i]:variables increments:vector
            k[j][0] = h * AuxEvaluate(s, j, r[i], increments);

        // k1
        increments[0] = h/2.0;
        for(int j = 1; j < vars; j++)
            increments[j] = k[j][0]/2.0;
        for(int j = 1; j < vars; j++)
            k[j][1] = h * AuxEvaluate(s, j, r[i], increments);

        // k2
        increments[0] = h/2.0;
        for(int j = 1; j < vars; j++)
            increments[j] = k[j][1]/2.0;
        for(int j = 1; j < vars; j++)
            k[j][2] = h * AuxEvaluate(s, j, r[i], increments);

        // k3
        increments[0] = h;
        for(int j = 1; j < vars; j++)
            increments[j] = k[j][2];
        for(int j = 1; j < vars; j++)
            k[j][3] = h * AuxEvaluate(s, j, r[i], increments);

        delete [] increments;

//        for(int m = 1; m < vars; m++){
//            cout << "   ";
//            for(int l = 0; l < 4; l++)
//                cout << k[m][l] << " ";
//            cout << endl;
//        }


        for(int j = 1; j < vars; j++){
//            cout << r[i][j] + ((k[j][0] + 2*(k[j][1] + k[j][2]) + k[j][3]))/6.0 << " ";
            r[i + 1][j] = r[i][j] + ((k[j][0] + 2*(k[j][1] + k[j][2]) + k[j][3]))/6.0;
        }
//        cout << endl;

//        for(int j = 0; j < vars; j++)
//            cout << r[i + 1][j] << " ";
//        cout << endl;

//        cout << r[i + 1][0] << " ";
//
//        for(int j = 1; j < vars; j++){ // for every fn(x) calculate
//            k[0] = Evaluate(s, j, r[i], 0, 0);
//            k[1] = Evaluate(s, j, r[i], 0.5 * h, 0.5 * k[0]);
//            k[2] = Evaluate(s, j, r[i], 0.5 * h, 0.5 * k[1]);
//            k[3] = Evaluate(s, j, r[i], h, k[2]);
//
////            cout << "    For " << j << " " << k[0] << " " << k[1] << " " << k[2] << " " << k[3] << endl;
////            cout << "K[1...4] = " << k[0] << " " << k[1] << " " << k[2] << " " << k[3] << endl;
//
//            r[i + 1][j] = r[i][j] + (h * (k[0] + 2*(k[1] + k[2]) + k[3]))/6.0;
//            cout << r[i + 1][j] << " ";
//        }
//        cout << endl;
    }

//    for(int i = 0; i < iterations; i++)
//        delete [] r[i];
//    delete [] r;
    return r;
}



double RK4sParser::AuxEvaluate(Subject* s, int model, double* dat, double* increments){
//    for(int i = 0; i < 3; i++)
//        cout << increments[i] << " e ";
//    cout << model << endl;

//    if(s->constants != NULL){
//        cout << "NÃO É NULL" << endl;
//    }
//    else{
//        cout << "É NULL" << endl;
//    }
//    cin.get();

    RK4LSIndividuo * s1 = dynamic_cast<RK4LSIndividuo*>(s);
    int coeficientCount = 0;
    stack<double> stk;
    double* inner_variable = AuxEvaluateFunctions(s, model, dat, increments);

    for(tuple<double, double> t : s->trees[model - 1]->linearModel) { // -1 because model is the n-1 tree of s and n y ou fn(x)
        int var = (int)get<0>(t);
        if( var == conf->constant ) { // constant
            stk.push(get<1>(t));
        } else if( var == conf->use_variable ) { // variable;
            double a = stk.top();
            stk.pop();
            stk.push(inner_variable[int(a)]);
        }else if( var == conf->variable ) { // variable;
            stk.push(dat[(int)get<1>(t)] + increments[(int)get<1>(t)]);
        } else if( var == conf->bynaryArithmeticOperators) { // bynaryArithmeticOperators
            double b = stk.top();
            stk.pop();
            double a = stk.top();
            stk.pop();
            stk.push(Operate(get<0>(t), get<1>(t), a, b));
        } else if( var ==  conf->unaryArithmeticOperators) { // unaryArithmeticOperators
            double a = stk.top();
            stk.pop();
            stk.push(Operate(get<0>(t), get<1>(t), a));
        } else if( var == conf->binaryLogicalOperators) { // binaryLogicalOperators
            double b = stk.top();
            stk.pop();
            double a = stk.top();
            stk.pop();
            stk.push(Operate(get<0>(t), get<1>(t), a, b));
        } else if( var == conf->unaryLogicalOperators) { // unaryLogicalOperators
            double a = stk.top();
            stk.pop();
            stk.push(Operate(get<0>(t), get<1>(t), a));
        } else if( var == conf->comparisonOperators) { // comparisonOperators
            double b = stk.top();
            stk.pop();
            double a = stk.top();
            stk.pop();
            stk.push(Operate(get<0>(t), get<1>(t), a, b));
        } else if( var == conf->programOperators) {
            //programOperators
        } else if( var == conf->adjustedCoeficient ) { // constant
//            cout << "" << coeficientCount << endl;
            if(s1->constants != NULL)
                stk.push(s1->constants[model - 1][coeficientCount]);
            else
                stk.push(1);
//            cout << "   " << stk.top() << endl;
//            cout << s1->constants[model - 1][coeficientCount] << " " << coeficientCount << endl;
            coeficientCount++;
        } else if( var == conf->hill_cube) { // comparisonOperators
            double c = stk.top();
            stk.pop();
            double b = stk.top();
            stk.pop();
            double a = stk.top();
            stk.pop();
            stk.push(Operate(get<0>(t), get<1>(t), a, b, c));
        }
    }
//    cout << "saiu" << endl;
//    cin.get();
    delete [] inner_variable;
    return stk.top();
}

double* RK4sParser::AuxEvaluateFunctions(Subject* s, int model, double* dat, double* increments){
//    for(int i = 0; i < 3; i++)
//        cout << increments[i] << " e ";
//    cout << model << endl;
    RK4LSIndividuo * s1 = dynamic_cast<RK4LSIndividuo*>(s);
    double* inner_variable = new double[conf->numFunc];

    for(int i = 0; i < conf->numFunc; i++){
        int coeficientCount = 0;
        stack<double> stk;

        for(tuple<double, double> t : s->functions[i]->linearModel) { // -1 because model is the n-1 tree of s and n y ou fn(x)
            int var = (int)get<0>(t);
            if( var == conf->constant ) { // constant
                stk.push(get<1>(t));
            } else if( var == conf->declare_variable ) { // variable;
                double b = stk.top();
                stk.pop();
                double a = stk.top();
                stk.pop();
                inner_variable[int(a)] = b;
            } else if( var == conf->use_variable ) { // variable;
                double a = stk.top();
                stk.pop();
                stk.push(inner_variable[int(a)]);
            }else if( var == conf->variable ) { // variable;
                stk.push(dat[(int)get<1>(t)] + increments[(int)get<1>(t)]);
            } else if( var == conf->bynaryArithmeticOperators) { // bynaryArithmeticOperators
                double b = stk.top();
                stk.pop();
                double a = stk.top();
                stk.pop();
                stk.push(Operate(get<0>(t), get<1>(t), a, b));
            } else if( var ==  conf->unaryArithmeticOperators) { // unaryArithmeticOperators
                double a = stk.top();
                stk.pop();
                stk.push(Operate(get<0>(t), get<1>(t), a));
            } else if( var == conf->binaryLogicalOperators) { // binaryLogicalOperators
                double b = stk.top();
                stk.pop();
                double a = stk.top();
                stk.pop();
                stk.push(Operate(get<0>(t), get<1>(t), a, b));
            } else if( var == conf->unaryLogicalOperators) { // unaryLogicalOperators
                double a = stk.top();
                stk.pop();
                stk.push(Operate(get<0>(t), get<1>(t), a));
            } else if( var == conf->comparisonOperators) { // comparisonOperators
                double b = stk.top();
                stk.pop();
                double a = stk.top();
                stk.pop();
                stk.push(Operate(get<0>(t), get<1>(t), a, b));
            } else if( var == conf->programOperators) {
                //programOperators
            } else if( var == conf->adjustedCoeficient ) { // constant
    //            cout << "   " << coeficientCount << endl;
                if(s1->constants != NULL)
                    stk.push(s1->constants[model - 1][coeficientCount]);
                else
                    stk.push(1);
    //            cout << s1->constants[model - 1][coeficientCount] << " " << coeficientCount << endl;
                coeficientCount++;
            } else if( var == conf->hill_cube) { // comparisonOperators
                double c = stk.top();
                stk.pop();
                double b = stk.top();
                stk.pop();
                double a = stk.top();
                stk.pop();
                stk.push(Operate(get<0>(t), get<1>(t), a, b, c));
            }
        }
        inner_variable[i] = stk.top();
    }//    cout << "saiu" << endl;
//    cin.get();
    return inner_variable;
}

double RK4sParser::AuxEvaluateTree(Subject* s){
    for(Tree* tree : s->trees){
//        tree->print();
//        cout << endl;
        vector<tuple<double, double> > partial;
        No* root = tree->root;
        Aux2EvaluateTree(root, &partial);

//        No* a;
//        No* b;
//
//        a = tree->subTree();
//        b = tree->subTree();
//
//        cout << "A" << endl;
//        a->print(0);
//        a->printSemantic();
//        cout << "B" << endl;
//        b->print(0);
//        b->printSemantic();
//
//        cout << a->semanticDistance(b) << endl;
//        cin.get();
    }

    return 0;
}

void RK4sParser::Aux2EvaluateTree(No* node, vector<tuple<double, double> >* partial){
    node->semantic.clear();
//    vector<string> skip = {"op", "uop"};
    if(node->type != -1){
//        cout << node->mask << " = (" << node->type << ")[" << node->value << "]" << endl;
        partial->push_back(make_tuple(node->type, node->value));
    }
    else{
        for(No* son : node->sons){
            Aux2EvaluateTree(son, partial);
        }

        for(int i = 0; i < tamDataset; i++){
            if(node->mask == "<op>" || node->mask == "<uop>"){
                node->semantic.push_back(0);
            }
            else{
                stack<double> stk;
                for(tuple<double, double> t : *partial) { // -1 because model is the n-1 tree of s and n y ou fn(x)
                    int var = (int)get<0>(t);
                    if( var == conf->constant ) { // constant
                        stk.push(get<1>(t));
                    } else if( var == conf->variable ) { // variable;
                        stk.push(dataset[i][(int)get<1>(t)]);
                    } else if( var == conf->bynaryArithmeticOperators) { // bynaryArithmeticOperators
                        double b = stk.top();
                        stk.pop();
                        double a = stk.top();
                        stk.pop();
                        stk.push(Operate(get<0>(t), get<1>(t), a, b));
                    } else if( var ==  conf->unaryArithmeticOperators) { // unaryArithmeticOperators
                        double a = stk.top();
                        stk.pop();
                        stk.push(Operate(get<0>(t), get<1>(t), a));
                    } else if( var == conf->binaryLogicalOperators) { // binaryLogicalOperators
                        double b = stk.top();
                        stk.pop();
                        double a = stk.top();
                        stk.pop();
                        stk.push(Operate(get<0>(t), get<1>(t), a, b));
                    } else if( var == conf->unaryLogicalOperators) { // unaryLogicalOperators
                        double a = stk.top();
                        stk.pop();
                        stk.push(Operate(get<0>(t), get<1>(t), a));
                    } else if( var == conf->comparisonOperators) { // comparisonOperators
                        double b = stk.top();
                        stk.pop();
                        double a = stk.top();
                        stk.pop();
                        stk.push(Operate(get<0>(t), get<1>(t), a, b));
                    } else if( var == conf->programOperators) {
                        //programOperators
                    } else if( var == conf->hill_cube) { // comparisonOperators
                        double c = stk.top();
                        stk.pop();
                        double b = stk.top();
                        stk.pop();
                        double a = stk.top();
                        stk.pop();
                        stk.push(Operate(get<0>(t), get<1>(t), a, b, c));
                    }
                }

                node->semantic.push_back(stk.top());
            }
        }

//        cout << node->mask << " = ";
//        for(tuple<double, double> t : *partial){
//            cout << "(" << get<0>(t) << ")[" << get<1>(t) << "] -> ";
//        }
//        cout << " = " << node->semantic.back() << endl;
    }

//    cout << node->height << "| semantic(" << node->mask << ") = ";
//    for(double s : node->semantic){
//        cout << s << ", ";
//    }
//    cout << endl;
}

void RK4sParser::printData(){
    for(int i = 0 ; i < tamDataset; i++){
        for(int j = 0 ; j< data->variables + data->prediction; j++){
            cout << dataset[i][j] << " ";
        }
        cout << endl;
    }
}
RK4sParser::~RK4sParser()
{
    //dtor
}
