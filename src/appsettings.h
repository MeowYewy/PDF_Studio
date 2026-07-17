#pragma once

#include <QObject>
#include <QSettings>
#include <QTranslator>

class AppSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool isDark READ isDark NOTIFY themeChanged)
    Q_PROPERTY(int languageRevision READ languageRevision NOTIFY languageChanged)
    Q_PROPERTY(int themeRevision READ themeRevision NOTIFY themeChanged)
    Q_PROPERTY(QString appVersion READ appVersion CONSTANT)
    Q_PROPERTY(QString lastOutputDir READ lastOutputDir WRITE setLastOutputDir NOTIFY lastOutputDirChanged)
    Q_PROPERTY(QStringList watermarkHistory READ watermarkHistory NOTIFY watermarkHistoryChanged)
    Q_PROPERTY(int watermarkHistoryRevision READ watermarkHistoryRevision NOTIFY watermarkHistoryChanged)

public:
    explicit AppSettings(QObject *parent = nullptr);

    QString language() const { return m_language; }
    QString theme() const { return m_theme; }
    bool isDark() const { return m_theme == QLatin1String("dark"); }
    int languageRevision() const { return m_languageRevision; }
    int themeRevision() const { return m_themeRevision; }
    QString appVersion() const { return QStringLiteral("0.2.2"); }
    QString lastOutputDir() const;
    QStringList watermarkHistory() const { return m_watermarkHistory; }
    int watermarkHistoryRevision() const { return m_watermarkHistoryRevision; }

    Q_INVOKABLE void setLanguage(const QString &lang);
    Q_INVOKABLE void setTheme(const QString &theme);
    Q_INVOKABLE QString trKey(const QString &key) const;
    Q_INVOKABLE void setLastOutputDir(const QString &dir);
    Q_INVOKABLE void rememberOutputPath(const QString &fileOrDir);
    Q_INVOKABLE QStringList recentFiles() const;
    Q_INVOKABLE void rememberRecentFile(const QString &filePath);
    Q_INVOKABLE void addWatermarkHistory(const QString &text);
    Q_INVOKABLE void removeWatermarkHistoryAt(int index);

signals:
    void languageChanged();
    void themeChanged();
    void lastOutputDirChanged();
    void watermarkHistoryChanged();

private:
    void loadTranslations(const QString &lang);
    void loadWatermarkHistory();
    void saveWatermarkHistory();

    QSettings m_store;
    QTranslator m_translator;
    QString m_language;
    QString m_theme;
    QStringList m_watermarkHistory;
    int m_languageRevision = 0;
    int m_themeRevision = 0;
    int m_watermarkHistoryRevision = 0;
};
