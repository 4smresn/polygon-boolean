#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QLabel>
#include <QDir>
#include <QCheckBox>
#include <QStatusBar>
#include "booleanopsdialog.h"
#include "op/booleanops.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_listWidget(nullptr)
    , m_renderWidget(nullptr)
    , m_toggleAllCheckbox(nullptr)
{
    setupUI();
    setWindowTitle("多边形查看器");
    resize(1000, 700);
}

MainWindow::~MainWindow()
{
    clearPolygons();
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    
    // 左侧面板
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    
    QLabel* label = new QLabel("模型列表:");
    leftLayout->addWidget(label);
    
    m_listWidget = new QListWidget();
    connect(m_listWidget, &QListWidget::itemChanged, this, &MainWindow::onItemChanged);
    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onItemSelectionChanged);
    leftLayout->addWidget(m_listWidget);
    
    // 全选/全不选复选框
    m_toggleAllCheckbox = new QCheckBox("显示/隐藏全部");
    m_toggleAllCheckbox->setTristate(true);
    m_toggleAllCheckbox->setCheckState(Qt::Checked);
    connect(m_toggleAllCheckbox, &QCheckBox::stateChanged, this, &MainWindow::onToggleAllChanged);
    leftLayout->addWidget(m_toggleAllCheckbox);
    
    QPushButton* loadButton = new QPushButton("加载点集文件");
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadPolygonFiles);
    leftLayout->addWidget(loadButton);
    
    QPushButton* clearButton = new QPushButton("清空所有模型");
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearAllModels);
    leftLayout->addWidget(clearButton);
    
    QPushButton* booleanButton = new QPushButton("布尔操作");
    connect(booleanButton, &QPushButton::clicked, this, &MainWindow::performBooleanOperation);
    leftLayout->addWidget(booleanButton);
    
    leftPanel->setMaximumWidth(250);
    mainLayout->addWidget(leftPanel);
    
    // 右侧渲染区域
    m_renderWidget = new RenderWidget();
    mainLayout->addWidget(m_renderWidget, 1);
}

void MainWindow::loadPolygonFiles()
{
    // 打开文件选择对话框
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        "选择点集文件",
        QDir::currentPath(),
        "点集文件 (*.txt);;所有文件 (*.*)"
    );
    
    if (filePaths.isEmpty()) {
        return;
    }
    
    // 不再清空现有数据，改为追加模式
    // clearPolygons();
    // m_listWidget->clear();
    
    // 加载每个选中的文件
    int loadedCount = 0;
    int failedCount = 0;
    for (const QString& filepath : filePaths) {
        QFileInfo fileInfo(filepath);
        QString filename = fileInfo.fileName();
        
        // 检查是否已经加载过这个文件
        bool alreadyLoaded = false;
        for (const Polygon* poly : m_polygons) {
            if (poly->getName() == filename) {
                alreadyLoaded = true;
                break;
            }
        }
        
        if (alreadyLoaded) {
            QMessageBox::information(this, "提示", 
                QString("文件 %1 已经加载过了，跳过。").arg(filename));
            continue;
        }
        
        Polygon* polygon = new Polygon(filename);
        if (polygon->loadFromFile(filepath)) {
            m_polygons.append(polygon);
            
            // 创建带复选框的列表项
            QListWidgetItem* item = new QListWidgetItem(filename);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);
            m_listWidget->addItem(item);
            
            loadedCount++;
        } else {
            delete polygon;
            failedCount++;
            QMessageBox::warning(this, "错误", 
                QString("无法加载文件: %1").arg(filename));
        }
    }
    
    // 更新渲染
    updateRenderWidget();
    updateToggleAllCheckbox();
}

void MainWindow::clearPolygons()
{
    for (Polygon* polygon : m_polygons) {
        delete polygon;
    }
    m_polygons.clear();
}

void MainWindow::clearAllModels()
{
    clearPolygons();
    m_listWidget->clear();
    updateRenderWidget();
    updateToggleAllCheckbox();
}

void MainWindow::onItemChanged(QListWidgetItem* item)
{
    Q_UNUSED(item);
    updateRenderWidget();
    updateToggleAllCheckbox();
}

void MainWindow::onToggleAllChanged(int state)
{
    // 阻止信号，避免递归触发
    m_listWidget->blockSignals(true);
    
    Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
    
    // 只处理全选和全不选，不处理部分选中状态
    if (checkState == Qt::Checked || checkState == Qt::Unchecked) {
        for (int i = 0; i < m_listWidget->count(); ++i) {
            QListWidgetItem* item = m_listWidget->item(i);
            item->setCheckState(checkState);
        }
        updateRenderWidget();
    }
    
    m_listWidget->blockSignals(false);
}

void MainWindow::updateRenderWidget()
{
    QVector<Polygon*> visiblePolygons;
    
    for (int i = 0; i < m_listWidget->count() && i < m_polygons.size(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            visiblePolygons.append(m_polygons[i]);
        }
    }
    
    m_renderWidget->setPolygons(visiblePolygons);
}

