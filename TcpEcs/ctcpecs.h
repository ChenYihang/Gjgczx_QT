#ifndef CTCPECS_H
#define CTCPECS_H

#include <QWidget>
#include <QTreeWidgetItem>
#include "csocketserver.h"
#include "csocketprocess.h"
#include "common.h"
#include "multifunc.h"
#include "cdataprocess.h"
namespace Ui {
class CtcpEcs;
}

class CtcpEcs : public QWidget
{
    Q_OBJECT

public:
    explicit CtcpEcs(QWidget *parent = 0);
    ~CtcpEcs();

private slots:
    void on_start_clicked();

    void slotAddNewClient(QString qsip,int iflag);

    void slotDelClient(int iflag);

    void dataDisPlay(QByteArray socketData, QString ip);

    void forwardDeal(QByteArray cmd,QString ip);

    void uiUinitialize();

    void on_del_clicked();

    void getCurrentClient();

    void on_Send_clicked();

    void on_single_clicked();

    void on_upload2_clicked();

    void on_upload1_clicked();

    void on_Stop_clicked();

    void on_pulseStart_clicked();

    void on_PulseStop_clicked();

    void on_pulseReset_clicked();

    void on_upload3_clicked();


    void on_setpath_clicked();

    void on_pulseUpload_clicked();



    void on_ADnum_currentIndexChanged(const QString &arg1);

    void on_ADrate_currentIndexChanged(const QString &arg1);

    void on_DArate_currentIndexChanged(const QString &arg1);

    void on_SendDataFile_clicked();

    void on_SendDataFile_DA_clicked();

    void on_infoShow_textChanged();

    void on_Clear_clicked();

    void on_verticalSlider_valueChanged(int value);


    void on_horizontalSlider_valueChanged(int value);


    void on_upload1_2_clicked();




    void on_doubleSpinBox_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_horizontalSlider_actionTriggered(int action);

    void on_motorReset_clicked();

private:
    Ui::CtcpEcs *ui;

    CsocketServer *MserverHandle;

    CsocketProcess *MprocessHandle;


    int currentClient;   //当前下位机
    int speedflag;
    int speedtemp;
    int speedflag_lr;
    int speedtemp_lr;


    QFile *DAFile;
    QFile *audioFile;
    int forwardFlag;


};

#endif // CTCPECS_H
