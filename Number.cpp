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

Number Number::operator+(Number &add)
{
    std::vector<TWORD> *ptr;
    bool sign = _sign;

    ptr = (_tmp) ? _num : (add._tmp) ? add._num : NULL;

    ptr = __add_sign_op(ptr, add, sign);

    if (_tmp)
        _num = NULL;
    else if (add._tmp)
        add._num = NULL;

    return Number (ptr, sign, true, true);
}

Number Number::operator-(Number &sub)
{
    std::vector<TWORD> *ptr;
    bool sign = _sign;

    ptr = (_tmp) ? _num : (sub._tmp) ? sub._num : NULL;

    ptr = __sub_sign_op(ptr, sub, sign);

    if (_tmp)
        _num = NULL;
    else if (sub._tmp)
        sub._num = NULL;


    return Number(ptr, sign, true, true);
}

Number& Number::operator-=(const Number &sub)
{
    __sub_sign_op(_num, sub, _sign);
    return *this;
}

Number& Number::operator+= (const Number &add)
{
    __add_sign_op(_num, add, _sign);
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
    // Free contents
    if (this->_num)
        delete _num;

    // Retrieving
    if (assign._tmp)
    {
        this->_num = assign._num;
        const_cast<Number&>(assign)._num = NULL;
    }
        // Copying
    else
    {
        this->_num = new std::vector<TWORD>;
        *_num = *assign._num;
    }
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
    // Trivial result
    else
    {
        int res = modcmp(*this, add);

        if (res == 0)
        {
            ptr = new std::vector<TWORD>;
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
    // Trivial result
    else
    {
        int res = modcmp(*this, sub);

        if (res == 0)
        {
            ptr = new std::vector<TWORD>;
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

    for (unsigned long i = 0; i < min; ++i)
    {
        tmp = (TDWORD) a[i] + b[i] + HIWORD(tmp);
        if (i < size)
            (*ret)[i] = LOWORD(tmp);
        else
            ret->push_back(LOWORD(tmp));
    }
    for (unsigned long i = min; i < max; ++i)
    {
        tmp = (TDWORD) extra[i] + HIWORD(tmp);
        if (i < size)
            (*ret)[i] = LOWORD(tmp);
        else
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
    const std::vector<TWORD> &extra = (a.size() > b.size()) ? a : b;
    unsigned short cf = 0;
    unsigned long count = 0;
    unsigned long min = MIN(a.size(), b.size());
    unsigned long max = MAX(a.size(), b.size());
    unsigned long size = ret->size();
    TSDWORD tmp;
    unsigned long i;

    for (i = 0; i < min; ++i)
    {
        if (a[i] <= (tmp = b[i] + (TDWORD) cf))
        {
            tmp -= (TDWORD) (1 << (sizeof(TWORD) * 8));
            cf = 1;
        } else
            cf = 0;

        if (i < size)
            (*ret)[i] = a[i] - tmp;
        else
            ret->push_back(a[i] - tmp);
    }
    for (i = min; i < max && cf; ++i)
    {
        if (extra[i] >= cf)
        {
            tmp = extra[i] - cf;
            cf = 0;
        }
        else
            tmp = extra[i] + (TDWORD)(1 << (sizeof(TWORD) * 8)) - cf;

        if (i < size)
            (*ret)[i] = tmp;
        else
            ret->push_back(tmp);
    }

    for (i = min; i < max; ++i)
        if (i < size)
            (*ret)[i] = extra[i];
        else
            ret->push_back(extra[i]);

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