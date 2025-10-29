#include "booleanopsdialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QFormLayout>

BooleanOpsDialog::BooleanOpsDialog(const QVector<Polygon*>& polygons, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Boolean Operations");
    setModal(true);
    
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    QLabel* titleLabel = new QLabel("Select Boolean Operation Parameters:");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    mainLayout->addWidget(titleLabel);
    
    mainLayout->addSpacing(10);
    
    // ========== Boolean Operation Selection ==========
    QHBoxLayout* combosLayout = new QHBoxLayout();
    
    // First operand
    QVBoxLayout* firstLayout = new QVBoxLayout();
    QLabel* firstLabel = new QLabel("First Operand");
    firstLabel->setAlignment(Qt::AlignCenter);
    firstLayout->addWidget(firstLabel);
    
    m_firstOperandCombo = new QComboBox();
    for (const Polygon* polygon : polygons) {
        m_firstOperandCombo->addItem(polygon->getName());
    }
    firstLayout->addWidget(m_firstOperandCombo);
    combosLayout->addLayout(firstLayout);
    
    // Operation type
    QVBoxLayout* operationLayout = new QVBoxLayout();
    QLabel* operationLabel = new QLabel("Operation Type");
    operationLabel->setAlignment(Qt::AlignCenter);
    operationLayout->addWidget(operationLabel);
    
    m_operationTypeCombo = new QComboBox();
    m_operationTypeCombo->addItem("Union (∪)", 0);
    m_operationTypeCombo->addItem("Intersection (∩)", 1);
    m_operationTypeCombo->addItem("Difference (−)", 2);
    operationLayout->addWidget(m_operationTypeCombo);
    combosLayout->addLayout(operationLayout);
    
    // Second operand
    QVBoxLayout* secondLayout = new QVBoxLayout();
    QLabel* secondLabel = new QLabel("Second Operand");
    secondLabel->setAlignment(Qt::AlignCenter);
    secondLayout->addWidget(secondLabel);
    
    m_secondOperandCombo = new QComboBox();
    for (const Polygon* polygon : polygons) {
        m_secondOperandCombo->addItem(polygon->getName());
    }
    // Default to selecting second model (if it exists)
    if (polygons.size() > 1) {
        m_secondOperandCombo->setCurrentIndex(1);
    }
    secondLayout->addWidget(m_secondOperandCombo);
    combosLayout->addLayout(secondLayout);
    
    mainLayout->addLayout(combosLayout);
    mainLayout->addSpacing(15);
    
    // ========== Tolerance Settings ==========
    QGroupBox* toleranceGroup = new QGroupBox("Tolerance Settings");
    QFormLayout* toleranceLayout = new QFormLayout(toleranceGroup);
    
    // Precision spinbox
    m_precisionSpinBox = new QSpinBox();
    m_precisionSpinBox->setMinimum(0);
    m_precisionSpinBox->setMaximum(15);
    m_precisionSpinBox->setValue(6);
    m_precisionSpinBox->setToolTip("Decimal precision (6 = 0.000001 accuracy)");
    toleranceLayout->addRow("Precision (decimal places):", m_precisionSpinBox);
    
    mainLayout->addWidget(toleranceGroup);
    mainLayout->addSpacing(10);
    
    // Buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText("OK");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(buttonBox);
    
    // Set window size
    setMinimumWidth(600);
    resize(700, 350);
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

double BooleanOpsDialog::getTolerance() const
{
    int tol = m_precisionSpinBox->value();
    return std::pow(10.0, -tol);
}
