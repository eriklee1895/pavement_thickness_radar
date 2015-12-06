#ifndef PARAMETERDIALOG_H
#define PARAMETERDIALOG_H

#include <QDialog>

class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QDialogButtonBox;

class ParameterDialog : public QDialog
{
    Q_OBJECT
public:
    ParameterDialog(QWidget *parent = 0);
    
signals:
    void send_detection_parameter(int first_search_point, int start_trace, int ref_win, int search_win);
public slots:
    void acceptButtonClicked();

public:
    QGroupBox *traceGroup;
    QRadioButton *allTraceRadioButton;
    QRadioButton *manualTraceButton;
    QLabel *firstSearchPointLabel;
    QLineEdit *firstSearchPointEdit;
    QLabel *firstSearchPointHintLabel;
    QLabel *stratTraceLabel;
    QLineEdit *startTraceEdit;
    QLabel *startHintLabel;

    QGroupBox *windowGroup;
    QRadioButton *defaultWindButton;
    QRadioButton *manualWindRadioButton;
    QLabel *refWindLabel;
    QLineEdit *refWindEdit;
    QLabel *searchWindLabel;
    QLineEdit *searchWindEdit;

    QDialogButtonBox *buttonBox;

    
};

#endif // PARAMETERDIALOG_H


