#include <QtGui>
#include <QColorDialog>
#include "node.h"
#include "propertiesdialog.h"

PropertiesDialog::PropertiesDialog(Node *node, QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    this->m_node = node;
    xSpinBox->setValue(int(node->x()));
    ySpinBox->setValue(int(node->y()));
    textLineEdit->setText(node->text());
    m_textColor = node->textColor();
    m_outlineColor = node->outlineColor();
    m_backgroundColor = node->backgroundColor();

    updateColorLabel(outlineColorLabel, m_outlineColor);
    updateColorLabel(backgroundColorLabel, m_backgroundColor);
    updateColorLabel(textColorLabel, m_textColor);
}

void PropertiesDialog::on_buttonBox_accepted()
{
    m_node->setPos(xSpinBox->value(), ySpinBox->value());
    m_node->setText(textLineEdit->text());
    m_node->setOutlineColor(m_outlineColor);
    m_node->setBackgroundColor(m_backgroundColor);
    m_node->setTextColor(m_textColor);
    m_node->update();
    QDialog::accept();
}

void PropertiesDialog::on_textColorButton_clicked()
{
    chooseColor(textColorLabel, &m_textColor);
}

void PropertiesDialog::on_outlineColorButton_clicked()
{
    chooseColor(outlineColorLabel, &m_outlineColor);
}

void PropertiesDialog::on_backgroundColorButton_clicked()
{
    chooseColor(backgroundColorLabel, &m_backgroundColor);
}

void PropertiesDialog::updateColorLabel(QLabel *label, const QColor &color)
{
    QPixmap pixmap(16, 16);
    pixmap.fill(color);
    label->setPixmap(pixmap);
}

void PropertiesDialog::chooseColor(QLabel *label, QColor *color)
{
    QColor newColor = QColorDialog::getColor(*color, this);
    if (newColor.isValid())
    {
        *color = newColor;
        updateColorLabel(label, *color);
    }
}
