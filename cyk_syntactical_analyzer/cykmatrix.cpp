#include "cykmatrix.h"

#include <QDebug>

CYKMatrix::CYKMatrix(int size)
    : _size(size)
{
    if (size > 0)
        initMatrix(size);
}

CYKMatrix::~CYKMatrix()
{
    if (_matrix) {
        for (int i=0; i < sz; ++i)
            delete []_matrix[i];
        delete [] _matrix;
    }
}

void CYKMatrix::initMatrix(int sz)
{
    Q_ASSERT(_matrix != NULL);
    if (sz <= 0)
        return;

    // inits lower triangular matrix
    _size(sz);
    _matrix = new CYKMatrix*[sz];
    for (int i=0; i < sz; ++i)
        _matrix[i] = new CYKMatrix[i + 1];
}

CYKCell &CYKMatrix::at(int i, int j) const
{
    Q_ASSERT(i >= 0 && i < _size);
    Q_ASSERT(j >= 0 && j < _size);

    return _matrix[i][j];
}
