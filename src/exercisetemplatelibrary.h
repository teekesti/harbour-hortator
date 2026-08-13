#ifndef EXERCISETEMPLATELIBRARY_H
#define EXERCISETEMPLATELIBRARY_H

#include <QAbstractListModel>
#include <QVector>

/*! A flat, persisted library of named exercise templates, independent of
any workout (see ADR-0008/0009). Naming an Exercise upserts a template
here by that name; picking a template when adding an Exercise to a Set
copies its fields by value. Matched by name - there is no separate id,
and names are not required to be unique with anything else (a later
upsert under an existing name simply overwrites that template's
defaults). */
class ExerciseTemplateLibrary : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        MinsRole,
        SecsRole,
        RepsRole
    };

    explicit ExerciseTemplateLibrary(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int count() const;
    Q_INVOKABLE QStringList allNames() const;

    /*! Creates a template under name, or overwrites the existing one
    matched by name with these defaults. */
    Q_INVOKABLE void upsertTemplate(const QString &name, int mins, int secs, int reps);
    Q_INVOKABLE void removeTemplate(const QString &name);
    Q_INVOKABLE bool hasTemplate(const QString &name) const;
    /*! Returns {"mins":, "secs":, "reps":} for the named template, or an
    empty map if no such template exists. */
    Q_INVOKABLE QVariantMap templateByName(const QString &name) const;

    QString storageFilePath() const;
    void setStorageFilePath(const QString &path);
    static QString defaultStorageFilePath();

signals:
    void countChanged();

private:
    struct Entry
    {
        QString name;
        int mins;
        int secs;
        int reps;
    };

    void load();
    void save() const;
    int indexOfName(const QString &name) const;

    QString mStorageFilePath;
    QVector<Entry> mEntries;
};

#endif // EXERCISETEMPLATELIBRARY_H
