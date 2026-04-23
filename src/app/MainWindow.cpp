#include "MainWindow.h"

MainWindow::MainWindow(QWindow *parent)
    : QQuickView(parent)
{
    setResizeMode(QQuickView::SizeRootObjectToView);
    setFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
}
