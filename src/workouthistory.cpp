#include "workouthistory.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QUuid>
#include <QVariantMap>
#include <algorithm>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
WorkoutHistory::WorkoutHistory(QObject *parent) : QAbstractListModel(parent)
{
    mStorageFilePath = defaultStorageFilePath();
    load();
}

QString WorkoutHistory::defaultStorageFilePath()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QDir(dir).filePath("history.json");
}

int WorkoutHistory::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mEntries.size();
}

QVariant WorkoutHistory::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mEntries.size())
    {
        return QVariant();
    }
    const Entry &entry = mEntries.at(index.row());
    switch (role)
    {
    case IdRole:
        return entry.id;
    case NameRole:
        return entry.name;
    case SummaryRole:
        return summarize(entry.workout);
    case PlaySequenceRole:
        return flattenPlaySequence(entry.workout);
    case EverPlayedRole:
        return entry.lastPlayedAt > 0;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> WorkoutHistory::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "entryId";
    roles[NameRole] = "name";
    roles[SummaryRole] = "summary";
    roles[PlaySequenceRole] = "playSequence";
    roles[EverPlayedRole] = "everPlayed";
    return roles;
}

int WorkoutHistory::count() const
{
    return mEntries.size();
}

QString WorkoutHistory::addEntry(const QString &name, const QJsonArray &workoutJson)
{
    Entry entry;
    entry.id = QUuid::createUuid().toString();
    entry.name = name;
    entry.createdAt = QDateTime::currentMSecsSinceEpoch() / 1000;
    entry.lastPlayedAt = 0;
    entry.workout = workoutJson;

    beginResetModel();
    mEntries.append(entry);
    resort();
    endResetModel();
    emit countChanged();
    save();
    return entry.id;
}

QJsonArray WorkoutHistory::workoutJson(const QString &id) const
{
    int i = indexOfId(id);
    if (i < 0)
    {
        return QJsonArray();
    }
    return mEntries.at(i).workout;
}

void WorkoutHistory::markPlayed(const QString &id)
{
    int i = indexOfId(id);
    if (i < 0)
    {
        return;
    }
    beginResetModel();
    mEntries[i].lastPlayedAt = QDateTime::currentMSecsSinceEpoch() / 1000;
    resort();
    endResetModel();
    save();
}

void WorkoutHistory::removeEntry(const QString &id)
{
    int i = indexOfId(id);
    if (i < 0)
    {
        return;
    }
    beginResetModel();
    mEntries.removeAt(i);
    endResetModel();
    emit countChanged();
    save();
}

QVariantList WorkoutHistory::flattenPlaySequence(const QJsonArray &workoutJson)
{
    QVariantList sequence;
    for (const QJsonValue &setVal : workoutJson)
    {
        QJsonObject setObj = setVal.toObject();
        int setRounds = qMax(1, setObj.value("rounds").toInt(1));
        QJsonArray exercises = setObj.value("exercises").toArray();
        for (int setRound = 0; setRound < setRounds; ++setRound)
        {
            for (const QJsonValue &exVal : exercises)
            {
                QJsonObject exObj = exVal.toObject();
                int exerciseRounds = qMax(1, exObj.value("rounds").toInt(1));
                int durationSeconds = exObj.value("mins").toInt(0) * 60
                        + exObj.value("secs").toInt(0);
                QString activityType = exObj.value("activityType").toString("work");
                for (int exerciseRound = 0; exerciseRound < exerciseRounds; ++exerciseRound)
                {
                    QVariantMap occurrence;
                    occurrence["activityType"] = activityType;
                    occurrence["durationSeconds"] = durationSeconds;
                    sequence.append(occurrence);
                }
            }
        }
    }
    return sequence;
}

QString WorkoutHistory::summarize(const QJsonArray &workoutJson)
{
    int setCount = workoutJson.size();
    int exerciseCount = 0;
    for (const QJsonValue &setVal : workoutJson)
    {
        exerciseCount += setVal.toObject().value("exercises").toArray().size();
    }
    QVariantList sequence = flattenPlaySequence(workoutJson);
    int totalSeconds = 0;
    int workSeconds = 0;
    int restSeconds = 0;
    for (const QVariant &occurrenceVar : sequence)
    {
        QVariantMap occurrence = occurrenceVar.toMap();
        int duration = occurrence.value("durationSeconds").toInt();
        totalSeconds += duration;
        if (occurrence.value("activityType").toString() == "work")
        {
            workSeconds += duration;
        }
        else
        {
            restSeconds += duration;
        }
    }
    auto fmtTime = [](int s) {
        return WorkoutHistory::tr("%1m %2s")
                .arg(s / 60)
                .arg(s % 60, 2, 10, QChar('0'));
    };
    return tr("%1 · %2 · %3 (%4 work / %5 rest)")
            .arg(tr("%n set(s)", "", setCount))
            .arg(tr("%n exercise(s)", "", exerciseCount))
            .arg(fmtTime(totalSeconds))
            .arg(fmtTime(workSeconds))
            .arg(fmtTime(restSeconds));
}

QString WorkoutHistory::storageFilePath() const
{
    return mStorageFilePath;
}

void WorkoutHistory::setStorageFilePath(const QString &path)
{
    mStorageFilePath = path;
    beginResetModel();
    load();
    endResetModel();
    emit countChanged();
}

void WorkoutHistory::load()
{
    mEntries.clear();
    QFile file(mStorageFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray entriesJson = doc.object().value("entries").toArray();
    for (const QJsonValue &entryVal : entriesJson)
    {
        QJsonObject entryObj = entryVal.toObject();
        Entry entry;
        entry.id = entryObj.value("id").toString();
        entry.name = entryObj.value("name").toString();
        entry.createdAt = (qint64)entryObj.value("createdAt").toDouble();
        entry.lastPlayedAt = (qint64)entryObj.value("lastPlayedAt").toDouble();
        entry.workout = entryObj.value("workout").toArray();
        if (!entry.id.isEmpty())
        {
            mEntries.append(entry);
        }
    }
    resort();
}

void WorkoutHistory::save() const
{
    QJsonArray entriesJson;
    for (const Entry &entry : mEntries)
    {
        QJsonObject entryObj;
        entryObj["id"] = entry.id;
        entryObj["name"] = entry.name;
        entryObj["createdAt"] = entry.createdAt;
        entryObj["lastPlayedAt"] = entry.lastPlayedAt;
        entryObj["workout"] = entry.workout;
        entriesJson.append(entryObj);
    }
    QJsonObject root;
    root["entries"] = entriesJson;

    QFile file(mStorageFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
    }
}

void WorkoutHistory::resort()
{
    std::stable_sort(mEntries.begin(), mEntries.end(),
                      [](const Entry &a, const Entry &b) {
        return sortKey(a) > sortKey(b);
    });
}

int WorkoutHistory::indexOfId(const QString &id) const
{
    for (int i = 0; i < mEntries.size(); ++i)
    {
        if (mEntries.at(i).id == id)
        {
            return i;
        }
    }
    return -1;
}

qint64 WorkoutHistory::sortKey(const Entry &entry)
{
    return entry.lastPlayedAt > 0 ? entry.lastPlayedAt : entry.createdAt;
}
