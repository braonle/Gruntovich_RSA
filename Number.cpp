/**
 * Created by Yaroslav Bondarenko
 */

#include "Number.h"

using namespace LongNumber;

Number::~Number()
{
    if (_num)
        delete _num;
}

Number Number::operator+(const Number &add)
{
    std::vector<TWORD> *ptr;
    bool sign = _sign;

    ptr = (_tmp) ? _num : (add._tmp) ? add._num : NULL;

    ptr = __add_sign_op(ptr, add, sign);

    if (_tmp)
        _num = NULL;
    else if (add._tmp)
        const_cast<Number&>(add)._num = NULL;

    return Number (ptr, sign, true);
}

Number Number::operator-(const Number &sub)
{
    std::vector<TWORD> *ptr;
    bool sign = _sign;

    ptr = (_tmp) ? _num : (sub._tmp) ? sub._num : NULL;

    ptr = __sub_sign_op(ptr, sub, sign);

    if (_tmp)
        _num = NULL;
    else if (sub._tmp)
        const_cast<Number&>(sub)._num = NULL;


    return Number(ptr, sign, true);
}

Number Number::operator*(const Number &mul)
{
    return Number(__mul(*_num, *mul._num),
                  !(_sign ^ mul._sign), true);
}

Number Number::operator* (TWORD mul)
{
    return Number(__mul(*_num, mul, NULL),
                  !(_sign ^ (mul >= 0)), true);
}

Number& Number::operator-=(const Number &sub)
{
    __sub_sign_op(_num, sub, _sign);
    return *this;
}

Number Number::operator/ (const Number &div)
{
    std::vector<TWORD> *ptr, *rm;
    bool sign = !(_sign ^ div._sign);

    switch(modcmp(*this, div))
    {
        case 0:
            if (_tmp)
            {
                ptr = _num;
                _num = NULL;
                ptr->clear();
            }
            else if (div._tmp)
            {
                ptr = div._num;
                const_cast<Number&>(div)._num = NULL;
                ptr->clear();
            }
            else
                ptr = new std::vector<TWORD>;

            ptr->push_back(1);
            break;
        case -1:
            if (_tmp)
            {
                ptr = _num;
                _num = NULL;
                ptr->clear();
            }
            else if (div._tmp)
            {
                ptr = div._num;
                const_cast<Number&>(div)._num = NULL;
                ptr->clear();
            }
            else
                ptr = new std::vector<TWORD>;

            ptr->push_back(0);
            break;
        default:
            ptr = __div(*_num, *div._num, &rm);
            if (!sign && (rm->size() > 1 || (*rm)[0] != 0))
            {
                std::vector<TWORD> a;
                a.push_back(1);
                __add(ptr, *ptr, a);
            }
            delete rm;
    }
    return Number(ptr, sign, true);
}

Number Number::operator% (const Number &div)
{
    std::vector<TWORD> *ptr, *tmp;
    switch(modcmp(*this, div))
    {
        case 0:
            ptr = new std::vector<TWORD>;
            ptr->push_back(0);
            break;
        case -1:
            if (_tmp)
            {
                ptr = _num;
                _num = NULL;
            }
            else
            {
                ptr = new std::vector<TWORD>;
                std::copy(_num->begin(), _num->end(), ptr->begin());
            }
            break;
        default:
            tmp = __div(*_num, *div._num, &ptr);
            delete tmp;
            if (_sign ^ div._sign && (ptr->size() > 1 || (*ptr)[0] != 0))
                __sub(ptr, *div._num, *ptr);
    }
    return Number(ptr, true, true);
}

Number& Number::operator+= (const Number &add)
{
    __add_sign_op(_num, add, _sign);
    return *this;
}

Number& Number::operator*=(const Number &mul)
{
    std::vector<TWORD> *ptr = __mul(*_num, *mul._num);
    _sign = !(_sign ^ mul._sign);
    delete _num;
    _num = ptr;
    return *this;
}

Number& Number::operator*= (TWORD mul)
{
    __mul(*_num, mul, _num);
    _sign = !(_sign ^ (mul >= 0));
    return *this;
}

Number& Number::operator/= (const Number &div)
{
    std::vector<TWORD> *ptr, *rm;
    _sign = !(_sign ^ div._sign);
    switch(modcmp(*this, div))
    {
        case 0:
            _num->clear();
            _num->push_back(1);
            break;
        case -1:
            _num->clear();
            _num->push_back(0);
            break;
        default:
            ptr = __div(*_num, *div._num, &rm);
            if (!_sign && (rm->size() > 1 || (*rm)[0] != 0))
            {
                std::vector<TWORD> a;
                a.push_back(1);
                __add(ptr, *ptr, a);
            }
            delete _num;
            delete rm;
            _num = ptr;
    }

    return *this;
}

