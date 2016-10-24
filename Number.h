/**
 * Created by Yaroslav Bondarenko
 */

#pragma once

#include <cstddef>
#include <cstring>
#include <malloc.h>
#include <iostream>
#include <iomanip>
#include <vector>

#define MAX(a,b) ((a>b)?a:b)
#define MIN(a,b) ((a>b)?b:a)
#define LOWORD(a) ((TWORD)a)
#define HIWORD(a) ((TWORD)(a>>(8*sizeof(TWORD))))

namespace LongNumber
{
    /// Main type of basic unit
    typedef unsigned int TWORD;
    /// Twice as large as TWORD
    typedef unsigned long TDWORD;

    ///Basic class for long calculation
    class Number
    {
    protected:
        std::vector<TWORD> *_num;   ///< Data storage
        bool _tmp = false;          ///< Operator-specific
                                    ///< Used explicitly in operators for performance optimizing.
                                    ///< Do not move to public fields.
                                    ///<
                                    ///< true - contents should be retrieved instead of copying;
                                    ///< false - explicit copy is required

        bool _sign;                 ///< true  - positive, false - negative

        /** Operator-specific
         *  Used in operators to make use of _tmp flag
         *
         * @param number - vector of basic units
         * @param sign   - sets the _sign
         * @param tmp    - could the contents be retrieved or should be copied
         */
        Number(std::vector<TWORD> *number, bool sign, bool tmp):
                _num(number), _tmp(tmp), _sign(sign){};

    public:
        /** Commom constructor
         *
         * @param number - vector of basic units
         * @param sign   - true = positive, false = negative (0 is positive)
         */
        Number(std::vector<TWORD> *number, bool sign): _num(number), _sign(sign){};

        /** Copy constructor, mainly used for assignment (=)
         *
         * @param cp - target Number to copy (temporary ones are emptied)
         */
        Number(const Number& cp);

        /**
         * @return          temporary Number object (contents to be retrieved)
         */
        Number operator+ (const Number &add) const ;

        /**
         * @return          temporary Number object (contents to be retrieved)
        */
        Number operator- (const Number &sub) const ;
        /** Retrieves contents from temporary objects; otherwise copies them
         *
         * @return          -   reference to itself for chain assignments
         */
        Number& operator= (const Number &assign);

        Number& operator+= (const Number &add);
        Number& operator-= (const Number &sub);

        bool operator == (const Number &arg) const;
        bool operator != (const Number &arg) const;
        bool operator > (const Number &arg) const;
        bool operator < (const Number &arg) const;
        bool operator >= (const Number &arg) const;
        bool operator <= (const Number &arg) const;

        /// Reverses the sign
        void negate(void);
        unsigned long getSize(void) const;
        bool getSign(void) const;

        /** Compares absolute values
         *
         * @return      a = b   -   0
         *              a > b   -   1
         *              a < b   -   -1
         */
        static int modcmp(const Number &a, const Number &b);

        friend std::ostream& operator<< (std::ostream& out, const Number& src);
        ~Number();

    protected:
        /** Addition of absolute values
         *
         * @return  result vector of TWORD without leading zeros
         */
        static std::vector<TWORD>* __add(std::vector<TWORD> *ret, const std::vector<TWORD> &a, const std::vector<TWORD> &b);

        /** Subtraction of absolute values
         *
         * @return  result vector of TWORD without leading zeros
         */
        static std::vector<TWORD>* __sub(std::vector<TWORD> *ret, const std::vector<TWORD> &a, const std::vector<TWORD> &b);
    };

    /** Hexadecimal output to standard streams (cout, string)
     *
     * @return  reference to itself for chain assignments
     */
    std::ostream& operator<< (std::ostream& out, const Number& src);
}