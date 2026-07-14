#include "redactionregionmodel.h"

#include <QUuid>
#include <algorithm>

RedactionRegionModel::RedactionRegionModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int RedactionRegionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_regions.size();
}

QVariant RedactionRegionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_regions.size())
        return {};

    const Region &r = m_regions.at(index.row());
    switch (role) {
    case IdRole: return r.id;
    case FilePathRole: return r.filePath;
    case PageRole: return r.page;
    case XRole: return r.normalized.x();
    case YRole: return r.normalized.y();
    case WidthRole: return r.normalized.width();
    case HeightRole: return r.normalized.height();
    case LabelRole: return r.label;
    case KindRole: return kindToString(r.kind);
    case SourceRole: return r.source == Source::Auto ? QStringLiteral("auto")
                                                     : QStringLiteral("manual");
    case EnabledRole: return r.enabled;
    default: return {};
    }
}

QHash<int, QByteArray> RedactionRegionModel::roleNames() const
{
    return {
        {IdRole, "regionId"},
        {FilePathRole, "filePath"},
        {PageRole, "page"},
        {XRole, "nx"},
        {YRole, "ny"},
        {WidthRole, "nw"},
        {HeightRole, "nh"},
        {LabelRole, "label"},
        {KindRole, "kind"},
        {SourceRole, "source"},
        {EnabledRole, "enabled"},
    };
}

void RedactionRegionModel::setSelectedIndex(int index)
{
    if (index < -1 || index >= m_regions.size())
        index = -1;
    if (m_selectedIndex == index)
        return;
    m_selectedIndex = index;
    emit selectedIndexChanged();
}

QString RedactionRegionModel::addManualRegion(const QString &filePath, int page,
                                             qreal x, qreal y, qreal w, qreal h)
{
    if (filePath.isEmpty() || page < 1 || w < 0.004 || h < 0.004)
        return {};

    Region r;
    r.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    r.filePath = filePath;
    r.page = page;
    r.normalized = QRectF(x, y, w, h).normalized();
    r.normalized = r.normalized.intersected(QRectF(0, 0, 1, 1));
    r.label = QStringLiteral("手动标记");
    r.kind = Kind::Manual;
    r.source = Source::Manual;
    r.enabled = true;

    const int row = m_regions.size();
    beginInsertRows({}, row, row);
    m_regions.append(r);
    endInsertRows();
    emit countChanged();
    emit regionsChanged();
    setSelectedIndex(row);
    return r.id;
}

void RedactionRegionModel::removeAt(int index)
{
    if (index < 0 || index >= m_regions.size())
        return;
    beginRemoveRows({}, index, index);
    m_regions.removeAt(index);
    endRemoveRows();
    if (m_selectedIndex == index)
        setSelectedIndex(-1);
    else if (m_selectedIndex > index)
        setSelectedIndex(m_selectedIndex - 1);
    emit countChanged();
    emit regionsChanged();
}

void RedactionRegionModel::removeSelected()
{
    removeAt(m_selectedIndex);
}

void RedactionRegionModel::clearForFile(const QString &filePath)
{
    if (filePath.isEmpty())
        return;
    beginResetModel();
    m_regions.erase(std::remove_if(m_regions.begin(), m_regions.end(),
                                    [&](const Region &r) { return r.filePath == filePath; }),
                    m_regions.end());
    endResetModel();
    setSelectedIndex(-1);
    emit countChanged();
    emit regionsChanged();
}

void RedactionRegionModel::clearAll()
{
    if (m_regions.isEmpty())
        return;
    beginResetModel();
    m_regions.clear();
    endResetModel();
    setSelectedIndex(-1);
    emit countChanged();
    emit regionsChanged();
}

void RedactionRegionModel::setRegionRect(int index, qreal x, qreal y, qreal w, qreal h)
{
    if (index < 0 || index >= m_regions.size())
        return;
    QRectF rect(x, y, w, h);
    rect = rect.normalized().intersected(QRectF(0, 0, 1, 1));
    if (rect.width() < 0.004 || rect.height() < 0.004)
        return;
    m_regions[index].normalized = rect;
    emitRegionChanged(index);
    emit regionsChanged();
}

void RedactionRegionModel::setRegionEnabled(int index, bool enabled)
{
    if (index < 0 || index >= m_regions.size())
        return;
    if (m_regions[index].enabled == enabled)
        return;
    m_regions[index].enabled = enabled;
    emitRegionChanged(index);
    emit regionsChanged();
}

void RedactionRegionModel::selectAt(int index)
{
    setSelectedIndex(index);
}

QVariantList RedactionRegionModel::regionsForPage(const QString &filePath, int page) const
{
    QVariantList out;
    for (int i = 0; i < m_regions.size(); ++i) {
        const Region &r = m_regions.at(i);
        if (r.filePath != filePath || r.page != page)
            continue;
        QVariantMap m;
        m.insert(QStringLiteral("index"), i);
        m.insert(QStringLiteral("regionId"), r.id);
        m.insert(QStringLiteral("nx"), r.normalized.x());
        m.insert(QStringLiteral("ny"), r.normalized.y());
        m.insert(QStringLiteral("nw"), r.normalized.width());
        m.insert(QStringLiteral("nh"), r.normalized.height());
        m.insert(QStringLiteral("label"), r.label);
        m.insert(QStringLiteral("kind"), kindToString(r.kind));
        m.insert(QStringLiteral("source"),
                 r.source == Source::Auto ? QStringLiteral("auto") : QStringLiteral("manual"));
        m.insert(QStringLiteral("enabled"), r.enabled);
        m.insert(QStringLiteral("selected"), i == m_selectedIndex);
        out.append(m);
    }
    return out;
}

void RedactionRegionModel::replaceAutoRegions(const QString &filePath,
                                              const QList<Region> &autoRegions)
{
    beginResetModel();
    QList<Region> kept;
    kept.reserve(m_regions.size());
    for (const Region &r : m_regions) {
        if (r.filePath != filePath || r.source == Source::Manual)
            kept.append(r);
    }
    for (Region r : autoRegions) {
        r.filePath = filePath;
        r.source = Source::Auto;
        if (r.id.isEmpty())
            r.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        kept.append(r);
    }
    m_regions = kept;
    endResetModel();
    setSelectedIndex(-1);
    emit countChanged();
    emit regionsChanged();
}

QList<RedactionRegionModel::Region>
RedactionRegionModel::enabledRegionsForFile(const QString &filePath) const
{
    QList<Region> out;
    for (const Region &r : m_regions) {
        if (r.enabled && r.filePath == filePath)
            out.append(r);
    }
    return out;
}

QString RedactionRegionModel::kindToString(Kind kind)
{
    switch (kind) {
    case Kind::Name: return QStringLiteral("name");
    case Kind::IdCard: return QStringLiteral("idCard");
    case Kind::Phone: return QStringLiteral("phone");
    case Kind::Address: return QStringLiteral("address");
    case Kind::Other: return QStringLiteral("other");
    case Kind::Manual: return QStringLiteral("manual");
    }
    return QStringLiteral("other");
}

int RedactionRegionModel::indexOfId(const QString &id) const
{
    for (int i = 0; i < m_regions.size(); ++i) {
        if (m_regions.at(i).id == id)
            return i;
    }
    return -1;
}

void RedactionRegionModel::emitRegionChanged(int row)
{
    const QModelIndex idx = index(row);
    emit dataChanged(idx, idx);
}
