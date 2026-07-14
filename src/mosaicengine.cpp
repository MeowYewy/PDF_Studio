#include "mosaicengine.h"

#include <QPainter>

QImage MosaicEngine::applyRegions(const QImage &source,
                                  const QList<RedactionRegionModel::Region> &regions,
                                  int page,
                                  Style style,
                                  int blockSize,
                                  const QColor &blockColor)
{
    if (source.isNull())
        return source;

    QImage out = source.convertToFormat(QImage::Format_RGB32);
    const int w = out.width();
    const int h = out.height();
    const int bs = qBound(4, blockSize, 64);

    for (const auto &r : regions) {
        if (!r.enabled || r.page != page)
            continue;
        const QRectF n = r.normalized.intersected(QRectF(0, 0, 1, 1));
        if (n.width() <= 0 || n.height() <= 0)
            continue;
        QRect px(int(n.x() * w),
                 int(n.y() * h),
                 qMax(1, int(n.width() * w)),
                 qMax(1, int(n.height() * h)));
        px = px.intersected(out.rect());
        if (!px.isValid() || px.isEmpty())
            continue;

        if (style == Style::SolidBlock)
            fillRect(out, px, blockColor);
        else
            pixelateRect(out, px, bs);
    }
    return out;
}

void MosaicEngine::pixelateRect(QImage &image, const QRect &rect, int blockSize)
{
    if (!rect.isValid() || image.isNull())
        return;
    const QRect area = rect.intersected(image.rect());
    if (area.isEmpty())
        return;

    for (int y = area.top(); y <= area.bottom(); y += blockSize) {
        for (int x = area.left(); x <= area.right(); x += blockSize) {
            const int bw = qMin(blockSize, area.right() - x + 1);
            const int bh = qMin(blockSize, area.bottom() - y + 1);
            qint64 r = 0, g = 0, b = 0;
            int count = 0;
            for (int yy = y; yy < y + bh; ++yy) {
                const QRgb *line = reinterpret_cast<const QRgb *>(image.constScanLine(yy));
                for (int xx = x; xx < x + bw; ++xx) {
                    const QRgb c = line[xx];
                    r += qRed(c);
                    g += qGreen(c);
                    b += qBlue(c);
                    ++count;
                }
            }
            if (count <= 0)
                continue;
            const QRgb avg = qRgb(int(r / count), int(g / count), int(b / count));
            for (int yy = y; yy < y + bh; ++yy) {
                QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(yy));
                for (int xx = x; xx < x + bw; ++xx)
                    line[xx] = avg;
            }
        }
    }
}

void MosaicEngine::fillRect(QImage &image, const QRect &rect, const QColor &color)
{
    QPainter p(&image);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(rect, color);
}
