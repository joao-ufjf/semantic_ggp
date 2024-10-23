#include "SemanticSimpleParser.h"

SemanticSimpleParser::SemanticSimpleParser()
{
    //ctor
}

SemanticSimpleParser::~SemanticSimpleParser()
{
    //dtor
}

string SemanticSimpleParser::nameParser()
{
    return "SemanticSimpleParser";
}

double SemanticSimpleParser::Evaluate(Subject* s)
{
    AuxEvaluateTree(s);

    return SimpleParser::Evaluate(s);
}

void SemanticSimpleParser::AuxEvaluateTree(Subject* s){
    for(Tree* tree : s->trees){
        vector<tuple<double, double> > partial;
        No* root = tree->root;

//        tree->root->detailed(0);
//        cin.get();

        Aux2EvaluateTree(root, &partial);
//        Aux3EvaluateTree(root);
    }
}

void SemanticSimpleParser::Aux2EvaluateTree(No* node, vector<tuple<double, double> >* partial){
    conf->node_evaluations++;
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

//        node->print(0);

        for(int i = 0; i < tamDataset; i++){
            if(node->mask == "<op>" || node->mask == "<uop>"){
                node->semantic.push_back(0);
            }
            else{
                stack<double> stk;

                this->resetStringStreams();

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
                stk.empty();
            }
        }

    }
}

vector<tuple<double, double> > SemanticSimpleParser::Aux3EvaluateTree(No* node){

//    if(node->type == -1){
//        stack<No*> nodes;
//        stack<double> stk;
//
//        nodes.push(node);
//
//        while(!nodes.empty()){
//            No* n = nodes.top();
//            nodes.pop();
//
//            if(n->type == 0){
//                for(No* son : n->sons){
//                    nodes.push(son);
//                }
//            }
//            else{
//                if( n->type == conf->constant ) { // constant
//                    stk.push(n->value);
//                } else if( n->type == conf->variable ) { // variable;
//                    stk.push(dataset[i][(int)n->value]);
//                } else if( n->type == conf->bynaryArithmeticOperators) { // bynaryArithmeticOperators
//                    double b = stk.top();
//                    stk.pop();
//                    double a = stk.top();
//                    stk.pop();
//                    stk.push(Operate(n->type, n->value, a, b));
//                } else if( n->type ==  conf->unaryArithmeticOperators) { // unaryArithmeticOperators
//                    double a = stk.top();
//                    stk.pop();
//                    stk.push(Operate(n->type, n->value, a));
//                } else if( n->type == conf->binaryLogicalOperators) { // binaryLogicalOperators
//                    double b = stk.top();
//                    stk.pop();
//                    double a = stk.top();
//                    stk.pop();
//                    stk.push(Operate(n->type, n->value, a, b));
//                } else if( n->type == conf->unaryLogicalOperators) { // unaryLogicalOperators
//                    double a = stk.top();
//                    stk.pop();
//                    stk.push(Operate(n->type, n->value, a));
//                } else if( n->type == conf->comparisonOperators) { // comparisonOperators
//                    double b = stk.top();
//                    stk.pop();
//                    double a = stk.top();
//                    stk.pop();
//                    stk.push(Operate(n->type, n->value, a, b));
//                } else if( n->type == conf->programOperators) {
//                    //programOperators
//                } else if( n->type == conf->hill_cube) { // comparisonOperators
//                    double c = stk.top();
//                    stk.pop();
//                    double b = stk.top();
//                    stk.pop();
//                    double a = stk.top();
//                    stk.pop();
//                    stk.push(Operate(n->type, n->value, a, b, c));
//                }
//            }
//        }
//    }





    vector<tuple<double, double> > partial;
//    node->semantic.clear();
//
////    vector<string> skip = {"op", "uop"};
//    if(node->type != -1){
////        cout << node->mask << " = (" << node->type << ")[" << node->value << "]" << endl;
//        partial.push_back(make_tuple(node->type, node->value));
//    }
//    else{
//        node->print(0);
//
//        vector<tuple<double, double> > linearModel;
//        node->getLinearModel(&linearModel);
//
//
////        cout << "FINAL " << endl;
////        for(tuple<double, double> t : linearModel){
////            cout << "(" << get<0>(t) << ", " << get<1>(t) << ") ";
////        }
////        cout << "---" << endl;
////
////        cin.get();
//
//        for(int i = 0; i < tamDataset; i++){
//            if(node->mask == "<op>" || node->mask == "<uop>"){
//                node->semantic.push_back(0);
//            }
//            else{
//                stack<double> stk;
//
//                this->resetStringStreams();
//
//                cout << endl << "No Semantic" << endl;
//                for(tuple<double, double> t : linearModel) { // -1 because model is the n-1 tree of s and n y ou fn(x)
//                    int var = (int)get<0>(t);
//                    if( var == conf->constant ) { // constant
//                        stk.push(get<1>(t));
//                    } else if( var == conf->variable ) { // variable;
//                        stk.push(dataset[i][(int)get<1>(t)]);
//                    } else if( var == conf->bynaryArithmeticOperators) { // bynaryArithmeticOperators
//                        double b = stk.top();
//                        stk.pop();
//                        double a = stk.top();
//                        stk.pop();
//                        stk.push(Operate(get<0>(t), get<1>(t), a, b));
//                    } else if( var ==  conf->unaryArithmeticOperators) { // unaryArithmeticOperators
//                        double a = stk.top();
//                        stk.pop();
//                        stk.push(Operate(get<0>(t), get<1>(t), a));
//                    } else if( var == conf->binaryLogicalOperators) { // binaryLogicalOperators
//                        double b = stk.top();
//                        stk.pop();
//                        double a = stk.top();
//                        stk.pop();
//                        stk.push(Operate(get<0>(t), get<1>(t), a, b));
//                    } else if( var == conf->unaryLogicalOperators) { // unaryLogicalOperators
//                        double a = stk.top();
//                        stk.pop();
//                        stk.push(Operate(get<0>(t), get<1>(t), a));
//                    } else if( var == conf->comparisonOperators) { // comparisonOperators
//                        double b = stk.top();
//                        stk.pop();
//                        double a = stk.top();
//                        stk.pop();
//                        stk.push(Operate(get<0>(t), get<1>(t), a, b));
//                    } else if( var == conf->programOperators) {
//                        //programOperators
//                    } else if( var == conf->hill_cube) { // comparisonOperators
//                        double c = stk.top();
//                        stk.pop();
//                        double b = stk.top();
//                        stk.pop();
//                        double a = stk.top();
//                        stk.pop();
//                        stk.push(Operate(get<0>(t), get<1>(t), a, b, c));
//                    }
//                }
//
//                node->semantic.push_back(stk.top());
//                cout << "Fim do semantic = " << stk.top() << endl;
//                cin.get();
//
//                stk.empty();
//            }
//        }
//        for(No* son : node->sons){
//            Aux3EvaluateTree(son);
//        }
//    }

    return partial;
}
