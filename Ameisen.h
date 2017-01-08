//
// Created by Christian on 15.10.2016.
//
#include <math.h>
#include <random>
#include <chrono>
#include <stdlib.h>
#include <time.h>
#include "enum.h"
#include <iomanip>
#ifndef OPS_AMEISEN_H
#define OPS_AMEISEN_H

struct Point2{
    Point* point;
    unsigned int index;
    Point2(Point* point, unsigned int index){
        this->point=point;
        this->index=index;
    }
};

inline std::ostream& operator<<(std::ostream& o, Point& p) { return p.print(o); }
class Ameisen {
    const int maxSolutionCount=4;
    int solutionCount=0;
    long newbest=0;
    const double beta=3;
    //const double weightchangeadd=0.05;
    const double slightlyworserange=0.01;
    const double decreasereduction=0.50;
    const double weightchangesubBase=100;
    const double weightchangesubConst=1;
    double weightchangesub;
    const long weightchangeaddBase=100;
    long weightchangeadd;
    //const long weightchangesub=5;
    double start;
    const double doubleerror= 1.00001;
    const double thresh= 100;
    const double upperthresh= 100000;
    double best;
    unsigned short dimension;
    Point** points;
    double** probability;
    double** distance;
    unsigned int pointcount;
    LogicType type;
    long cycles=0;
    std::vector<std::vector<Point2>*>* bestways = new std::vector<std::vector<Point2>*>;
    public:
    Ameisen(unsigned int pointcount,Point** points,LogicType type){
        start=pointcount*100;
        weightchangesub=weightchangesubBase;//+weightchangesubConst; /(pointcount*pointcount-pointcount)
        weightchangeadd=weightchangeaddBase*pointcount;

        if(pointcount<2)
            throw " pointcount must be more than 1 \n";
        this->type=type;
        time_t seed;
        time(&seed);
        srand (seed);
        best=0;
        this->points=points;
        dimension=points[0]->getDimension();
        this->pointcount=pointcount;
        probability= new double*[pointcount];
        switch(type){
            case Path:
            case Line:
                probability[0]=new double[pointcount];
                for(unsigned int i=1;i<pointcount;i++){
                    probability[i]=new double[pointcount];
                    probability[i][0]=0;
                    for (unsigned int j=1;j<pointcount;j++)
                        probability[i][j]=start;
                    probability[i][i]=0;
                }
                probability[0][0]=0;
                for (unsigned int i=1;i<pointcount;i++){
                    probability[0][i]=start;
                }
                break;
            case Circle:
                for(unsigned int i=0;i<pointcount;i++){
                    probability[i]=new double[pointcount];
                    for (unsigned int j=0;j<pointcount;j++)
                        probability[i][j]=start;
                    probability[i][i]=0;
                }
                break;

        }


        distance= new double*[pointcount];
        for(unsigned int i=0;i<pointcount;i++){
            distance[i]=new double[pointcount];
        }
        instanciatedistance();

    }

    double calcdistance(Point* p1,Point* p2){
        double calc=0;
        for(unsigned short i=0;i<dimension;i++){
            calc+=pow(((*p1)[i]-(*p2)[i]),2);
        }
        return pow(calc,0.5);
    }

    void instanciatedistance(){
        //TODO Parallel
        for(unsigned int i=0;i<pointcount;i++){
            distance[i][i]=0;
            for(unsigned int j=i+1;j<pointcount;j++){
                double distancec=calcdistance(points[i],points[j]);
                distance[i][j]=distancec;
                distance[j][i]=distancec;
            }
        }
    }

    std::ostream& print(std::ostream& o){
        o<<"{";
        for (unsigned int i=0;i<pointcount;i++) {
            if (!(i % (int)sqrt(pointcount))) { o << std::endl; }
            o << *(points[i]) << "  ";
        }
        o<<std::endl<<"}";
        return o;
    }

    std::ostream& printdist(std::ostream& o){
        o<<"{";
        for(unsigned int i=0;i<pointcount;i++){
            o<<std::endl;
            for(unsigned int j=0;j<pointcount;j++){
                o<<"|"<<distance[i][j]<<"|";
            }
        }
        o<<std::endl<<"}";
        return o;
    }

    std::ostream& printscore(std::ostream& o){
        //if(pointcount>10){return o<<"to large to print";}
        o<<"{";
        for(unsigned int i=0;i<pointcount;i++){
            o<<std::endl<<"P"<<i<<"    ";
            //double temp=0;
            for(unsigned int j=0;j<pointcount;j++){
                //temp+=probability[i][j];
                o<<"|"<<probability[i][j]<<"|";
            }
            // o<<"|check:"<<temp;
        }
        o<<std::endl<<"}";
        o<<std::endl<<"new realy better ways:"<<newbest<<std::endl;
        o<<std::endl<<"cyclecount:"<<cycles<<std::endl;
        o<<std::endl<<"best:"<<best<<std::endl;
        return o;
    }

