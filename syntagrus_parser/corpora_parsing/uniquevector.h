#ifndef UNIQUEVECTOR_H
#define UNIQUEVECTOR_H

#include <QVector>
#include <QHash>

template <typename TIndex, typename TValue>
class UniqueVector
{
    QVector<TValue> values;
    QHash<TValue, TIndex> indexesHash;
public:

    void append(const TValue &value)
    {
        Q_ASSERT(indexesHash.contains(value));

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
            return -1;
        return indexesHash[feature];
    }
    int size() const { return values.size();}

    void clear() {
        values.clear();
        indexesHash.clear();
    }

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
