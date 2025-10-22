#ifndef BOOLEANOPSDIALOG_H
#define BOOLEANOPSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include "polygon.h"
#include "op/booleanops.h"

class BooleanOpsDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit BooleanOpsDialog(const QVector<Polygon*>& polygons, QWidget* parent = nullptr);
    
    int getFirstOperandIndex() const;
    int getOperationType() const;
    int getSecondOperandIndex() const;
    BooleanOps::Tolerance getTolerance() const;
    
private:
    QComboBox* m_firstOperandCombo;
    QComboBox* m_operationTypeCombo;
    QComboBox* m_secondOperandCombo;
    QSpinBox* m_precisionSpinBox;
};

#endif // BOOLEANOPSDIALOG_H
