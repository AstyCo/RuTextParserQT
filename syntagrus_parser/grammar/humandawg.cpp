#include "humandawg.h"

using namespace  dawgdic;

HumanDAWG::HumanDAWG(Completer *completer)
    : _dic(completer->dic()), _guide(completer->guide()), _root(new NodeDAWG())
{
    buildFromCompleter(completer);
}

bool HumanDAWG::contains(const QList<LabelType> &body) const
{
    QSharedPointer<NodeDAWG> curNode = _root;

    for (int i=0; i < body.size(); ++i) {
        if (!curNode->childs.contains(body.at(i)))
            return false;
        curNode = curNode->childs[body.at(i)];
    }
    return true;
}

int HumanDAWG::finalCount() const
{
    return _root->finalCount();
}

int HumanDAWG::intermediateCount() const
{
    return _root->intermediateCount();
}

void HumanDAWG::buildFromCompleter(Completer *completer)
{
    BaseType index = _dic.root();

    makeStep4H(index, _root);
}

HumanDAWG::IndexPaths HumanDAWG::makeStep4(BaseType index)
{
    IndexPaths result;
    IndexPaths step1 = makeStep41(index);
    if (step1.isEmpty())
        return result; // no childs

    return makeStep4n(step1, 3);
}

HumanDAWG::IndexPaths HumanDAWG::makeStep41(BaseType index)
{
    IndexPaths result;
    UCharType child_label = _guide.child(index);
    if (child_label == '\0')
        return result; // no childs
    BaseType childIndex = index;
    if (!_dic.Follow(child_label, &childIndex))
        Q_ASSERT(false);

    QByteArray singleChar;
    singleChar.append(child_label);
    result[childIndex].insert(singleChar);

    for ( ; ; ) {
        UCharType sibling_label = _guide.sibling(childIndex);
        if (sibling_label == '\0') {
            // no more siblings
            return result;
        }
        childIndex = index;
        if (!_dic.Follow(sibling_label, &childIndex))
            Q_ASSERT(false);

        QByteArray singleChar;
        singleChar.append(sibling_label);
        result[childIndex].insert(singleChar);
    }
    // never goes there
}

QSet<QByteArray> combine(const QSet<QByteArray> first, const QSet<QByteArray> second)
{
    QSet<QByteArray> result;

    foreach (const QByteArray &fs, first)
        foreach (const QByteArray &ss, second)
            result.insert(fs + ss);

    return result;
}

HumanDAWG::IndexPaths HumanDAWG::makeStep4n(const IndexPaths &lastPaths, int n)
{
    if (n <= 0) {
        return lastPaths;
    }
    IndexPaths newPaths;

    IndexPaths::const_iterator it(lastPaths.constBegin());
    while (it != lastPaths.constEnd()) {
        IndexPaths tempPaths = makeStep41(it.key());    // indexes achieved by 1 step from last index
        IndexPaths::const_iterator it2(tempPaths.constBegin());
        while (it2 != tempPaths.constEnd()) {
            newPaths[it2.key()] += combine(it.value(), it2.value());

            ++it2;
        }
        ++it;
    }
    return makeStep4n(newPaths, n - 1);
}

LabelType HumanDAWG::ltFromByteArray(const QByteArray &array)
{
    Q_ASSERT(array.size() == 4);

    const char *key = array.constData();

    LabelType result;
    result.first = produceDirection(key[3] & 0x40);

    nonterminal nt = (key[0] & 0x7F) |
            ((key[1] & 0x7F) << 7) |
            ((key[2] & 0x7F) << 14) |
            ((key[3] & 0x3F) << 21);

    result.second = nt;
    return result;
}

void HumanDAWG::makeStep4H(BaseType index, QSharedPointer<NodeDAWG> node)
{
    if (_hashIndexToNode.contains(index))
        return; // parsed already
    _hashIndexToNode.insert(index, node);

    IndexPaths step4Indexes = makeStep4(index);

    IndexPaths::const_iterator it(step4Indexes.constBegin());
    while (it != step4Indexes.constEnd()) {
        BaseType childIndex = it.key();
        QSharedPointer<NodeDAWG> childNode;
        if (!_hashIndexToNode.contains(childIndex)) {
            childNode = QSharedPointer<NodeDAWG>(new NodeDAWG());
            makeStep4H(childIndex, childNode);
        }
        else {
            childNode = _hashIndexToNode[childIndex];
        }

        foreach (const QByteArray &str, it.value()) {
            node->childs.insert(ltFromByteArray(str), childNode);
        }
        ++it;
    }
    if (_dic.has_value(index)) {
        ValueType value = _dic.value(index);
//        qDebug() << "value "<< value;
        Q_ASSERT(value == 1);

        node->final = true;
    }

}

int NodeDAWG::finalCount() const
{
    int result = (final ? 1 : 0);
    foreach (QSharedPointer<NodeDAWG> node, childs)
        result += node->finalCount();
    return result;
}

int NodeDAWG::intermediateCount() const
{
    int result = (final ? 0 : 1);
    foreach (QSharedPointer<NodeDAWG> node, childs)
        result += node->intermediateCount();
    return result;
}

void NodeDAWG::prepare()
{
    prepareThis();
    QMap<LabelType, QSharedPointer<NodeDAWG>>::iterator it(childs.begin());
    while (it != childs.end()) {
        it.value()->prepare();

        ++it;
    }
}

void NodeDAWG::prepareThis()
{
    visited = false;
    nt = -1;
    len2fix.clear();
}

