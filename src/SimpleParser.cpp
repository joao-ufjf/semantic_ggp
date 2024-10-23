#include "SimpleParser.h"

SimpleParser::SimpleParser() {
    diffEvo = new DifferencialEvolution();
    evoStrat = new EvolutionStrategie();
}

string SimpleParser::nameParser() {
    return "SimpleParser";
}

void SimpleParser::Optimize(Subject* s){
    if(conf->approach == 1 || conf->approach == 2 || conf->approach == 9 || conf->approach == 13 || conf->approach == 14 || conf->approach == 21){
        diffEvo->Optimize(s, this);
    }else{
        evoStrat->Optimize(s, this);
    }
}

double SimpleParser::Evaluate(Subject* s) {
    //return  1;
    double r = 0;
    double fit = 0 ;
    for(int arvore = 0; arvore < conf->numTree; arvore++) {
        s->trees[arvore]->fitness = 0;
        for( int j = 0; j < tamDataset; j++) { // para todos os dados do conjunto de treinamento
            r = AuxEvaluate(s,arvore + 1,dataset[j]);

//            s->trees[arvore]->root->print();
//            cout << "Evaluate: " << j << " r=" << r << " d=" << dataset[j][data->variables + arvore] << endl;
//
//            for(int x = 0; x < data->variables + arvore; x++)
//                cout << dataset[j][x] << " ";
//            cout << " = " << r;
//            cin.get();

            if(std::isinf(r) || std::isnan(r)) {
                s->trees[arvore]->fitness = INFINITY;
                fit = INFINITY;
//                s->trees[arvore]->root->print();
//                cin.get();
                break;
            }
            fit += abs(r - dataset[j][data->variables + arvore]);
//            fit += pow(r - dataset[j][data->variables + arvore], 2);
        }
//        fit /= tamDataset;
//        fit = sqrt(fit);
        //cout << "Fit: " << fit << endl;
        s->trees[arvore]->fitness = fit;
    }
    return fit;
}

void SimpleParser::resetStringStreams(){
    integer.clear();
    integer.str("");
    integer << "";
    decimal.clear();
    decimal.str("");
    decimal << ".";
}

double SimpleParser::Operate(int opType, int opValue, double a, double b, double c) {
    double r;

    if(opType == conf->bynaryArithmeticOperators) {
//        cout << "a = " << a << " b = " << b << endl;
        if(opValue == 0)
            r = a + b;
        else if(opValue == 1)
            r = a - b;
        else if(opValue == 2)
            r = a * b;
        else if(opValue == 3){
            if(b < 0.00005){
                r = 1;
            }
            else{
                r = a / b;
            }
        }
        else if(opValue == 4)
            r = pow(a, b);
        else if(opValue == 5){
            double int_value, dec_value;
            string int_str, dec_str;
            stringstream ss;

            int_str = this->integer.str();
            dec_str = this->decimal.str();

            ss << int_str;
            ss << dec_str;

            double res;
            ss >> res;

            r = res;

            resetStringStreams();
        }
        else if(opValue == 6){
            this->integer << b;

            double i;
            this->integer >> i;

            r = i;
        }
        else if(opValue == 7){
            this->decimal << b;

            double d;
            this->decimal >> d;

            r = d;
        }
    } else if(opType == conf->unaryArithmeticOperators) {
        if(opValue == 0){
            if(a <= 0){
                r = a;
            }
            else{
                r = log(a);
            }
        }
        else if(opValue == 1)
            r = exp(a);
        else if(opValue == 2){
            if(a < 0){
                r = a;
            }
            else{
                r = sqrt(a);
            }
        }
        else if(opValue == 3)
            r = cos(a);
        else if(opValue == 4)
            r = sin(a);
        else if(opValue == 5)
            r = tan(a);
        else if(opValue == 6)
            r = acos(a);
        else if(opValue == 7)
            r = asin(a);
        else if(opValue == 8)
            r = atan(a);
        else if(opValue == 9)
            r = cosh(a);
        else if(opValue == 10)
            r = sinh(a);
        else if(opValue == 11)
            r = tanh(a);
        else if(opValue == 12)
            r = int(a);
        else if(opValue == 13){
            this->integer << a;

            double res;
            this->integer >> res;

            r = res;
        }
        else if(opValue == 14){
            this->decimal << a;

            double res;
            this->decimal >> res;

            r = res;
        }
    } else if(opType == conf->binaryLogicalOperators) {
        if(opValue == 0)
            r = a && b;
        else if(opValue == 1)
            r = a || b;
        else if(opValue == 2)
            r = bool(a) ^ bool(b);
    } else if(opType == conf->unaryLogicalOperators) {
        if(opValue == 0)
            r = !bool(a);
        else if(opValue == 1)
            r = 1;
        else if(opValue == 2)
            r = 0;
    } else if(opType == conf->comparisonOperators) {
        if(opValue == 0)
            r = a < b;
        else if(opValue == 1)
            r = a <= b;
        else if(opValue == 2)
            r = a == b;
        else if(opValue == 3)
            r = a >= b;
        else if(opValue == 4)
            r = a > b;
        else if(opValue == 5)
            r = a != b;
    } else if(opType == conf->programOperators) {
        if(opValue == 0){ // if-else
            if(c)
                r = a;
            else
                r = b;
        }
    } else if(opType == conf->hill_cube) {
        c = int(c);
        if(opValue == 0){ // hill_cube
            r = pow(a, c)/(pow(a, c) + pow(b, c));
        }
    }

//    if(std::isnan(r) || std::isinf(r)) {
//        cout << "      " << opType << " " << opValue << " " << a << " " << b << " " << r << endl;
//    }

    return r;
}

