#include <QtGui>
#include "loadrefwavedialog.h"
#include "gpr_fileheader.h"
#include "dsptools.h"
#include "pavementevaluation.h"

LoadRefWaveDialog::LoadRefWaveDialog(PavementEvaluation *p, QWidget *parent) :
    QDialog(parent)
{
    //---------------载入定标数据波形GroupBox-------------------
    loadWavesGroup = new QGroupBox(tr("载入定标数据"));
    airDataFileNameEdit = new QLineEdit;
    airDataFileNameEdit->setReadOnly(true);
    metalDataFileNameEdit = new QLineEdit;
    metalDataFileNameEdit->setReadOnly(true);
    browseAirDataButton = new QPushButton(tr("浏览..."));
    browseMetalDataButton = new QPushButton(tr("浏览..."));

    //---------------定标波形预览GroupBox-------------------
    previewRefWavesGroup = new QGroupBox(tr("定标波形预览"));
    airDataPointsNumLabel = new QLabel(tr("数据点数："));
    metalDataPointsNumLabel = new QLabel(tr("数据点数："));
    // 对空采集波形CurvePlotter Settings
    airWavePlotter = new CurvePlotter;
    airWavePlotter->setDisplayStyle(CurvePlotter::HorizontalDisplay);
    airWavePlotter->setCurveColor(Qt::blue);
    airWavePlotter->setFixedSize(500, 150);
    // 金属板波形CurvePlotter Settings
    metalWavePlotter = new CurvePlotter;
    metalWavePlotter->setDisplayStyle(CurvePlotter::HorizontalDisplay);
    metalWavePlotter->setCurveColor(Qt::red);
    metalWavePlotter->setFixedSize(500, 150);

    //---------------确定和取消按钮-------------------
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));

    //-----------------Layout---------------------
    //---------------载入定标数据波形GroupBox Layout-------------------
    QGridLayout *loadWavesLayout = new QGridLayout;
    loadWavesLayout->addWidget(new QLabel(tr("金属板反射数据：")), 0, 0);
    loadWavesLayout->addWidget(metalDataFileNameEdit, 0, 1);
    loadWavesLayout->addWidget(browseMetalDataButton, 0, 2);
    loadWavesLayout->addWidget(new QLabel(tr("对空采集数据：")), 1, 0);
    loadWavesLayout->addWidget(airDataFileNameEdit, 1, 1);
    loadWavesLayout->addWidget(browseAirDataButton, 1, 2);
    loadWavesGroup->setLayout(loadWavesLayout);

    //---------------预览定标波形GroupBox Layout-------------------
    QGridLayout *previewRefWavesLayout = new QGridLayout;
    previewRefWavesLayout->addWidget(new QLabel(tr("金属板反射波形：")), 0, 0);
    previewRefWavesLayout->addWidget(metalDataPointsNumLabel, 0, 5);
    previewRefWavesLayout->addWidget(metalWavePlotter, 1, 0, 1, 6);
    previewRefWavesLayout->addWidget(new QLabel(tr("对空采集波形：")), 2, 0);
    previewRefWavesLayout->addWidget(airDataPointsNumLabel, 2, 5);
    previewRefWavesLayout->addWidget(airWavePlotter, 3, 0, 1, 6);
    previewRefWavesGroup->setLayout(previewRefWavesLayout);

    //------------Signals & slots---------------
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptButtonClicked()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(browseAirDataButton, SIGNAL(clicked()),
            this, SLOT(onBrowseAirDataButtonClicked()));
    connect(browseMetalDataButton, SIGNAL(clicked()),
            this, SLOT(onBrowseMetalDataButtonClicked()));
    connect(airDataFileNameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(onRefDataFileChanged()));
    connect(metalDataFileNameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(onRefDataFileChanged()));

    //---------------对话框总Layout-------------------
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(loadWavesGroup);
    mainLayout->addWidget(previewRefWavesGroup);
    mainLayout->addWidget(buttonBox);

    //---------------对话框参数设置-------------------
    setLayout(mainLayout);
    setWindowTitle(tr("定标波形设置"));
    setFixedSize(this->sizeHint());

    //---------------Initialization--------------------------
    pavement_evaluation = p;
    isAirDataSetted = p->is_air_data_loaded;
    isMetalDataSetted = p->is_metal_data_loaded;

    qDebug() << "isAirDataSetted = " << isAirDataSetted << ", isMetalDataSetted = " << isMetalDataSetted;

    if (isAirDataSetted) {
        airDataFileName = p->air_data_file_name;
        airDataFileNameEdit->setText(p->air_data_file_name);
        airWavePlotter->setCurveData(0, p->air_data);
        airDataPointsNumLabel->setText(tr("数据点数：%1").arg(p->air_data.count()));
    }
    if (isMetalDataSetted) {
        metalDataFileName = p->metal_data_file_name;
        metalDataFileNameEdit->setText(p->metal_data_file_name);
        metalWavePlotter->setCurveData(0, p->metal_data);
        metalDataPointsNumLabel->setText(tr("数据点数：%1").arg(p->metal_data.count()));
    }

}


