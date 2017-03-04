#include "sentenceincorpora.h"

RecordNode *SentenceInCorpora::root() const
{
    return _root;
}

void SentenceInCorpora::setRoot(RecordNode * const root)
{
    _root = root;
}

bool compareById(const RecordInCorpora &record, const int &id)
{
    return record._id == id;
}

RecordNode *SentenceInCorpora::nodeById(int id) const
{
    return nodeH(id, _root, compareById);
}

SentenceInCorpora::SentenceInCorpora(RecordNode *rootRecord)
    : _root(rootRecord)
{

}

QDataStream &operator<<(QDataStream &ds, const SentenceInCorpora &s)
{
    ds << s._error;

    if (s._root) {
        ds << true;
        ds << *s._root;
    }
    else {
        ds << false;
    }
    return ds;
}

QDataStream &operator>>(QDataStream &ds, SentenceInCorpora &s)
{
    ds >> s._error;

    bool rootExists;
    ds >> rootExists;

    if (rootExists) {
        s._root = new RecordNode();
        ds >> *s._root;
    }

    return ds;
}


