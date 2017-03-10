#include "fhog.h"
#include "widget.h"

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Session.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <ctype.h>
#include <iostream>

using namespace Annotator::Plugins;

Annotator::Plugins::FHOG::FHOG() {
}

FHOG::~FHOG() {}

QString FHOG::getName() { return "FHOG"; }

QWidget *FHOG::getWidget() { return &widget; }

bool FHOG::setFrame(shared_ptr<Frame> frame, cv::Mat image) {
  this->lastFrame = this->frame;
  this->frame = frame;
  this->frameImg = image;
  return lastFrame != frame;
}

// first call
void FHOG::setObject(shared_ptr<Object> object) {
  if (object != this->object) {
    this->object = object;
    widget.setObjectPixmap(getImgCrop(object->getFirstAnnotation(), 96));
  }
}

shared_ptr<Object> FHOG::getObject() const { return object; }

void FHOG::setLastAnnotation(shared_ptr<Annotation> /*annotation*/)
{

}

std::vector<shared_ptr<Commands::Command>> FHOG::getCommands() {
  std::vector<shared_ptr<Commands::Command>> commands;
  return commands;
}

cv::Rect FHOG::findObject() {
  return cv::Rect();
}

QPixmap FHOG::getImgCrop(
    shared_ptr<AnnotatorLib::Annotation> annotation, int size) const {
    if(annotation == nullptr)
        return QPixmap();
  cv::Mat tmp = project->getImageSet()->getImage(
      annotation->getFrame()->getFrameNumber());

  float x = std::max(annotation->getX(), 0.f);
  float y = std::max(annotation->getY(), 0.f);
  float w = std::min(annotation->getWidth(), tmp.cols - x);
  float h = std::min(annotation->getHeight(), tmp.rows - y);

  cv::Rect rect(x, y, w, h);

  cv::Mat cropped;
  try {
    tmp(rect).copyTo(cropped);
  } catch (cv::Exception &e) {
    std::cout << e.what();
  }

  cropped.convertTo(cropped, CV_8U);
  cv::cvtColor(cropped, cropped, CV_BGR2RGB);

  QImage img((const unsigned char *)(cropped.data), cropped.cols, cropped.rows,
             cropped.step, QImage::Format_RGB888);

  QPixmap pim = QPixmap::fromImage(img);
  pim = pim.scaledToHeight(size);
  return pim;
}