/**
 * @brief 载入对空数据
 */
void LoadRefWaveDialog::onBrowseAirDataButtonClicked()
{
    airDataFileName = QFileDialog::getOpenFileName(
                this,
                tr("选择GPR对空数据"), ".",
                tr("GPR 文件 (*.gpr)"));
    if (!airDataFileName.isEmpty())
        readGprData(airDataFileName, air_data_vector);
    airDataFileNameEdit->setText(airDataFileName);
    // 画出对空采集的波形
    airWavePlotter->setCurveData(0, air_data_vector);
    isAirDataSetted = true;
}


/**
 * @brief 载入金属板反射数据
 */
void LoadRefWaveDialog::onBrowseMetalDataButtonClicked()
{
    metalDataFileName = QFileDialog::getOpenFileName(
                this,
                tr("选择GPR金属板反射数据"), ".",
                tr("GPR 文件 (*.gpr)"));
    if (!metalDataFileName.isEmpty())
        readGprData(metalDataFileName, metal_data_vector);
    metalDataFileNameEdit->setText(metalDataFileName);
    // 画出对空采集的波形
    metalWavePlotter->setCurveData(0, metal_data_vector);
    isMetalDataSetted = true;
}



void LoadRefWaveDialog::onRefDataFileChanged()
{
    airDataPointsNumLabel->setText(tr("数据点数：%1").arg(air_data_vector.count()));
    metalDataPointsNumLabel->setText(tr("数据点数：%1").arg(metal_data_vector.count()));
}



/**
 * @brief
 */
void LoadRefWaveDialog::acceptButtonClicked()
{
    if (isAirDataSetted) {
        pavement_evaluation->air_data_file_name = airDataFileName;
        pavement_evaluation->air_data = air_data_vector;
        pavement_evaluation->is_air_data_loaded = true;
    }

    if (isMetalDataSetted) {
        pavement_evaluation->metal_data_file_name = metalDataFileName;
        pavement_evaluation->metal_data = metal_data_vector;
        pavement_evaluation->is_metal_data_loaded = true;
    }
}



//-------------------------------------------------------------------------------------------------
//函数名称：readGprData()
//函数说明：读取GPR文件，把文件头信息和数据分别存入相应变量
//参数说明：
//输出：gprFileName - 用户选择的GPR文件
//-------------------------------------------------------------------------------------------------
bool LoadRefWaveDialog::readGprData(const QString &gprFileName, QVector<double> &points)
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
    // 采样频率
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

    for (int i = 0; i < sampling_num; i++){
        temp_data[i] /= sampling_num;
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
    QMessageBox::information(NULL, "读取GPR文件", "读取GPR文件完毕！", QMessageBox::Yes);

    //去直流
    dspTools::subtract_mean(temp_data);

    points.resize(sampling_num);
    points.clear();

    for (int i = 0; i < sampling_num; i++)
        points.append(temp_data[i]);
    QMessageBox::information(NULL, "去直流", "去直流完毕！", QMessageBox::Yes);

    return true;
}



