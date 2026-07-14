#include "piidetector.h"

#include "officetextextractor.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStringConverter>
#include <QTextStream>

#if defined(HAS_QT_PDF)
#  include <QPdfDocument>
#  include <QPdfSelection>
#endif

namespace {

RedactionRegionModel::Region makeRegion(const QString &filePath,
                                        int page,
                                        const QRectF &norm,
                                        const QString &label,
                                        RedactionRegionModel::Kind kind)
{
    RedactionRegionModel::Region r;
    r.filePath = filePath;
    r.page = page;
    r.normalized = norm.normalized().intersected(QRectF(0, 0, 1, 1));
    r.label = label;
    r.kind = kind;
    r.source = RedactionRegionModel::Source::Auto;
    r.enabled = true;
    return r;
}

QRectF lineBand(int lineIndex, int totalLines, qreal startFrac, qreal endFrac)
{
    const int lines = qMax(1, totalLines);
    const qreal lineH = 1.0 / lines;
    const qreal y = qBound(0.0, lineIndex * lineH, 1.0 - lineH * 0.6);
    const qreal x = qBound(0.0, startFrac, 0.95);
    const qreal w = qBound(0.02, endFrac - startFrac, 1.0 - x);
    return QRectF(x, y, w, lineH * 0.85);
}

QList<RedactionRegionModel::Region> scanPlainText(const QString &filePath,
                                                  int page,
                                                  const QString &text)
{
    QList<RedactionRegionModel::Region> found;
    if (text.trimmed().isEmpty())
        return found;

    const QStringList lines = text.split(QRegularExpression(QStringLiteral("[\r\n]+")),
                                         Qt::SkipEmptyParts);
    const int totalLines = qMax(1, lines.size());

    static const QRegularExpression idRe(
        QStringLiteral(R"((?<![0-9A-Za-z])([1-9]\d{5}(?:18|19|20)\d{2}(?:0[1-9]|1[0-2])(?:0[1-9]|[12]\d|3[01])\d{3}[\dXx])(?![0-9A-Za-z]))"));
    static const QRegularExpression phoneRe(
        QStringLiteral(R"((?<!\d)(1[3-9]\d{9})(?!\d))"));
    static const QRegularExpression nameLabeledRe(
        QStringLiteral(R"((?:患者姓名|病人姓名|姓\s*名|患者|病人)\s*[：:\s]\s*([\u4e00-\u9fa5·]{2,8}))"));
    static const QRegularExpression idLabeledRe(
        QStringLiteral(R"((?:身份证号|身份证|证件号|身份证号码)\s*[：:\s]\s*([1-9]\d{16}[\dXx]))"));
    static const QRegularExpression addrLabeledRe(
        QStringLiteral(R"((?:住址|地址|现住址|户籍地址)\s*[：:\s]\s*([^\r\n，。；;]{6,40}))"));

    auto appendMatch = [&](const QString &line, int lineIndex, int matchStart, int matchLen,
                           const QString &label, RedactionRegionModel::Kind kind) {
        const int len = qMax(1, line.size());
        const qreal startFrac = qreal(matchStart) / len;
        const qreal endFrac = qreal(matchStart + matchLen) / len;
        const qreal pad = 0.01;
        found.append(makeRegion(filePath, page,
                                lineBand(lineIndex, totalLines,
                                         qMax(0.0, startFrac - pad),
                                         qMin(1.0, endFrac + pad)),
                                label, kind));
    };

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines.at(i);

        auto idIt = idLabeledRe.globalMatch(line);
        while (idIt.hasNext()) {
            const auto m = idIt.next();
            const QString id = m.captured(1);
            if (PiiDetector::isValidChineseId(id))
                appendMatch(line, i, m.capturedStart(1), m.capturedLength(1),
                            QStringLiteral("身份证号"), RedactionRegionModel::Kind::IdCard);
        }

        auto nameIt = nameLabeledRe.globalMatch(line);
        while (nameIt.hasNext()) {
            const auto m = nameIt.next();
            appendMatch(line, i, m.capturedStart(1), m.capturedLength(1),
                        QStringLiteral("姓名"), RedactionRegionModel::Kind::Name);
        }

        auto addrIt = addrLabeledRe.globalMatch(line);
        while (addrIt.hasNext()) {
            const auto m = addrIt.next();
            appendMatch(line, i, m.capturedStart(1), m.capturedLength(1),
                        QStringLiteral("地址"), RedactionRegionModel::Kind::Address);
        }

        auto phoneIt = phoneRe.globalMatch(line);
        while (phoneIt.hasNext()) {
            const auto m = phoneIt.next();
            appendMatch(line, i, m.capturedStart(1), m.capturedLength(1),
                        QStringLiteral("手机号"), RedactionRegionModel::Kind::Phone);
        }

        auto bareIdIt = idRe.globalMatch(line);
        while (bareIdIt.hasNext()) {
            const auto m = bareIdIt.next();
            const QString id = m.captured(1);
            if (!PiiDetector::isValidChineseId(id))
                continue;
            bool dup = false;
            for (const auto &r : found) {
                if (r.page == page && r.kind == RedactionRegionModel::Kind::IdCard
                    && qAbs(r.normalized.y() - lineBand(i, totalLines, 0, 1).y()) < 0.02)
                    dup = true;
            }
            if (!dup)
                appendMatch(line, i, m.capturedStart(1), m.capturedLength(1),
                            QStringLiteral("身份证号"), RedactionRegionModel::Kind::IdCard);
        }
    }

