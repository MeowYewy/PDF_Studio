#include "pdfpagerenderer.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QRegularExpression>

#include <climits>

#include <algorithm>

#ifdef HAS_QT_PDF
#include <QPdfDocument>
#endif

namespace {

QMap<int, QString> collectPopplerOutputs(const QDir &dir, const QString &prefix,
                                         int startPage, int endPage)
{
    QMap<int, QString> outputs;

    const QStringList patterns = {
        prefix + QStringLiteral("-*.png"),
        prefix + QStringLiteral(".png"),
    };

    const QStringList files = dir.entryList(patterns, QDir::Files, QDir::Name);
    for (const QString &file : files) {
        const QString fullPath = dir.filePath(file);

        if (file == prefix + QStringLiteral(".png")) {
            if (startPage == endPage)
                outputs.insert(startPage, fullPath);
            continue;
        }

        const int dash = file.lastIndexOf(QLatin1Char('-'));
        if (dash < 0)
            continue;

        const QString pageToken = file.mid(dash + 1);
        if (!pageToken.endsWith(QStringLiteral(".png"), Qt::CaseInsensitive))
            continue;

        bool ok = false;
        const int page = pageToken.left(pageToken.size() - 4).toInt(&ok);
        if (!ok || page < startPage || page > endPage)
            continue;

        outputs.insert(page, fullPath);
    }

    return outputs;
}

void configurePopplerProcess(QProcess &proc, const QString &pdftoppm)
{
    proc.setProgram(pdftoppm);

    const QFileInfo toolInfo(pdftoppm);
    if (toolInfo.exists()) {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        const QString toolDir = QDir::toNativeSeparators(toolInfo.absolutePath());
        env.insert(QStringLiteral("PATH"),
                   toolDir + QLatin1Char(';') + env.value(QStringLiteral("PATH")));
        proc.setProcessEnvironment(env);
    }
}

bool runPopplerRange(const QString &pdftoppm, const QString &pdfPath,
                     const QString &outputDir, const QString &prefix,
                     int dpi, int startPage, int endPage,
                     QMap<int, QString> *outputs)
{
    const QString outPrefix = outputDir + QLatin1Char('/') + prefix;
    QProcess proc;
    configurePopplerProcess(proc, pdftoppm);

    const QStringList args = {
        QStringLiteral("-png"),
        QStringLiteral("-r"), QString::number(dpi),
        QStringLiteral("-f"), QString::number(startPage),
        QStringLiteral("-l"), QString::number(endPage),
        pdfPath,
        outPrefix,
    };
    proc.setArguments(args);
    proc.setWorkingDirectory(outputDir);
    proc.start();
    if (!proc.waitForFinished(300000) || proc.exitCode() != 0)
        return false;

    const QMap<int, QString> found = collectPopplerOutputs(QDir(outputDir), prefix, startPage, endPage);
    for (auto it = found.cbegin(); it != found.cend(); ++it)
        outputs->insert(it.key(), it.value());

    return !found.isEmpty();
}

int pageNumberFromPath(const QString &path)
{
    const QFileInfo info(path);
    const QString base = info.completeBaseName();
    const int dash = base.lastIndexOf(QLatin1Char('-'));
    if (dash < 0)
        return 1;

    bool ok = false;
    const int page = base.mid(dash + 1).toInt(&ok);
    return ok ? page : 1;
}

} // namespace

QString PdfPageRenderer::findPdftoppm()
{
    const QStringList candidates = {
        QDir(QCoreApplication::applicationDirPath())
            .filePath(QStringLiteral("tools/poppler/pdftoppm.exe")),
        QDir(QCoreApplication::applicationDirPath())
            .filePath(QStringLiteral("../tools/poppler/pdftoppm.exe")),
        QStringLiteral("pdftoppm"),
    };

    for (const QString &path : candidates) {
        if (path == QStringLiteral("pdftoppm"))
            return path;
        if (QFileInfo::exists(path))
            return path;
    }
    return {};
}

bool PdfPageRenderer::hasQtPdf()
{
#ifdef HAS_QT_PDF
    return true;
#else
    return false;
#endif
}

QImage PdfPageRenderer::renderPageWithQt(const QString &pdfPath, int pageIndex, int dpi)
{
#ifdef HAS_QT_PDF
    QPdfDocument doc;
    if (doc.load(pdfPath) != QPdfDocument::Error::None)
        return {};

    const QSizeF pageSize = doc.pagePointSize(pageIndex);
    if (pageSize.isEmpty())
        return {};

    const qreal scale = dpi / 72.0;
    const QSize imageSize(qMax(1, int(pageSize.width() * scale)),
                          qMax(1, int(pageSize.height() * scale)));
    return doc.render(pageIndex, imageSize);
#else
    Q_UNUSED(pdfPath)
    Q_UNUSED(pageIndex)
    Q_UNUSED(dpi)
    return {};
#endif
}

QStringList PdfPageRenderer::renderPdfPages(const QString &pdfPath, const QString &outputDir,
                                            const QString &prefix, int dpi,
                                            int firstPage, int lastPage)
{
    QDir().mkpath(outputDir);

    const int startIndex = qMax(1, firstPage);
    const int endIndex = lastPage < 0 ? startIndex : qMax(startIndex, lastPage);

#ifdef HAS_QT_PDF
    QPdfDocument doc;
    if (doc.load(pdfPath) == QPdfDocument::Error::None) {
        QStringList paths;
        const int count = doc.pageCount();
        const int last = qMin(endIndex, count);

        // Render from the already-loaded document; reloading it per page made
        // large files extremely slow.
        for (int page = startIndex; page <= last; ++page) {
            const int i = page - 1;
            const QSizeF pageSize = doc.pagePointSize(i);
            if (pageSize.isEmpty())
                continue;

            const qreal scale = dpi / 72.0;
            const QSize imageSize(qMax(1, int(pageSize.width() * scale)),
                                  qMax(1, int(pageSize.height() * scale)));
            const QImage image = doc.render(i, imageSize);
            if (image.isNull())
                continue;

            const QString dest = outputDir + QLatin1Char('/')
                + prefix + QLatin1Char('-') + QString::number(page) + QStringLiteral(".png");
            if (image.save(dest))
                paths.append(dest);
        }
        if (!paths.isEmpty())
            return paths;
    }
#endif

    const QString pdftoppm = findPdftoppm();
    if (pdftoppm.isEmpty())
        return {};

    QMap<int, QString> outputs;

    // Render contiguous runs of missing pages in a single pdftoppm call —
    // one process per page was a major slowdown for large documents.
    int page = startIndex;
    while (page <= endIndex) {
        if (outputs.contains(page)) {
            ++page;
            continue;
        }
        int runEnd = page;
        while (runEnd + 1 <= endIndex && !outputs.contains(runEnd + 1))
            ++runEnd;
        runPopplerRange(pdftoppm, pdfPath, outputDir, prefix, dpi, page, runEnd, &outputs);
        page = runEnd + 1;
    }

    QStringList paths;
    paths.reserve(outputs.size());
    for (auto it = outputs.cbegin(); it != outputs.cend(); ++it)
        paths.append(it.value());

    std::sort(paths.begin(), paths.end(), [](const QString &a, const QString &b) {
        return pageNumberFromPath(a) < pageNumberFromPath(b);
    });

    return paths;
}
