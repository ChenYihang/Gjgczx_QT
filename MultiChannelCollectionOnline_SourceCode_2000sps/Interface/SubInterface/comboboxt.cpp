#include "comboboxt.h"

ComboBoxT::ComboBoxT(QString titleT,QWidget *parent) : QWidget(parent){

    title = new QLabel(titleT);
    title->setFixedHeight(25);
    QFontMetrics metrics = QFontMetrics(title->font());
    title->setFixedWidth(metrics.width(titleT));
    box = new QComboBox();
    box->setStyleSheet("border:1px groove gray;border-radius:5px;");
    box->setFixedHeight(25);
    layout = new QHBoxLayout();
    layout->addWidget(title);
    layout->addWidget(box);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    setContentsMargins(0,0,0,0);
}
ComboBoxT::~ComboBoxT(){
    delete title;
    delete box;
    delete layout;
}
