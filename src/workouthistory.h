#ifndef WORKOUTHISTORY_H
#define WORKOUTHISTORY_H

#include <QAbstractListModel>
#include <QJsonArray>
#include <QVariantList>
#include <QVector>

/*! A persisted list of named, independently-snapshotted workouts the user
has explicitly saved for replay later (see ADR-0005/0006/0014/0015/0016).
Each entry owns its own copy of the Set/Exercise tree as JSON - editing the
Draft or one entry never affects another. Kept sorted with the
most-recently-played entry first, falling back to creation time for
entries that have never been played. */
class WorkoutHistory : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles
    {
        IdRole = Qt::UserRole + 1,
        NameRole,
        SummaryRole,
        PlaySequenceRole,
        EverPlayedRole
    };

    explicit WorkoutHistory(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int count() const;

    /*! Saves an independent snapshot of workoutJson under the given name,
    timestamped as created now and never played. Returns the new entry's
    id. */
    Q_INVOKABLE QString addEntry(const QString &name, const QJsonArray &workoutJson);
    /*! Returns the saved workout tree for the given entry, or an empty
    array if no such entry exists. */
    Q_INVOKABLE QJsonArray workoutJson(const QString &id) const;
    /*! Marks the entry as played now; re-sorts so it moves to the front
    of the most-recently-played order. */
    Q_INVOKABLE void markPlayed(const QString &id);
    Q_INVOKABLE void removeEntry(const QString &id);

    /*! Flattens a workout's Set/Exercise JSON tree into one entry per
    Exercise occurrence (accounting for Set and Exercise rounds), each
    {"activityType": ..., "durationSeconds": ...} - the data the Summary
    Bar needs. Exposed standalone so the editor can reuse it for the live
    Draft too. */
    static QVariantList flattenPlaySequence(const QJsonArray &workoutJson);
    /*! A short, human-readable one-line summary (set/exercise counts,
    total duration, work/rest breakdown) for a workout tree. */
    static QString summarize(const QJsonArray &workoutJson);

    /*! Path entries are persisted to. Overridable so tests don't touch
    the real per-user history file. */
    QString storageFilePath() const;
    void setStorageFilePath(const QString &path);
    /*! The default per-user storage path a freshly-constructed
    WorkoutHistory will use. Exposed so tests can isolate themselves
    from the real history file. */
    static QString defaultStorageFilePath();

signals:
    void countChanged();

private:
    struct Entry
    {
        QString id;
        QString name;
        qint64 createdAt;
        qint64 lastPlayedAt; // 0 if never played
        QJsonArray workout;
    };

    void load();
    void save() const;
    void resort();
    int indexOfId(const QString &id) const;
    static qint64 sortKey(const Entry &entry);

    QString mStorageFilePath;
    QVector<Entry> mEntries;
};

#endif // WORKOUTHISTORY_H