void MainWindow::updateToggleAllCheckbox()
{
    if (m_listWidget->count() == 0) {
        m_toggleAllCheckbox->setCheckState(Qt::Unchecked);
        return;
    }
    
    int checkedCount = 0;
    for (int i = 0; i < m_listWidget->count(); ++i) {
        if (m_listWidget->item(i)->checkState() == Qt::Checked) {
            checkedCount++;
        }
    }
    
    // 阻止信号，避免触发 onToggleAllChanged
    m_toggleAllCheckbox->blockSignals(true);
    
    if (checkedCount == 0) {
        m_toggleAllCheckbox->setCheckState(Qt::Unchecked);
    } else if (checkedCount == m_listWidget->count()) {
        m_toggleAllCheckbox->setCheckState(Qt::Checked);
    } else {
        m_toggleAllCheckbox->setCheckState(Qt::PartiallyChecked);
    }
    
    m_toggleAllCheckbox->blockSignals(false);
}

void MainWindow::onItemSelectionChanged()
{
    // 清除所有模型的高亮状态
    for (Polygon* polygon : m_polygons) {
        polygon->setHighlighted(false);
    }
    
    // 设置选中模型的高亮状态
    QList<QListWidgetItem*> selectedItems = m_listWidget->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        int row = m_listWidget->row(item);
        if (row >= 0 && row < m_polygons.size()) {
            m_polygons[row]->setHighlighted(true);
        }
    }
    
    // 更新渲染
    updateRenderWidget();
}

// 修改 performBooleanOperation 函数以处理多个结果

void MainWindow::performBooleanOperation()
{
    if (m_polygons.size() < 2) {
        QMessageBox::warning(this, "警告", 
            "至少需要两个模型才能执行布尔操作！");
        return;
    }
    
    // 显示布尔操作对话框
    BooleanOpsDialog dialog(m_polygons, this);
    if (dialog.exec() == QDialog::Accepted) {
        // 获取用户选择
        int firstIdx = dialog.getFirstOperandIndex();
        int operationType = dialog.getOperationType();
        int secondIdx = dialog.getSecondOperandIndex();
        
        // 检查是否选择了相同的模型
        if (firstIdx == secondIdx) {
            QMessageBox::warning(this, "警告", 
                "不能对同一个模型执行布尔操作！");
            return;
        }
        
        // 转换为标准库格式
        BooleanOps::PolygonData poly1 = BooleanOps::fromQtPolygon(m_polygons[firstIdx]);
        BooleanOps::PolygonData poly2 = BooleanOps::fromQtPolygon(m_polygons[secondIdx]);
        
        // 确定操作类型
        BooleanOps::Operation op;
        QString opName;
        QString opSymbol;
        switch (operationType) {
            case 0:
                op = BooleanOps::Operation::Union;
                opName = "并集";
                opSymbol = " ∪ ";
                break;
            case 1:
                op = BooleanOps::Operation::Intersection;
                opName = "交集";
                opSymbol = " ∩ ";
                break;
            case 2:
                op = BooleanOps::Operation::Difference;
                opName = "差集";
                opSymbol = " − ";
                break;
            default:
                return;
        }
        
        // 执行布尔运算
        std::vector<BooleanOps::PolygonData> results = BooleanOps::performOperation(poly1, poly2, op);
        
        if (results.empty()) {
            QMessageBox::information(this, "结果", 
                QString("布尔运算 %1%2%3 的结果为空")
                    .arg(m_polygons[firstIdx]->getName())
                    .arg(opSymbol)
                    .arg(m_polygons[secondIdx]->getName()));
            return;
        }
        
        // 为每个结果创建新模型
        int addedCount = 0;
        for (size_t i = 0; i < results.size(); ++i) {
            QString resultName = QString("%1_结果_%2").arg(opName).arg(++m_resultCounter);
            if (results.size() > 1) {
                resultName += QString("_%1").arg(i + 1);
            }
            
            Polygon* resultPolygon = BooleanOps::toQtPolygon(results[i], resultName);
            
            if (resultPolygon && resultPolygon->isValid()) {
                m_polygons.append(resultPolygon);
                
                // 添加到列表
                QListWidgetItem* item = new QListWidgetItem(resultName);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(Qt::Checked);
                m_listWidget->addItem(item);
                
                addedCount++;
            } else {
                delete resultPolygon;
            }
        }
        
        // 更新渲染
        updateRenderWidget();
        updateToggleAllCheckbox();
        
        // 显示成功消息
        QMessageBox::information(this, "成功", 
            QString("布尔运算 %1%2%3 完成！\n生成了 %4 个结果多边形。")
                .arg(m_polygons[firstIdx]->getName())
                .arg(opSymbol)
                .arg(m_polygons[secondIdx]->getName())
                .arg(addedCount));
    }
}
