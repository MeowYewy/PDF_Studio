#pragma once

#include "redactionregionmodel.h"

#include <QObject>
#include <QString>
#include <functional>

class DesensitizeEngine
{
public:
    using ProgressFn = std::function<void(double)>;

    static bool exportFile(const QString &inputPath,
                           const QString &outputPath,
                           const QList<RedactionRegionModel::Region> &regions,
                           bool useSolidBlock,
                           int mosaicBlockSize,
                           QString *errorMessage = nullptr,
                           const ProgressFn &progress = {});
};
