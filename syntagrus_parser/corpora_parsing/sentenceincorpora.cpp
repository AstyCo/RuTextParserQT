#include "sentenceincorpora.h"

RecordNode *SentenceInCorpora::root() const
{
    return _root;
}

void SentenceInCorpora::setRoot(RecordNode * const root)
{
    _root = root;
}

void SentenceInCorpora::updateSentence()
{
    _qDebugSentence.clear();
    if (!_root)
        return;

    _qDebugSentence.resize(_root->size());
    fillSentenceInfo();
}

bool compareById(const RecordInCorpora &record, const int &id)
{
    return record._id == id;
}

RecordNode *SentenceInCorpora::nodeById(int id) const
{
    return nodeH(id, _root, compareById);
}

const SentenceInfo &SentenceInCorpora::qDebugSentence() const
{
    return _qDebugSentence;
}

int SentenceInCorpora::size() const
{
    return _qDebugSentence.size();
}

void SentenceInCorpora::setSkip()
{
    _skip = true;
}

bool SentenceInCorpora::skip() const
{
    return _skip;
}

void SentenceInCorpora::fillSentenceInfo(RecordNode *node)
{
    if (!node) {
        if (!_root)
            return;

        node = _root;
    }

    const RecordInCorpora &record = node->record();

    _qDebugSentence[record._id - 1] = WordInCorpora(record._feat,
                                                    record._dom,
                                                    record._id,
                                                    record._word);

    // insert link
    if (record._dom != -1) {
        Q_ASSERT(_qDebugSentence[record._dom - 1].isValid());
        // _qDebugSentence[record._dom] definetly initialized already
        _qDebugSentence[record._dom - 1].append(makeSintRel(record._dom, record._link));
    }

    // recursion
    foreach(RecordNode * childNode, node->childNodes())
        fillSentenceInfo(childNode);
}



SentenceInCorpora::SentenceInCorpora(RecordNode *rootRecord)
    : _skip(false), _root(rootRecord)
{

}

QDataStream &operator<<(QDataStream &ds, const SentenceInCorpora &s)
{
    ds << s._error;
    ds << s._qDebugSentence;
    ds << s._skip;

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
    ds >> s._qDebugSentence;
    ds >> s._skip;

    bool rootExists;
    ds >> rootExists;

    if (rootExists) {
        s._root = new RecordNode();
        ds >> *s._root;
    }

    return ds;
}


