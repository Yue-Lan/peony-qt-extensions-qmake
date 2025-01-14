#include "mate-terminal-menu-plugin.h"
#include <file-info.h>
#include <gio/gio.h>

#include <QAction>
#include <QUrl>

#include <QTranslator>
#include <QLocale>
#include <QApplication>

#include <QDebug>

using namespace Peony;

MateTerminalMenuPlugin::MateTerminalMenuPlugin(QObject *parent) : QObject (parent)
{
    QTranslator *t = new QTranslator(this);
    qDebug()<<"\n\n\n\n\n\n\ntranslate:"<<t->load(":/translations/peony-qt-mate-terminal-extension_"+QLocale::system().name());
    QApplication::installTranslator(t);
}

void openTerminal(const QString &uri){
    qDebug()<<"triggered"<<uri;
    QUrl url = uri;
    auto directory = url.path().toUtf8().constData();
    gchar **argv = nullptr;
    g_shell_parse_argv ("mate-terminal", nullptr, &argv, nullptr);
    GError *err = nullptr;
    g_spawn_async (directory,
                   argv,
                   nullptr,
                   G_SPAWN_SEARCH_PATH,
                   nullptr,
                   nullptr,
                   nullptr,
                   &err);
    if (err) {
        qDebug()<<err->message;
        g_error_free(err);
        err = nullptr;
    }
    g_strfreev (argv);
}

QList<QAction *> MateTerminalMenuPlugin::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction *> actions;
    if (types == MenuPluginInterface::DirectoryView || types == MenuPluginInterface::DesktopWindow) {
        if (selectionUris.isEmpty()) {
            QAction *dirAction = new QAction(QIcon::fromTheme("utilities-terminal-symbolic"), tr("Open Directory in Terminal"));
            dirAction->connect(dirAction, &QAction::triggered, [=](){
                openTerminal(uri);
            });
            actions<<dirAction;
        }
        if (selectionUris.count() == 1) {
            auto info = FileInfo::fromUri(selectionUris.first(), false);
            if (info->isDir()) {
                QAction *dirAction = new QAction(QIcon::fromTheme("utilities-terminal-symbolic"), tr("Open Directory in Terminal"));
                dirAction->connect(dirAction, &QAction::triggered, [=](){
                    openTerminal(selectionUris.first());
                });
                actions<<dirAction;
            }
        }
    }
    return actions;
}
