#ifndef REMOVECOUPLINGPULSEDIALOG_H
#define REMOVECOUPLINGPULSEDIALOG_H

#include <QDialog>
#include "curveplotter.h"

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QDialogButtonBox;
class QString;
class QPointF;
class PavementEvaluation;


class RemoveCouplingPulseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RemoveCouplingPulseDialog(PavementEvaluation *p, QWidget *parent = 0);
    
signals:
    
public slots:
    void acceptButtonClicked();

private slots:
    void onBrowseAirDataButtonClicked();    // 点击浏览对空GPR文件的行为
    void onAirDataFileChanged();
    void onCancelButtonClicked();           // 点击浏览对空GPR文件的行为
private:
    //----------GUI Elements-------------
    QGroupBox *airDataGroup;
    QLabel *pointsNumLabel;
    QLineEdit *airDataFileNameEdit;
    QPushButton *browseAirDataButton;
    CurvePlotter *airWavePlotter;
    QDialogButtonBox *buttonBox;

    //---------Private Members-------------
    QString airDataFileName;        // 对空数据文件名
    QVector<double> air_data_vector;       // 对空数据波形向量
    bool isAirDataSetted;               // 标记是否已经载入了对空数据
    PavementEvaluation *pavement_evaluation;
    //---------Private Functions------------
    bool readGprData(const QString &gprFileName, QVector<double> &points);
    
};

#endif // REMOVECOUPLINGPULSEDIALOG_H
