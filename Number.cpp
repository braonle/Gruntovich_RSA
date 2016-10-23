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
        this->_num = new std::vector<TWORD>(*cp._num);

    this->_sign = cp._sign;
}

Number Number::operator+(Number &add) {
    std::vector<TWORD> *ptr;
    bool sign = _sign;

    // Signs are equal -> addition
    if (_sign && add._sign || !_sign && !add._sign)
    {
        ptr = __add(*_num, *add._num);
        sign = _sign;
    }
    else
    {
        int res = modcmp(*this, add);

        // Trivial result
        if (res == 0)
        {
            ptr = new std::vector<TWORD>;
            ptr->push_back(0);
            return Number(ptr, true, true);
        }
        else if (res == -1)
        {
            sign = add._sign;
            ptr = __sub(*add._num, *_num);
        }
        else
            ptr = __sub(*_num, *add._num);
    }

    return Number(ptr, sign, true);
}

Number Number::operator-(Number &sub) {
    std::vector<TWORD> *ptr;
    bool sign = _sign;

    // Sign combination is equal to addition
    if (_sign && !sub._sign || !_sign && sub._sign)
    {
        ptr = __add(*_num, *sub._num);
        sign = _sign;
    }
    else
    {
        int res = modcmp(*this, sub);

        // Trivial result
        if (res == 0)
        {
            ptr = new std::vector<TWORD>;
            ptr->push_back(0);
            return Number(ptr, true, true);
        }
        else if (res == -1)
        {
            sign = !sub._sign;
            ptr = __sub(*sub._num, *_num);
        }
        else
            ptr = __sub(*_num, *sub._num);
    }

    return Number(ptr, sign, true);
}

Number& Number::operator=(const Number &assign)
{
    // Free contents
    if (!this->_num)
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

std::vector<TWORD>* Number::__add(std::vector<TWORD> &a, std::vector<TWORD> &b)
{
    std::vector<TWORD> *ret = new std::vector<TWORD>;
    std::vector<TWORD> &extra = (a.size() > b.size()) ? a : b;
    TDWORD tmp = 0;

    for (unsigned long i = 0; i < MAX(a.size(), b.size()); ++i)
    {
        if (i < MIN(a.size(), b.size()))
            tmp = (TDWORD) a[i] + b[i] + HIWORD(tmp);
        else
            tmp = (TDWORD) extra[i] + HIWORD(tmp);

        ret->push_back(LOWORD(tmp));
    }
    if (HIWORD(tmp))
        ret->push_back(HIWORD(tmp));

    return ret;
}

std::vector<TWORD>* Number::__sub(std::vector<TWORD> &a, std::vector<TWORD> &b)
{
    std::vector<TWORD> *ret = new std::vector<TWORD>;
    std::vector<TWORD> &extra = (a.size() > b.size()) ? a : b;
    unsigned short cf = 0;
    unsigned long count = 0;

    for (unsigned long i = 0; i < MAX(a.size(), b.size()); ++i)
        if (i < MIN(a.size(), b.size()))
            if (a[i] >= b[i] + (TDWORD)cf)
            {
                ret->push_back(a[i] - b[i] - cf);
                cf = 0;
            }
            else
            {
                ret->push_back(a[i] + (TDWORD)(1 << (sizeof(TWORD) * 8)) - b[i] - cf);
                cf = 1;
            }
        else
            if (extra[i] >= cf)
            {
                ret->push_back(extra[i] - cf);
                cf = 0;
            }
            else
            {
                ret->push_back(extra[i] + (TDWORD)(1 << (sizeof(TWORD) * 8)) - cf);
                cf = 1;
            }

    // Removal of leading zeros
    for (unsigned long i = ret->size(); i > 0; --i)
        if (!ret->at(i - 1))
            ++count;
        else
            i = 1;
    if (count)
        ret->resize(ret->size() - count);

    return ret;
}

bool Number::operator<=(Number &arg)
{
    return !operator>(arg);
}

bool Number::operator>=(Number &arg)
{
    return !operator<(arg);
}

bool Number::operator>(Number &arg)
{
    if (_sign && !arg._sign)
        return true;
    if (!_sign && arg._sign)
        return false;
    return modcmp(*this, arg) == 1 && _sign;
}

bool Number::operator<(Number &arg)
{
    if (_sign && !arg._sign)
        return false;
    if (!_sign && arg._sign)
        return true;
    return modcmp(*this, arg) == -1 && _sign;
}

bool Number::operator==(Number &arg)
{
    if (_num->size() != arg._num->size() || _sign != arg._sign)
        return false;
    else
        for (unsigned long i = 0; i < _num->size(); i++)
            if (_num[i] != arg._num[i])
                return false;
    return true;
}

void Number::negate(void)
{
    _sign = !_sign;
}

int Number::modcmp(Number &a, Number &b)
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

std::ostream& LongNumber::operator<<(std::ostream &out, Number& src)
{
    out << ((src._sign) ? "0x" : "-0x");
    for (unsigned long i = src._num->size(); i >0; --i)
        out << std::setfill('0') << std::setw(sizeof(TWORD) * 2)
            << std::hex << std::uppercase << (*src._num)[i - 1];
    return out;
}

unsigned long Number::getSize(void)
{
    return _num->size();
}

bool Number::getSign(void)
{
    return _sign;
}