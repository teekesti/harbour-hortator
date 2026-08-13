#include "exercisetemplatelibrary.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
ExerciseTemplateLibrary::ExerciseTemplateLibrary(QObject *parent) : QAbstractListModel(parent)
{
    mStorageFilePath = defaultStorageFilePath();
    load();
}

QString ExerciseTemplateLibrary::defaultStorageFilePath()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QDir(dir).filePath("exercise_templates.json");
}

int ExerciseTemplateLibrary::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mEntries.size();
}

QVariant ExerciseTemplateLibrary::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mEntries.size())
    {
        return QVariant();
    }
    const Entry &entry = mEntries.at(index.row());
    switch (role)
    {
    case NameRole:
        return entry.name;
    case MinsRole:
        return entry.mins;
    case SecsRole:
        return entry.secs;
    case RepsRole:
        return entry.reps;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ExerciseTemplateLibrary::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[MinsRole] = "mins";
    roles[SecsRole] = "secs";
    roles[RepsRole] = "reps";
    return roles;
}

int ExerciseTemplateLibrary::count() const
{
    return mEntries.size();
}

void ExerciseTemplateLibrary::upsertTemplate(const QString &name, int mins, int secs, int reps)
{
    if (name.isEmpty())
    {
        return;
    }
    int i = indexOfName(name);
    if (i >= 0)
    {
        mEntries[i].mins = mins;
        mEntries[i].secs = secs;
        mEntries[i].reps = reps;
        QModelIndex idx = index(i);
        emit dataChanged(idx, idx);
    }
    else
    {
        Entry entry;
        entry.name = name;
        entry.mins = mins;
        entry.secs = secs;
        entry.reps = reps;
        beginInsertRows(QModelIndex(), mEntries.size(), mEntries.size());
        mEntries.append(entry);
        endInsertRows();
        emit countChanged();
    }
    save();
}

void ExerciseTemplateLibrary::removeTemplate(const QString &name)
{
    int i = indexOfName(name);
    if (i < 0)
    {
        return;
    }
    beginRemoveRows(QModelIndex(), i, i);
    mEntries.removeAt(i);
    endRemoveRows();
    emit countChanged();
    save();
}

bool ExerciseTemplateLibrary::hasTemplate(const QString &name) const
{
    return indexOfName(name) >= 0;
}

QVariantMap ExerciseTemplateLibrary::templateByName(const QString &name) const
{
    int i = indexOfName(name);
    if (i < 0)
    {
        return QVariantMap();
    }
    QVariantMap result;
    result["mins"] = mEntries.at(i).mins;
    result["secs"] = mEntries.at(i).secs;
    result["reps"] = mEntries.at(i).reps;
    return result;
}

QString ExerciseTemplateLibrary::storageFilePath() const
{
    return mStorageFilePath;
}

void ExerciseTemplateLibrary::setStorageFilePath(const QString &path)
{
    mStorageFilePath = path;
    beginResetModel();
    load();
    endResetModel();
    emit countChanged();
}

void ExerciseTemplateLibrary::load()
{
    mEntries.clear();
    QFile file(mStorageFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray entriesJson = doc.object().value("templates").toArray();
    for (const QJsonValue &entryVal : entriesJson)
    {
        QJsonObject entryObj = entryVal.toObject();
        Entry entry;
        entry.name = entryObj.value("name").toString();
        entry.mins = entryObj.value("mins").toInt(1);
        entry.secs = entryObj.value("secs").toInt(0);
        entry.reps = entryObj.value("reps").toInt(0);
        if (!entry.name.isEmpty())
        {
            mEntries.append(entry);
        }
    }
}

void ExerciseTemplateLibrary::save() const
{
    QJsonArray entriesJson;
    for (const Entry &entry : mEntries)
    {
        QJsonObject entryObj;
        entryObj["name"] = entry.name;
        entryObj["mins"] = entry.mins;
        entryObj["secs"] = entry.secs;
        entryObj["reps"] = entry.reps;
        entriesJson.append(entryObj);
    }
    QJsonObject root;
    root["templates"] = entriesJson;

    QFile file(mStorageFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    }
}

int ExerciseTemplateLibrary::indexOfName(const QString &name) const
{
    for (int i = 0; i < mEntries.size(); ++i)
    {
        if (mEntries.at(i).name == name)
        {
            return i;
        }
    }
    return -1;
}
