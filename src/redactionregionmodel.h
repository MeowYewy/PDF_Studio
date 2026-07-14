#pragma once

#include <QAbstractListModel>
#include <QColor>
#include <QRectF>
#include <QUuid>

class RedactionRegionModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int selectedIndex READ selectedIndex WRITE setSelectedIndex NOTIFY selectedIndexChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        FilePathRole,
        PageRole,
        XRole,
        YRole,
        WidthRole,
        HeightRole,
        LabelRole,
        KindRole,
        SourceRole,
        EnabledRole,
    };

    enum class Kind {
        Name,
        IdCard,
        Phone,
        Address,
        Other,
        Manual,
    };
    Q_ENUM(Kind)

    enum class Source {
        Auto,
        Manual,
    };
    Q_ENUM(Source)

    struct Region {
        QString id;
        QString filePath;
        int page = 1;           // 1-based
        QRectF normalized;      // 0..1 relative to page
        QString label;
        Kind kind = Kind::Manual;
        Source source = Source::Manual;
        bool enabled = true;
    };

    explicit RedactionRegionModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int count() const { return m_regions.size(); }
    int selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index);

    Q_INVOKABLE QString addManualRegion(const QString &filePath, int page,
                                        qreal x, qreal y, qreal w, qreal h);
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE void removeSelected();
    Q_INVOKABLE void clearForFile(const QString &filePath);
    Q_INVOKABLE void clearAll();
    Q_INVOKABLE void setRegionRect(int index, qreal x, qreal y, qreal w, qreal h);
    Q_INVOKABLE void setRegionEnabled(int index, bool enabled);
    Q_INVOKABLE void selectAt(int index);
    Q_INVOKABLE QVariantList regionsForPage(const QString &filePath, int page) const;

    void replaceAutoRegions(const QString &filePath, const QList<Region> &autoRegions);
    QList<Region> enabledRegionsForFile(const QString &filePath) const;
    static QString kindToString(Kind kind);

signals:
    void countChanged();
    void selectedIndexChanged();
    void regionsChanged();

private:
    int indexOfId(const QString &id) const;
    void emitRegionChanged(int row);

    QList<Region> m_regions;
    int m_selectedIndex = -1;
};
