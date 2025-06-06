#include "registersdatadelegate.h"
#include "mainwindow.h"
#include "QsLog.h"
#include <QtDebug>
#include <QPainter>
#include <QSpinBox>
#include <QLineEdit>
// Modify by ChatGPT advice 
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "eutils.h"

void RegistersDataDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{

    QStyledItemDelegate::paint(painter, option, index);

}

QWidget *RegistersDataDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{

    if (m_frmt == EUtils::Bin) {//Bin
            if (m_is16Bit) {
                QLineEdit *editor = new QLineEdit(parent);
                editor->setInputMask("bbbbbbbbbbbbbbbb");
                return editor;
            }
            else {
                QSpinBox *editor = new QSpinBox(parent);
                editor->setMinimum(0);
                editor->setMaximum(1);
                return editor;
            }
    }
    else if (m_frmt == EUtils::Dec) {//Dec
            QLineEdit *editor = new QLineEdit(parent);
            QRegularExpression rx("-{0,1}[0-9]{1,5}");
            QValidator *validator = new QRegularExpressionValidator(rx);
            editor->setValidator(validator);
            return editor;
    }
    else if (m_frmt == EUtils::Float) {//Float
            QLineEdit *editor = new QLineEdit(parent);
            QRegularExpression rx("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");
            QValidator *validator = new QRegularExpressionValidator(rx);
            editor->setValidator(validator);
            return editor;
    }
    else if (m_frmt == EUtils::Hex) {//Hex
            QLineEdit *editor = new QLineEdit(parent);
            editor->setInputMask("hhhh");
            return editor;
    }
    else {//Default = Dec
            QLineEdit *editor = new QLineEdit(parent);
            QRegularExpression rx("-{0,1}[0-9]{1,5}");
            QValidator *validator = new QRegularExpressionValidator(rx);
            editor->setValidator(validator);
            return editor;
    }

}

void RegistersDataDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{

    QString value = index.model()->data(index, Qt::EditRole).toString();

    if (m_frmt == EUtils::Bin && !m_is16Bit) {//Bin
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        spinBox->setValue(value.toInt());
    }
    else { //Bin 16 Bit, Dec, Hex, Float?
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        lineEdit->setText(value);
    }
}

void RegistersDataDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{

    QString value;
    int intVal;
    float floatVal;
    bool ok;

    if (m_frmt == EUtils::Bin && !m_is16Bit) {//Bin
        QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
        intVal = (spinBox->text()).toInt(&ok,m_frmt);
        value = EUtils::formatValue(intVal, m_frmt, m_is16Bit, m_isSigned);
    }
    else if (m_frmt == EUtils::Float){ //Float
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        floatVal = (lineEdit->text()).toFloat(&ok);
        value = EUtils::formatValue32(floatVal, m_floatPrecision);
    }
    else { //Bin 16 Bit, Dec, Hex
        QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
        intVal = (lineEdit->text()).toInt(&ok,m_frmt);
        if (intVal > 65535){
            mainWin->showUpInfoBar(tr("Set value failed\nValue is greater than 65535."), InfoBar::Error);
            QLOG_WARN() <<  "Set value failed. Value is greater than 65535";
            return;
        }
        else if (intVal < -32768){
            mainWin->showUpInfoBar(tr("Set value failed\nValue is smaller than -32768."), InfoBar::Error);
            QLOG_WARN() <<  "Set value failed. Value is smaller than -32768";
            return;
        }
        else
        {
            mainWin->hideInfoBar();
        }
        value = EUtils::formatValue(intVal, m_frmt, m_is16Bit, m_isSigned);
    }

    QLOG_TRACE() <<  "Set model data value = " << value;

    model->setData(index, value, Qt::EditRole);
}

void RegistersDataDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

void RegistersDataDelegate::setFrmt(int frmt)
{

    m_frmt = frmt;

}

void RegistersDataDelegate::setIs16Bit(bool is16Bit)
{

    m_is16Bit = is16Bit;

}

void RegistersDataDelegate::setIsSigned(bool isSigned)
{

    m_isSigned = isSigned;

}

void RegistersDataDelegate::setFloatPrecision(int precision)
{

    m_floatPrecision = precision;

}
