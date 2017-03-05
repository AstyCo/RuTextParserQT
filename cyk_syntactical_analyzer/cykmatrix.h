#ifndef CYKMATRIX_H
#define CYKMATRIX_H

#include "grammar/cnfgrammar.h"

typedef unsigned char uchar;

class CYKMatrix
{
    uchar _size;
    NonterminalList *_column;
    const CYKMatrix *_submatrix;

public:
    CYKMatrix();

    uchar size() const { return _size; }


    NonterminalList &at(int i, int j) const;

    NonterminalList *column();
    NonterminalList &columnAt(int i);

    CYKMatrix *produceNext() const;
    QVector<NonterminalList> row(int i) const;

private:
    CYKMatrix(uchar size, const CYKMatrix *submatrix);
    // forbids copy/assignment
    CYKMatrix(const CYKMatrix &m);
    const CYKMatrix &operator=(const CYKMatrix &m);
};


#endif // CYKMATRIX_H
