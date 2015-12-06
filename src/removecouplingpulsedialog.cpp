#include <QtGui>
#include "removecouplingpulsedialog.h"
#include "gpr_fileheader.h"
#include "dsptools.h"
#include "pavementevaluation.h"

#include <correlation.h>

using namespace splab;

RemoveCouplingPulseDialog::RemoveCouplingPulseDialog(PavementEvaluation *p, QWidget *parent) :
    QDialog(parent)
{
    //---------------对空波形设置GroupBox-------------------
    airDataGroup = new QGroupBox(tr("对空数据预览"));
    pointsNumLabel = new QLabel(tr("数据点数：0"));
    airWavePlotter = new CurvePlotter;
    airWavePlotter->setDisplayStyle(CurvePlotter::HorizontalDisplay);
    airWavePlotter->setCurveColor(Qt::blue);
    airWavePlotter->setFixedSize(500, 150);
    airDataFileNameEdit = new QLineEdit;
    airDataFileNameEdit->setReadOnly(true);
    browseAirDataButton = new QPushButton(tr("浏览..."));
    //---------------确定和取消按钮-------------------
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

    //------------Signals & slots---------------
    connect(browseAirDataButton, SIGNAL(clicked()),
            this, SLOT(onBrowseAirDataButtonClicked()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptButtonClicked()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(onCancelButtonClicked()));
    connect(airDataFileNameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(onAirDataFileChanged()));

    //-----------------Layout---------------------
    //---------------载入定标数据波形GroupBox Layout-------------------
    QGridLayout *airDataLayout = new QGridLayout;
    airDataLayout->addWidget(new QLabel(tr("对空采集波形：")), 0, 0);
    airDataLayout->addWidget(pointsNumLabel, 0, 5);
    airDataLayout->addWidget(airWavePlotter, 1, 0, 1, 6);
    airDataLayout->addWidget(new QLabel(tr("载入对空采集数据：")), 2, 0);
    airDataLayout->addWidget(airDataFileNameEdit, 2, 1, 1, 4);
    airDataLayout->addWidget(browseAirDataButton, 2, 5);
    airDataGroup->setLayout(airDataLayout);

    //---------------对话框总Layout-------------------
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(airDataGroup);
    mainLayout->addWidget(buttonBox);

    //---------------对话框参数设置-------------------
    setLayout(mainLayout);
    setWindowTitle(tr("去除直耦波"));
    setFixedSize(this->sizeHint());

    //---------------Initialization--------------------------
    pavement_evaluation = p;
    isAirDataSetted = p->is_air_data_loaded;
    airDataFileName = p->air_data_file_name;
    // 如果在新的去直耦波对话框开启前，对空波形已经加载过则把对空波形的信息重新传递给该对话框
    if (isAirDataSetted) {
        airDataFileNameEdit->setText(p->air_data_file_name);
        air_data_vector = p->air_data;
        pointsNumLabel->setText(tr("数据点数：%1").arg(p->air_data.count()));
        airWavePlotter->setCurveData(0, air_data_vector);
    }
}




/**
 * @brief 载入对空数据
 */
void RemoveCouplingPulseDialog::onBrowseAirDataButtonClicked()
{
    airDataFileName = QFileDialog::getOpenFileName(
                this,
                tr("选择GPR对空数据"), ".",
                tr("GPR 文件 (*.gpr)"));
    if (!airDataFileName.isEmpty()) {
        // 读取GPR文件
        readGprData(airDataFileName, air_data_vector);
        // 在对话框中显示对空数据的文件名
        airDataFileNameEdit->setText(airDataFileName);
        // 画出对空采集的波形
        airWavePlotter->setCurveData(0, air_data_vector);
        // 设置对空数据已经读取标志
        isAirDataSetted = true;
    }
}



/**
 * @brief 确定对话框的响应行为
 */
void RemoveCouplingPulseDialog::acceptButtonClicked()
{
    // 判断对空波形是否已经载入
    if (air_data_vector.count() > 0 && !airDataFileNameEdit->text().isEmpty())
        isAirDataSetted = true; // 已经载入对空波形，则设置标志

    // 保证载入的文件不为空
    if (airDataFileName.isEmpty()) {
        QMessageBox::warning(this, tr("去除直耦波"),
                             tr("没有载入对空波形！请重新操作"),
                             QMessageBox::Yes);
        return;
    }

    // 对空波形已经载入且长度与GPR文件一致，则可将载入的取均值后的对空波形传递给pavement_evaluation
    if (isAirDataSetted && air_data_vector.count() == pavement_evaluation->sampling_num) {
        pavement_evaluation->air_data_file_name = airDataFileName;
        pavement_evaluation->air_data = air_data_vector;
        pavement_evaluation->is_air_data_loaded = true;
    } else {
        pavement_evaluation->is_air_data_loaded = false;
        QMessageBox::warning(this, tr("去除直耦波"),
                             tr("对空波形长度与GPR数据不一致，无法正确去除直耦波！"),
                             QMessageBox::Yes);
        return;
    }
}


//--------------------------------------------------------------------
//函数名称：onAirDataFileChanged()
//函数说明：载入了新的对空采集GPR文件的响应
//参数说明：None
//输出：None
//--------------------------------------------------------------------
void RemoveCouplingPulseDialog::onAirDataFileChanged()
{
    pointsNumLabel->setText(tr("数据点数：%1").arg(air_data_vector.count()));
}


///
/// \brief 点击取消按钮的行为
///
void RemoveCouplingPulseDialog::onCancelButtonClicked()
{
    isAirDataSetted = false;
    pavement_evaluation->is_air_data_loaded = false;
    airDataFileNameEdit->setText("");
    pointsNumLabel->setText(tr("数据点数：0"));
    air_data_vector.clear();
}


//---------------------------------------------------------------------
//函数名称：readGprData()
//函数说明：读取对空波形GPR文件，然后各道取平均得到单道的对空波形
//参数说明：gprFileName - 对空波形GPR文件路径
//输出：points - 保存单道对空波形的Vector
//---------------------------------------------------------------------
bool RemoveCouplingPulseDialog::readGprData(const QString &gprFileName,
                                            QVector<double> &points)
{
    //保存文件头信息的结构体
    _GPR_HEAD gprFileHead;
    //读文件头
    QFile fileRead(gprFileName);
    if (!fileRead.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("GPR Viewer"),
                             tr("读取文件失败！"),
                             QMessageBox::Yes);
        return false;
    }
    fileRead.read((char *)&gprFileHead, sizeof(_GPR_HEAD));
    // 从文件头中提取信息
    // 每道点数(包括90字节道头+数据点数)
    int iSmpNum = gprFileHead.iSmpNum;
    // 每道数据点数
    int sampling_num = iSmpNum - 90/sizeof(ushort);
//    // 采样频率
//    int fs = gprFileHead.dFreq;
    // 时窗
//    int timeWindow = gprFileHead.dTimeWnd;
    // 每道字节数
    int totalByteEachTrace = iSmpNum *sizeof(ushort);
    // 每道数据的字节数
    int data_bytes_each_trace = totalByteEachTrace - 90;
    // 道数
    int traceNum = (fileRead.size() - sizeof(_GPR_HEAD)) / totalByteEachTrace;


    //初始化进度条
    QProgressDialog *readingProgressDialog = new QProgressDialog;
    readingProgressDialog->setMinimumDuration(5);   //进度条至少显示5ms
    readingProgressDialog->setWindowTitle(tr("Please Wait"));
    readingProgressDialog->setLabelText(tr("读取GPR数据中..."));
    readingProgressDialog->setCancelButtonText(tr("取消"));
    readingProgressDialog->setRange(0,traceNum-1);
    //模态对话框，在其没有被关闭之前，用户不能与同一个应用程序的其他窗口进行交互
    readingProgressDialog->setModal(true);

    // 分配内存
    char *trace_head = new char[90];    //每道道头长度为90字节
    ushort *usData = new ushort[sampling_num];   //一次读取的每道数据
    ushort *usDataDeal = new ushort[sampling_num];

    QVector<double> temp_data(sampling_num, 0);

    // 定位文件指针
    int iStartPos = sizeof(_GPR_HEAD) + 6;
    fileRead.seek(iStartPos);
    //逐道读取数据
    for (int col = 0; col < traceNum; col++) {
        fileRead.read((char *)trace_head, 90);    //先读90字节道头
        //读本道数据
        if (fileRead.read((char *)usData, data_bytes_each_trace) == data_bytes_each_trace) {
            //进度条更新一次
            readingProgressDialog->setValue(col);
            if (readingProgressDialog->wasCanceled()){
                fileRead.remove();
                if (usData != NULL)
                {
                    delete[] usData;
                    usData = NULL;
                }
                if (usDataDeal != NULL)
                {
                    delete[] usDataDeal;
                    usDataDeal = NULL;
                }
                return false;
            }
            //把本道数据存入vector
            for (int i = 0; i < sampling_num; i++){
                temp_data[i] += usData[i];
            }
        }
    }
    // 取均值
    for (int i = 0; i < sampling_num; i++){
        temp_data[i] /= traceNum;
    }


    //释放资源
    fileRead.close();
    if (usData != NULL)
    {
        delete[] usData;
        usData = NULL;
    }
    if (usDataDeal != NULL)
    {
        delete[] usDataDeal;
        usDataDeal = NULL;
    }
    //QMessageBox::information(NULL, "读取GPR文件", "读取GPR文件完毕！", QMessageBox::Yes);

    //去直流
    dspTools::subtract_mean(temp_data);

    points.resize(sampling_num);
    points.clear();
    // 把对空波形保存到目标Vector中
    for (int i = 0; i < sampling_num; i++)
        points.append(temp_data[i]);
    //QMessageBox::information(NULL, "去直流", "去直流完毕！", QMessageBox::Yes);

    return true;
}








