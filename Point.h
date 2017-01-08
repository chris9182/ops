//
// Created by Christian on 15.10.2016.
//
#ifndef OPS_POINT_H
#define OPS_POINT_H
#define DEBUG

class Point {
    double *coordinates;
    unsigned short dimension;
public:
    Point(unsigned short dimension, double *coordinates){
        this->dimension=dimension;
        this->coordinates= coordinates;
    }
    std::ostream& print(std::ostream& o){
        o<<"|";
        for (unsigned short i=0;i<dimension;i++)
        o<<coordinates[i]<<",";
        o<<"|";
        return o;
    }
    double *getCoordinates(){return coordinates;}
    unsigned short getDimension(){return dimension;}
    double getCoordinate(int dim){return coordinates[dim];}
    ~Point(){
         delete[] coordinates;
    }
    double operator[](unsigned short position){

#ifdef DEBUG
        if(position<dimension)
#endif
        return this->coordinates[position];
#ifdef DEBUG
        else throw "Illegal Position Request";
#endif
    }
};


#endif //OPS_POINT_H
