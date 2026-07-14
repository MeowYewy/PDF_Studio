#pragma once

#include "redactionregionmodel.h"

#include <QImage>
#include <QObject>

class MosaicEngine
{
public:
    enum class Style {
        Pixelate,   // irreversible pixel mosaic
        SolidBlock, // solid color block
    };

    static QImage applyRegions(const QImage &source,
                               const QList<RedactionRegionModel::Region> &regions,
                               int page,
                               Style style = Style::Pixelate,
                               int blockSize = 14,
                               const QColor &blockColor = QColor(32, 32, 32));

    static void pixelateRect(QImage &image, const QRect &rect, int blockSize);
    static void fillRect(QImage &image, const QRect &rect, const QColor &color);
};
