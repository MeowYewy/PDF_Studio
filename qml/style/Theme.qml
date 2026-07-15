pragma Singleton
import QtQuick

QtObject {
    id: theme

    readonly property int languageRevision: AppSettings.languageRevision

    function tr(key) {
        const _ = AppSettings.languageRevision
        return AppSettings.trKey(key)
    }

    // Bind to revision counters so all colors refresh on theme/language change
    readonly property int _themeRev: AppSettings.themeRevision
    readonly property bool dark: AppSettings.isDark

    readonly property color bg: dark ? "#1C1C1E" : "#F5F5F7"
    readonly property color surface: dark ? "#2C2C2E" : "#FFFFFF"
    readonly property color surfaceAlt: dark ? "#3A3A3C" : "#FAFAFA"
    readonly property color text: dark ? "#F2F2F7" : "#1D1D1F"
    readonly property color textSecondary: dark ? "#C7C7CC" : "#636366"
    readonly property color textBody: dark ? "#D1D1D6" : "#48484A"
    readonly property color accent: "#8A5CF5"
    readonly property color accentLight: dark ? "#B794F6" : "#A78BFA"
    readonly property color accentSoft: dark ? "#8A5CF533" : "#8A5CF514"
    readonly property color bgAccent: dark ? "#8A5CF518" : "#8A5CF50D"
    readonly property color bgGradientEnd: dark ? "#141416" : "#ECECF0"
    readonly property color border: dark ? "#48484A" : "#E5E5EA"
    readonly property color shadow: dark ? "#00000066" : "#00000018"
    readonly property color tabInactive: dark ? "#3A3A3C" : "#EFEFF0"
    readonly property color menuText: dark ? "#F2F2F7" : "#1D1D1F"
    readonly property color menuTextSelected: "#FFFFFF"
    readonly property color menuHover: dark ? "#48484A" : "#E8E8ED"
    readonly property color menuSelectedBg: accent
    readonly property color menuUnselectedText: text
    readonly property color dimOverlay: dark ? "#000000" : "#000000"
    readonly property real dimOpacity: dark ? 0.45 : 0.28

    readonly property color shadowColor: "#000000"
    readonly property real shadowOpacity1: dark ? 0.35 : 0.10
    readonly property real shadowOpacity2: dark ? 0.18 : 0.05
    readonly property int shadowOffset1: 4
    readonly property int shadowOffset2: 12

    readonly property int animFast: 140
    readonly property int animNormal: 220
    readonly property int animSlow: 320
    readonly property color success: "#34C759"
    readonly property color danger: "#FF3B30"

    readonly property int radiusSm: 8
    readonly property int radiusMd: 12
    readonly property int radiusLg: 16
    readonly property int headerHeight: 56
    readonly property int compactControlWidth: 96
    readonly property real watermarkFontHeightRatio: 0.048
    readonly property real watermarkOpacity: 0.22
    readonly property real watermarkAngle: -35

    readonly property string cjkFontFamily: "Microsoft YaHei"
    readonly property string cjkFontFamilyTW: "Microsoft JhengHei"
    readonly property string latinFontFamily: "Segoe UI"

    readonly property string uiFontFamily: {
        const _ = AppSettings.languageRevision
        if (AppSettings.language === "en" || AppSettings.language === "fr")
            return latinFontFamily
        return cjkFontFamily
    }

    readonly property font mainFont: Qt.font({ family: uiFontFamily, pixelSize: 15 })
    readonly property font mainFontBold: Qt.font({ family: uiFontFamily, pixelSize: 15, weight: Font.DemiBold })
    readonly property font titleFont: Qt.font({ family: uiFontFamily, pixelSize: 22, weight: Font.DemiBold })
    // Brand name always matches zh_CN/zh_TW (YaHei DemiBold); do not follow Latin UI font on en/fr
    readonly property font brandTitleFont: Qt.font({ family: cjkFontFamily, pixelSize: 22, weight: Font.DemiBold })
    readonly property font tabFont: Qt.font({ family: uiFontFamily, pixelSize: 14, weight: Font.Medium })
    readonly property font captionFont: Qt.font({ family: uiFontFamily, pixelSize: 13 })
    readonly property font captionBoldFont: Qt.font({ family: uiFontFamily, pixelSize: 13, weight: Font.DemiBold })
}
