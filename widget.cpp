#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
}

Widget::~Widget() { delete ui; }

void Widget::setObjectPixmap(QPixmap pixmap)
{
    ui->objectPixmap->setPixmap(pixmap);
}
