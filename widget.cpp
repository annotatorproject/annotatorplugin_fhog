#include "widget.h"
#include "fhog.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
  ui->setupUi(this);
}

Widget::~Widget() { delete ui; }

void Widget::setObjectPixmap(QPixmap pixmap) {
  ui->objectPixmap->setPixmap(pixmap);
}

void Widget::setFHOG(Annotator::Plugins::FHOG *fhog) { this->fhog = fhog; }

void Widget::setProgress(int percent) { ui->progressBar->setValue(percent); }

void Widget::on_trainButton_clicked() { this->fhog->train(); }
