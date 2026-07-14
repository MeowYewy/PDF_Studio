#include "updatechecker.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUrl>
#include <QVariantMap>

namespace {

constexpr auto kUpdateManifestUrl =
    "https://raw.githubusercontent.com/MeowYewy/ProjectO/main/resources/update.json";

constexpr auto kDefaultSilentInstallArgs =
    "/VERYSILENT /SUPPRESSMSGBOXES /CLOSEAPPLICATIONS /RESTARTAPPLICATIONS";

QStringList splitVersion(const QString &version)
{
    QString cleaned = version.trimmed();
    if (cleaned.startsWith(QLatin1Char('v'), Qt::CaseInsensitive))
        cleaned = cleaned.mid(1);

    QStringList parts = cleaned.split(QLatin1Char('.'), Qt::KeepEmptyParts);
    while (parts.size() < 3)
        parts.append(QStringLiteral("0"));
    return parts;
}

} // namespace

UpdateChecker::UpdateChecker(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_changelog(loadChangelog())
{
}

QVariantList UpdateChecker::loadChangelog()
{
    const QStringList candidates = {
        QStringLiteral(":/ProjectO/resources/changelog.json"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/ProjectO/resources/changelog.json"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/resources/changelog.json"),
    };

    for (const QString &resourcePath : candidates) {
        QFile file(resourcePath);
        if (!file.open(QIODevice::ReadOnly))
            continue;

        const QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (!doc.isArray())
            continue;

        QVariantList entries;
        entries.reserve(doc.array().size());
        for (const QJsonValue &value : doc.array()) {
            const QJsonObject obj = value.toObject();
            QVariantMap item;
            item.insert(QStringLiteral("version"), obj.value(QStringLiteral("version")).toString());
            item.insert(QStringLiteral("date"), obj.value(QStringLiteral("date")).toString());

            const QJsonValue notesValue = obj.value(QStringLiteral("notes"));
            if (notesValue.isObject()) {
                const QJsonObject notesObj = notesValue.toObject();
                QVariantMap notesMap;
                for (auto it = notesObj.begin(); it != notesObj.end(); ++it)
                    notesMap.insert(it.key(), it.value().toString());
                item.insert(QStringLiteral("notesMap"), notesMap);
            } else {
                const QString fallback = notesValue.toString();
                item.insert(QStringLiteral("notes"), fallback);
            }
            entries.append(item);
        }
        if (!entries.isEmpty())
            return entries;
    }

    return {};
}

QString UpdateChecker::localVersion() const
{
    return QCoreApplication::applicationVersion();
}

int UpdateChecker::changelogEntryCount() const
{
    return m_changelog.size();
}

QVariantMap UpdateChecker::changelogEntryAt(int index) const
{
    if (index < 0 || index >= m_changelog.size())
        return {};
    return m_changelog.at(index).toMap();
}

int UpdateChecker::compareVersions(const QString &lhs, const QString &rhs)
{
    const QStringList left = splitVersion(lhs);
    const QStringList right = splitVersion(rhs);
    const int count = qMax(left.size(), right.size());

    for (int i = 0; i < count; ++i) {
        const int l = i < left.size() ? left.at(i).toInt() : 0;
        const int r = i < right.size() ? right.at(i).toInt() : 0;
        if (l < r)
            return -1;
        if (l > r)
            return 1;
    }
    return 0;
}

void UpdateChecker::setStatus(int status)
{
    if (m_status == status)
        return;
    m_status = status;
    emit statusChanged();
}

void UpdateChecker::setHasUpdate(bool value)
{
    if (m_hasUpdate == value)
        return;
    m_hasUpdate = value;
    emit hasUpdateChanged();
}

void UpdateChecker::checkForUpdates()
{
    if (m_status == Checking || m_status == Downloading)
        return;

    if (m_activeReply) {
        m_activeReply->abort();
        m_activeReply->deleteLater();
        m_activeReply = nullptr;
    }

    setStatus(Checking);

    QNetworkRequest request{QUrl(QString::fromUtf8(kUpdateManifestUrl))};
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("ProjectO-Updater"));
    m_activeReply = m_network->get(request);

    connect(m_activeReply, &QNetworkReply::finished, this, [this]() {
        QNetworkReply *reply = m_activeReply;
        m_activeReply = nullptr;

        if (!reply) {
            setStatus(CheckFailed);
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            reply->deleteLater();
            setStatus(CheckFailed);
            return;
        }

        parseUpdateManifest(reply->readAll());
        reply->deleteLater();
    });
}

