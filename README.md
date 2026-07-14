# ProjectO — 本地医疗文档脱敏工具

**本地处理 · 马赛克脱敏 · 手动校对**

| 相关 | 内容 |
|------|------|
| **版本** | v0.1.0 |
| **平台** | Windows 10 / 11，64 位 |
| **本地路径** | `C:\Users\liu18\Documents\TechG\ProjectO` |
| **Qt Creator** | `C:\Qt\Tools\QtCreator\bin\qtcreator.exe` |
| **作者** | MeowYewy |

---

## 功能

1. **左侧添加文件**：支持 `docx` / `pdf` / `png` / `jpeg`
2. **自由排序**：混排拖拽，或按分类（PDF / 图片 / 文档）一键分组
3. **确认识别**：右侧预览对敏感信息做不可逆马赛克/色块（姓名、身份证号等）
4. **手动模块**：手动画框、拖动移动、四角改大小、Delete 删除系统/手动标记
5. **导出**：将脱敏结果导出为 PDF / 图片到本地目录

---

## 用 Qt Creator 打开（本地）

1. 将本仓库同步/克隆到：
   ```
   C:\Users\liu18\Documents\TechG\ProjectO
   ```
2. 启动 Qt Creator：
   ```
   C:\Qt\Tools\QtCreator\bin\qtcreator.exe
   ```
3. **文件 → 打开文件或项目** → 选择该目录下的 `CMakeLists.txt`
4. Kit 选择 **Qt 6 + MinGW 64-bit**（与本机已装 Qt 一致）
5. 配置并运行（或命令行：`scripts\build-release.bat` 后 `run.bat`）

> CMake 默认会查找 `C:\Qt\6.11.1\mingw_64`；若版本不同，在 Qt Creator 里选对 Kit，或设置环境变量 `QT_DIR`。

---

## 使用流程

```
添加文件 →（可选）混排/分类排序 →「确认并识别」
    → 右侧查看马赛克预览 →「手动标记」增删改
    →「导出脱敏」选择输出目录
```

### 自动识别说明（v0.1）

| 来源 | 能力 |
|------|------|
| **PDF 文本层 / Word** | 身份证号（含校验位）、手机号、姓名/地址标签启发式 |
| **扫描件 / 纯图片** | 自动框依赖 OCR（预留接口）；请用手动标记补全 |
| **导出马赛克** | 真实像素块平均，不可恢复原文 |

---

## 项目结构

```
ProjectO/
├── qml/                  # 界面（工作区、预览叠层、手动编辑）
├── src/                  # C++：文件列表、预览、PII 识别、马赛克、导出
├── resources/            # 图标、changelog、update 清单
├── scripts/              # Windows 构建与发布
├── packaging/windows/    # 安装包相关
├── docs/                 # 产品说明
└── CMakeLists.txt        # Qt Creator / CMake 工程入口
```

---

## 技术栈

- Qt 6 + QML + Quick Controls 2
- PDF 预览：Qt Pdf（优先）/ poppler `pdftoppm`
- 脱敏：本地正则 + 关键词启发式；马赛克由 `MosaicEngine` 写入像素

---

## 许可证

MIT
