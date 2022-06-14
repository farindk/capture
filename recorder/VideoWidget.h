
#ifndef CAPTURE_VIDEOWIDGET_H
#define CAPTURE_VIDEOWIDGET_H

#include <QWidget>
#include <QVideoFrame>

class VideoWidget : public QWidget
{
  Q_OBJECT

public:
  explicit VideoWidget(QWidget* parent) : QWidget(parent) { }

  void push_image(const QVideoFrame& frame);

protected:
  void paintEvent(QPaintEvent *event) override;

  QImage mCurrentImage;
};


#endif //CAPTURE_VIDEOWIDGET_H
