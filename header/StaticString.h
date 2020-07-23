//
// Created by berg on 23/07/2020.
//

#ifndef BEVA_STATICSTRING_H
#define BEVA_STATICSTRING_H


#include <string>
#include <string.h>
#include <vector>
#include <iostream>

using namespace std;

typedef unsigned integerPointer;


extern integerPointer  currFreePosition;
extern integerPointer dataEnd;
extern char *_data_static;

class StaticString {
public:
    // one value per object
    integerPointer  dataPos;
    unsigned stringSize;

    inline string substr(unsigned inicio, unsigned length) {
        string tmp;
        if (inicio+length <=stringSize) {
            tmp.reserve(length+1);
            tmp =strncpy((char *)(tmp.c_str()),_data_static+dataPos+inicio,length);
        } else {
            if (inicio < stringSize) {
                tmp.reserve(length+1);
                strncpy((char *)(tmp.c_str()),_data_static+dataPos+inicio,length);
            } else {
                tmp="";
            }
        }
        return tmp;
    }

    inline unsigned length() { return stringSize;}
    inline char operator[](unsigned pos) { return *(_data_static+dataPos+pos); }
    inline static void setDataBaseMemory(char *ptr,unsigned size) { _data_static = ptr; dataEnd=size; currFreePosition= 0;}

    StaticString(string s) {
        insertNewString(s);
    }

    inline  char *c_str(){ return _data_static+dataPos; }


    void insertNewString(string s) {
        dataPos = currFreePosition;
        if (dataPos < dataEnd) {
            stringSize = s.length();
            strcpy(_data_static+dataPos,s.c_str());
            currFreePosition += s.length()+1;
        } else {
            cout << "ERRO DE INSERCAO NO DATASET EM MEMORIA\n" << endl;
            exit(1);
        }
    }
};


#endif //BEVA_STATICSTRING_H
