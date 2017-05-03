#ifndef UNIQUEVECTOR_H
#define UNIQUEVECTOR_H

#include <QVector>
#include <QHash>

template <typename TIndex, typename TValue>
class UniqueVector
{
    QVector<TValue> values;
    QHash<TValue, TIndex> indexesHash;

    const TIndex _invalidIndex;
public:
    UniqueVector(const TIndex &invalidIndex = -1) : _invalidIndex(invalidIndex) {}

    void append(const TValue &value)
    {
        Q_ASSERT(!indexesHash.contains(value));

        values.append(value);
        indexesHash.insert(value, values.size() - 1);  // last index
    }
    const TValue &value(const TIndex &id) const
    {
        Q_ASSERT(id >= 0 && id < values.size());
        return values[id];
    }
    TIndex index(const TValue &feature) const
    {
        if (!indexesHash.contains(feature))
            return _invalidIndex;
        return indexesHash[feature];
    }

    bool contains(const TValue &value)
    {
        return indexesHash.contains(value);
    }

    int size() const { return values.size();}

    void clear() {
        values.clear();
        indexesHash.clear();
    }

    bool isValid(const TIndex &index) const { return index != _invalidIndex;}

    template <typename TI, typename TV>
    friend QDataStream &operator<<(QDataStream &ds, const UniqueVector<TI, TV> &f);

    template <typename TI, typename TV>
    friend QDataStream &operator>>(QDataStream &ds, UniqueVector<TI, TV> &f);
};

template <typename TIndex, typename TValue>
QDataStream &operator<<(QDataStream &ds, const UniqueVector<TIndex, TValue> &f)
{
    ds << f.values;
    ds << f.indexesHash;

    return ds;
}

template <typename TIndex, typename TValue>
QDataStream &operator>>(QDataStream &ds, UniqueVector<TIndex, TValue> &f)
{
    ds >> f.values;
    ds >> f.indexesHash;

    return ds;
}

#endif // UNIQUEVECTOR_H
