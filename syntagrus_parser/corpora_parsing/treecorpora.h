#ifndef TREECORPORA_H
#define TREECORPORA_H

#include "recordincorpora.h"
#include "sentenceincorpora.h"

#include <QVector>


typedef QVector<SentenceInCorpora> TextInCorpora;
typedef QVector<TextInCorpora> TreeCorpora;

#endif // TREECORPORA_H
