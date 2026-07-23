#include "appcontroller.h"



#include "pdfpreviewmodel.h"

#include "officeconverter.h"
#include "previewimageprovider.h"
#include "watermarklayout.h"



#include <QAbstractItemModel>
#include <QColor>
#include <QCoreApplication>

#include "filepicker.h"

#include <QDir>
#include <QFileInfo>

#include <QFutureWatcher>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>
#include <QTemporaryDir>
#include <QtConcurrent/QtConcurrentRun>

#include <functional>

namespace {

QString normalizeLocalPath(const QString &path)
{
    if (path.isEmpty())
        return {};
    QString s = path;
    if (s.startsWith(QStringLiteral("file:"), Qt::CaseInsensitive))
        s = QUrl(s).toLocalFile();
    s = QDir::cleanPath(QDir::fromNativeSeparators(s));
    return QFileInfo(s).absoluteFilePath();
}

} // namespace

AppController::AppController(PreviewImageProvider *imageProvider, AppSettings *settings,
                             FilePicker *filePicker, QObject *parent)

    : QObject(parent)

    , m_settings(settings)
    , m_filePicker(filePicker)

{

    if (imageProvider)

        m_preview.setImageProvider(imageProvider);

    if (settings)

        m_preview.setAppSettings(settings);



    connect(&m_files, &FileListModel::filesChanged, this, [this]() {

        m_preview.rebuildFromPaths(m_files.paths());

        pruneStalePageRanges();

        emit fileCountChanged();

        notifyPreviewChanged();

    });

    connect(&m_files, &FileListModel::countChanged, this, &AppController::fileCountChanged);

    connect(&m_preview, &PdfPreviewModel::pageCountChanged, this, &AppController::notifyPreviewChanged);

    connect(&m_preview, &PdfPreviewModel::currentFileChanged, this, &AppController::notifyPreviewChanged);

    connect(&m_preview, &PdfPreviewModel::isLoadingChanged, this, &AppController::notifyPreviewChanged);

    connect(&m_preview, &QAbstractItemModel::dataChanged, this, &AppController::notifyPreviewChanged);

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



void AppController::notifyPreviewChanged()

{

    emit previewChanged();

}



QString AppController::currentFileName() const

{

    if (m_preview.currentFile().isEmpty())

        return {};

    return QFileInfo(m_preview.currentFile()).fileName();

}



void AppController::setCurrentTab(int tab)

{

    if (m_currentTab == tab)

        return;

    m_currentTab = tab;

    emit currentTabChanged();

}



void AppController::addFilesFromList(const QVariantList &paths)

{

    QStringList list;

    list.reserve(paths.size());

    for (const QVariant &v : paths) {

        QString s;
        if (v.canConvert<QUrl>())
            s = v.toUrl().toLocalFile();
        else
            s = v.toString();
        if (s.startsWith(QStringLiteral("file:"), Qt::CaseInsensitive))
            s = QUrl(s).toLocalFile();

        if (!s.isEmpty())

            list.append(s);

    }

    addFiles(list);

}



void AppController::addFiles(const QStringList &paths)

{

    if (paths.isEmpty())

        return;



    m_files.addFiles(paths);
    // filesChanged already rebuilds preview; avoid duplicate rebuild/setCurrentFile
    // races when multiple files (e.g. PDF + Word) are dropped together.

    setStatus(QStringLiteral("%1 file(s) added").arg(m_files.count()));

    emit fileCountChanged();

    notifyPreviewChanged();

}



QStringList AppController::pickPathsSync(const QString &mode,
                                         const QString &suggested,
                                         const QString &filter,
                                         const QString &exportKind)
{
    if (!m_filePicker)
        return {};
    if (!m_filePicker->openSync(mode, defaultDialogDir(), suggested, filter, exportKind))
        return {};
    return m_filePicker->resultPaths();
}

void AppController::browseAndAddFiles()
{
    const QStringList selected = pickPathsSync(QStringLiteral("openMulti"));
    if (selected.isEmpty())
        return;

    if (m_settings) {
        for (const QString &path : selected)
            m_settings->rememberRecentFile(path);
    }
    addFiles(selected);
}



void AppController::clearFiles()
{
    m_files.clear();
    m_preview.setCurrentFile({});
    setStatus({});
    if (!m_pageRanges.isEmpty()) {
        m_pageRanges.clear();
        emit pageRangesChanged();
    }
    emit fileCountChanged();
}

void AppController::removeFileAt(int index)
{
    const QString path = filePathAt(index);
    m_files.removeAt(index);
    if (!path.isEmpty() && m_pageRanges.remove(normalizeLocalPath(path)) > 0)
        emit pageRangesChanged();
}

QVariantMap AppController::pageRanges() const
{
    QVariantMap map;
    for (auto it = m_pageRanges.cbegin(); it != m_pageRanges.cend(); ++it)
        map.insert(it.key(), it.value());
    return map;
}

bool AppController::anyPageRangeSet() const
{
    for (auto it = m_pageRanges.cbegin(); it != m_pageRanges.cend(); ++it) {
        if (!it.value().trimmed().isEmpty())
            return true;
    }
    return false;
}

void AppController::setPageRange(const QString &path, const QString &text)
{
    const QString key = normalizeLocalPath(path);
    if (key.isEmpty())
        return;

    const QString trimmed = text.trimmed();
    if (m_pageRanges.value(key) == trimmed)
        return;

    if (trimmed.isEmpty())
        m_pageRanges.remove(key);
    else
        m_pageRanges.insert(key, trimmed);
    emit pageRangesChanged();
}

void AppController::commitPreviewPageRange(const QString &text)
{
    QString path = normalizeLocalPath(m_preview.currentFile());
    if (path.isEmpty() && m_files.count() > 0)
        path = normalizeLocalPath(m_files.paths().first());
    setPageRange(path, text);
}

QString AppController::pageRange(const QString &path) const
{
    return m_pageRanges.value(normalizeLocalPath(path));
}

void AppController::pruneStalePageRanges()
{
    const QStringList paths = m_files.paths();
    bool changed = false;
    for (auto it = m_pageRanges.begin(); it != m_pageRanges.end();) {
        if (!paths.contains(it.key())) {
            it = m_pageRanges.erase(it);
            changed = true;
        } else {
            ++it;
        }
    }
    if (changed)
        emit pageRangesChanged();
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



QString AppController::defaultDialogDir() const

{

    if (m_settings) {

        const QString saved = m_settings->lastOutputDir();

        if (!saved.isEmpty() && QFileInfo::exists(saved))

            return saved;

    }

    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

}



QString AppController::browseOutputFile(const QString &suggested, const QString &filter)
{
    const QStringList paths = pickPathsSync(QStringLiteral("save"), suggested, filter);
    return paths.isEmpty() ? QString() : paths.first();
}

QString AppController::browseOutputDir(const QString &suggestedBase,
                                       const QString &exportKind)
{
    const QStringList paths = pickPathsSync(QStringLiteral("folder"), suggestedBase, {},
                                            exportKind);
    return paths.isEmpty() ? QString() : paths.first();
}



void AppController::rememberOutput(const QString &path)

{

    if (m_settings && !path.isEmpty())

        m_settings->rememberOutputPath(path);

}



QVariantList AppController::watermarkLayoutItems(const QString &text, int count,
                                               qreal pageWidth, qreal pageHeight) const
{
    const QList<WatermarkLayout::Item> items =
        WatermarkLayout::computeItems(text, count, pageWidth, pageHeight);

    QVariantList result;
    result.reserve(items.size());
    for (const WatermarkLayout::Item &item : items) {
        QVariantMap entry;
        entry.insert(QStringLiteral("x"), item.x);
        entry.insert(QStringLiteral("y"), item.y);
        result.append(entry);
    }
    return result;
}



namespace {

QString resolveInput(PdfEngine &engine, const QString &path, QTemporaryDir *tempDir, int *tempSerial,
                     const QString &needPdfMsg, QString *error)
{
    QString engineError;
    const QString pdf = engine.resolveToPdfPath(path, tempDir, tempSerial, &engineError);
    if (!pdf.isEmpty())
        return pdf;

    if (error) {
        if (engineError.contains(QStringLiteral("Unsupported format")) || engineError.isEmpty())
            *error = needPdfMsg;
        else
            *error = engineError;
    }
    return {};
}

QString resolveQpdfPageRange(PdfEngine &engine, const QString &pdfPath,
                             const QString &normalizedRange, bool excludePages, QString *error)
{
    if (!excludePages)
        return normalizedRange;

    if (normalizedRange.isEmpty()) {
        if (error)
            *error = QStringLiteral("Specify pages to exclude");
        return {};
    }

    const int totalPages = engine.pageCount(pdfPath);
    if (totalPages <= 0) {
        if (error)
            *error = QStringLiteral("Cannot read page count");
        return {};
    }

    const QString range = PdfEngine::pageRangeForQpdf(normalizedRange, true, totalPages);
    if (range.isEmpty()) {
        if (error)
            *error = QStringLiteral("No pages left after exclusion");
        return {};
    }
    return range;
}

QString previewActionInput(const QStringList &paths, const QString &previewFile)
{
    if (!previewFile.isEmpty() && paths.contains(previewFile))
        return previewFile;
    return paths.isEmpty() ? QString() : paths.first();
}

} // namespace

void AppController::runCurrentAction(int optionValue, const QString &extraText,
                                     const QString &extraColor, bool excludePages,
                                     const QString &pageRangeText)
{
    if (m_busy)
        return;

    if (m_currentTab <= 2)
        commitPreviewPageRange(pageRangeText);

    const QStringList paths = m_files.paths();
    if (paths.isEmpty()) {
        setStatus(QStringLiteral("No files"));
        emit actionFinished(false, QStringLiteral("No files"));
        return;
    }

    const auto fail = [this](const QString &message) {
        setStatus(message);
        emit actionFinished(false, message);
    };

    const QString invalidRangeMsg = m_settings
        ? m_settings->trKey(QStringLiteral("pageRangeInvalid"))
        : QStringLiteral("Invalid page range");

    const QString excludeNeedRangeMsg = m_settings
        ? m_settings->trKey(QStringLiteral("excludePagesNeedRange"))
        : QStringLiteral("Specify pages to exclude");

    const QString needPdfMsg = m_settings
        ? m_settings->trKey(QStringLiteral("needPdf"))
        : QStringLiteral("This file type is not supported for this operation");

    std::function<QString()> task;
    QString outputPath;
    QString watermarkText;

    switch (m_currentTab) {

    case 0: {
        const QString splitInput = previewActionInput(paths, m_preview.currentFile());

        bool rangeOk = true;
        QString raw = pageRange(splitInput);
        if (raw.trimmed().isEmpty())
            raw = pageRangeText.trimmed();
        const QString normalized = PdfEngine::normalizePageRange(raw, &rangeOk);
        if (!rangeOk) {
            fail(invalidRangeMsg);
            return;
        }
        if (excludePages && normalized.isEmpty()) {
            fail(excludeNeedRangeMsg);
            return;
        }
        const QString defaultBase = QFileInfo(splitInput).completeBaseName();
        const QString outDir = browseOutputDir(defaultBase, QStringLiteral("split"));
        if (outDir.isEmpty())
            return;
        const QString splitBase = m_filePicker && !m_filePicker->fileName().trimmed().isEmpty()
            ? m_filePicker->fileName().trimmed()
            : defaultBase;
        const QString splitSep = m_filePicker ? m_filePicker->splitSeparator() : QStringLiteral("_");
        const int splitNumStyle = m_filePicker ? m_filePicker->splitNumberStyle() : 0;
        const QString splitLang = m_settings ? m_settings->language() : QStringLiteral("zh_CN");
        outputPath = outDir;
        task = [this, splitInput, outDir, normalized, excludePages, splitBase, splitSep,
                splitNumStyle, splitLang, needPdfMsg]() {
            QTemporaryDir tempDir;
            if (!tempDir.isValid())
                return QStringLiteral("Cannot create temp directory");
            int serial = 0;
            QString resolveError;
            const QString pdf =
                resolveInput(m_engine, splitInput, &tempDir, &serial, needPdfMsg, &resolveError);
            if (pdf.isEmpty())
                return resolveError;

            QString rangeError;
            const QString range =
                resolveQpdfPageRange(m_engine, pdf, normalized, excludePages, &rangeError);
            if (!rangeError.isEmpty())
                return rangeError;

            return m_engine.splitPdf(pdf, outDir, true, range, splitBase, splitSep, splitNumStyle,
                                     splitLang);
        };
        break;
    }

    case 1: {
        if (paths.isEmpty()) {
            fail(QStringLiteral("No files"));
            return;
        }

        QString sharedExcludeRange;
        if (excludePages) {
            bool sharedOk = true;
            QString rawShared = pageRange(m_preview.currentFile());
            if (rawShared.trimmed().isEmpty())
                rawShared = pageRangeText.trimmed();
            const QString previewRange = PdfEngine::normalizePageRange(rawShared, &sharedOk);
            if (!sharedOk) {
                fail(invalidRangeMsg);
                return;
            }
            sharedExcludeRange = previewRange;
        }

        QStringList ranges;
        ranges.reserve(paths.size());
        for (const QString &path : paths) {
            bool rangeOk = true;
            QString raw = pageRange(path);
            if (raw.trimmed().isEmpty() && excludePages && !sharedExcludeRange.isEmpty())
                raw = sharedExcludeRange;

            const QString normalized = PdfEngine::normalizePageRange(raw, &rangeOk);
            if (!rangeOk) {
                fail(invalidRangeMsg);
                return;
            }
            if (excludePages && normalized.isEmpty()) {
                fail(excludeNeedRangeMsg);
                return;
            }
            ranges.append(normalized);
        }
        const QString out = browseOutputFile(QStringLiteral("merged.pdf"));
        if (out.isEmpty())
            return;
        outputPath = out;
        task = [this, paths, out, ranges, excludePages, needPdfMsg]() -> QString {
            QTemporaryDir tempDir;
            if (!tempDir.isValid())
                return QStringLiteral("Cannot create temp directory");
            int serial = 0;
            QStringList resolved;
            resolved.reserve(paths.size());
            for (const QString &path : paths) {
                QString resolveError;
                const QString pdf =
                    resolveInput(m_engine, path, &tempDir, &serial, needPdfMsg, &resolveError);
                if (pdf.isEmpty())
                    return resolveError;
                resolved.append(pdf);
            }
            return m_engine.mergePdfs(resolved, out, ranges, excludePages);
        };
        break;
    }

    case 2: {
        const QString rotateInput = previewActionInput(paths, m_preview.currentFile());

        bool rangeOk = true;
        QString raw = pageRange(rotateInput);
        if (raw.trimmed().isEmpty())
            raw = pageRangeText.trimmed();
        const QString normalized = PdfEngine::normalizePageRange(raw, &rangeOk);
        if (!rangeOk) {
            fail(invalidRangeMsg);
            return;
        }
        if (excludePages && normalized.isEmpty()) {
            fail(excludeNeedRangeMsg);
            return;
        }
        const QString out = browseOutputFile(
            QFileInfo(rotateInput).completeBaseName() + QStringLiteral("_rotated.pdf"));
        if (out.isEmpty())
            return;
        const QString input = rotateInput;
        outputPath = out;
        task = [this, input, out, optionValue, normalized, excludePages, needPdfMsg]() {
            QTemporaryDir tempDir;
            if (!tempDir.isValid())
                return QStringLiteral("Cannot create temp directory");
            int serial = 0;
            QString resolveError;
            const QString pdf =
                resolveInput(m_engine, input, &tempDir, &serial, needPdfMsg, &resolveError);
            if (pdf.isEmpty())
                return resolveError;

            QString rangeError;
            const QString range =
                resolveQpdfPageRange(m_engine, pdf, normalized, excludePages, &rangeError);
            if (!rangeError.isEmpty())
                return rangeError;

            return m_engine.rotatePdf(pdf, out, optionValue, range);
        };
        break;
    }

    case 3: {
        if (optionValue == 0) {
            if (paths.size() == 1) {
                const QString out = browseOutputFile(
                    QFileInfo(paths.first()).completeBaseName() + QStringLiteral(".pdf"),
                    QStringLiteral("PDF (*.pdf)"));
                if (out.isEmpty())
                    return;
                outputPath = out;
                task = [this, paths, out]() {
                    return m_engine.convertToPdf(paths, out);
                };
            } else {
                const QString outDir = browseOutputDir({}, QStringLiteral("convert"));
                if (outDir.isEmpty())
                    return;
                outputPath = outDir;
                task = [this, paths, outDir]() {
                    return m_engine.convertEachToPdf(paths, outDir);
                };
            }
        } else if (optionValue == 3) {
            const QString out = browseOutputFile(
                QFileInfo(paths.first()).completeBaseName() + QStringLiteral(".docx"));
            if (out.isEmpty())
                return;
            const QString input = paths.first();
            outputPath = out;
            task = [this, input, out]() {
                return m_engine.convertPdfToWord(input, out);
            };
        } else {
            const QString format = optionValue == 1 ? QStringLiteral("png")
                                                    : QStringLiteral("jpeg");
            const QString outDir = browseOutputDir({}, QStringLiteral("convert"));
            if (outDir.isEmpty())
                return;
            outputPath = outDir;
            task = [this, paths, outDir, format, needPdfMsg]() {
                for (const QString &path : paths) {
                    QTemporaryDir tempDir;
                    if (!tempDir.isValid())
                        return QStringLiteral("Cannot create temp directory");
                    int serial = 0;
                    QString resolveError;
                    const QString pdf =
                        resolveInput(m_engine, path, &tempDir, &serial, needPdfMsg, &resolveError);
                    if (pdf.isEmpty())
                        return resolveError;
                    const QString baseName = QFileInfo(path).completeBaseName();
                    const QString err = m_engine.exportPdfAsImages(pdf, outDir, format, baseName);
                    if (!err.isEmpty())
                        return err;
                }
                return QString();
            };
        }
        break;
    }

    case 4: {
        const QString compressInput = previewActionInput(paths, m_preview.currentFile());
        const QString out = browseOutputFile(
            QFileInfo(compressInput).completeBaseName() + QStringLiteral("_compressed.pdf"));
        if (out.isEmpty())
            return;
        const QString input = compressInput;
        outputPath = out;
        task = [this, input, out, optionValue, needPdfMsg]() {
            QTemporaryDir tempDir;
            if (!tempDir.isValid())
                return QStringLiteral("Cannot create temp directory");
            int serial = 0;
            QString resolveError;
            const QString pdf =
                resolveInput(m_engine, input, &tempDir, &serial, needPdfMsg, &resolveError);
            if (pdf.isEmpty())
                return resolveError;
            return m_engine.compressPdf(pdf, out, optionValue);
        };
        break;
    }

    case 5: {
        const QString text = extraText.trimmed();
        if (text.isEmpty()) {
            fail(QStringLiteral("Watermark text is required"));
            return;
        }
        const QString out = browseOutputFile(
            QFileInfo(paths.first()).completeBaseName() + QStringLiteral("_watermarked.pdf"));
        if (out.isEmpty())
            return;
        const QString input = paths.first();
        outputPath = out;
        watermarkText = text;
        QColor wmColor(extraColor);
        if (!wmColor.isValid())
            wmColor = QColor(90, 90, 90);
        task = [this, input, out, text, optionValue, wmColor, needPdfMsg]() {
            QTemporaryDir tempDir;
            if (!tempDir.isValid())
                return QStringLiteral("Cannot create temp directory");
            int serial = 0;
            QString resolveError;
            const QString pdf =
                resolveInput(m_engine, input, &tempDir, &serial, needPdfMsg, &resolveError);
            if (pdf.isEmpty())
                return resolveError;
            return m_engine.watermarkPdf(pdf, out, text, optionValue, wmColor);
        };
        break;
    }

    default:
        return;
    }

    if (!task)
        return;

    setBusy(true);
    setProgress(0.35);

    // Heavy work runs on a worker thread so the UI stays responsive.
    auto *watcher = new QFutureWatcher<QString>(this);
    connect(watcher, &QFutureWatcher<QString>::finished, this,
            [this, watcher, outputPath, watermarkText]() {
        const QString error = watcher->result();
        watcher->deleteLater();

        setProgress(error.isEmpty() ? 1.0 : 0.0);
        setBusy(false);

        if (error.isEmpty()) {
            if (!outputPath.isEmpty())
                rememberOutput(outputPath);
            if (!watermarkText.isEmpty() && m_settings)
                m_settings->addWatermarkHistory(watermarkText);
            const QString okMsg = m_settings
                ? m_settings->trKey(QStringLiteral("success"))
                : QStringLiteral("OK");
            setStatus(okMsg);
            emit actionFinished(true, okMsg);
        } else {
            setStatus(error);
            emit actionFinished(false, error);
        }

        QTimer::singleShot(600, this, [this]() {
            if (!m_busy)
                setProgress(0);
        });
    });

    watcher->setFuture(QtConcurrent::run(task));
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

    const double clamped = qBound(0.0, value, 1.0);

    if (qFuzzyCompare(m_progress, clamped))

        return;

    m_progress = clamped;

    emit progressChanged();

}


