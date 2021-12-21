#include "widget.h"
#include "ui_widget.h"
#include "cmain.h"

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    char filename[120] = "D:\\homework\\21+\\complie\\exam4\\Project1\\Debug\\SAMPLE.TNY";
    fun(filename);
}

Widget::~Widget()
{
    delete ui;
}

