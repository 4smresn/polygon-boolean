#ifndef BOOLEANOPSDIALOG_H
#define BOOLEANOPSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include "polygon.h"

class BooleanOpsDialog : public QDialog {
    Q_OBJECT
    
public:
    explicit BooleanOpsDialog(const QVector<Polygon*>& polygons, QWidget* parent = nullptr);
    
    int getFirstOperandIndex() const;
    int getOperationType() const;
    int getSecondOperandIndex() const;
    
private:
    QComboBox* m_firstOperandCombo;
    QComboBox* m_operationTypeCombo;
    QComboBox* m_secondOperandCombo;
};

#endif // BOOLEANOPSDIALOG_H
