#include "desensitizeengine.h"

#include "mosaicengine.h"
#include "pdfpagerenderer.h"

#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QImageWriter>
#include <QPainter>
#include <QPdfWriter>
#include <QStandardPaths>
#include <QUuid>

#if defined(HAS_QT_PDF)
#  include <QPdfDocument>
#endif

namespace {

bool writeImage(const QImage &img, const QString &path, QString *err)
{
    QImageWriter writer(path);
    if (!writer.write(img)) {
        if (err)
            *err = writer.errorString();
        return false;
    }
    return true;
}

bool pagesToPdf(const QList<QImage> &pages, const QString &outPath, QString *err)
{
    if (pages.isEmpty()) {
        if (err)
            *err = QStringLiteral("No pages to export");
        return false;
    }

    QPdfWriter writer(outPath);
    writer.setTitle(QStringLiteral("ProjectO Desensitized"));
    writer.setPageMargins(QMarginsF(0, 0, 0, 0));

    QPainter painter;
    bool started = false;
    for (int i = 0; i < pages.size(); ++i) {
        const QImage &img = pages.at(i);
        if (img.isNull())
            continue;

        const QPageSize pageSize(QSizeF(img.width() * 25.4 / 140.0,
                                        img.height() * 25.4 / 140.0),
                                 QPageSize::Millimeter);
        writer.setPageSize(pageSize);
        if (!started) {
            if (!painter.begin(&writer)) {
                if (err)
                    *err = QStringLiteral("Failed to start PDF writer");
                return false;
            }
            started = true;
        } else {
            writer.newPage();
        }
        painter.drawImage(QRect(0, 0, writer.width(), writer.height()), img);
    }
    if (!started) {
        if (err)
            *err = QStringLiteral("No valid pages");
        return false;
    }
    painter.end();
    return true;
}

QList<QImage> loadRenderedPdf(const QString &pdfPath, QString *err)
{
    const QString cache = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                          + QStringLiteral("/ProjectO_render_")
                          + QUuid::createUuid().toString(QUuid::WithoutBraces);
    QDir().mkpath(cache);
    QStringList allPaths;

    // Prefer Qt Pdf for accurate page enumeration + render
    QList<QImage> images;
#ifdef HAS_QT_PDF
    {
        QPdfDocument doc;
        if (doc.load(pdfPath) == QPdfDocument::Error::None) {
            const int count = doc.pageCount();
            for (int i = 0; i < count; ++i) {
                QImage img = PdfPageRenderer::renderPageWithQt(pdfPath, i, 140);
                if (!img.isNull())
                    images.append(img);
            }
        }
    }
#endif
    if (images.isEmpty()) {
        allPaths = PdfPageRenderer::renderPdfPages(
            pdfPath, cache, QStringLiteral("p"), 140, 1, 9999);
        for (const QString &p : allPaths) {
            QImage img(p);
            if (!img.isNull())
                images.append(img);
        }
    }

    if (images.isEmpty() && err)
        *err = QStringLiteral("Failed to render PDF pages");

    QDir(cache).removeRecursively();
    return images;
}

} // namespace

bool DesensitizeEngine::exportFile(const QString &inputPath,
                                   const QString &outputPath,
                                   const QList<RedactionRegionModel::Region> &regions,
                                   bool useSolidBlock,
                                   int mosaicBlockSize,
                                   QString *errorMessage,
                                   const ProgressFn &progress)
{
    const QFileInfo inInfo(inputPath);
    const QString ext = inInfo.suffix().toLower();
    const auto style = useSolidBlock ? MosaicEngine::Style::SolidBlock
                                     : MosaicEngine::Style::Pixelate;

    auto report = [&](double v) {
        if (progress)
            progress(v);
    };

    report(0.05);

    QList<QImage> pages;

    if (ext == QLatin1String("png") || ext == QLatin1String("jpg")
        || ext == QLatin1String("jpeg") || ext == QLatin1String("webp")
        || ext == QLatin1String("bmp") || ext == QLatin1String("tif")
        || ext == QLatin1String("tiff")) {
        QImage img(inputPath);
        if (img.isNull()) {
            if (errorMessage)
                *errorMessage = QStringLiteral("Cannot open image");
            return false;
        }
        pages = {img};
    } else if (ext == QLatin1String("pdf")) {
        pages = loadRenderedPdf(inputPath, errorMessage);
        if (pages.isEmpty())
            return false;
    } else {
        // DOCX and others: export requires a rasterized preview image supplied later.
        // For v0.1 generate a single white page and burn regions (manual boxes still valid
        // relative to preview once user confirms on preview). Prefer PDF/image inputs.
        if (errorMessage)
            *errorMessage = QStringLiteral(
                "当前版本导出优先支持 PDF / PNG / JPEG；Office 请先在预览中确认后导出为 PDF");
        QImage page(1240, 1754, QImage::Format_RGB32);
        page.fill(QColor(252, 252, 253));
        pages = {page};
    }

    QList<QImage> outPages;
    outPages.reserve(pages.size());
    for (int i = 0; i < pages.size(); ++i) {
        outPages.append(MosaicEngine::applyRegions(pages.at(i), regions, i + 1,
                                                   style, mosaicBlockSize));
        report(0.1 + 0.75 * double(i + 1) / pages.size());
    }

    const QString outExt = QFileInfo(outputPath).suffix().toLower();
    if (outExt == QLatin1String("png") || outExt == QLatin1String("jpg")
        || outExt == QLatin1String("jpeg") || outExt == QLatin1String("webp")) {
        if (!writeImage(outPages.first(), outputPath, errorMessage))
            return false;
    } else {
        QString pdfPath = outputPath;
        if (outExt.isEmpty())
            pdfPath += QStringLiteral(".pdf");
        else if (outExt != QLatin1String("pdf")) {
            pdfPath = QFileInfo(outputPath).path() + QLatin1Char('/')
                      + QFileInfo(outputPath).completeBaseName() + QStringLiteral(".pdf");
        }
        if (!pagesToPdf(outPages, pdfPath, errorMessage))
            return false;
    }

    report(1.0);
    return true;
}
