#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QListWidget>
#include <QCheckBox>
#include "polygon.h"
#include "renderwidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
private slots:
    void loadPolygonFiles();
    void onItemChanged(QListWidgetItem* item);
    void onToggleAllChanged(int state);
    void clearAllModels();
    void onItemSelectionChanged();
    
private:
    void setupUI();
    void clearPolygons();
    void updateRenderWidget();
    void updateToggleAllCheckbox();
    
    QListWidget* m_listWidget;
    RenderWidget* m_renderWidget;
    QCheckBox* m_toggleAllCheckbox;
    QVector<Polygon*> m_polygons;
};

#endif // MAINWINDOW_H
