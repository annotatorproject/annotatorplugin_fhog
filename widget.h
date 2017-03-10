#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPixmap>

namespace Ui {
class Widget;
}

class Widget : public QWidget {
  Q_OBJECT

 public:
  explicit Widget(QWidget *parent = 0);
  ~Widget();
  void setObjectPixmap(QPixmap pixmap);

 private slots:

 private:
  Ui::Widget *ui;
};

#endif  // WIDGET_H
