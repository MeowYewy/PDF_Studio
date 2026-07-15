#include "updatechecker.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QSaveFile>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>
#include <QVariantMap>
#include <QWindow>

namespace {

// Same GitHub repo: will be renamed PDF_Studio → PageCase (redirects keep old URLs alive).
// Prefer PageCase; fall back to PDF_Studio so checks work before/after the rename.
constexpr auto kManifestUrlPrimary =
    "https://raw.githubusercontent.com/MeowYewy/PageCase/main/resources/update.json";
constexpr auto kManifestUrlFallback =
    "https://raw.githubusercontent.com/MeowYewy/PDF_Studio/main/resources/update.json";

// /SILENT shows Inno progress UI; /VERYSILENT hides everything.
constexpr auto kDefaultSilentInstallArgs =
    "/SILENT /SUPPRESSMSGBOXES /CLOSEAPPLICATIONS /RESTARTAPPLICATIONS";

// Reject truncated / HTML error pages posing as installers.
constexpr qint64 kMinInstallerBytes = 512 * 1024;

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

void configureUpdateRequest(QNetworkRequest &request)
{
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("PageCase-Updater/0.2"));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setTransferTimeout(60'000);
}

} // namespace

UpdateChecker::UpdateChecker(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_changelog(loadChangelog())
    , m_manifestUrls({QString::fromUtf8(kManifestUrlPrimary),
                      QString::fromUtf8(kManifestUrlFallback)})
{
}

QVariantList UpdateChecker::loadChangelog()
{
    const QStringList candidates = {
        QStringLiteral(":/qt/qml/PageCase/resources/changelog.json"),
        QCoreApplication::applicationDirPath() + QStringLiteral("/PageCase/resources/changelog.json"),
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
    const bool wasReady = installerReady();
    m_status = status;
    emit statusChanged();
    if (wasReady != installerReady())
        emit installerReadyChanged();
}

void UpdateChecker::setHasUpdate(bool value)
{
    if (m_hasUpdate == value)
        return;
    m_hasUpdate = value;
    emit hasUpdateChanged();
}

void UpdateChecker::abortActiveReply()
{
    if (!m_activeReply)
        return;
    m_activeReply->disconnect(this);
    m_activeReply->abort();
    m_activeReply->deleteLater();
    m_activeReply = nullptr;
}

void UpdateChecker::checkForUpdates()
{
    if (m_status == Checking || m_status == Downloading || m_installLaunched)
        return;

    abortActiveReply();
    m_manifestUrlIndex = 0;
    setStatus(Checking);
    fetchManifestAt(0);
}

void UpdateChecker::fetchManifestAt(int index)
{
    if (index < 0 || index >= m_manifestUrls.size()) {
        setStatus(CheckFailed);
        return;
    }

    m_manifestUrlIndex = index;
    QNetworkRequest request{QUrl(m_manifestUrls.at(index))};
    configureUpdateRequest(request);
    m_activeReply = m_network->get(request);

    connect(m_activeReply, &QNetworkReply::finished, this, [this]() {
        QNetworkReply *reply = m_activeReply;
        m_activeReply = nullptr;

        if (!reply) {
            setStatus(CheckFailed);
            return;
        }

        const bool ok = reply->error() == QNetworkReply::NoError;
        const QByteArray body = ok ? reply->readAll() : QByteArray();
        reply->deleteLater();

        if (!ok) {
            if (m_manifestUrlIndex + 1 < m_manifestUrls.size()) {
                fetchManifestAt(m_manifestUrlIndex + 1);
                return;
            }
            setStatus(CheckFailed);
            return;
        }

        parseUpdateManifest(body);
    });
}

void UpdateChecker::parseUpdateManifest(const QByteArray &data)
{
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        if (m_manifestUrlIndex + 1 < m_manifestUrls.size()) {
            fetchManifestAt(m_manifestUrlIndex + 1);
            return;
        }
        setStatus(CheckFailed);
        return;
    }

    const QJsonObject root = doc.object();
    const QString remoteVersion = root.value(QStringLiteral("version")).toString().trimmed();
    const QString downloadUrl = root.value(QStringLiteral("downloadUrl")).toString().trimmed();

    if (remoteVersion.isEmpty() || downloadUrl.isEmpty()) {
        if (m_manifestUrlIndex + 1 < m_manifestUrls.size()) {
            fetchManifestAt(m_manifestUrlIndex + 1);
            return;
        }
        setStatus(CheckFailed);
        return;
    }

    const QUrl url(downloadUrl);
    if (!url.isValid() || (url.scheme() != QLatin1String("https") && url.scheme() != QLatin1String("http"))) {
        setStatus(CheckFailed);
        return;
    }

    m_latestVersion = remoteVersion;
    m_downloadUrl = downloadUrl;
    m_silentInstallArgs = root.value(QStringLiteral("silentInstallArgs")).toString();
    if (m_silentInstallArgs.trimmed().isEmpty())
        m_silentInstallArgs = QString::fromUtf8(kDefaultSilentInstallArgs);
    // Prefer visible progress bar over fully hidden install.
    m_silentInstallArgs.replace(QStringLiteral("/VERYSILENT"), QStringLiteral("/SILENT"),
                                Qt::CaseInsensitive);
    emit latestVersionChanged();
    emit downloadUrlChanged();

    if (compareVersions(localVersion(), remoteVersion) < 0) {
        setHasUpdate(true);
        // Keep existing package if already downloaded for this version.
        if (!m_installerPath.isEmpty() && QFile::exists(m_installerPath)
            && m_installerPath.contains(remoteVersion) && looksLikeWindowsInstaller(m_installerPath)) {
            setStatus(ReadyToInstall);
            return;
        }
        setStatus(UpdateAvailable);
        QTimer::singleShot(0, this, &UpdateChecker::startDownload);
        return;
    }

    m_installerPath.clear();
    setHasUpdate(false);
    setStatus(UpToDate);
}

void UpdateChecker::downloadUpdate()
{
    if (m_installLaunched)
        return;
    if (m_status == ReadyToInstall) {
        installUpdate();
        return;
    }
    if (!m_hasUpdate || m_downloadUrl.isEmpty()) {
        if (m_status != Checking)
            checkForUpdates();
        return;
    }
    startDownload();
}

bool UpdateChecker::looksLikeWindowsInstaller(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (file.size() < kMinInstallerBytes)
        return false;
    const QByteArray magic = file.read(2);
    return magic == QByteArrayLiteral("MZ");
}

void UpdateChecker::startDownload()
{
    if (m_downloadUrl.isEmpty() || m_installLaunched) {
        setStatus(DownloadFailed);
        return;
    }

    abortActiveReply();

    setStatus(Downloading);
    m_downloadProgress = 0;
    emit downloadProgressChanged();

    QNetworkRequest request{QUrl(m_downloadUrl)};
    configureUpdateRequest(request);
    request.setTransferTimeout(10 * 60'000);
    m_activeReply = m_network->get(request);

    connect(m_activeReply, &QNetworkReply::downloadProgress, this,
            [this](qint64 received, qint64 total) {
                if (total <= 0)
                    return;
                const int progress = int((received * 100) / total);
                if (progress == m_downloadProgress)
                    return;
                m_downloadProgress = qBound(0, progress, 99);
                emit downloadProgressChanged();
            });

    connect(m_activeReply, &QNetworkReply::finished, this, [this]() {
        QNetworkReply *reply = m_activeReply;
        m_activeReply = nullptr;

        if (!reply || reply->error() != QNetworkReply::NoError) {
            if (reply)
                reply->deleteLater();
            m_installerPath.clear();
            setStatus(DownloadFailed);
            return;
        }

        const QUrl url(m_downloadUrl);
        QString fileName = QFileInfo(url.path()).fileName();
        if (fileName.isEmpty() || !fileName.endsWith(QLatin1String(".exe"), Qt::CaseInsensitive))
            fileName = QStringLiteral("PageCase_%1_update.exe").arg(m_latestVersion);

        const QString destPath = QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                                     .filePath(fileName);

        // Remove a previous partial/corrupt download with the same name.
        if (QFile::exists(destPath))
            QFile::remove(destPath);

        QSaveFile out(destPath);
        if (!out.open(QIODevice::WriteOnly)) {
            reply->deleteLater();
            m_installerPath.clear();
            setStatus(DownloadFailed);
            return;
        }

        const QByteArray payload = reply->readAll();
        reply->deleteLater();

        if (payload.size() < kMinInstallerBytes || !payload.startsWith("MZ")) {
            m_installerPath.clear();
            setStatus(DownloadFailed);
            return;
        }

        if (out.write(payload) != payload.size() || !out.commit()) {
            m_installerPath.clear();
            setStatus(DownloadFailed);
            return;
        }

        if (!looksLikeWindowsInstaller(destPath)) {
            QFile::remove(destPath);
            m_installerPath.clear();
            setStatus(DownloadFailed);
            return;
        }

        m_installerPath = destPath;
        m_downloadProgress = 100;
        emit downloadProgressChanged();
        setStatus(ReadyToInstall);
        emit installerReadyChanged();
    });
}

void UpdateChecker::quitForInstaller()
{
    // Close top-level windows first so QML teardown is orderly; then quit.
    // Avoid racing Inno's CloseApplications against a half-alive UI.
    const auto windows = QGuiApplication::topLevelWindows();
    for (QWindow *window : windows) {
        if (window)
            window->close();
    }
    QTimer::singleShot(250, qApp, []() {
        QCoreApplication::exit(0);
    });
}

void UpdateChecker::installUpdate()
{
    if (m_installLaunched)
        return;

    if (m_installerPath.isEmpty() || !QFile::exists(m_installerPath)
        || !looksLikeWindowsInstaller(m_installerPath)) {
        m_installerPath.clear();
        setStatus(DownloadFailed);
        return;
    }

    QString argsLine = m_silentInstallArgs.trimmed().isEmpty()
                           ? QString::fromUtf8(kDefaultSilentInstallArgs)
                           : m_silentInstallArgs;
    // Ensure close/restart flags survive a partial remote override.
    if (!argsLine.contains(QStringLiteral("/CLOSEAPPLICATIONS"), Qt::CaseInsensitive))
        argsLine += QStringLiteral(" /CLOSEAPPLICATIONS");
    if (!argsLine.contains(QStringLiteral("/RESTARTAPPLICATIONS"), Qt::CaseInsensitive))
        argsLine += QStringLiteral(" /RESTARTAPPLICATIONS");

    const QStringList args = QProcess::splitCommand(argsLine);

    qint64 pid = 0;
    const bool started = QProcess::startDetached(m_installerPath, args, QString(), &pid);
    if (!started || pid <= 0) {
        // Last resort: no args (user sees wizard) — still better than stuck ReadyToInstall.
        pid = 0;
        if (!QProcess::startDetached(m_installerPath, QStringList(), QString(), &pid) || pid <= 0) {
            setStatus(DownloadFailed);
            return;
        }
    }

    m_installLaunched = true;
    abortActiveReply();

    // Give the installer process time to initialize before we exit; Inno then
    // force-closes PageCase.exe / PDFStudio.exe via CloseApplicationsFilter.
    QTimer::singleShot(500, this, &UpdateChecker::quitForInstaller);
}
