#include "appsettings.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QHash>

namespace {

QHash<QString, QHash<QString, QString>> buildStrings()
{
    QHash<QString, QHash<QString, QString>> all;

    all["zh_CN"] = {
        {"appName", "PageCase"},
        {"tabSplit", "拆分"},
        {"tabMerge", "合并"},
        {"tabRotate", "旋转"},
        {"tabConvert", "转换"},
        {"dropHint", "拖放文件到此处或点击浏览文件"},
        {"browse", "浏览文件"},
        {"run", "开始处理"},
        {"clear", "清空"},
        {"preview", "文件预览"},
        {"noFiles", "尚未添加文件"},
        {"previewLoadFailed", "无法加载预览"},
        {"filesAdded", "个文件已添加"},
        {"language", "语言"},
        {"theme", "界面"},
        {"light", "浅色"},
        {"dark", "深色"},
        {"about", "关于"},
        {"aboutTitle", "关于 PageCase"},
        {"aboutTagline", "简洁优雅的PDF工具"},
        {"close", "关闭"},
        {"fileList", "文件"},
        {"addedFiles", "已添加"},
        {"version", "版本"},
        {"aboutAuthor", "MeowYewy"},
        {"aboutCopyright", "版权所有"},
        {"checkUpdate", "检查更新"},
        {"newVersion", "新版本"},
        {"installUpdate", "安装"},
        {"installConfirmTitle", "安装更新"},
        {"installConfirmMessage", "未保存的内容将会丢失，是否继续？"},
        {"updateNow", "立即更新"},
        {"downloadingUpdate", "下载中"},
        {"upToDate", "已是最新"},
        {"updateFailed", "检查失败"},
        {"changelog", "更新日志"},
        {"changelogTitle", "更新日志"},
        {"changelogEmpty", "暂无更新记录"},
        {"splitDesc", "将多页 PDF 拆分为单页"},
        {"mergeDesc", "将多个 PDF 合并为一个文件"},
        {"rotateDesc", "旋转PDF页面至对应方向"},
        {"convertDesc", "在 PDF 与 Word、图片等格式间转换"},
        {"tabCompress", "压缩"},
        {"tabWatermark", "水印"},
        {"compressDesc", "减小 PDF 文件体积"},
        {"watermarkDesc", "为 PDF 每一页添加文字水印"},
        {"watermarkPlaceholder", "水印文字"},
        {"formatPdf", "PDF"},
        {"formatPng", "PNG"},
        {"formatJpeg", "JPEG"},
        {"formatWord", "Word"},
        {"pageRangeInvalid", "页码格式无效"},
        {"needPdf", "该文件类型不支持此操作"},
        {"compressLow", "标准"},
        {"compressMid", "较高"},
        {"compressHigh", "最高"},
        {"success", "已完成"},
        {"failed", "处理失败"},
        {"pages", "页"},
        {"pickerOpenTitle", "选择文件"},
        {"pickerSaveTitle", "保存文件"},
        {"pickerFolderTitle", "选择文件夹"},
        {"pickerCancel", "取消"},
        {"pickerOpen", "打开"},
        {"pickerSave", "保存"},
        {"pickerSelectFolder", "开始处理"},
        {"pickerFileName", "文件名"},
        {"pickerFileType", "文件类型"},
        {"pickerName", "名称"},
        {"pickerSize", "大小"},
        {"pickerModified", "修改时间"},
        {"pickerNewFolder", "新建文件夹"},
        {"pickerShowHidden", "显示隐藏项"},
        {"pickerUp", "上一级"},
        {"pickerRefresh", "刷新"},
        {"pickerPlaceHome", "当前"},
        {"pickerPlaceDesktop", "桌面"},
        {"pickerPlaceDocuments", "文档"},
        {"pickerPlaceDownloads", "下载"},
        {"pickerPlaceComputer", "此电脑"},
        {"pickerPathMissing", "无法访问此路径"},
        {"pickerInvalidFolderName", "文件夹名称无效"},
        {"pickerCreateFolderFailed", "无法创建文件夹"},
        {"pickerFolderExistsTitle", "名称已存在"},
        {"pickerFolderExistsMessage", "当前位置已有同名文件夹"},
        {"pickerFileExistsMessage", "当前位置已有同名文件"},
        {"splitNumLower", "一"},
        {"splitNumUpper", "壹"},
        {"pickerOk", "确定"},
        {"pickerCopy", "复制"},
        {"pickerCut", "剪切"},
        {"pickerPaste", "粘贴"},
        {"pickerRename", "重命名"},
        {"pickerDelete", "删除"},
        {"pickerDeleteTitle", "确认删除"},
        {"pickerDeleteMessageOne", "确定将「%1」移到回收站吗？"},
        {"pickerDeleteMessageMany", "确定将选中的 %1 个项目移到回收站吗？"},
        {"pickerNameExistsMessage", "当前位置已有同名文件或文件夹"},
        {"pickerPasteFailed", "部分内容无法粘贴"},
        {"pickerRenameFailed", "无法重命名"},
        {"pickerDeleteFailed", "部分内容无法删除"},
        {"pickerSearch", "搜索当前目录"},
        {"pickerOverwriteTitle", "文件已存在"},
        {"pickerOverwriteMessage", "目标位置已有同名文件，要替换吗？"},
        {"pickerNewFolderName", "新文件夹名称"},
        {"pickerEmpty", "此文件夹为空"},
        {"pickerFilterAll", "所有文件"},
        {"pickerFilterRecent", "最近打开"},
        {"pickerFilterPdf", "PDF 文档"},
        {"pickerFilterOffice", "Office 文档"},
        {"pickerFilterImages", "图片"},
        {"pickerFilterText", "文本文件"},
        {"pickerRecentEmpty", "暂无最近打开的文件"},
    };

    all["zh_TW"] = {
        {"appName", "PageCase"},
        {"tabSplit", "拆分"},
        {"tabMerge", "合併"},
        {"tabRotate", "旋轉"},
        {"tabConvert", "轉換"},
        {"dropHint", "拖放檔案到此處或點擊瀏覽檔案"},
        {"browse", "瀏覽檔案"},
        {"run", "開始處理"},
        {"clear", "清空"},
        {"preview", "檔案預覽"},
        {"noFiles", "尚未新增檔案"},
        {"previewLoadFailed", "無法載入預覽"},
        {"filesAdded", "個檔案已新增"},
        {"language", "語言"},
        {"theme", "介面"},
        {"light", "淺色"},
        {"dark", "深色"},
        {"about", "關於"},
        {"aboutTitle", "關於 PageCase"},
        {"aboutTagline", "簡潔優雅的PDF工具"},
        {"close", "關閉"},
        {"fileList", "檔案"},
        {"addedFiles", "已新增"},
        {"version", "版本"},
        {"aboutAuthor", "MeowYewy"},
        {"aboutCopyright", "版權所有"},
        {"checkUpdate", "檢查更新"},
        {"newVersion", "新版本"},
        {"installUpdate", "安裝"},
        {"installConfirmTitle", "安裝更新"},
        {"installConfirmMessage", "未儲存的內容將會遺失，是否繼續？"},
        {"updateNow", "立即更新"},
        {"downloadingUpdate", "下載中"},
        {"upToDate", "已是最新版本"},
        {"updateFailed", "檢查失敗"},
        {"changelog", "更新日誌"},
        {"changelogTitle", "更新日誌"},
        {"changelogEmpty", "暫無更新記錄"},
        {"splitDesc", "將多頁 PDF 拆分為單頁"},
        {"mergeDesc", "將多個 PDF 合併為一個檔案"},
        {"rotateDesc", "旋轉 PDF 頁面至對應方向"},
        {"convertDesc", "在 PDF 與 Word、圖片等格式間轉換"},
        {"tabCompress", "壓縮"},
        {"tabWatermark", "浮水印"},
        {"compressDesc", "縮小 PDF 檔案體積"},
        {"watermarkDesc", "為 PDF 每一頁新增文字浮水印"},
        {"watermarkPlaceholder", "浮水印文字"},
        {"formatPdf", "PDF"},
        {"formatPng", "PNG"},
        {"formatJpeg", "JPEG"},
        {"formatWord", "Word"},
        {"pageRangeInvalid", "頁碼格式無效"},
        {"needPdf", "該檔案類型不支援此操作"},
        {"compressLow", "標準"},
        {"compressMid", "較高"},
        {"compressHigh", "最高"},
        {"success", "已完成"},
        {"failed", "處理失敗"},
        {"pages", "頁"},
        {"pickerOpenTitle", "選擇檔案"},
        {"pickerSaveTitle", "儲存檔案"},
        {"pickerFolderTitle", "選擇資料夾"},
        {"pickerCancel", "取消"},
        {"pickerOpen", "打開"},
        {"pickerSave", "儲存"},
        {"pickerSelectFolder", "開始處理"},
        {"pickerFileName", "檔案名稱"},
        {"pickerFileType", "檔案類型"},
        {"pickerName", "名稱"},
        {"pickerSize", "大小"},
        {"pickerModified", "修改時間"},
        {"pickerNewFolder", "新建資料夾"},
        {"pickerShowHidden", "顯示隱藏項目"},
        {"pickerUp", "上一層"},
        {"pickerRefresh", "重新整理"},
        {"pickerPlaceHome", "目前"},
        {"pickerPlaceDesktop", "桌面"},
        {"pickerPlaceDocuments", "文件"},
        {"pickerPlaceDownloads", "下載"},
        {"pickerPlaceComputer", "本機"},
        {"pickerPathMissing", "無法存取此路徑"},
        {"pickerInvalidFolderName", "資料夾名稱無效"},
        {"pickerCreateFolderFailed", "無法建立資料夾"},
        {"pickerFolderExistsTitle", "名稱已存在"},
        {"pickerFolderExistsMessage", "目前位置已有同名資料夾"},
        {"pickerFileExistsMessage", "目前位置已有同名檔案"},
        {"splitNumLower", "一"},
        {"splitNumUpper", "壹"},
        {"pickerOk", "確定"},
        {"pickerCopy", "複製"},
        {"pickerCut", "剪下"},
        {"pickerPaste", "貼上"},
        {"pickerRename", "重新命名"},
        {"pickerDelete", "刪除"},
        {"pickerDeleteTitle", "確認刪除"},
        {"pickerDeleteMessageOne", "確定將「%1」移到資源回收筒嗎？"},
        {"pickerDeleteMessageMany", "確定將選取的 %1 個項目移到資源回收筒嗎？"},
        {"pickerNameExistsMessage", "目前位置已有同名檔案或資料夾"},
        {"pickerPasteFailed", "部分內容無法貼上"},
        {"pickerRenameFailed", "無法重新命名"},
        {"pickerDeleteFailed", "部分內容無法刪除"},
        {"pickerSearch", "搜尋目前資料夾"},
        {"pickerOverwriteTitle", "檔案已存在"},
        {"pickerOverwriteMessage", "目標位置已有同名檔案，要取代嗎？"},
        {"pickerNewFolderName", "新資料夾名稱"},
        {"pickerEmpty", "此資料夾為空"},
        {"pickerFilterAll", "所有檔案"},
        {"pickerFilterRecent", "最近開啟"},
        {"pickerFilterPdf", "PDF 文件"},
        {"pickerFilterOffice", "Office 文件"},
        {"pickerFilterImages", "圖片"},
        {"pickerFilterText", "文字檔案"},
        {"pickerRecentEmpty", "暫無最近開啟的檔案"},
    };

    all["en"] = {
        {"appName", "PageCase"},
        {"tabSplit", "Split"},
        {"tabMerge", "Merge"},
        {"tabRotate", "Rotate"},
        {"tabConvert", "Convert"},
        {"dropHint", "Drop files here or click to browse files"},
        {"browse", "Browse"},
        {"run", "Process"},
        {"clear", "Clear"},
        {"preview", "Preview"},
        {"noFiles", "No files added yet"},
        {"previewLoadFailed", "Could not load preview"},
        {"filesAdded", "file(s) added"},
        {"language", "Language"},
        {"theme", "Appearance"},
        {"light", "Light"},
        {"dark", "Dark"},
        {"about", "About"},
        {"aboutTitle", "About PageCase"},
        {"aboutTagline", "Simple, elegant PDF tools"},
        {"close", "Close"},
        {"fileList", "Files"},
        {"addedFiles", "Added"},
        {"version", "Version"},
        {"aboutAuthor", "MeowYewy"},
        {"aboutCopyright", "All rights reserved"},
        {"checkUpdate", "Check for Updates"},
        {"newVersion", "New"},
        {"installUpdate", "Install"},
        {"installConfirmTitle", "Install update"},
        {"installConfirmMessage", "Unsaved changes will be lost. Continue?"},
        {"updateNow", "Update"},
        {"downloadingUpdate", "Downloading"},
        {"upToDate", "Up to date"},
        {"updateFailed", "Check failed"},
        {"changelog", "Changelog"},
        {"changelogTitle", "Changelog"},
        {"changelogEmpty", "No release notes yet"},
        {"splitDesc", "Split a multi-page PDF into single-page files"},
        {"mergeDesc", "Combine multiple PDFs into one"},
        {"rotateDesc", "Rotate PDF pages to the desired orientation"},
        {"convertDesc", "Convert between PDF, Word and images"},
        {"tabCompress", "Compress"},
        {"tabWatermark", "Watermark"},
        {"compressDesc", "Reduce PDF file size"},
        {"watermarkDesc", "Add a text watermark to every PDF page"},
        {"watermarkPlaceholder", "Watermark text"},
        {"formatPdf", "PDF"},
        {"formatPng", "PNG"},
        {"formatJpeg", "JPEG"},
        {"formatWord", "Word"},
        {"pageRangeInvalid", "Invalid page range"},
        {"needPdf", "This file type is not supported for this operation"},
        {"compressLow", "Standard"},
        {"compressMid", "High"},
        {"compressHigh", "Maximum"},
        {"success", "Completed"},
        {"failed", "Failed"},
        {"pages", "pages"},
        {"pickerOpenTitle", "Select Files"},
        {"pickerSaveTitle", "Save File"},
        {"pickerFolderTitle", "Select Folder"},
        {"pickerCancel", "Cancel"},
        {"pickerOpen", "Open"},
        {"pickerSave", "Save"},
        {"pickerSelectFolder", "Start Processing"},
        {"pickerFileName", "File name"},
        {"pickerFileType", "File type"},
        {"pickerName", "Name"},
        {"pickerSize", "Size"},
        {"pickerModified", "Modified"},
        {"pickerNewFolder", "New Folder"},
        {"pickerShowHidden", "Show hidden items"},
        {"pickerUp", "Up"},
        {"pickerRefresh", "Refresh"},
        {"pickerPlaceHome", "Current"},
        {"pickerPlaceDesktop", "Desktop"},
        {"pickerPlaceDocuments", "Documents"},
        {"pickerPlaceDownloads", "Downloads"},
        {"pickerPlaceComputer", "This PC"},
        {"pickerPathMissing", "This location is unavailable"},
        {"pickerInvalidFolderName", "Invalid folder name"},
        {"pickerCreateFolderFailed", "Could not create folder"},
        {"pickerFolderExistsTitle", "Name already exists"},
        {"pickerFolderExistsMessage", "A folder with this name already exists here"},
        {"pickerFileExistsMessage", "A file with this name already exists here"},
        {"splitNumLower", "One"},
        {"splitNumUpper", "壹"},
        {"pickerOk", "OK"},
        {"pickerCopy", "Copy"},
        {"pickerCut", "Cut"},
        {"pickerPaste", "Paste"},
        {"pickerRename", "Rename"},
        {"pickerDelete", "Delete"},
        {"pickerDeleteTitle", "Confirm Delete"},
        {"pickerDeleteMessageOne", "Move “%1” to the Recycle Bin?"},
        {"pickerDeleteMessageMany", "Move the %1 selected items to the Recycle Bin?"},
        {"pickerNameExistsMessage", "A file or folder with this name already exists here"},
        {"pickerPasteFailed", "Some items could not be pasted"},
        {"pickerRenameFailed", "Could not rename"},
        {"pickerDeleteFailed", "Some items could not be deleted"},
        {"pickerSearch", "Search this folder"},
        {"pickerOverwriteTitle", "File already exists"},
        {"pickerOverwriteMessage", "A file with this name already exists. Replace it?"},
        {"pickerNewFolderName", "New folder name"},
        {"pickerEmpty", "This folder is empty"},
        {"pickerFilterAll", "All Files"},
        {"pickerFilterRecent", "Recent"},
        {"pickerFilterPdf", "PDF Documents"},
        {"pickerFilterOffice", "Office Documents"},
        {"pickerFilterImages", "Images"},
        {"pickerFilterText", "Text Files"},
        {"pickerRecentEmpty", "No recently opened files"},
    };

    return all;
}

} // namespace

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
    , m_store(QStringLiteral("TechG"), QStringLiteral("PageCase"))
    , m_language(m_store.value(QStringLiteral("language"), QStringLiteral("zh_CN")).toString())
    , m_theme(m_store.value(QStringLiteral("theme"), QStringLiteral("light")).toString())
{
    if (m_language == QLatin1String("fr")) {
        m_language = QStringLiteral("en");
        m_store.setValue(QStringLiteral("language"), m_language);
    }
    loadTranslations(m_language);
    loadWatermarkHistory();
}

void AppSettings::setLanguage(const QString &lang)
{
    QString effective = lang;
    if (effective == QLatin1String("fr"))
        effective = QStringLiteral("en");

    if (m_language == effective)
        return;
    m_language = effective;
    m_store.setValue(QStringLiteral("language"), effective);
    ++m_languageRevision;
    loadTranslations(effective);
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

QStringList AppSettings::recentFiles() const
{
    return m_store.value(QStringLiteral("recentFiles")).toStringList();
}

void AppSettings::rememberRecentFile(const QString &filePath)
{
    const QString trimmed = filePath.trimmed();
    if (trimmed.isEmpty())
        return;

    QFileInfo info(trimmed);
    if (!info.isFile())
        return;

    const QString normalized = QDir::cleanPath(info.absoluteFilePath());
    QStringList list = recentFiles();
    list.removeAll(normalized);
    list.prepend(normalized);
    while (list.size() > 20)
        list.removeLast();
    m_store.setValue(QStringLiteral("recentFiles"), list);
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
    // Inline dictionary via trKey(); external .qm files can be added later.
}