Number& Number::operator%= (const Number &div)
{
    std::vector<TWORD> *ptr, *tmp;
    _sign = true;
    switch(modcmp(*this, div))
    {
        case 0:
            _num->clear();
            _num->push_back(0);
            break;
        case 1:
            tmp = __div(*_num, *div._num, &ptr);
            delete tmp;
            if (_sign ^ div._sign && (ptr->size() > 1 || (*ptr)[0] != 0))
                __sub(ptr, *div._num, *ptr);
            delete _num;
            _num = ptr;
    }
    return *this;
}

bool Number::operator<=(const Number &arg) const
{
    return !operator>(arg);
}

bool Number::operator>=(const Number &arg) const
{
    return !operator<(arg);
}

bool Number::operator>(const Number &arg) const
{
    if (_sign && !arg._sign)
        return true;
    if (!_sign && arg._sign)
        return false;
    return modcmp(*this, arg) == 1 && _sign;
}

bool Number::operator<(const Number &arg) const
{
    if (_sign && !arg._sign)
        return false;
    if (!_sign && arg._sign)
        return true;
    return modcmp(*this, arg) == -1 && _sign;
}

bool Number::operator==(const Number &arg) const
{
    if (_num->size() != arg._num->size() || _sign != arg._sign)
        return false;
    return *this->_num == *arg._num;
}

bool Number::operator!= (const Number &arg) const
{
    return !operator==(arg);
}

Number& Number::operator=(const Number &assign)
{
    // Retrieving
    if (assign._tmp)
    {
        if (this->_num)
            delete _num;
        this->_num = assign._num;
        const_cast<Number&>(assign)._num = NULL;
    }
        // Copying
    else
        *_num = *assign._num;

    this->_sign = assign._sign;

    return *this;
}

void Number::negate(void)
{
    _sign = !_sign;
}

std::ostream& LongNumber::operator<<(std::ostream &out, const Number& src)
{
    out << ((src._sign) ? "0x" : "-0x")
        << std::setfill('0')
        << std::hex
        << std::uppercase;

    for (unsigned long i = src._num->size(); i > 0; --i)
        out << std::setw(sizeof(TWORD) * 2) << (*src._num)[i - 1];

    out << std::setfill(' ')
        << std::setw(1)
        << std::dec
        << std::nouppercase;
    return out;
}

unsigned long Number::getSize(void) const
{
    return _num->size();
}

bool Number::getSign(void) const
{
    return _sign;
}

inline std::vector<TWORD>* Number::__add_sign_op(std::vector<TWORD> *ptr, const Number &add, bool &sign)
{
    // Signs are equal -> addition
    if (_sign && add._sign || !_sign && !add._sign)
    {
        ptr = __add(ptr, *_num, *add._num);
        sign = _sign;
    }
    else
    {
        int res = modcmp(*this, add);
        // Trivial result
        if (res == 0)
        {
            if (!ptr)
                ptr = new std::vector<TWORD>;
            ptr->clear();
            ptr->push_back(0);
            _sign = true;
        }
        else if (res == -1)
        {
            sign = add._sign;
            ptr = __sub(ptr, *add._num, *_num);
        }
        else
            ptr = __sub(ptr, *_num, *add._num);
    }

    return ptr;
}

inline std::vector<TWORD>* Number::__sub_sign_op(std::vector<TWORD> *ptr, const Number &sub, bool &sign)
{
    // Sign combination is equal to addition
    if (_sign && !sub._sign || !_sign && sub._sign)
    {
        ptr = __add(ptr, *_num, *sub._num);
        sign = _sign;
    }
    else
    {
        int res = modcmp(*this, sub);
        // Trivial result
        if (res == 0)
        {
            if (!ptr)
                ptr = new std::vector<TWORD>;
            ptr->clear();
            ptr->push_back(0);
        }
        else if (res == -1)
        {
            sign = !sub._sign;
            ptr = __sub(ptr, *sub._num, *_num);
        }
        else
            ptr = __sub(ptr, *_num, *sub._num);
    }

    return ptr;
}