    return found;
}

QString readTextFile(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    QTextStream in(&f);
    in.setEncoding(QStringConverter::Utf8);
    return in.readAll();
}

} // namespace

PiiDetector::PiiDetector(QObject *parent)
    : QObject(parent)
{
}

bool PiiDetector::isValidChineseId(const QString &id18)
{
    if (id18.size() != 18)
        return false;
    static const int weights[17] = {7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2};
    static const char checks[11] = {'1', '0', 'X', '9', '8', '7', '6', '5', '4', '3', '2'};
    int sum = 0;
    for (int i = 0; i < 17; ++i) {
        const QChar c = id18.at(i);
        if (!c.isDigit())
            return false;
        sum += c.digitValue() * weights[i];
    }
    const QChar expected = QChar(checks[sum % 11]);
    const QChar actual = id18.at(17).toUpper();
    return actual == expected;
}

QList<RedactionRegionModel::Region> PiiDetector::detectInText(const QString &filePath,
                                                              int page,
                                                              const QString &text,
                                                              int) const
{
    return scanPlainText(filePath, page, text);
}

QList<RedactionRegionModel::Region> PiiDetector::detectInImage(const QString &filePath,
                                                               int page,
                                                               const QImage &image) const
{
    Q_UNUSED(image);
    Q_UNUSED(filePath);
    Q_UNUSED(page);
    // Hook: integrate Tesseract / Windows.Media.Ocr, then call detectInText().
    return {};
}

QList<RedactionRegionModel::Region> PiiDetector::detectInFile(const QString &filePath) const
{
    QList<RedactionRegionModel::Region> all;
    const QString ext = QFileInfo(filePath).suffix().toLower();

    if (ext == QLatin1String("docx") || ext == QLatin1String("docm")
        || ext == QLatin1String("odt") || ext == QLatin1String("rtf")
        || ext == QLatin1String("txt") || ext == QLatin1String("md")) {
        QString text;
        if (ext == QLatin1String("txt") || ext == QLatin1String("md"))
            text = readTextFile(filePath);
        else
            text = OfficeTextExtractor::extractPlainText(filePath);
        all += detectInText(filePath, 1, text);
        return all;
    }

    if (ext == QLatin1String("pdf")) {
#if defined(HAS_QT_PDF)
        QPdfDocument doc;
        if (doc.load(filePath) == QPdfDocument::Error::None) {
            const int pages = doc.pageCount();
            for (int p = 0; p < pages; ++p) {
                QString pageText;
#  if defined(QT_PDF_LIB) || 1
                // QPdfDocument::getAllText available in Qt 6.4+
                const QPdfSelection sel = doc.getAllText(p);
                pageText = sel.text();
#  endif
                if (!pageText.isEmpty())
                    all += detectInText(filePath, p + 1, pageText);
            }
        }
#endif
        return all;
    }

    if (ext == QLatin1String("png") || ext == QLatin1String("jpg")
        || ext == QLatin1String("jpeg") || ext == QLatin1String("webp")
        || ext == QLatin1String("bmp") || ext == QLatin1String("tif")
        || ext == QLatin1String("tiff")) {
        QImage img(filePath);
        if (!img.isNull())
            all += detectInImage(filePath, 1, img);
        return all;
    }

    return all;
}
