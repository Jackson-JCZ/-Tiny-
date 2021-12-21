#include "widget.h"
#include "cmain.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

Widget::Widget(QWidget* parent)
    : QWidget(parent)
{
    QPushButton* btn1 = new QPushButton("打开源程序");
    connect(btn1, SIGNAL(clicked()), this, SLOT(openFile()));
    QPushButton* btn2 = new QPushButton("保存源程序");
    connect(btn2, SIGNAL(clicked()), this, SLOT(saveFile()));
    QPushButton* btn3 = new QPushButton("生成语法树");
    connect(btn3, SIGNAL(clicked()), this, SLOT(genTree()));
    QHBoxLayout* Layout1 = new QHBoxLayout;
    Layout1->addStretch();
    Layout1->addWidget(btn1);
    Layout1->addStretch();
    Layout1->addWidget(btn2);
    Layout1->addStretch();
    Layout1->addWidget(btn3);
    Layout1->addStretch();

    textEdit = new QTextEdit;
    textBrowser = new QTextBrowser;
    QHBoxLayout* Layout2 = new QHBoxLayout;
    Layout2->addWidget(new QLabel("源程序:"));
    Layout2->addWidget(textEdit);
    Layout2->addWidget(new QLabel("语法树:"));
    Layout2->addWidget(textBrowser);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(Layout1);
    mainLayout->addLayout(Layout2);
    this->setLayout(mainLayout);
}

Widget::~Widget()
{
}

/* 函数功能：读取文件中的文法规则，并显示到界面上 */
void Widget::openFile()
{
    // 选择文件路径
    QString fileName = QFileDialog::getOpenFileName(
                           this,
                           "打开文件",
                           ".",
                           "tiny文件(*.tny);;所有文件(*.*)");
    // 如果没有选择文件，弹窗警告
    if(fileName.isEmpty()) {
        QMessageBox::warning(this, "警告", "没有选择文件！");
        return;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString line = in.readLine();
    QString s = "";
    // 读取内容
    while(!line.isNull()) {
        s += line + "\n";
        line = in.readLine();
    }
    textEdit->setText(s);
}

/* 函数功能：将输入的文法规则保存为txt文件 */
void Widget::saveFile()
{
    // 如果文本框没有内容，弹窗警告
    if(textEdit->toPlainText() == "") {
        QMessageBox::warning(this, "警告", "没有输入！");
        return;
    }

    // 选择保存路径
    QString path = QFileDialog::getSaveFileName(
                       this,
                       "保存文件",
                       "./源程序",
                       "tiny程序(*.tny);;All files(*.*)");
    if(path.isEmpty()) {
        return;
    }
    QFile* savefile = new QFile(path);
    savefile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    savefile->write(textEdit->toPlainText().toUtf8());
    savefile->close();
    QMessageBox::information(this, "完成", "文件保存成功！");
}

void Widget::genTree()
{
    // 如果文本框没有内容，弹窗警告
    if(textEdit->toPlainText() == "") {
        QMessageBox::warning(this, "警告", "没有输入！");
        return;
    }

    // 生成临时文件
    QString path = "./tmp.tny";
    QFile* savefile = new QFile(path);
    savefile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    savefile->write(textEdit->toPlainText().toUtf8());
    savefile->close();

    // 生成语法树
    textBrowser->setText(QString::fromStdString(fun(path.toLatin1().data())));

    // 删除临时文件
    QFile::remove(path);
}