double SimpleParser::AuxEvaluate(Subject* s, int model, double* dat) {
    resetStringStreams();

    SimpleIndividuo * s1 = dynamic_cast<SimpleIndividuo*>(s);
    int coeficientCount = 0;
    stack<double> stk;
    for(tuple<double, double> t : s->trees[model - 1]->linearModel) { // -1 because model is the n-1 tree of s and n y ou fn(x)
        int var = (int)get<0>(t);

//        cout << var << " - " << conf->bynaryArithmeticOperators << " = " << var - conf->bynaryArithmeticOperators << endl;
//        cin.get();

        if( var == conf->constant ) { // constant
            stk.push(get<1>(t));
        } else if( var == conf->variable ) { // variable;
            stk.push(dat[(int)get<1>(t)]);
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
            if((int)get<1>(t) == 0){ //if-selse
                double bool_ = stk.top();
                stk.pop();
                double b = stk.top();
                stk.pop();
                double a = stk.top();
                stk.pop();
                stk.push(Operate(get<0>(t), get<1>(t), a, b, bool_));
            }
            //programOperators
        } else if( var == conf->adjustedCoeficient ) { // constant
//            cout << "   " << coeficientCount << endl;
            if(s1->constants != NULL)
                stk.push(s1->constants[model - 1][coeficientCount]);
            else
                stk.push(1);
//            cout << s1->constants[model - 1][coeficientCount] << " " << coeficientCount << endl;
            coeficientCount++;
        }

    }

    double result = stk.top();

    stk.empty();

    return result;
}

SimpleParser::~SimpleParser() {
    //dtor
}

void SimpleParser::Normalize() {
    this->norm_dataset = new double*[conf->numVar + conf->numTree + conf->numFunc];
    for(int i = 0; i < conf->numVar + conf->numTree + conf->numFunc; i++){
        norm_dataset[i] = new double[3];

        double minimum = INFINITY;
        double maximum = -INFINITY;
        for(int j = 0; j < tamDataset; j++){
            minimum = min(minimum, dataset[j][i]);
            maximum = max(maximum, dataset[j][i]);
        }

        this->norm_dataset[i][0] = minimum;
        this->norm_dataset[i][1] = maximum;
        this->norm_dataset[i][2] = maximum - minimum;
    }
}

double SimpleParser::Normalize(double value, int var) {
//    (x - min)/(max - min)
//    return (value - norm_dataset[var][0])/norm_dataset[var][2];

//    (x)/(max)
    return (value)/norm_dataset[var][1];
}
