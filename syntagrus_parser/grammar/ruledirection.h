#ifndef RULEDIRECTION_H
#define RULEDIRECTION_H

#include <QDataStream>

typedef qint32 nonterminal;

inline char *nonterminalToKey(nonterminal nt)
{
//    static const nonterminal
//            mask0_7 = 0xFF,
//            mask8_15 = 0xFF << 8,
//            mask16_23 = 0xFF << 16,
//            mask24_31 = 0xFF << 24;
    static const nonterminal
            mask0_6 = 0x7F ;/*,
            mask7_13 = 0x8F << 7,
            mask14_20 = 0x8F << 14,
            mask21_27 = 0x8F<< 21;*/


    static const char topbit = 0x80;

    char *key = new char[5];

    key[0] = (static_cast<char>(nt)       & mask0_6) | topbit;
    key[1] = (static_cast<char>(nt >> 7)  & mask0_6) | topbit;
    key[2] = (static_cast<char>(nt >> 14) & mask0_6) | topbit;
    key[3] = (static_cast<char>(nt >> 21) & mask0_6) | topbit;
    key[4] = 0;

    return key;
}

enum RuleDirection
{
    LeftRule,
    RightRule
};
inline bool isRight(RuleDirection direction) { return direction == RightRule;}
inline bool isLeft(RuleDirection direction) { return direction == LeftRule;}
inline RuleDirection produceDirection(bool produceRight) { return (produceRight ? RightRule : LeftRule);}

inline QDataStream &operator<<(QDataStream &ds, const RuleDirection &s)
{
    ds << isRight(s);

    return ds;
}

inline  QDataStream &operator>>(QDataStream &ds, RuleDirection &s)
{
    bool rightDirection;
    ds >> rightDirection;
    s = produceDirection(rightDirection);

    return ds;
}


#endif // RULEDIRECTION_H
