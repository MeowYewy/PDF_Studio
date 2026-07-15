#include "appcontroller.h"



#include "pdfpreviewmodel.h"

#include "previewimageprovider.h"
#include "watermarklayout.h"



#include <QAbstractItemModel>

#include <QCoreApplication>

#include "filepicker.h"

#include <QFileInfo>

#include <QStandardPaths>
#include <QTimer>
#include <QUrl>



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

        const QString s = v.toString();

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

    m_preview.rebuildFromPaths(m_files.paths());



    if (!m_files.paths().isEmpty())

        m_preview.setCurrentFile(m_files.paths().first());



    setStatus(QStringLiteral("%1 file(s) added").arg(m_files.count()));

    emit fileCountChanged();

    notifyPreviewChanged();

}



QStringList AppController::pickPathsSync(const QString &mode,
                                         const QString &suggested,
                                         const QString &filter)
{
    if (!m_filePicker)
        return {};
    if (!m_filePicker->openSync(mode, defaultDialogDir(), suggested, filter))
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
    emit fileCountChanged();
}

void AppController::removeFileAt(int index)
{
    m_files.removeAt(index);
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

QString AppController::browseOutputDir()
{
    const QStringList paths = pickPathsSync(QStringLiteral("folder"));
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



void AppController::runCurrentAction(int optionValue, const QString &extraText)

{

    if (m_busy)

        return;



    const QStringList paths = m_files.paths();

    if (paths.isEmpty()) {

        setStatus(QStringLiteral("No files"));

        emit actionFinished(false, QStringLiteral("No files"));

        return;

    }



    setBusy(true);

    setProgress(0.08);

    QString error;

    QString outputPath;



    switch (m_currentTab) {

    case 0: {

        const QString outDir = browseOutputDir();

        if (outDir.isEmpty()) {

            setBusy(false);

            setProgress(0);

            return;

        }

        setProgress(0.35);

        error = m_engine.splitPdf(paths.first(), outDir, true);

        if (error.isEmpty())

            rememberOutput(outDir);

        break;

    }

    case 1: {

        if (paths.size() < 2) {

            error = QStringLiteral("Need at least 2 PDF files");

            break;

        }

        const QString out = browseOutputFile(QStringLiteral("merged.pdf"));

        if (out.isEmpty()) {

            setBusy(false);

            setProgress(0);

            return;

        }

        setProgress(0.35);

        error = m_engine.mergePdfs(paths, out);

        outputPath = out;

        break;

    }

    case 2: {

        const QString out = browseOutputFile(

            QFileInfo(paths.first()).completeBaseName() + QStringLiteral("_rotated.pdf"));

        if (out.isEmpty()) {

            setBusy(false);

            setProgress(0);

            return;

        }

        setProgress(0.35);

        error = m_engine.rotatePdf(paths.first(), out, optionValue);

        outputPath = out;

        break;

    }

    case 3: {

        if (optionValue == 0) {

            const QString out = browseOutputFile(QStringLiteral("converted.pdf"));

            if (out.isEmpty()) {

                setBusy(false);

                setProgress(0);

                return;

            }

            setProgress(0.35);

            error = m_engine.convertToPdf(paths, out);

            outputPath = out;

        } else {

            const QString format = optionValue == 1 ? QStringLiteral("png")

                                                    : QStringLiteral("jpeg");

            const QString outDir = browseOutputDir();

            if (outDir.isEmpty()) {

                setBusy(false);

                setProgress(0);

                return;

            }

            setProgress(0.35);

            error = m_engine.exportPdfAsImages(paths.first(), outDir, format);

            if (error.isEmpty())

                rememberOutput(outDir);

        }

        break;

    }

    case 4: {

        const QString out = browseOutputFile(

            QFileInfo(paths.first()).completeBaseName() + QStringLiteral("_compressed.pdf"));

        if (out.isEmpty()) {

            setBusy(false);

            setProgress(0);

            return;

        }

        setProgress(0.35);

        error = m_engine.compressPdf(paths.first(), out, optionValue);

        outputPath = out;

        break;

    }

    case 5: {

        const QString text = extraText.trimmed();

        if (text.isEmpty()) {

            error = QStringLiteral("Watermark text is required");

            break;

        }

        const QString out = browseOutputFile(

            QFileInfo(paths.first()).completeBaseName() + QStringLiteral("_watermarked.pdf"));

        if (out.isEmpty()) {

            setBusy(false);

            setProgress(0);

            return;

        }

        setProgress(0.35);

        error = m_engine.watermarkPdf(paths.first(), out, text, optionValue);

        outputPath = out;

        if (error.isEmpty() && m_settings)
            m_settings->addWatermarkHistory(text);

        break;

    }

    default:

        break;

    }



    setProgress(error.isEmpty() ? 1.0 : 0.0);

    setBusy(false);



    if (error.isEmpty()) {

        if (!outputPath.isEmpty())

            rememberOutput(outputPath);

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


