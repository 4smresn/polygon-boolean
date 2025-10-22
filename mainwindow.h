#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include "polygon.h"
#include "renderwidget.h"

class QCheckBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void loadPolygonFiles();
    void clearAllModels();
    void onItemChanged(QListWidgetItem* item);
    void onToggleAllChanged(int state);
    void onItemSelectionChanged();
    void performBooleanOperation();

private:
    void setupUI();
    void clearPolygons();
    void updateRenderWidget();
    void updateToggleAllCheckbox();

    QListWidget* m_listWidget;
    RenderWidget* m_renderWidget;
    QVector<Polygon*> m_polygons;
    QCheckBox* m_toggleAllCheckbox;
    int m_resultCounter = 0;  // 添加结果计数器
};

#endif // MAINWINDOW_H
