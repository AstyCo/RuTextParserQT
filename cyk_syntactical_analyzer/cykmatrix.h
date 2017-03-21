#ifndef CYKMATRIX_H
#define CYKMATRIX_H

#include "grammar/cnfgrammar.h"
#include "cykcell.h"

class CYKMatrix
{
    short _size;
    CYKCell **_matrix;

public:
    CYKMatrix(int size = 0);
    ~CYKMatrix();
    void initMatrix(int sz);

    short size() const { return _size; }


    CYKCell &at(int i, int j) const;

private:

    Q_DISABLE_COPY(CYKMatrix);
};


#endif // CYKMATRIX_H
