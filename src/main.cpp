#include "updatechecker.h"

#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlError>
#include <QQuickStyle>
#include <QQuickWindow>

#include "appcontroller.h"
#include "appsettings.h"
#include "filepicker.h"
#include "previewimageprovider.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("TechG");
    app.setOrganizationDomain("techg.local");
    app.setApplicationName("PageCase");
    app.setApplicationVersion(QStringLiteral("0.2.0"));

    QIcon appIcon(QStringLiteral(":/qt/qml/PageCase/resources/app-icon.png"));
    if (appIcon.isNull())
        appIcon = QIcon(QStringLiteral(":/qt/qml/PageCase/resources/logo.svg"));
    app.setWindowIcon(appIcon);

    QQuickStyle::setStyle("Basic");

    auto *imageProvider = new PreviewImageProvider();
    AppSettings settings;
    UpdateChecker updateChecker;
    FilePicker filePicker(&settings);
    AppController controller(imageProvider, &settings, &filePicker);

    QQmlApplicationEngine engine;
    engine.addImageProvider(QStringLiteral("preview"), imageProvider);
    engine.rootContext()->setContextProperty("AppSettings", &settings);
    engine.rootContext()->setContextProperty("AppController", &controller);
    engine.rootContext()->setContextProperty("FilePicker", &filePicker);
    engine.rootContext()->setContextProperty("UpdateChecker", &updateChecker);

    QStringList qmlErrors;
    QObject::connect(&engine, &QQmlEngine::warnings, &app,
                     [&qmlErrors](const QList<QQmlError> &warnings) {
                         for (const QQmlError &err : warnings)
                             qmlErrors << err.toString();
                     });

    const auto showLoadError = [](const QString &detail) {
        QMessageBox::critical(nullptr, QStringLiteral("PageCase"),
                              QStringLiteral("Failed to load the user interface.\n"
                                             "UI 加载失败。\n\n%1")
                                  .arg(detail));
    };

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, [&]() {
            showLoadError(qmlErrors.isEmpty()
                              ? QStringLiteral("objectCreationFailed")
                              : qmlErrors.join(QLatin1Char('\n')));
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.loadFromModule("PageCase", "Main");

    if (!engine.rootObjects().isEmpty()) {
        if (auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().first()))
            window->setIcon(appIcon);
        return app.exec();
    }

    showLoadError(qmlErrors.isEmpty()
                      ? QStringLiteral("No QML window was created.")
                      : qmlErrors.join(QLatin1Char('\n')));
    return -1;
}
