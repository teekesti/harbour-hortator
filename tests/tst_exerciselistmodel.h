#ifndef TST_EXERCISELISTMODEL_H
#define TST_EXERCISELISTMODEL_H

#include <QObject>

class TstExerciseListModel : public QObject
{
    Q_OBJECT

private slots:
    void appendIncreasesCount();
    void insertAtPosition();
    void dataReturnsExercisePointer();
    void roleNamesMapping();
    void totalDurationChangedOnInsert();
    void removeDecreasesCountAndDuration();
    void removeOutOfRangeIsNoOp();
    void clearEmptiesModel();
    void clearOnEmptyIsNoOp();
    void exerciseDurationChangePropagates();
    void moveItemsSingleDown();
    void moveItemsSingleUp();
    void moveItemsMultiSelect();
    void copyItemsInsertsClones();
};

#endif // TST_EXERCISELISTMODEL_H
