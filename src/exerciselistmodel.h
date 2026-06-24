#pragma once

#include <QAbstractListModel>
#include <QTime>
#include <QJsonArray>

class ExerciseSet;

class ExerciseListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:

    enum Roles {SetRole = Qt::UserRole + 1};

    explicit ExerciseListModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = SetRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void appendSet(ExerciseSet *set);
    void insertSet(ExerciseSet *set, int position);
    Q_INVOKABLE void removeSet(int index);
    Q_INVOKABLE void clear();
    inline bool isEmpty() const { return mSets.isEmpty(); }
    inline ExerciseSet *at(int i) const { return mSets.at(i); }
    inline int size() const { return mSets.size(); }
    Q_INVOKABLE int count() const {return mSets.count(); }
    Q_INVOKABLE void moveItems(QList<int> selectedIndices, int targetIndex);
    Q_INVOKABLE void copyItems(QList<int> selectedIndices, int targetIndex);
    /*! Serializes the full Set/Exercise tree to JSON, for the
    Draft/History stores. */
    QJsonArray toJson() const;

signals:
    /*! Emitted with the new absolute total (not a delta) whenever the
    workout's grand total duration changes, for any reason. */
    void totalDurationChanged(int newTotalDurationSeconds);
    void countChanged();

private slots:
    void onSetChanged();

private:
    int totalDurationSeconds() const;

private:
   QVector<ExerciseSet*> mSets;

};