    std::ostream& printprob(std::ostream& o){
        //if(pointcount>10){return o<<"to large to print";}
        o<<"{";
        for(unsigned int i=0;i<pointcount;i++){
            o<<std::endl<<"P"<<i<<"    ";
            double sum=0;
            for(unsigned int j=0;j<pointcount;j++){
                sum+=probability[i][j];
            }
            //double temp=0;
            for(unsigned int j=0;j<pointcount;j++){
                //temp+=probability[i][j];
                o<<"|"<< std::setprecision(2) << std::fixed <<probability[i][j]/sum*100<<"%|";
            }
            // o<<"|check:"<<temp;
        }
        o<<std::endl<<"}";
        o<<std::endl<<"new realy better ways:"<<newbest<<std::endl;
        o<<std::endl<<"cyclecount:"<<cycles<<std::endl;
        o<<std::endl<<"best:"<<best<<std::endl;
        return o;
    }

    void calculate(int n){
        newbest=0;
        cycles+=n;
        switch (type){
            case Line:
                for(int i=0; i<n ; ++i)
                    calcLine();
                break;
            case Circle:
                for(int i=0; i<n ; ++i)
                    calcCircle();
                break;
            case Path:
              //  distance[0][6]=20000;
                for(int i=0; i<n ; ++i)
                    calcPath();
                break;
        }

    }

    ~Ameisen(){

        for(unsigned int i=0;i<pointcount;i++){
            delete[] probability[i];
        }
        delete[] probability;
        for(unsigned int i=0;i<pointcount;i++){
            delete[] distance[i];
        }
        delete[] distance;
        //TODO delete vectordings
        //delete bestway;
    }

private:
    void calcLine(){

        std::vector<Point2>* pin= new std::vector<Point2>();
        std::vector<Point2>* pout= new std::vector<Point2>();
        for(unsigned int i=1;i<pointcount;i++){
            pin->push_back(Point2((points[i]),i));
        }
        pout->push_back(Point2((points[0]),0));
        unsigned int i;
        unsigned int j;
        double top;
        double bot;
        unsigned int randi;
        i=(*pout)[0].index;
/*
        std::cout<<std::endl<<"pin:";
        for(int i=0;i<pointcount-1;i++){
            std::cout<<(*pin)[i].index<<"|";
        }
        std::cout<<std::endl;

*/
        do{
            do{
                if(pin->size()==1){randi=0; break;}
                randi=rand()%(pin->size());
                j=(*pin)[randi].index;
                top=probability[i][j]*pow((1/distance[i][j]),beta);
                bot=0;

                for(unsigned int k=0;k<pin->size();k++){
                        bot += probability[i][(*pin)[k].index] * pow((1 / distance[i][(*pin)[k].index]), beta);
                    // achtung! if (!bot)return;
                }

            }while(((double) rand() / (RAND_MAX))>(top/bot));
            i=(*pin)[randi].index;
            pout->push_back((*pin)[randi]);
            pin->erase(pin->begin()+randi);
            /*
            if(pin->size()==1){
                pout->push_back((*pin)[0]);
                pin->erase(pin->begin());
            }
             */
/*
            std::cout<<std::endl<<"pin:";
            for(int i=0;i<pin->size();i++){
                std::cout<<(*pin)[i].index<<"|";
            }
            std::cout<<std::endl;

            std::cout<<std::endl<<"pout:";
            for(int i=0;i<pout->size();i++){
                std::cout<<(*pout)[i].index<<"|";
            }
            std::cout<<std::endl;
*/
        }
        while(0<pin->size());



/*
        std::cout<<std::endl<<"pout:";
        for(int i=0;i<pointcount;i++){
            std::cout<<(*pout)[i].index<<"|";
        }
        std::cout<<std::endl;
*/

        double summe=0;
        for(unsigned int l=1;l<pointcount;l++){
            summe+=distance[(*pout)[l-1].index][(*pout)[l].index];
        }
        //summe+=distance[(*pout)[0].index][(*pout)[pointcount-1].index];
        //std::cout<<summe<<std::endl;
        if(!best||(best*doubleerror)>=summe){
            best=summe;
            //std::cout<<"improvement"<<std::endl;
            for(unsigned int l=1;l<pointcount;l++){
                unsigned int cur=(*pout)[l-1].index;
                unsigned int next=(*pout)[l].index;
                //probability[cur][next]*=1+weightchangeadd;
                probability[cur][next]+=weightchangeadd;

            }

        }
        else{


            for(unsigned int l=1;l<pointcount;l++){
                unsigned int cur=(*pout)[l-1].index;
                unsigned int next=(*pout)[l].index;
                /*
                if(probability[cur][next]/(1+weightchangesub)>thresh){
                    probability[cur][next]/=(1+weightchangesub);
                }
                */
                //TODO check
                if(probability[cur][next]-weightchangesub>thresh){probability[cur][next]-=weightchangesub;}



            }

        }

    }

