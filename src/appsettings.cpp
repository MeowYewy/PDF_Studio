#include "appsettings.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QHash>

namespace {

QHash<QString, QHash<QString, QString>> buildStrings()
{
    QHash<QString, QHash<QString, QString>> all;

    all["zh_CN"] = {
        {"appName", "ProjectO 脱敏工具"},
        {"dropHint", "拖放 PDF / 图片 / Word 到此处，或点击浏览"},
        {"browse", "浏览文件"},
        {"clear", "清空"},
        {"export", "导出脱敏"},
        {"confirmAnalyze", "确认并识别"},
        {"preview", "脱敏预览"},
        {"previewHint", "添加文件后点击「确认并识别」，右侧将显示脱敏预览"},
        {"noFiles", "尚未添加文件"},
        {"language", "语言"},
        {"theme", "界面"},
        {"light", "浅色"},
        {"dark", "深色"},
        {"about", "关于"},
        {"aboutTitle", "关于 ProjectO"},
        {"aboutTagline", "本地医疗文档脱敏工具"},
        {"close", "关闭"},
        {"fileList", "文件"},
        {"addedFiles", "已添加"},
        {"version", "版本"},
        {"aboutAuthor", "MeowYewy"},
        {"aboutCopyright", "版权所有"},
        {"checkUpdate", "检查更新"},
        {"newVersion", "新版本"},
        {"updateNow", "立即更新"},
        {"downloadingUpdate", "下载中"},
        {"upToDate", "已是最新"},
        {"updateFailed", "检查失败"},
        {"changelog", "更新日志"},
        {"changelogTitle", "更新日志"},
        {"changelogEmpty", "暂无更新记录"},
        {"success", "已完成"},
        {"failed", "处理失败"},
        {"pages", "页"},
        {"sortMode", "排序"},
        {"sortMixed", "混排"},
        {"sortCategory", "分类"},
        {"workspaceDesc", "添加文件 → 调整顺序 → 确认识别敏感信息 → 核对/手动画框 → 导出"},
        {"workspaceDescReady", "已生成脱敏预览。可开启手动模式增删改马赛克区域，然后导出。"},
        {"styleMosaic", "马赛克"},
        {"styleBlock", "色块"},
        {"manualOn", "手动中"},
        {"manualOff", "手动标记"},
        {"deleteMark", "删除标记"},
        {"regionsCount", "标记 %1 处"},
    };

    all["zh_TW"] = {
        {"appName", "ProjectO 脫敏工具"},
        {"dropHint", "拖放 PDF / 圖片 / Word 到此處，或點擊瀏覽"},
        {"browse", "瀏覽檔案"},
        {"clear", "清空"},
        {"export", "匯出脫敏"},
        {"confirmAnalyze", "確認並識別"},
        {"preview", "脫敏預覽"},
        {"previewHint", "新增檔案後點擊「確認並識別」，右側將顯示脫敏預覽"},
        {"noFiles", "尚未新增檔案"},
        {"language", "語言"},
        {"theme", "介面"},
        {"light", "淺色"},
        {"dark", "深色"},
        {"about", "關於"},
        {"aboutTitle", "關於 ProjectO"},
        {"aboutTagline", "本機醫療文件脫敏工具"},
        {"close", "關閉"},
        {"fileList", "檔案"},
        {"addedFiles", "已新增"},
        {"version", "版本"},
        {"aboutAuthor", "MeowYewy"},
        {"aboutCopyright", "版權所有"},
        {"checkUpdate", "檢查更新"},
        {"newVersion", "新版本"},
        {"updateNow", "立即更新"},
        {"downloadingUpdate", "下載中"},
        {"upToDate", "已是最新版本"},
        {"updateFailed", "檢查失敗"},
        {"changelog", "更新日誌"},
        {"changelogTitle", "更新日誌"},
        {"changelogEmpty", "暫無更新記錄"},
        {"success", "已完成"},
        {"failed", "處理失敗"},
        {"pages", "頁"},
        {"sortMode", "排序"},
        {"sortMixed", "混排"},
        {"sortCategory", "分類"},
        {"workspaceDesc", "新增檔案 → 調整順序 → 確認識別敏感資訊 → 核對/手動畫框 → 匯出"},
        {"workspaceDescReady", "已產生脫敏預覽。可開啟手動模式增刪改馬賽克區域，然後匯出。"},
        {"styleMosaic", "馬賽克"},
        {"styleBlock", "色塊"},
        {"manualOn", "手動中"},
        {"manualOff", "手動標記"},
        {"deleteMark", "刪除標記"},
        {"regionsCount", "標記 %1 處"},
    };

    all["en"] = {
        {"appName", "ProjectO Redactor"},
        {"dropHint", "Drop PDF / images / Word here, or browse"},
        {"browse", "Browse"},
        {"clear", "Clear"},
        {"export", "Export"},
        {"confirmAnalyze", "Confirm & Detect"},
        {"preview", "Redacted Preview"},
        {"previewHint", "Add files, then tap Confirm & Detect to preview redactions"},
        {"noFiles", "No files added yet"},
        {"language", "Language"},
        {"theme", "Appearance"},
        {"light", "Light"},
        {"dark", "Dark"},
        {"about", "About"},
        {"aboutTitle", "About ProjectO"},
        {"aboutTagline", "Local medical document redaction"},
        {"close", "Close"},
        {"fileList", "Files"},
        {"addedFiles", "Added"},
        {"version", "Version"},
        {"aboutAuthor", "MeowYewy"},
        {"aboutCopyright", "All rights reserved"},
        {"checkUpdate", "Check for Updates"},
        {"newVersion", "New"},
        {"updateNow", "Update"},
        {"downloadingUpdate", "Downloading"},
        {"upToDate", "Up to date"},
        {"updateFailed", "Check failed"},
        {"changelog", "Changelog"},
        {"changelogTitle", "Changelog"},
        {"changelogEmpty", "No release notes yet"},
        {"success", "Completed"},
        {"failed", "Failed"},
        {"pages", "pages"},
        {"sortMode", "Order"},
        {"sortMixed", "Mixed"},
        {"sortCategory", "By type"},
        {"workspaceDesc", "Add files → reorder → detect PII → review/edit → export"},
        {"workspaceDescReady", "Preview ready. Toggle manual mode to add/resize/delete regions, then export."},
        {"styleMosaic", "Mosaic"},
        {"styleBlock", "Block"},
        {"manualOn", "Manual ON"},
        {"manualOff", "Manual"},
        {"deleteMark", "Delete mark"},
        {"regionsCount", "%1 marks"},
    };

    all["fr"] = {
        {"appName", "ProjectO Redaction"},
        {"dropHint", "Déposez PDF / images / Word ici, ou parcourez"},
        {"browse", "Parcourir"},
        {"clear", "Effacer"},
        {"export", "Exporter"},
        {"confirmAnalyze", "Confirmer"},
        {"preview", "Aperçu masqué"},
        {"previewHint", "Ajoutez des fichiers, puis confirmez pour détecter"},
        {"noFiles", "Aucun fichier ajouté"},
        {"language", "Langue"},
        {"theme", "Apparence"},
        {"light", "Clair"},
        {"dark", "Sombre"},
        {"about", "À propos"},
        {"aboutTitle", "À propos de ProjectO"},
        {"aboutTagline", "Anonymisation locale de documents médicaux"},
        {"close", "Fermer"},
        {"fileList", "Fichiers"},
        {"addedFiles", "Ajoutés"},
        {"version", "Version"},
        {"aboutAuthor", "MeowYewy"},
        {"aboutCopyright", "Tous droits réservés"},
        {"checkUpdate", "Vérifier les mises à jour"},
        {"newVersion", "Nouveau"},
        {"updateNow", "Mettre à jour"},
        {"downloadingUpdate", "Téléchargement"},
        {"upToDate", "À jour"},
        {"updateFailed", "Échec"},
        {"changelog", "Journal"},
        {"changelogTitle", "Journal des mises à jour"},
        {"changelogEmpty", "Aucune note de version"},
        {"success", "Terminé"},
        {"failed", "Échec"},
        {"pages", "pages"},
        {"sortMode", "Ordre"},
        {"sortMixed", "Mixte"},
        {"sortCategory", "Par type"},
        {"workspaceDesc", "Ajouter → réordonner → détecter → vérifier → exporter"},
        {"workspaceDescReady", "Aperçu prêt. Mode manuel pour ajouter/redimensionner/supprimer."},
        {"styleMosaic", "Mosaïque"},
        {"styleBlock", "Bloc"},
        {"manualOn", "Manuel ON"},
        {"manualOff", "Manuel"},
        {"deleteMark", "Supprimer"},
        {"regionsCount", "%1 zones"},
    };

    return all;
}

} // namespace

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
    , m_store(QStringLiteral("TechG"), QStringLiteral("ProjectO"))
    , m_language(m_store.value(QStringLiteral("language"), QStringLiteral("zh_CN")).toString())
    , m_theme(m_store.value(QStringLiteral("theme"), QStringLiteral("light")).toString())
{
    loadTranslations(m_language);
    loadWatermarkHistory();
}

