#ifndef TST_TIMEDEXERCISE_H
#define TST_TIMEDEXERCISE_H

#include <QObject>

class TstTimedExercise : public QObject
{
    Q_OBJECT

private slots:
    void defaults();
    void duration();
    void durationSeconds_data();
    void durationSeconds();
    void isValid_data();
    void isValid();
    void rpm_data();
    void rpm();
    void setMinsEmitsOnChange();
    void setMinsNoOpDoesNotEmit();
    void setSecsEmitsDurationDelta();
    void setRepsEmitsOnChange();
    void setActivityTypeEmitsOnChange();
    void toggleActivityType();
    void clone();
};

#endif // TST_TIMEDEXERCISE_H
