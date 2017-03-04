#ifndef TREECORPORA_H
#define TREECORPORA_H

#include "recordincorpora.h"
#include "sentenceincorpora.h"

#include <QVector>


typedef QVector<SentenceInCorpora> TextInCorpora;
typedef QVector<TextInCorpora> TreeCorpora;

//class TreeCorpora
//{
//    QVector<TextInCorpora> _texts;
//public:
//    TreeCorpora();

//    QVector<TextInCorpora> &texts();
//};

#endif // TREECORPORA_H
