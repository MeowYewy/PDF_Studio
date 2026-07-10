# PDF Studio for Windows

**简洁优雅的PDF工具**

<p align="center">
  <img src="resources/logo.svg" alt="PDF Studio" width="72" />
</p>

| 相关 | 内容 |
|------|------|
| **版本** | v0.1.0 |
| **平台** | Windows 10 / 11，64 位 |
| **语言** | 简体中文 · 繁體中文 · English · Français |
| **主题** | 浅色 / 深色 |
| **作者** | [MeowYewy](https://github.com/MeowYewy) |

---

## 为什么选择 PDF Studio

- **本地处理**：在电脑上完成，不上传云端，适合日常办公与隐私敏感文件
- **功能聚焦**：只做最常用的六件事，界面干净，没有臃肿菜单
- **实时预览**：添加文件后即可滚动预览
- **细节到位**：拖放导入、合并拖拽排序、记住输出目录、处理进度与完成提示

---

## 功能一览

| 功能 | 说明 |
|------|------|
| **拆分** | 将多页 PDF 按页拆成多个单页文件 |
| **合并** | 将多个 PDF 合并为一个；支持拖拽调整顺序 |
| **旋转** | 将 PDF 全部页面旋转 90° / 180° / 270°，预览区实时显示效果 |
| **转换** | 图片、文本、Office 等转为 PDF；或将 PDF 导出为 PNG / JPEG |
| **压缩** | 三档压缩强度，减小 PDF 体积 |
| **水印** | 为每一页添加文字水印；支持 1–5 条对角线平铺、预览与导出一致 |

### 预览支持

- **PDF**：多页滚动预览（poppler / Qt PDF）
- **图片**：png、jpg、gif、webp、tiff 等
- **Office**：docx、xlsx、pptx、odt、rtf 等文字提取预览（非排版还原）
- **文本**：txt、md、csv、log 等

---

**系统要求**

- Windows 10 / 11，64 位

---

## 项目结构

```
PDF_Studio/
├── qml/                  # QML 界面（页面、组件、主题）
├── src/                  # C++ 引擎、预览、设置、更新检查
├── resources/            # 图标、changelog、update 清单
├── tools/                # qpdf、poppler（构建/打包时复制到输出目录）
├── scripts/              # Windows 构建与发布脚本
├── packaging/windows/    # Inno Setup、第三方许可、发布说明
├── dist/                 # 打包输出（gitignore）
├── APP_VERSION.txt       # 当前版本号（勿命名为 VERSION）
└── docs/                 # 产品介绍 / 官网文案
```

---

## 技术栈

- **UI**：Qt 6.11 + QML + Qt Quick Controls 2
- **PDF 引擎**：qpdf（CLI 封装）
- **预览**：Qt PDF（可选）/ poppler pdftoppm
- **Office 预览**：自研文本提取（docx / xlsx / pptx 等）

---

## 许可证

MIT

---

## 致谢

- [qpdf](https://github.com/qpdf/qpdf) — PDF 变换
- [poppler](https://poppler.freedesktop.org/) — PDF 渲染预览
- [Qt](https://www.qt.io/) — 跨平台 UI 框架

---

<p align="center"><sub>PDF Studio · MeowYewy</sub></p>
