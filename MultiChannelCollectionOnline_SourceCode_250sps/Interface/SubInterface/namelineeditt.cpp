#include "namelineeditt.h"

NameLineEditT::NameLineEditT(QString titleT,QWidget *parent) : QWidget(parent){

    title = new QLabel(titleT);
    title->setFixedHeight(25);
    QFontMetrics metrics = QFontMetrics(title->font());
    title->setFixedWidth(metrics.width(titleT));
    value = new QLineEdit();
    value->setFixedHeight(25);
    value->setEnabled(false);
    layout = new QHBoxLayout();
    layout->addWidget(title);
    layout->addWidget(value);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    setContentsMargins(0,0,0,0);
}
NameLineEditT::~NameLineEditT(){
    delete title;
    delete value;
    delete layout;
}