void AppSettings::setLanguage(const QString &lang)
{
    if (m_language == lang)
        return;
    m_language = lang;
    m_store.setValue(QStringLiteral("language"), lang);
    ++m_languageRevision;
    loadTranslations(lang);
    emit languageChanged();
}

void AppSettings::setTheme(const QString &theme)
{
    if (m_theme == theme)
        return;
    m_theme = theme;
    m_store.setValue(QStringLiteral("theme"), theme);
    ++m_themeRevision;
    emit themeChanged();
}

QString AppSettings::trKey(const QString &key) const
{
    static const auto strings = buildStrings();
    const auto langMap = strings.value(m_language, strings.value(QStringLiteral("zh_CN")));
    return langMap.value(key, key);
}

QString AppSettings::lastOutputDir() const
{
    return m_store.value(QStringLiteral("lastOutputDir")).toString();
}

void AppSettings::setLastOutputDir(const QString &dir)
{
    const QString normalized = dir.trimmed();
    if (lastOutputDir() == normalized)
        return;
    m_store.setValue(QStringLiteral("lastOutputDir"), normalized);
    emit lastOutputDirChanged();
}

void AppSettings::rememberOutputPath(const QString &fileOrDir)
{
    const QString trimmed = fileOrDir.trimmed();
    if (trimmed.isEmpty())
        return;

    QFileInfo info(trimmed);
    const QString dir = info.isDir() ? info.absoluteFilePath() : info.absolutePath();
    if (!dir.isEmpty())
        setLastOutputDir(dir);
}

void AppSettings::loadWatermarkHistory()
{
    m_watermarkHistory = m_store.value(QStringLiteral("watermarkHistory")).toStringList();
    while (m_watermarkHistory.size() > 3)
        m_watermarkHistory.removeLast();
}

void AppSettings::saveWatermarkHistory()
{
    m_store.setValue(QStringLiteral("watermarkHistory"), m_watermarkHistory);
}

void AppSettings::addWatermarkHistory(const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty())
        return;

    m_watermarkHistory.removeAll(trimmed);
    m_watermarkHistory.prepend(trimmed);
    while (m_watermarkHistory.size() > 3)
        m_watermarkHistory.removeLast();

    saveWatermarkHistory();
    ++m_watermarkHistoryRevision;
    emit watermarkHistoryChanged();
}

void AppSettings::removeWatermarkHistoryAt(int index)
{
    if (index < 0 || index >= m_watermarkHistory.size())
        return;

    m_watermarkHistory.removeAt(index);
    saveWatermarkHistory();
    ++m_watermarkHistoryRevision;
    emit watermarkHistoryChanged();
}

void AppSettings::loadTranslations(const QString &)
{
}
