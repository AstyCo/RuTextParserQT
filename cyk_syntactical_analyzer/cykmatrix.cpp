#include "cykmatrix.h"

#include <QDebug>

CYKMatrix::CYKMatrix()
    : _size(1)
{
    _column = new NonterminalList[1];
}

CYKMatrix::CYKMatrix(uchar size, const CYKMatrix *submatrix)
    :_size(size), _submatrix(submatrix)
{
    Q_ASSERT(_size == _submatrix->size() + 1);

    _column = new NonterminalList[_size];
}

NonterminalList &CYKMatrix::at(int i, int j) const
{
    Q_ASSERT(i >= 0);
    Q_ASSERT(j >= 0);

    if (j == 0) {
        Q_ASSERT(i < _size);
        return _column[i];
    }
    return _submatrix->at(i - 1, j - 1);
}

NonterminalList *CYKMatrix::column()
{
    return _column;
}

NonterminalList &CYKMatrix::columnAt(int i)
{
    Q_ASSERT(i >=0 && i < _size);

    return _column[i] ;
}

CYKMatrix *CYKMatrix::produceNext() const
{
    return new CYKMatrix(_size + 1, this);
}

QVector<NonterminalList> CYKMatrix::row(int i) const
{
    QVector<NonterminalList> res(_size);
    for (int j = 0; j < _size; ++j) {
        res[j] = at(i, j);
    }

    return res;
}
