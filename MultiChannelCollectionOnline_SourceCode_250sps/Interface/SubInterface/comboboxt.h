#ifndef COMBOBOXT_H
#define COMBOBOXT_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QComboBox>
#include <QFontMetrics>

class ComboBoxT : public QWidget
{
    Q_OBJECT
    public:
        explicit ComboBoxT(QString title,QWidget *parent = nullptr);
        ~ComboBoxT();
    signals:

    public slots:

    public:
    inline void SetTitle(QString text){title->setText(text);}
    inline QComboBox *GetBox(){return box;}
    inline void AddItem(QString text){box->addItem(text);}
    inline int CurrentIndex(){return box->currentIndex();}
    inline void Clear(){box->clear();}
    inline void setIndex(int i){box->setCurrentIndex(i);}

    private:
        QLabel *title;
        QComboBox *box;
        QHBoxLayout *layout;
};

#endif // COMBOBOXT_H
