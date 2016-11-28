#include "Number.h"

#define LENGTH 256
#define SIZE LENGTH/(8*sizeof(TWORD))

using namespace std;
using namespace LongNumber;

void test(bool);

int main()
{
    srand(time(0));

    test(true);
/*
    for (unsigned i = 0; i < 1000000; i++)
    {
        if (!(i % 100000))
            cout << "\r" << i << flush;

        test(false);
    }
*/    cout << endl;
    return 0;
}

void t_add_sub(Number &A, Number &B, bool out)
{
    Number C;
    C = A + B;

    if (out)
    {
        cout << "A = " << A << endl;
        cout << "B = " << B << endl;
        cout << "C = " << C << endl;
        cout << "C - B = " << C - B << endl;
        cout << ((C - B) == A) << endl;
//        cout << ((A += B) == C) << endl;
//        cout << ((A -= B) == (C - B)) << endl;
    }
    else if ((C - B) != A || (A += B) != C || (A -= B) != (C - B))
        cout << endl << "A = " << A << endl
             << "B = " << B << endl
             << "C = " << C << endl
             << "C - B = " << C - B << endl;
}

void t_mul_div(Number &A, Number &B, bool out)
{
    Number C;
    C = A * B;
    std::vector<TWORD> *tmp = new std::vector<TWORD>;
    tmp->push_back(1);
    Number D(tmp, true);

    if (out)
    {
//        Number **rt = C.divide(B);
        cout << "A = " << A << endl;
        cout << "B = " << B << endl;
        cout << "C = " << C << endl;
        cout << "C / B = " << C / B << endl;
        cout << "C % B = " << C % B << endl;
        cout << "(C - 1) / C = " << (C - D) / C << endl;
//        cout << "C / A = " << (C /= A) << endl;
//        cout << "C % A = " << (C %= A) << endl;
//        delete rt[0];
//        delete rt[1];
//        delete[] rt;
    }
    else if ((C / B) != A || (A *= B) != C || (A %= B) != (C % B))
        cout << endl << "A = " << A << endl
             << "B = " << B << endl
             << "C = " << C << endl
             << "C / B = " << C / B << endl;
}

void mul_mod(Number &A, Number &B, Number &mod, bool out)
{
	Number D;
	D = A(B, mod);
	if (D != A*B % mod)
		cout << "D = " << D << endl 
			<< "A = " << A << endl 
			<< "B = " << B << endl
			<< "mod = " << mod << endl;
}

void pow_mod(Number &A, Number &pow, Number &pow2, Number &mod, bool out)
{
	Number D;
	cout << A << endl;
	D = A(&pow, mod);
	cout << D << endl;
	D = D(&pow2, mod);
	if (A != D)
		cout << "A = " << A << endl
			<< "D = " << D << endl;
}

void test(bool out)
{
    vector<TWORD> *a = new vector<TWORD>;
    vector<TWORD> *m1 = new vector<TWORD>;
    vector<TWORD> *m = new vector<TWORD>;
    vector<TWORD> *m2 = new vector<TWORD>;
    
    for (int i = 0; i < SIZE; ++i)
    {
        a->push_back((rand() << 1) % (TWORD)(1 << 8*sizeof(TWORD)));
//        b->push_back((rand() << 1) % (TWORDq)i(1 << 8*sizeof(TWORD)));
//	m->push_back((rand() << 1) % (TWORD)(1 << 8*sizeof(TWORD)));
    }

//    m->pop_back();

	m->push_back(0xffcb6d13);
	m->push_back(0xba83d640);
	m->push_back(0x9c721158);
	m->push_back(0x1b7bdae5);
	m->push_back(0x3d6d4387);
	m->push_back(0x678f8a17);
	m->push_back(0x74cd0aa3);
	m->push_back(0xd9ae128b);

	m1->push_back(0x609155f3);
	m1->push_back(0x0f69b60e);
	m1->push_back(0x8ff074fc);
	m1->push_back(0x8abe1307);
	m1->push_back(0x76f66893);
	m1->push_back(0x8f676cd2);
	m1->push_back(0x5ea0ff91);
	m1->push_back(0xd2078c90);

	m2->push_back(0xb511080b);
	m2->push_back(0xe1c6d27c);
	m2->push_back(0x3ba72076);
	m2->push_back(0x9a538711);
	m2->push_back(0x286be430);
	m2->push_back(0x2d38abbc);
	m2->push_back(0x0cd0456c);
	m2->push_back(0x8add50d7);


    
	Number A(a), e(m1), d(m2), mod(m);

    pow_mod(A, e, d, mod, out);
}

