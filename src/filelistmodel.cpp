#include "filelistmodel.h"

#include <QFileInfo>
#include <QMimeDatabase>
#include <algorithm>

FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_paths.size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_paths.size())
        return {};

    const QFileInfo info(m_paths.at(index.row()));
    const QString cat = categoryForPath(info.absoluteFilePath());
    switch (role) {
    case PathRole: return info.absoluteFilePath();
    case NameRole: return info.fileName();
    case TypeRole: return QMimeDatabase().mimeTypeForFile(info).name();
    case CategoryRole: return cat;
    case CategoryLabelRole: return categoryLabel(cat);
    default: return {};
    }
}

QHash<int, QByteArray> FileListModel::roleNames() const
{
    return {
        {PathRole, "path"},
        {NameRole, "name"},
        {TypeRole, "mimeType"},
        {CategoryRole, "category"},
        {CategoryLabelRole, "categoryLabel"},
    };
}

QString FileListModel::sortMode() const
{
    return m_sortMode == SortMode::ByCategory ? QStringLiteral("category")
                                              : QStringLiteral("mixed");
}

void FileListModel::setSortMode(const QString &mode)
{
    const SortMode next = (mode == QLatin1String("category")) ? SortMode::ByCategory
                                                              : SortMode::Mixed;
    if (m_sortMode == next)
        return;
    m_sortMode = next;
    if (m_sortMode == SortMode::ByCategory)
        applyCategorySort();
    emit sortModeChanged();
}

QString FileListModel::categoryForPath(const QString &path)
{
    const QString ext = QFileInfo(path).suffix().toLower();
    if (ext == QLatin1String("pdf"))
        return QStringLiteral("pdf");
    if (ext == QLatin1String("png") || ext == QLatin1String("jpg")
        || ext == QLatin1String("jpeg") || ext == QLatin1String("webp")
        || ext == QLatin1String("bmp") || ext == QLatin1String("tif")
        || ext == QLatin1String("tiff"))
        return QStringLiteral("image");
    if (ext == QLatin1String("docx") || ext == QLatin1String("docm")
        || ext == QLatin1String("odt") || ext == QLatin1String("rtf")
        || ext == QLatin1String("doc"))
        return QStringLiteral("office");
    return QStringLiteral("other");
}

QString FileListModel::categoryLabel(const QString &category)
{
    if (category == QLatin1String("pdf"))
        return QStringLiteral("PDF");
    if (category == QLatin1String("image"))
        return QStringLiteral("图片");
    if (category == QLatin1String("office"))
        return QStringLiteral("文档");
    return QStringLiteral("其他");
}

QString FileListModel::categoryOf(const QString &path) const
{
    return categoryForPath(path);
}

void FileListModel::addFiles(const QStringList &paths)
{
    QStringList added;
    for (const QString &p : paths) {
        const QString resolved = QFileInfo(p).absoluteFilePath();
        const QString cat = categoryForPath(resolved);
        if (cat == QLatin1String("other"))
            continue; // only accept pdf / images / office
        if (!m_paths.contains(resolved))
            added.append(resolved);
    }
    if (added.isEmpty())
        return;

    const int first = m_paths.size();
    beginInsertRows({}, first, first + added.size() - 1);
    m_paths.append(added);
    endInsertRows();

    if (m_sortMode == SortMode::ByCategory)
        applyCategorySort();

    emit countChanged();
    emit filesChanged();
}

void FileListModel::removeAt(int index)
{
    if (index < 0 || index >= m_paths.size())
        return;
    beginRemoveRows({}, index, index);
    m_paths.removeAt(index);
    endRemoveRows();
    emit countChanged();
    emit filesChanged();
}

void FileListModel::clear()
{
    if (m_paths.isEmpty())
        return;
    beginResetModel();
    m_paths.clear();
    endResetModel();
    emit countChanged();
    emit filesChanged();
}

void FileListModel::move(int from, int to)
{
    if (from < 0 || from >= m_paths.size() || to < 0 || to >= m_paths.size() || from == to)
        return;
    const int dest = to > from ? to + 1 : to;
    beginMoveRows({}, from, from, {}, dest);
    m_paths.move(from, to);
    endMoveRows();
    // Manual drag implies mixed order preference
    if (m_sortMode != SortMode::Mixed) {
        m_sortMode = SortMode::Mixed;
        emit sortModeChanged();
    }
    emit filesChanged();
}

void FileListModel::sortByCategory()
{
    m_sortMode = SortMode::ByCategory;
    applyCategorySort();
    emit sortModeChanged();
}

void FileListModel::applyCategorySort()
{
    if (m_paths.size() < 2)
        return;

    static const QStringList order = {
        QStringLiteral("pdf"),
        QStringLiteral("image"),
        QStringLiteral("office"),
        QStringLiteral("other"),
    };

    QStringList sorted = m_paths;
    std::stable_sort(sorted.begin(), sorted.end(), [](const QString &a, const QString &b) {
        return order.indexOf(categoryForPath(a)) < order.indexOf(categoryForPath(b));
    });

    if (sorted == m_paths)
        return;

    beginResetModel();
    m_paths = sorted;
    endResetModel();
    emit filesChanged();
}
