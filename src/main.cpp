#include "updatechecker.h"

#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QQuickWindow>

#include "appcontroller.h"
#include "appsettings.h"
#include "previewimageprovider.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("TechG");
    app.setOrganizationDomain("techg.local");
    app.setApplicationName("ProjectO");
    app.setApplicationVersion(QStringLiteral("0.1.0"));

    QIcon appIcon(QStringLiteral(":/ProjectO/resources/app-icon.png"));
    if (appIcon.isNull())
        appIcon = QIcon(QStringLiteral(":/ProjectO/resources/logo.svg"));
    app.setWindowIcon(appIcon);

    QQuickStyle::setStyle("Basic");

    auto *imageProvider = new PreviewImageProvider();
    AppSettings settings;
    UpdateChecker updateChecker;
    AppController controller(imageProvider, &settings);

    QQmlApplicationEngine engine;
    engine.addImageProvider(QStringLiteral("preview"), imageProvider);
    engine.rootContext()->setContextProperty("AppSettings", &settings);
    engine.rootContext()->setContextProperty("AppController", &controller);
    engine.rootContext()->setContextProperty("UpdateChecker", &updateChecker);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("ProjectO", "Main");

    const auto roots = engine.rootObjects();
    if (!roots.isEmpty()) {
        if (auto *window = qobject_cast<QQuickWindow *>(roots.first()))
            window->setIcon(appIcon);
    }

    return app.exec();
}
