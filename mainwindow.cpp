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
    setWindowTitle("Polygon Viewer");
    
    // 设置窗口大小
    resize(1920, 1080);
    setMinimumSize(1920, 1080);
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
    
    QLabel* label = new QLabel("Model List:");
    leftLayout->addWidget(label);
    
    m_listWidget = new QListWidget();
    connect(m_listWidget, &QListWidget::itemChanged, this, &MainWindow::onItemChanged);
    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::onItemSelectionChanged);
    leftLayout->addWidget(m_listWidget);
    
    // Toggle all checkbox
    m_toggleAllCheckbox = new QCheckBox("Show/Hide All");
    m_toggleAllCheckbox->setTristate(true);
    m_toggleAllCheckbox->setCheckState(Qt::Checked);
    connect(m_toggleAllCheckbox, &QCheckBox::stateChanged, this, &MainWindow::onToggleAllChanged);
    leftLayout->addWidget(m_toggleAllCheckbox);
    
    QPushButton* loadButton = new QPushButton("Load Polygon Files");
    loadButton->setMinimumHeight(45);  // 设置按钮高度
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::loadPolygonFiles);
    leftLayout->addWidget(loadButton);
    
    QPushButton* clearButton = new QPushButton("Clear All Models");
    clearButton->setMinimumHeight(45);  // 设置按钮高度
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearAllModels);
    leftLayout->addWidget(clearButton);
    
    QPushButton* booleanButton = new QPushButton("Boolean Operations");
    booleanButton->setMinimumHeight(45);  // 设置按钮高度
    connect(booleanButton, &QPushButton::clicked, this, &MainWindow::performBooleanOperation);
    leftLayout->addWidget(booleanButton);
    
    leftPanel->setMaximumWidth(300);  // 从 250 增加到 300
    mainLayout->addWidget(leftPanel);
    
    // 右侧渲染区域
    m_renderWidget = new RenderWidget();
    mainLayout->addWidget(m_renderWidget, 1);
}

void MainWindow::loadPolygonFiles()
{
    // Open file selection dialog
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        "Select Polygon Files",
        "/home/f/cpp-project/polygon-boolean/case",
        "Polygon Files (*.txt);;All Files (*.*)"
    );
    
    if (filePaths.isEmpty()) {
        return;
    }
    
    // 不再清空现有数据，改为追加模式
    // clearPolygons();
    // m_listWidget->clear();
    
    // 加载每个选中的文件
    
    for (const QString& filepath : filePaths) {
        QFileInfo fileInfo(filepath);
        QString filename = QString("%1").arg(loadedCount, 4, 10, QChar('0'));
        
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
            QMessageBox::warning(this, "Error", 
                QString("Failed to load file: %1").arg(filename));
        }
    }
    
    // Update rendering - use setPolygons to center all polygons
    QVector<Polygon*> visiblePolygons;
    for (int i = 0; i < m_listWidget->count() && i < m_polygons.size(); ++i) {
        QListWidgetItem* item = m_listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            visiblePolygons.append(m_polygons[i]);
        }
    }
    m_renderWidget->setPolygons(visiblePolygons);
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
    m_renderWidget->setPolygons(QVector<Polygon*>());  // Clear with setPolygons
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
    
    for(int i=0; i<visiblePolygons.size(); i++)
    {
        if(visiblePolygons[i]->isHighlighted())
        {
            Polygon* tmp=visiblePolygons.back();
            visiblePolygons.back()=visiblePolygons[i];
            visiblePolygons[i]=tmp;
            break;
        }
    }
    
    // Only update visibility, do not recalculate centering
    m_renderWidget->updatePolygonsVisibility(visiblePolygons);
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
    
    // Prevent signals to avoid recursive triggering
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
    // Clear highlight state for all polygons
    for (Polygon* polygon : m_polygons) {
        polygon->setHighlighted(false);
    }
    
    // Set highlight state for selected polygons
    QList<QListWidgetItem*> selectedItems = m_listWidget->selectedItems();
    for (QListWidgetItem* item : selectedItems) {
        int row = m_listWidget->row(item);
        if (row >= 0 && row < m_polygons.size()) {
            m_polygons[row]->setHighlighted(true);
        }
    }
    
    // Update rendering
    updateRenderWidget();
}

// Handle multiple results from boolean operations

void MainWindow::performBooleanOperation()
{
    // Show boolean operations dialog
    BooleanOpsDialog dialog(m_polygons, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Get user selection
        int firstIdx = dialog.getFirstOperandIndex();
        int operationType = dialog.getOperationType();
        int secondIdx = dialog.getSecondOperandIndex();
        double tolerance = dialog.getTolerance();
        
        
        // Convert to standard library format
        BooleanOps::OpPolygon poly1 = BooleanOps::fromQtPolygon(m_polygons[firstIdx]);
        BooleanOps::OpPolygon poly2 = BooleanOps::fromQtPolygon(m_polygons[secondIdx]);
        
       
        QString opName;
        QString opSymbol;
        switch (operationType) {
            case 0:
                opName = "Union";
                opSymbol = " ∪ ";
                break;
            case 1:
                opName = "Intersection";
                opSymbol = " ∩ ";
                break;
            case 2:
                opName = "Difference";
                opSymbol = " − ";
                break;
            default:
                return;
        }
        
        // Perform boolean operation with user-specified tolerance
        std::vector<BooleanOps::OpPolygon> results = BooleanOps::performOperation(poly1, poly2, operationType, tolerance);
        
        // Create new model for each result
        int addedCount = 0;
        for (size_t i = 0; i < results.size(); ++i) {
            QString resultName = QString("%1%2%3").arg(m_polygons[firstIdx]->getName()).arg(opName).arg(m_polygons[secondIdx]->getName());
            if (results.size() > 1) {
                resultName += QString("(%1)").arg(i + 1);
            }
            
            Polygon* resultPolygon = BooleanOps::toQtPolygon(results[i], resultName);
            
            if (resultPolygon && resultPolygon->isValid()) {
                m_polygons.append(resultPolygon);
                
                // Add to list
                QListWidgetItem* item = new QListWidgetItem(resultName);
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(Qt::Checked);
                m_listWidget->addItem(item);
                
                addedCount++;
            } else {
                delete resultPolygon;
            }
        }
        
        // Update rendering - use setPolygons to recenter after adding new polygons
        QVector<Polygon*> visiblePolygons;
        for (int i = 0; i < m_listWidget->count() && i < m_polygons.size(); ++i) {
            QListWidgetItem* item = m_listWidget->item(i);
            if (item->checkState() == Qt::Checked) {
                visiblePolygons.append(m_polygons[i]);
            }
        }
        m_renderWidget->setPolygons(visiblePolygons,false);
        updateToggleAllCheckbox();
    }
}
