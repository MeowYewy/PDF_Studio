#pragma once

#include "redactionregionmodel.h"

#include <QImage>
#include <QObject>
#include <QString>

/**
 * Local PII detector for medical-style documents.
 * - Text docs (PDF text layer / DOCX): regex + keyword heuristics with approximate boxes
 * - Raster pages: optional external OCR hook; without OCR, returns empty auto list
 *   (manual marking still works). Identity card numbers use GB 11643 checksum.
 */
class PiiDetector : public QObject
{
    Q_OBJECT

public:
    explicit PiiDetector(QObject *parent = nullptr);

    QList<RedactionRegionModel::Region> detectInFile(const QString &filePath) const;
    QList<RedactionRegionModel::Region> detectInText(const QString &filePath,
                                                     int page,
                                                     const QString &text,
                                                     int approxCharsPerLine = 40) const;
    QList<RedactionRegionModel::Region> detectInImage(const QString &filePath,
                                                      int page,
                                                      const QImage &image) const;

    static bool isValidChineseId(const QString &id18);
};
