#include "appcontroller.h"

#include "desensitizeengine.h"
#include "previewimageprovider.h"

#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>

AppController::AppController(PreviewImageProvider *imageProvider, AppSettings *settings,
                             QObject *parent)
    : QObject(parent)
    , m_settings(settings)
{
    if (imageProvider)
        m_preview.setImageProvider(imageProvider);
    if (settings)
        m_preview.setAppSettings(settings);

    connect(&m_files, &FileListModel::filesChanged, this, [this]() {
        m_preview.rebuildFromPaths(m_files.paths());
        emit fileCountChanged();
        notifyPreviewChanged();
        if (m_confirmed)
            m_confirmed = false, emit confirmedChanged();
    });
    connect(&m_files, &FileListModel::countChanged, this, &AppController::fileCountChanged);
    connect(&m_preview, &PdfPreviewModel::pageCountChanged, this, &AppController::notifyPreviewChanged);
    connect(&m_preview, &PdfPreviewModel::currentFileChanged, this, &AppController::notifyPreviewChanged);
    connect(&m_preview, &PdfPreviewModel::isLoadingChanged, this, &AppController::notifyPreviewChanged);
    connect(&m_preview, &QAbstractItemModel::dataChanged, this, &AppController::notifyPreviewChanged);
    connect(&m_regions, &RedactionRegionModel::countChanged, this, &AppController::regionCountChanged);
    connect(&m_regions, &RedactionRegionModel::regionsChanged, this, &AppController::regionCountChanged);
}

QVariantList AppController::previewPages() const
{
    QVariantList pages;
    const int rows = m_preview.rowCount();
    pages.reserve(rows);
    for (int i = 0; i < rows; ++i) {
        const QModelIndex idx = m_preview.index(i, 0);
        QVariantMap page;
        page.insert(QStringLiteral("source"),
                    m_preview.data(idx, PdfPreviewModel::ImageSourceRole));
        page.insert(QStringLiteral("label"),
                    m_preview.data(idx, PdfPreviewModel::LabelRole));
        page.insert(QStringLiteral("pageNumber"),
                    m_preview.data(idx, PdfPreviewModel::PageNumberRole));
        page.insert(QStringLiteral("aspectRatio"),
                    m_preview.data(idx, PdfPreviewModel::AspectRatioRole));
        page.insert(QStringLiteral("pending"),
                    m_preview.data(idx, PdfPreviewModel::PendingRole));
        pages.append(page);
    }
    return pages;
}

QString AppController::currentFileName() const
{
    const QString path = m_preview.currentFile();
    if (path.isEmpty())
        return {};
    return QFileInfo(path).fileName();
}

QString AppController::currentFilePath() const
{
    return m_preview.currentFile();
}

void AppController::addFiles(const QStringList &paths)
{
    m_files.addFiles(paths);
    if (m_files.count() > 0 && m_preview.currentFile().isEmpty())
        selectPreviewFile(m_files.paths().first());
}

void AppController::addFilesFromList(const QVariantList &paths)
{
    QStringList list;
    for (const QVariant &v : paths)
        list.append(v.toString());
    addFiles(list);
}

void AppController::browseAndAddFiles()
{
    const QString filter = QStringLiteral(
        "Supported (*.pdf *.png *.jpg *.jpeg *.docx *.docm *.odt *.rtf);;"
        "PDF (*.pdf);;Images (*.png *.jpg *.jpeg);;Office (*.docx *.docm *.odt *.rtf);;All (*.*)");
    const QStringList selected = QFileDialog::getOpenFileNames(
        nullptr,
        QStringLiteral("添加文件"),
        defaultDialogDir(),
        filter);
    if (!selected.isEmpty())
        addFiles(selected);
}

void AppController::clearFiles()
{
    m_files.clear();
    m_regions.clearAll();
    m_confirmed = false;
    emit confirmedChanged();
    notifyPreviewChanged();
}

void AppController::removeFileAt(int index)
{
    const QString path = filePathAt(index);
    m_files.removeAt(index);
    if (!path.isEmpty())
        m_regions.clearForFile(path);
}

void AppController::selectPreviewFile(const QString &path)
{
    m_preview.setCurrentFile(path);
    notifyPreviewChanged();
}

void AppController::moveFile(int from, int to)
{
    m_files.move(from, to);
}

void AppController::setSortMode(const QString &mode)
{
    m_files.setSortMode(mode);
}

void AppController::ensurePreviewPagesLoaded(int startPage, int endPage)
{
    m_preview.ensurePagesLoaded(startPage, endPage);
}

QString AppController::filePathAt(int index) const
{
    const QStringList paths = m_files.paths();
    if (index < 0 || index >= paths.size())
        return {};
    return paths.at(index);
}