    void calcCircle(){

        std::vector<Point2>* pin= new std::vector<Point2>();
        std::vector<Point2>* pout= new std::vector<Point2>();
        for(unsigned int i=0;i<pointcount;i++){
            pin->push_back(Point2((points[i]),i));
        }
        int randfirst = rand()%(pin->size());
        pout->push_back((*pin)[randfirst]);
        pin->erase(pin->begin()+randfirst);
        unsigned int i;
        unsigned int j;
        double top;
        double bot;
        unsigned int randi;
        i=(*pout)[0].index;
/*
        std::cout<<std::endl<<"pin:";
        for(int i=0;i<pointcount-1;i++){
            std::cout<<(*pin)[i].index<<"|";
        }
        std::cout<<std::endl;

*/
        do{
            do{
                if(pin->size()==1){randi=0; break;}
                randi=rand()%(pin->size());
                j=(*pin)[randi].index;
                top=probability[i][j]*pow((1/distance[i][j]),beta);
                bot=0;

                for(unsigned int k=0;k<pin->size();k++){
                        bot += probability[i][(*pin)[k].index] * pow((1 / distance[i][(*pin)[k].index]), beta);
                }

            }while(((double) rand() / (RAND_MAX))>(top/bot));
            i=(*pin)[randi].index;
            pout->push_back((*pin)[randi]);
            pin->erase(pin->begin()+randi);
            /*
            if(pin->size()==1){
                pout->push_back((*pin)[0]);
                pin->erase(pin->begin());
            }
             */
/*
            std::cout<<std::endl<<"pin:";
            for(int i=0;i<pin->size();i++){
                std::cout<<(*pin)[i].index<<"|";
            }
            std::cout<<std::endl;

            std::cout<<std::endl<<"pout:";
            for(int i=0;i<pout->size();i++){
                std::cout<<(*pout)[i].index<<"|";
            }
            std::cout<<std::endl;
*/
        }
        while(0<pin->size());
        pout->push_back((*pout)[0]);

/*
        std::cout<<std::endl<<"pout:";
        for(int i=0;i<pointcount+1;i++){
            std::cout<<(*pout)[i].index<<"|";
        }
        std::cout<<std::endl;
*/

        double summe=0;
        for(unsigned int l=1;l<pointcount+1;l++){
            summe+=distance[(*pout)[l-1].index][(*pout)[l].index];
        }
        //summe+=distance[(*pout)[0].index][(*pout)[pointcount-1].index];
        //std::cout<<summe<<std::endl;
        if(!best||(best*doubleerror)>=summe){
            bool bestbest=false;
            //todo bei anderen
        if(summe*doubleerror<best){bestbest=true;newbest++;}
            best=summe;
            //std::cout<<"improvement"<<std::endl;
            for(unsigned int l=1;l<pointcount+1;l++){
                unsigned int cur = (*pout)[l - 1].index;
                unsigned int next = (*pout)[l].index;
                if(bestbest){
                    double temp=0;
                    //TODO array?
                    for(unsigned int i=0;i<pointcount;i++) {
                        if (probability[cur][i] > temp)
                            temp = probability[cur][i];

                    }
                    probability[cur][next] = temp;
                    //todo to other functions

                }

                    //probability[cur][next]*=1+weightchangeadd;
                    probability[cur][next] += weightchangeadd;
                //todo further testing
                    if (probability[cur][next]>upperthresh){probability[cur][next]=upperthresh;}
            }
            if(!bestbest){delete pout;}
            else{pushSolution(pout);}
            delete pin;

        }
        else{


            for(unsigned int l=1;l<pointcount;l++){
                unsigned int cur=(*pout)[l-1].index;
                unsigned int next=(*pout)[l].index;
                /*
                if(probability[cur][next]/(1+weightchangesub)>thresh){
                    probability[cur][next]/=(1+weightchangesub);
                }
                 */
                double reduction=weightchangesub;
                //todo do we realy want this?
                if(summe>best/(1+slightlyworserange)){reduction-=reduction*decreasereduction;}
                //TODO check
                if(probability[cur][next]-reduction>thresh)
                {
                    probability[cur][next]-=reduction;
                }
            }
            delete pout;
            delete pin;

        }

    }

