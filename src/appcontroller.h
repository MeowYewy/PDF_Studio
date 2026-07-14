#pragma once

#include <QVariantList>

#include "appsettings.h"
#include "filelistmodel.h"
#include "pdfpreviewmodel.h"
#include "piidetector.h"
#include "redactionregionmodel.h"

class PreviewImageProvider;

class AppController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(FileListModel *files READ files CONSTANT)
    Q_PROPERTY(PdfPreviewModel *preview READ preview CONSTANT)
    Q_PROPERTY(RedactionRegionModel *regions READ regions CONSTANT)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(int fileCount READ fileCount NOTIFY fileCountChanged)
    Q_PROPERTY(QStringList filePaths READ filePaths NOTIFY fileCountChanged)
    Q_PROPERTY(QVariantList previewPages READ previewPages NOTIFY previewChanged)
    Q_PROPERTY(QString currentFileName READ currentFileName NOTIFY previewChanged)
    Q_PROPERTY(QString currentFilePath READ currentFilePath NOTIFY previewChanged)
    Q_PROPERTY(bool previewLoading READ previewLoading NOTIFY previewChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool processing READ processing NOTIFY processingChanged)
    Q_PROPERTY(bool confirmed READ confirmed NOTIFY confirmedChanged)
    Q_PROPERTY(bool manualMode READ manualMode WRITE setManualMode NOTIFY manualModeChanged)
    Q_PROPERTY(bool useSolidBlock READ useSolidBlock WRITE setUseSolidBlock NOTIFY mosaicStyleChanged)
    Q_PROPERTY(int mosaicBlockSize READ mosaicBlockSize WRITE setMosaicBlockSize NOTIFY mosaicStyleChanged)
    Q_PROPERTY(int regionCount READ regionCount NOTIFY regionCountChanged)

public:
    explicit AppController(PreviewImageProvider *imageProvider = nullptr,
                           AppSettings *settings = nullptr,
                           QObject *parent = nullptr);

    FileListModel *files() { return &m_files; }
    PdfPreviewModel *preview() { return &m_preview; }
    RedactionRegionModel *regions() { return &m_regions; }
    QString statusMessage() const { return m_status; }
    bool busy() const { return m_busy; }
    int fileCount() const { return m_files.count(); }
    QStringList filePaths() const { return m_files.paths(); }
    QVariantList previewPages() const;
    QString currentFileName() const;
    QString currentFilePath() const;
    bool previewLoading() const { return m_preview.isLoading(); }
    double progress() const { return m_progress; }
    bool processing() const { return m_busy; }
    bool confirmed() const { return m_confirmed; }
    bool manualMode() const { return m_manualMode; }
    bool useSolidBlock() const { return m_useSolidBlock; }
    int mosaicBlockSize() const { return m_mosaicBlockSize; }
    int regionCount() const { return m_regions.count(); }

    Q_INVOKABLE void addFiles(const QStringList &paths);
    Q_INVOKABLE void addFilesFromList(const QVariantList &paths);
    Q_INVOKABLE void browseAndAddFiles();
    Q_INVOKABLE void clearFiles();
    Q_INVOKABLE void removeFileAt(int index);
    Q_INVOKABLE void selectPreviewFile(const QString &path);
    Q_INVOKABLE void moveFile(int from, int to);
    Q_INVOKABLE void setSortMode(const QString &mode);
    Q_INVOKABLE void ensurePreviewPagesLoaded(int startPage, int endPage);
    Q_INVOKABLE QString filePathAt(int index) const;
    Q_INVOKABLE void confirmAndAnalyze();
    Q_INVOKABLE void reanalyzeCurrent();
    Q_INVOKABLE void setManualMode(bool enabled);
    Q_INVOKABLE void setUseSolidBlock(bool solid);
    Q_INVOKABLE void setMosaicBlockSize(int size);
    Q_INVOKABLE QVariantList regionsForCurrentPage(int page) const;
    Q_INVOKABLE void exportDesensitized();
    Q_INVOKABLE QString browseOutputFile(const QString &suggested, const QString &filter = {});
    Q_INVOKABLE QString browseOutputDir();

signals:
    void statusMessageChanged();
    void busyChanged();
    void fileCountChanged();
    void previewChanged();
    void progressChanged();
    void processingChanged();
    void confirmedChanged();
    void manualModeChanged();
    void mosaicStyleChanged();
    void regionCountChanged();
    void actionFinished(bool ok, const QString &message);

private:
    void setStatus(const QString &msg);
    void setBusy(bool busy);
    void setProgress(double value);
    void notifyPreviewChanged();
    void analyzeFile(const QString &path);
    QString defaultDialogDir() const;
    void rememberOutput(const QString &path);

    FileListModel m_files;
    PdfPreviewModel m_preview;
    RedactionRegionModel m_regions;
    PiiDetector m_detector;
    AppSettings *m_settings = nullptr;
    QString m_status;
    bool m_busy = false;
    double m_progress = 0.0;
    bool m_confirmed = false;
    bool m_manualMode = false;
    bool m_useSolidBlock = false;
    int m_mosaicBlockSize = 14;
};
