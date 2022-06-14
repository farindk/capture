
#include <QSettings>
#include "Settings.h"
#include "camera/CameraCapture.h"


static std::string append_number(const std::string& str, int nr)
{
  return str + "-" + std::to_string(nr);
}


void Settings::load_camera_settings(CameraCapture& capture, int id)
{
  // --- load previous settings

  CameraSettings cameraSettings;

  QSettings settings("fishcounting", "capture");

  settings.beginGroup(QString::fromStdString(append_number("camera", id)));

  auto cameraName = settings.value("name");
  if (!cameraName.isNull()) {
    cameraSettings.cameraName = cameraName.toString();
  }

  auto cameraResolution = settings.value("resolution");
  if (!cameraResolution.isNull()) {
    cameraSettings.resolution = cameraResolution.toSize();
  }

  auto cameraFrameRate = settings.value("framerate");
  if (!cameraFrameRate.isNull()) {
    cameraSettings.frameRate = cameraFrameRate.toReal();
  }

  capture.set_user_preferred_settings(cameraSettings);
}


template <typename T> QVariant optional_to_variant(std::optional<T> value) {
  QVariant v;
  if (value) {
    v = *value;
  }
  return v;
}

void Settings::save_camera_settings(const CameraCapture& capture, int id)
{
  QSettings settings("fishcounting", "capture");
  settings.beginGroup(QString::fromStdString(append_number("camera", id)));
  settings.setValue("name", optional_to_variant(capture.get_current_camera()));
  settings.setValue("resolution", optional_to_variant(capture.get_current_resolution()));
  settings.setValue("framerate", optional_to_variant(capture.get_current_frame_rate()));
}


void Settings::load_recording_settings(Recorder& recorder, int recorderId)
{
}


void Settings::save_recording_settings(const Recorder& recorder, int recorderId)
{
}