void AppController::confirmAndAnalyze()
{
    if (m_files.count() <= 0) {
        emit actionFinished(false, QStringLiteral("请先添加文件"));
        return;
    }

    setBusy(true);
    setProgress(0.05);
    setStatus(QStringLiteral("正在识别敏感信息…"));

    const QStringList paths = m_files.paths();
    for (int i = 0; i < paths.size(); ++i) {
        analyzeFile(paths.at(i));
        setProgress(0.1 + 0.85 * double(i + 1) / paths.size());
        QCoreApplication::processEvents();
    }

    m_confirmed = true;
    emit confirmedChanged();
    setBusy(false);
    setProgress(1.0);
    setStatus(QStringLiteral("识别完成"));
    emit actionFinished(true,
                        QStringLiteral("已标记 %1 处敏感区域，可在右侧预览中核对或手动调整")
                            .arg(m_regions.count()));
    setProgress(0.0);
}

void AppController::reanalyzeCurrent()
{
    const QString path = currentFilePath();
    if (path.isEmpty())
        return;
    setBusy(true);
    analyzeFile(path);
    setBusy(false);
    m_confirmed = true;
    emit confirmedChanged();
    emit actionFinished(true, QStringLiteral("已重新识别当前文件"));
}

void AppController::analyzeFile(const QString &path)
{
    const auto detected = m_detector.detectInFile(path);
    m_regions.replaceAutoRegions(path, detected);
}

void AppController::setManualMode(bool enabled)
{
    if (m_manualMode == enabled)
        return;
    m_manualMode = enabled;
    emit manualModeChanged();
}

void AppController::setUseSolidBlock(bool solid)
{
    if (m_useSolidBlock == solid)
        return;
    m_useSolidBlock = solid;
    emit mosaicStyleChanged();
}

void AppController::setMosaicBlockSize(int size)
{
    const int clamped = qBound(6, size, 48);
    if (m_mosaicBlockSize == clamped)
        return;
    m_mosaicBlockSize = clamped;
    emit mosaicStyleChanged();
}

QVariantList AppController::regionsForCurrentPage(int page) const
{
    return m_regions.regionsForPage(currentFilePath(), page);
}

void AppController::exportDesensitized()
{
    if (!m_confirmed) {
        emit actionFinished(false, QStringLiteral("请先确认并生成脱敏预览"));
        return;
    }
    if (m_files.count() <= 0) {
        emit actionFinished(false, QStringLiteral("没有可导出的文件"));
        return;
    }

    const QString outDir = browseOutputDir();
    if (outDir.isEmpty())
        return;

    setBusy(true);
    setProgress(0.02);
    int okCount = 0;
    QString lastError;

    const QStringList paths = m_files.paths();
    for (int i = 0; i < paths.size(); ++i) {
        const QString inPath = paths.at(i);
        const QFileInfo info(inPath);
        const QString outPath = outDir + QLatin1Char('/')
                                + info.completeBaseName()
                                + QStringLiteral("_desensitized.pdf");
        QString err;
        const bool ok = DesensitizeEngine::exportFile(
            inPath,
            outPath,
            m_regions.enabledRegionsForFile(inPath),
            m_useSolidBlock,
            m_mosaicBlockSize,
            &err,
            [this](double p) { setProgress(p); });
        if (ok)
            ++okCount;
        else
            lastError = err;
        setProgress(double(i + 1) / paths.size());
        QCoreApplication::processEvents();
    }

    rememberOutput(outDir);
    setBusy(false);
    setProgress(0.0);

    if (okCount == paths.size())
        emit actionFinished(true, QStringLiteral("已导出 %1 个脱敏文件").arg(okCount));
    else
        emit actionFinished(false,
                            QStringLiteral("成功 %1 / %2。%3")
                                .arg(okCount)
                                .arg(paths.size())
                                .arg(lastError));
}

QString AppController::browseOutputFile(const QString &suggested, const QString &filter)
{
    return QFileDialog::getSaveFileName(
        nullptr,
        QStringLiteral("保存文件"),
        defaultDialogDir() + QLatin1Char('/') + suggested,
        filter.isEmpty() ? QStringLiteral("PDF (*.pdf);;PNG (*.png);;JPEG (*.jpg)") : filter);
}

QString AppController::browseOutputDir()
{
    return QFileDialog::getExistingDirectory(
        nullptr,
        QStringLiteral("选择导出目录"),
        defaultDialogDir());
}

void AppController::setStatus(const QString &msg)
{
    if (m_status == msg)
        return;
    m_status = msg;
    emit statusMessageChanged();
}

void AppController::setBusy(bool busy)
{
    if (m_busy == busy)
        return;
    m_busy = busy;
    emit busyChanged();
    emit processingChanged();
}

void AppController::setProgress(double value)
{
    if (qFuzzyCompare(m_progress, value))
        return;
    m_progress = value;
    emit progressChanged();
}

void AppController::notifyPreviewChanged()
{
    emit previewChanged();
}

QString AppController::defaultDialogDir() const
{
    if (m_settings) {
        const QString last = m_settings->lastOutputDir();
        if (!last.isEmpty())
            return last;
    }
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
}

void AppController::rememberOutput(const QString &path)
{
    if (m_settings)
        m_settings->rememberOutputPath(path);
}
