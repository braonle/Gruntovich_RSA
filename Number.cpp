#include "Number.h"

using namespace LongNumber;

Number::~Number()
{
    if (!_tmp && !_num)
        delete _num;
}

Number::Number(const Number &cp)
{
    if (cp._tmp)
    {
        this->_num = cp._num;
        const_cast<Number&>(cp)._num = NULL;
    }
    else
    {
        this->_num = new TWORD[cp._size];
        memcpy(this->_num, cp._num, cp._size * sizeof(TWORD));
    }
    this->_size = cp._size;
}

Number Number::operator+(Number &add) {
    USHORT size = MAX(_size, add._size) + (USHORT)1;
    TWORD *ptr = new TWORD[size];
    TWORD *extra = NULL;
    TDWORD tmp = 0;
    USHORT i;

    if (_size > add._size)
        extra = _num;
    else if (add._size > _size && i < add._size)
        extra = add._num;

    for (i = 0; i < size - 1; ++i)
        if (i < MIN(_size, add._size))
        {
            tmp = (TDWORD)this->_num[i] + add._num[i] + HIWORD(tmp);
            ptr[i] = LOWORD(tmp);
        }
        else
        {
            tmp = (TDWORD) extra[i] + HIWORD(tmp);
            ptr[i] = LOWORD(tmp);
        }

    if (HIWORD(tmp))
        ptr[size - 1] = HIWORD(tmp);
    else
        ptr = (TWORD *) realloc(ptr, --size);

    return Number(ptr, size, true);
}

Number& Number::operator=(const Number &assign)
{
    if (!this->_num)
        delete _num;

    if (assign._tmp)
    {
        this->_num = assign._num;
        const_cast<Number&>(assign)._num = NULL;
    }
    else
    {
        this->_num = new TWORD[assign._size];
        memcpy(this->_num, assign._num, assign._size * sizeof(TWORD));
    }
    this->_size = assign._size;

    return *this;
}

USHORT Number::getContent(TWORD **dest)
{
    *dest = this->_num;
    return this->_size;
}