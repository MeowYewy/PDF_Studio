#pragma once

#include <QAbstractListModel>
#include <QStringList>

class FileListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString sortMode READ sortMode WRITE setSortMode NOTIFY sortModeChanged)

public:
    enum Roles {
        PathRole = Qt::UserRole + 1,
        NameRole,
        TypeRole,
        CategoryRole,
        CategoryLabelRole,
    };

    enum class SortMode {
        Mixed,      // free order (default)
        ByCategory, // group PDF / Image / Office, keep relative order within group
    };
    Q_ENUM(SortMode)

    explicit FileListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int count() const { return m_paths.size(); }

    QString sortMode() const;
    void setSortMode(const QString &mode);

    Q_INVOKABLE void addFiles(const QStringList &paths);
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE void sortByCategory();
    Q_INVOKABLE QString categoryOf(const QString &path) const;
    QStringList paths() const { return m_paths; }

    static QString categoryForPath(const QString &path);
    static QString categoryLabel(const QString &category);

signals:
    void countChanged();
    void filesChanged();
    void sortModeChanged();

private:
    void applyCategorySort();

    QStringList m_paths;
    SortMode m_sortMode = SortMode::Mixed;
};
