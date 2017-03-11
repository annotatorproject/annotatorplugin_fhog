#include "widget.h"
#include <QtWidgets/QFileDialog>
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

void Widget::on_saveButton_clicked() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Trained File"),
                                                  "", tr("SVM (*.svm)"));
  fhog->saveSVM(fileName.toStdString());
}

void Widget::on_loadButton_clicked() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Load Trained File"),
                                                  "", tr("SVM (*.svm)"));
  fhog->loadSVM(fileName.toStdString());
}