void UpdateChecker::parseUpdateManifest(const QByteArray &data)
{
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        setStatus(CheckFailed);
        return;
    }

    const QJsonObject root = doc.object();
    const QString remoteVersion = root.value(QStringLiteral("version")).toString();
    const QString downloadUrl = root.value(QStringLiteral("downloadUrl")).toString();

    if (remoteVersion.isEmpty()) {
        setStatus(CheckFailed);
        return;
    }

    m_latestVersion = remoteVersion;
    m_downloadUrl = downloadUrl;
    m_silentInstallArgs = root.value(QStringLiteral("silentInstallArgs")).toString();
    if (m_silentInstallArgs.trimmed().isEmpty())
        m_silentInstallArgs = QString::fromUtf8(kDefaultSilentInstallArgs);
    emit latestVersionChanged();
    emit downloadUrlChanged();

    if (compareVersions(localVersion(), remoteVersion) < 0) {
        setHasUpdate(true);
        setStatus(UpdateAvailable);
        return;
    }

    setHasUpdate(false);
    setStatus(UpToDate);
}

void UpdateChecker::downloadUpdate()
{
    if (!m_hasUpdate || m_downloadUrl.isEmpty()) {
        if (m_status != Checking)
            checkForUpdates();
        return;
    }

    if (m_activeReply) {
        m_activeReply->abort();
        m_activeReply->deleteLater();
        m_activeReply = nullptr;
    }

    setStatus(Downloading);
    m_downloadProgress = 0;
    emit downloadProgressChanged();

    QNetworkRequest request{QUrl(m_downloadUrl)};
    m_activeReply = m_network->get(request);

    connect(m_activeReply, &QNetworkReply::downloadProgress, this,
            [this](qint64 received, qint64 total) {
                if (total <= 0)
                    return;
                const int progress = int((received * 100) / total);
                if (progress == m_downloadProgress)
                    return;
                m_downloadProgress = progress;
                emit downloadProgressChanged();
            });

    connect(m_activeReply, &QNetworkReply::finished, this, [this]() {
        QNetworkReply *reply = m_activeReply;
        m_activeReply = nullptr;

        if (!reply || reply->error() != QNetworkReply::NoError) {
            if (reply)
                reply->deleteLater();
            setStatus(CheckFailed);
            return;
        }

        const QUrl url(m_downloadUrl);
        QString fileName = QFileInfo(url.path()).fileName();
        if (fileName.isEmpty())
            fileName = QStringLiteral("ProjectO-update.exe");

        const QString destPath = QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                                     .filePath(fileName);

        QSaveFile out(destPath);
        if (!out.open(QIODevice::WriteOnly)) {
            reply->deleteLater();
            setStatus(CheckFailed);
            return;
        }

        out.write(reply->readAll());
        if (!out.commit()) {
            reply->deleteLater();
            setStatus(CheckFailed);
            return;
        }

        reply->deleteLater();

        const QString argsLine = m_silentInstallArgs.trimmed().isEmpty()
                                     ? QString::fromUtf8(kDefaultSilentInstallArgs)
                                     : m_silentInstallArgs;
        const QStringList args =
            QProcess::splitCommand(argsLine);

        if (!QProcess::startDetached(destPath, args)) {
            // Fallback: open installer normally if silent launch fails
            if (!QProcess::startDetached(destPath, QStringList())) {
                setStatus(CheckFailed);
                return;
            }
        }

        setStatus(UpdateAvailable);
        QCoreApplication::quit();
    });
}
