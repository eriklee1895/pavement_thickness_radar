#ifndef LOADREFWAVEDIALOG_H
#define LOADREFWAVEDIALOG_H

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


class LoadRefWaveDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoadRefWaveDialog(PavementEvaluation *p, QWidget *parent = 0);
    
signals:
    
public slots:
    void acceptButtonClicked();

private slots:
    void onBrowseAirDataButtonClicked();
    void onBrowseMetalDataButtonClicked();
    void onRefDataFileChanged();

private:
    //----------GUI Elements-------------
    QGroupBox *loadWavesGroup;
    QLabel *airDataPointsNumLabel;
    QLabel *metalDataPointsNumLabel;
    QLineEdit *airDataFileNameEdit;
    QLineEdit *metalDataFileNameEdit;
    QPushButton *browseAirDataButton;
    QPushButton *browseMetalDataButton;

    QGroupBox *previewRefWavesGroup;
    CurvePlotter *airWavePlotter;
    CurvePlotter *metalWavePlotter;

    QDialogButtonBox *buttonBox;

    //---------Private Members-------------
    QString airDataFileName;        // 对空数据文件名
    QString metalDataFileName;      // 金属板数据文件名
    QVector<double> air_data_vector;       // 对空数据波形向量
    QVector<double> metal_data_vector;     // 金属板波形向量
    bool isAirDataSetted;               // 标记是否已经载入了对空数据
    bool isMetalDataSetted;             // 标记是否已经载入了金属板数据

    PavementEvaluation *pavement_evaluation;
    //---------Private Functions------------
    bool readGprData(const QString &gprFileName, QVector<double> &points);
};

#endif // LOADREFWAVEDIALOG_H
