#include "fhog.h"
#include "widget.h"

#include <AnnotatorLib/Annotation.h>
#include <AnnotatorLib/Commands/NewAnnotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Session.h>

#include <opencv2/core/core.hpp>

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