std::vector<TWORD>* Number::__add(std::vector<TWORD> *res,
                                  const std::vector<TWORD> &a, const std::vector<TWORD> &b)
{
    std::vector<TWORD> *ret = (res == NULL) ? (new std::vector<TWORD>) : res;
    const std::vector<TWORD> &extra = (a.size() > b.size()) ? a : b;
    TDWORD tmp = 0;
    unsigned long min = MIN(a.size(), b.size());
    unsigned long max = MAX(a.size(), b.size());
    unsigned long size = ret->size();
    unsigned long i = 0;

    for (; i < min && i < size; ++i)
    {
        tmp = (TDWORD) a[i] + b[i] + HIWORD(tmp);
        (*ret)[i] = LOWORD(tmp);
    }
    for (; i < min && i >= size; ++i)
    {
        tmp = (TDWORD) a[i] + b[i] + HIWORD(tmp);
        ret->push_back(LOWORD(tmp));
    }

    i = min;
    for (; i < max && i < size; ++i)
    {
        tmp = (TDWORD) extra[i] + HIWORD(tmp);
        (*ret)[i] = LOWORD(tmp);
    }
    for (; i < max && i >= size; ++i)
    {
        tmp = (TDWORD) extra[i] + HIWORD(tmp);
        ret->push_back(LOWORD(tmp));
    }

    if (HIWORD(tmp))
        ret->push_back(HIWORD(tmp));

    return ret;
}

std::vector<TWORD>* Number::__sub(std::vector<TWORD> *res,
                                  const std::vector<TWORD> &a, const std::vector<TWORD> &b)
{
    std::vector<TWORD> *ret = (res == NULL) ? (new std::vector<TWORD>) : res;
    unsigned short cf = 0;
    unsigned long count = 0;
    unsigned long min = MIN(a.size(), b.size());
    unsigned long max = MAX(a.size(), b.size());
    unsigned long size = ret->size();
    TWORD tmp;
    unsigned long i = 0;

    //a and b intercept eah other
    for (; i < min && i < size; ++i)
        if (a[i] < (tmp = b[i] + (TDWORD) cf))
        {
            (*ret)[i] = (TDWORD) (1 << (sizeof(TWORD) * 8)) + a[i] - tmp;
            cf = 1;
        } else
        {
            cf = 0;
            (*ret)[i] = a[i] - tmp;
        }
    for (; i < min && i >= size; ++i)
        if (a[i] < (tmp = b[i] + (TDWORD) cf))
        {
            ret->push_back((TDWORD) (1 << (sizeof(TWORD) * 8)) + a[i] - tmp);
            cf = 1;
        } else
        {
            cf = 0;
            ret->push_back(a[i] - tmp);
        }

    //Dealing with carry until disappears
    i = min;
    for (; i < max && cf && i < size; ++i)
        if (a[i] >= cf)
        {
            (*ret)[i] = a[i] - cf;
            cf = 0;
        }
        else
            (*ret)[i] = a[i] + (TDWORD)(1 << (sizeof(TWORD) * 8)) - cf;
    for (; i < max && cf && i >= size; ++i)
        if (a[i] >= cf)
        {
            ret->push_back(a[i] - cf);
            cf = 0;
        }
        else
            ret->push_back(a[i] + (TDWORD)(1 << (sizeof(TWORD) * 8)) - cf);

    //Copy the remaining contents
    for (; i < max && i < size; ++i)
            (*ret)[i] = a[i];
    for (; i < max && i >= size; ++i)
            ret->push_back(a[i]);

    // Removal of leading zeros
    for (i = ret->size(); i > 0; --i)
        if (!ret->at(i - 1))
            ++count;
        else
            i = 1;
    if (count)
        ret->resize(ret->size() - count);

    return ret;
}

int Number::modcmp(const Number &a, const Number &b)
{
    if (a._num->size() != b._num->size())
        return (a._num->size() > b._num->size()) ? 1 : -1;
    for (unsigned long i = a._num->size(); i > 0; i--)
        if (a._num->at(i - 1) > b._num->at(i - 1))
            return 1;
        else if (a._num->at(i - 1) < b._num->at(i - 1))
            return -1;
    return 0;
}

std::vector<TWORD>* Number::__mul(const std::vector<TWORD> &a, const std::vector<TWORD> &b)
{
    std::vector<TWORD> *ret = new std::vector<TWORD>;
    TDWORD tmp;

    for (int i = 0; i < a.size(); ++i)
    {
        tmp = 0;
        for (int j = 0; j < b.size(); ++j)
        {
            tmp = (TDWORD) a[i] * b[j] + HIWORD(tmp);
            if (i + j < ret->size())
            {
                // ret += not allowed <- TWORD overflow
                tmp += (*ret)[i+j];
                (*ret)[i + j] = LOWORD(tmp);

            }
            else
                ret->push_back(LOWORD(tmp));
        }

        if (HIWORD(tmp))
            if (i + b.size() < ret->size())
            {
                tmp += (*ret)[i + b.size()];
                (*ret)[i + b.size()] = LOWORD(tmp);
                if (HIWORD(tmp))
                    ret->push_back(1);
            }
            else
                ret->push_back(HIWORD(tmp));
    }

    return ret;
}

