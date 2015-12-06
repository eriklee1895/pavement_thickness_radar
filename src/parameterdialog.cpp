#include <QtGui>
#include "parameterdialog.h"

ParameterDialog::ParameterDialog(QWidget *parent) :
    QDialog(parent)
{
    //---------------检测道设置GroupBox-------------------
    traceGroup = new QGroupBox(tr("检测道设置"));
    allTraceRadioButton = new QRadioButton(tr("检测所有道"));
    manualTraceButton = new QRadioButton(tr("手动设置"));
    firstSearchPointLabel = new QLabel(tr("起始道层位大致位置点："));
    firstSearchPointEdit = new QLineEdit(tr("1"));
    firstSearchPointHintLabel = new QLabel(tr("(1-99)"));
    stratTraceLabel = new QLabel(tr("起始检测道："));
    startTraceEdit = new QLineEdit(tr("1"));
    startHintLabel = new QLabel(tr("(1-99)"));

    allTraceRadioButton->setChecked(true);
    startTraceEdit->setEnabled(false);

    connect(manualTraceButton, SIGNAL(toggled(bool)),
            startTraceEdit, SLOT(setEnabled(bool)));

    //---------------检测窗设置GroupBox-------------------
    windowGroup = new QGroupBox(tr("检测窗设置"));
    defaultWindButton = new QRadioButton(tr("使用默认值"));
    defaultWindButton->setChecked(true);
    manualWindRadioButton = new QRadioButton(tr("手动设置"));
    refWindLabel = new QLabel(tr("参考窗(点数):"));
    refWindEdit = new QLineEdit(tr("60"));
    searchWindLabel = new QLabel(tr("搜索窗(点数)："));
    searchWindEdit = new QLineEdit(tr("80"));

    allTraceRadioButton->setChecked(true);
    refWindEdit->setEnabled(false);
    searchWindEdit->setEnabled(false);

    connect(manualWindRadioButton, SIGNAL(toggled(bool)),
            refWindEdit, SLOT(setEnabled(bool)));
    connect(manualWindRadioButton, SIGNAL(toggled(bool)),
            searchWindEdit, SLOT(setEnabled(bool)));

    //---------------确定和取消按钮-------------------
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("确定"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("取消"));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(acceptButtonClicked()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    //布局
    //---------------检测道设置GroupBox Layout-------------------
    QGridLayout  *traceLayout = new QGridLayout;
    QHBoxLayout *traceRadioButtonLayout = new QHBoxLayout;
    traceRadioButtonLayout->addWidget(allTraceRadioButton);
    traceRadioButtonLayout->addWidget(manualTraceButton);
    traceLayout->addLayout(traceRadioButtonLayout, 0, 0, 1, 3);
    traceLayout->addWidget(firstSearchPointLabel, 1, 0);
    traceLayout->addWidget(firstSearchPointEdit, 1, 1);
    traceLayout->addWidget(firstSearchPointHintLabel, 1, 2);
    traceLayout->addWidget(stratTraceLabel, 2, 0);
    traceLayout->addWidget(startTraceEdit, 2, 1);
    traceLayout->addWidget(startHintLabel, 2, 2);
    traceGroup->setLayout(traceLayout);

    //---------------测窗设置GroupBox Layout-------------------
    QGridLayout  *windLayout = new QGridLayout;
    QHBoxLayout *windRadioButtonLayout = new QHBoxLayout;
    //windRadioButtonLayout->setAlignment(this, Qt::AlignRight);
    windRadioButtonLayout->addWidget(defaultWindButton);
    windRadioButtonLayout->addWidget(manualWindRadioButton);
    windLayout->addLayout(windRadioButtonLayout, 0, 0, 1, 2);
    windLayout->addWidget(refWindLabel, 1, 0);
    windLayout->addWidget(refWindEdit, 1, 1);
    windLayout->addWidget(searchWindLabel, 2, 0);
    windLayout->addWidget(searchWindEdit, 2, 1);
    windowGroup->setLayout(windLayout);

    //---------------对话框总Layout-------------------
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(traceGroup);
    mainLayout->addWidget(windowGroup);
    mainLayout->addWidget(buttonBox);

    //---------------对话框参数设置-------------------
    setLayout(mainLayout);
    setWindowTitle(tr("层位追踪参数设置"));
    setFixedSize(this->sizeHint());
}

void ParameterDialog::acceptButtonClicked()
{
    int first_search_point = firstSearchPointEdit->text().toInt();
    int start_trace = startTraceEdit->text().toInt();

    int ref_win = refWindEdit->text().toInt();
    int search_win = searchWindEdit->text().toInt();

    emit send_detection_parameter(first_search_point, start_trace, ref_win, search_win);

    qDebug() << "对话框确定按钮按下" << endl;
}









