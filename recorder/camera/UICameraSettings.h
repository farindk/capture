//
// Created by farindk on 12.01.21.
//

#ifndef CAPTURE_UICAMERASETTINGS_H
#define CAPTURE_UICAMERASETTINGS_H

#include <QWidget>
#include <QCamera>
#include <optional>

QT_BEGIN_NAMESPACE
namespace Ui { class UICameraSettings; }
QT_END_NAMESPACE

class CameraCapture;


class UICameraSettings : public QWidget
{
Q_OBJECT

public:
  explicit UICameraSettings(QWidget* parent = nullptr);

  ~UICameraSettings() override;

  void set_cameras(const QList<QString>& cameras);

  void set_resolutions(const QList<QSize>& resolutions);

  void set_framerates(const QList<QCamera::FrameRateRange>& framerates);

  void set_enable(bool enable);

  void connect_to_CameraCapture(CameraCapture& capture);

signals:
  void camera_selected(std::optional<QString> camera);
  void resolution_selected(std::optional<QSize> resolution);
  void framerate_selected(std::optional<qreal> framerate);

private:
  Ui::UICameraSettings* ui;

  std::optional<QString> mCurrentCamera;
  std::optional<QSize> mCurrentResolution;
  std::optional<qreal> mCurrentFrameRate;

  void set_camera_widget_to_current();
  void set_resolution_widget_to_current();
  void set_frame_rate_widget_to_current();
};

#endif //CAPTURE_UICAMERASETTINGS_H
