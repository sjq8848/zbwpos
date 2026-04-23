#pragma once
#include <QQuickView>

class MainWindow : public QQuickView {
    Q_OBJECT
public:
    explicit MainWindow(QWindow *parent = nullptr);
};
