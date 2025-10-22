#include "booleanopsdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

BooleanOpsDialog::BooleanOpsDialog(const QVector<Polygon*>& polygons, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("布尔操作");
    setModal(true);
    
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 标题
    QLabel* titleLabel = new QLabel("请选择布尔操作参数：");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(titleLabel);
    
    mainLayout->addSpacing(10);
    
    // 水平布局：三个列表并排
    QHBoxLayout* combosLayout = new QHBoxLayout();
    
    // 第一操作数
    QVBoxLayout* firstLayout = new QVBoxLayout();
    QLabel* firstLabel = new QLabel("第一操作数");
    firstLabel->setAlignment(Qt::AlignCenter);
    firstLayout->addWidget(firstLabel);
    
    m_firstOperandCombo = new QComboBox();
    for (const Polygon* polygon : polygons) {
        m_firstOperandCombo->addItem(polygon->getName());
    }
    firstLayout->addWidget(m_firstOperandCombo);
    combosLayout->addLayout(firstLayout);
    
    // 操作类型
    QVBoxLayout* operationLayout = new QVBoxLayout();
    QLabel* operationLabel = new QLabel("操作类型");
    operationLabel->setAlignment(Qt::AlignCenter);
    operationLayout->addWidget(operationLabel);
    
    m_operationTypeCombo = new QComboBox();
    m_operationTypeCombo->addItem("并集 (∪)", 0);
    m_operationTypeCombo->addItem("交集 (∩)", 1);
    m_operationTypeCombo->addItem("差集 (−)", 2);
    operationLayout->addWidget(m_operationTypeCombo);
    combosLayout->addLayout(operationLayout);
    
    // 第二操作数
    QVBoxLayout* secondLayout = new QVBoxLayout();
    QLabel* secondLabel = new QLabel("第二操作数");
    secondLabel->setAlignment(Qt::AlignCenter);
    secondLayout->addWidget(secondLabel);
    
    m_secondOperandCombo = new QComboBox();
    for (const Polygon* polygon : polygons) {
        m_secondOperandCombo->addItem(polygon->getName());
    }
    // 默认选择第二个模型（如果存在）
    if (polygons.size() > 1) {
        m_secondOperandCombo->setCurrentIndex(1);
    }
    secondLayout->addWidget(m_secondOperandCombo);
    combosLayout->addLayout(secondLayout);
    
    mainLayout->addLayout(combosLayout);
    mainLayout->addSpacing(20);
    
    // 按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
    
    // 设置窗口大小
    setMinimumWidth(500);
    resize(600, 200);
}

int BooleanOpsDialog::getFirstOperandIndex() const
{
    return m_firstOperandCombo->currentIndex();
}

int BooleanOpsDialog::getOperationType() const
{
    return m_operationTypeCombo->currentData().toInt();
}

int BooleanOpsDialog::getSecondOperandIndex() const
{
    return m_secondOperandCombo->currentIndex();
}