    void calcPath(){

        std::vector<Point2>* pin= new std::vector<Point2>();
        std::vector<Point2>* pout= new std::vector<Point2>();
        for(unsigned int i=1;i<pointcount;i++){
            pin->push_back(Point2((points[i]),i));
        }
        pout->push_back(Point2((points[0]),0));
        unsigned int i;
        unsigned int j;
        double top;
        double bot;
        double summe=0;
        unsigned int randi;
        unsigned int bound;
        i=(*pout)[0].index;
/*
        std::cout<<std::endl<<"pin:";
        for(int i=0;i<pointcount-1;i++){
            std::cout<<(*pin)[i].index<<"|";
        }
        std::cout<<std::endl;

*/
        do{
            do{
                if(pin->size()==1){randi=0; break;}
                randi=rand()%(pin->size());
                j=(*pin)[randi].index;
                top=probability[i][j]*pow((1/distance[i][j]),beta);
                bot=0;

                for(unsigned int k=0;k<pin->size();k++){
                    bot += probability[i][(*pin)[k].index] * pow((1 / distance[i][(*pin)[k].index]), beta);
                }
                if (!bot){
                    bound = pout->size();
                    goto oneway;
                }
            }while(((double) rand() / (RAND_MAX))>(top/bot));
            i=(*pin)[randi].index;
            pout->push_back((*pin)[randi]);
            if(i==pointcount-1)
                break;
            pin->erase(pin->begin()+randi);
            /*
            if(pin->size()==1){
                pout->push_back((*pin)[0]);
                pin->erase(pin->begin());
            }
             */
/*
            std::cout<<std::endl<<"pin:";
            for(int i=0;i<pin->size();i++){
                std::cout<<(*pin)[i].index<<"|";
            }
            std::cout<<std::endl;

            std::cout<<std::endl<<"pout:";
            for(int i=0;i<pout->size();i++){
                std::cout<<(*pout)[i].index<<"|";
            }
            std::cout<<std::endl;
*/
        }
        while(0<pin->size());
        //pout->push_back((*pout)[0]);

/*
        std::cout<<std::endl<<"pout:";
        for(int i=0;i<pointcount+1;i++){
            std::cout<<(*pout)[i].index<<"|";
        }
        std::cout<<std::endl;
*/
        bound = pout->size();
        for(unsigned int l=1;l<bound;l++){
            summe+=distance[(*pout)[l-1].index][(*pout)[l].index];
        }
        //summe+=distance[(*pout)[0].index][(*pout)[pointcount-1].index];
        //std::cout<<summe<<std::endl;
        if(!best||(best*doubleerror)>=summe){
            bool bestbest=false;
            //todo bei anderen
            if(summe*doubleerror<best){bestbest=true;newbest++;}
            best=summe;
            //std::cout<<"improvement"<<std::endl;
            for(unsigned int l=1;l<bound;l++){
                unsigned int cur = (*pout)[l - 1].index;
                unsigned int next = (*pout)[l].index;
                if(bestbest){
                    double temp=0;
                    //TODO array?
                    for(unsigned int i=0;i<pointcount;i++) {
                        if (probability[cur][i] > temp)
                            temp = probability[cur][i];

                    }
                    probability[cur][next] = temp;
                    //todo to other functions
                }

                //probability[cur][next]*=1+weightchangeadd;
                probability[cur][next] += weightchangeadd;
                //todo further testing
                if (probability[cur][next]>upperthresh){probability[cur][next]=upperthresh;}
            }
            if(!bestbest){delete pout;}
            else{pushSolution(pout);}
            delete pin;

        }
        else{

            oneway:
            for(unsigned int l=1;l<bound;l++){
                unsigned int cur=(*pout)[l-1].index;
                unsigned int next=(*pout)[l].index;
                /*
                if(probability[cur][next]/(1+weightchangesub)>thresh){
                    probability[cur][next]/=(1+weightchangesub);
                }
                 */
                double reduction=weightchangesub;
                //todo do we realy want this?
                if(summe>best/(1+slightlyworserange)){reduction-=reduction*decreasereduction;}
                //TODO check
                if(probability[cur][next]-reduction>thresh)
                {
                    probability[cur][next]-=reduction;
                }
            }
            delete pout;
            delete pin;

        }

    }

    void pushSolution(std::vector<Point2>* solution){
        bestways->insert(bestways->begin(),solution);
        if(solutionCount>=maxSolutionCount){
            delete ((*bestways)[maxSolutionCount-1]);
            bestways->erase(bestways->begin()+ maxSolutionCount-1);
        }
        else{solutionCount++;}
    }
};


#endif //OPS_AMEISEN_H
