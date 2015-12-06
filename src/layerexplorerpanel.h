#ifndef LAYEREXPLORERPANEL_H
#define LAYEREXPLORERPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
class QGroupBox;
class QLabel;
class QComboBox;
class QCheckBox;


class ColorTag : public QWidget
{
    Q_OBJECT
public:
    explicit ColorTag(QColor color = Qt::red, QWidget *parent = 0);
    void setTagColor(QColor color) {tagColor = color;}
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

signals:
    void interfaceColorChanged(QColor color);   //当改变了该界面颜色时，发出signal

private:
    QColor tagColor;
    bool isMouseOnTag;

};


//class LayerVisibleCheckBox : public QCheckBox
//{
//    Q_OBJECT
//public:
//    LayerVisibleCheckBox(QWidget *parent = 0);

//};



class PickLayerButton : public QPushButton
{
    Q_OBJECT
public:
    PickLayerButton(QWidget *parent = 0);

};



class DeleteLayerButton : public QPushButton
{
    Q_OBJECT
public:
    DeleteLayerButton(QWidget *parent = 0);

};




class LayerExplorerPanel : public QWidget
{
    Q_OBJECT
public:
    LayerExplorerPanel(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    // ---------GUI元素-------------
    QGroupBox *m_LayerExplorerGroup;
    QVector< QLabel* > layerLabels;
    QVector<ColorTag*> colorTags;
    QVector< QCheckBox* > layerVisibleCheckBoxs;
    QVector< PickLayerButton* > pickLayerButtons;
    QVector< DeleteLayerButton* > deleteLayerButtons;

    // --------私有成员变量---------
    int layerNum;

    // ---------私有方法------------
    void createColorComboBox(QComboBox *comboBox);
    void layoutPanel();
};






#endif // LAYEREXPLORERPANEL_H


