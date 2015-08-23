#ifndef WEIGHT_STRING_BUILDER
#define WEIGHT_STRING_BUILDER

#include <QObject>

// @TODO Make this a value type when upgraded to Qt 5.5, so no allocations on the heap are done.
class WeightStringBuilder : public QObject {
    Q_OBJECT

public:
    static WeightStringBuilder* create(qreal _weight) { return new WeightStringBuilder(_weight); }

public slots:
    QString get() { QString result = build(); this->deleteLater(); return result; }
    WeightStringBuilder* setWeight(qreal value) { weight = value; return this; }
    WeightStringBuilder* setBodyWeight(bool value) { withBodyWeight = value; return this; }
    WeightStringBuilder* setSpaceBetween(bool value) { withSpaceBetween = value; return this; }
    WeightStringBuilder* setLowerCase(bool value) { lowerCase = value; return this; }
    WeightStringBuilder* setNeat(bool value) { neat = value; return this; }

private:
    explicit WeightStringBuilder(QObject *parent = 0) : QObject(parent) {}
    WeightStringBuilder(qreal weight_, QObject *parent = 0) : QObject(parent), weight(weight_) {}
    QString build();

    qreal weight = 0;
    bool withBodyWeight = false;
    bool withSpaceBetween = false;
    bool lowerCase = false;
    bool neat = true;
};

#endif // WEIGHT_STRING_BUILDER

