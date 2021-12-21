#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QTextBrowser>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget* parent = nullptr);
    ~Widget();

private:
    QTextEdit* textEdit;
    QTextBrowser* textBrowser;

private slots:
    void openFile();
    void saveFile();
    void genTree();
};
#endif // WIDGET_H
