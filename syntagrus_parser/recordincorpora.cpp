#include "recordincorpora.h"

RecordInCorpora::RecordInCorpora()
{

}

RecordInCorpora::RecordInCorpora(int dom, const QString &feat, int id,
                                 const QString &lemma, const QString &link, const QString &word)
    : _dom(dom), _feat(feat), _id(id), _lemma(lemma), _link(link), _word(word)
{

}
