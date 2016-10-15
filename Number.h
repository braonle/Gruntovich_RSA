#ifndef __NUMBER__
#define __NUMBER__

#include <cstddef>
#include <pth.h>
#include <cstring>
#include <malloc.h>

#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a>b)?b:a)
#define LOWORD(a) ((TWORD)a)
#define HIWORD(a) ((TWORD)(a>>(8*sizeof(TWORD))))

namespace LongNumber
{
    typedef unsigned int TWORD;
    typedef unsigned long TDWORD;
    typedef unsigned short USHORT;

    class Number
    {
    protected:
        TWORD *_num;
        USHORT _size;
        bool _tmp = false;
        Number(TWORD *number, USHORT size, bool tmp): _num(number), _size(size), _tmp(tmp){};
    public:
        Number(TWORD *number, USHORT size): _num(number), _size(size){};
        Number(const Number& cp);
        Number operator+ (Number &add);
        Number& operator= (const Number &assign);
        USHORT getContent(TWORD** dest);
        ~Number();
    };
}

#endif