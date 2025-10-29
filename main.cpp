#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置全局字体大小
    QFont font = app.font();
    font.setPointSize(12);  // 从默认的 9-10 增大到 11
    app.setFont(font);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
