#ifndef NAMELINEEDITT_H
#define NAMELINEEDITT_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFontMetrics>

class NameLineEditT : public QWidget
{
    Q_OBJECT
    public:
        explicit NameLineEditT(QString title,QWidget *parent = nullptr);
        ~NameLineEditT();
    signals:

    public slots:

    public:
    inline void SetTitle(QString text){title->setText(text);}
    inline void SetValue(QString text){value->setText(text);}

    private:
        QLabel *title;
        QLineEdit *value;
        QHBoxLayout *layout;
};

#endif // NAMELINEEDITT_H