std::vector<TWORD>* Number::__mul(const std::vector<TWORD> &a, const TWORD b, std::vector<TWORD> *res)
{
    std::vector<TWORD> *ret = (!res) ? (new std::vector<TWORD>) : res;
    TDWORD tmp = 0;
    int i = 0;
    unsigned long edge = ret->size();
    unsigned long size = a.size();

    for (i = 0; i < size && i < edge; ++i)
    {
        tmp = (TDWORD) a[i] * b + HIWORD(tmp);
        (*ret)[i] = LOWORD(tmp);
    }

    for (; i < size; ++i)
    {
        tmp = (TDWORD) a[i] * b + HIWORD(tmp);
        ret->push_back(LOWORD(tmp));
    }

    if (HIWORD(tmp))
        if (i < edge)
            (*ret)[i] = HIWORD(tmp);
        else
            ret->push_back(HIWORD(tmp));

    return ret;
}

std::vector<TWORD>* Number::__div(std::vector<TWORD> &A, std::vector<TWORD> &B,
                                            std::vector<TWORD> **rem)
{
    std::vector<TWORD> *ret = new std::vector<TWORD>;
    long ptr = A.size() - 1;
    Number a(new std::vector<TWORD>, true, false), b(&B,true, false), rm (NULL, true, false);
    TWORD cmpvar = A[A.size() - 1];
    TWORD cmp = B[B.size() - 1];

    auto construct = [&] (int s) -> void
    {
        long sz2 = (rm._num) ? rm._num->size() : 0;
        long sz1 = B.size() + s - 1 - sz2;

        sz1 = (ptr < sz1) ? ptr : sz1;

        a._num->clear();
        for (long i = sz1; i >= 0 ; --i)
            a._num->push_back(A[ptr - i]);
        for (long i = 0; i < sz2; ++i)
            a._num->push_back((*rm._num)[i]);
        ptr -= sz1 + 1;
    };

    while (ptr >= 0)
    {
        construct(cmpvar < cmp ? 1 : 0);

        TDWORD lo = 0, hi = ((TDWORD)1 << (sizeof(TWORD) * 8)) - 1;
        TWORD x = 0;

        while (lo <= hi)
        {
            TWORD tmp = (lo + hi) >> 1;
            if (a >= b * tmp)
            {
                x = tmp;
                lo = tmp + 1;
            }
            else
                hi = tmp - 1;
        }

        rm = a - b * x;
        ret->push_back(x);
        cmpvar = (*rm._num)[rm._num->size() - 1];
    }

    if (rem)
    {
        *rem = rm._num;
        rm._num = NULL;
    }
    b._num = NULL;

    ptr = ret->size() - 1;
    long size = ret->size() / 2 - 1;
    for (long i = 0; i <= size; ++i)
    {
        cmp = (*ret)[i];
        (*ret)[i] = (*ret)[ptr - i];
        (*ret)[ptr - i] = cmp;
    }

    return ret;
}

Number** Number::divide(Number &div)
{
    std::vector<TWORD> *ptr, *rm;
    bool sign = !(_sign ^ div._sign);
    switch(modcmp(*this, div))
    {
        case 0:
            if (_tmp)
            {
                ptr = _num;
                _num = NULL;
            }
            else
                ptr = new std::vector<TWORD>;
            ptr->push_back(1);

            if (div._tmp)
            {
                rm = div._num;
                div._num = NULL;
            }
            else
                rm = new std::vector<TWORD>;
            rm->push_back(0);

            break;
        case -1:

            if (_tmp)
            {
                rm = _num;
                _num = NULL;
            }
            else
            {
                rm = new std::vector<TWORD>;
                std::copy(_num->begin(), _num->end(), rm->begin());
            }

            if (div._tmp)
            {
                ptr = div._num;
                div._num = NULL;
                ptr->clear();
            }
            else
                ptr = new std::vector<TWORD>;

            ptr->push_back(0);

            break;
        default:
            ptr = __div(*_num, *div._num, &rm);
            if (!sign && (rm->size() > 1 || (*rm)[0] != 0))
            {
                std::vector<TWORD> a;
                a.push_back(1);
                __add(ptr, *ptr, a);
                __sub(ptr, *div._num, *ptr);
            }
    }

    Number **ret = new Number*[2];
    ret[0] = new Number(ptr, sign, false);
    ret[1] = new Number(rm, true, false);

    return ret;
}