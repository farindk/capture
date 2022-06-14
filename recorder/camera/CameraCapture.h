
#ifndef CAPTURE_CAMERACAPTURE_H
#define CAPTURE_CAMERACAPTURE_H

#include <QCamera>
#include <optional>


struct CameraSettings
{
  std::optional<QString> cameraName;
  std::optional<QSize> resolution;
  std::optional<qreal> frameRate;
};


class CameraCapture : public QObject
{
  Q_OBJECT

public:
  CameraCapture();

  ~CameraCapture() noexcept override;

  [[nodiscard]] QList<QString> get_cameras() const;
  [[nodiscard]] QList<QSize> get_resolutions() const { return mPossibleResolutions; }
  [[nodiscard]] QList<QCamera::FrameRateRange> get_framerates() const { return mPossibleFrameRates; }

  void set_user_preferred_camera(const std::optional<QString>& camera);
  void set_user_preferred_resolution(const std::optional<QSize>& s);
  void set_user_preferred_frame_rate(const std::optional<qreal>& fr);

  void set_user_preferred_settings(const CameraSettings&);

  [[nodiscard]] std::optional<QString> get_current_camera() const { return mActiveSettings.cameraName; }
  [[nodiscard]] std::optional<QSize> get_current_resolution() const { return mActiveSettings.resolution; }
  [[nodiscard]] std::optional<qreal> get_current_frame_rate() const { return mActiveSettings.frameRate; }

//  [[nodiscard]] QCamera::State get_state() const;
  [[nodiscard]] QCamera::Status get_status() const;

  // The number of start/stop calls is reference counted. The video grabbing is only stopped when
  // no active captures exist anymore.
  void start_capture();
  void stop_capture();

signals:
  void resolution_selection_changed();
  void framerate_selection_changed();

  void current_camera_changed();
  void current_resolution_changed();
  void current_frame_rate_changed();
  void any_settings_changed(); // TODO: parameter what changed. Can/should this replace the signals above?

  void status_changed(QCamera::Status status);
  void frame_captured(const QVideoFrame& frame);

private:
  QCamera* mCamera = nullptr;

  CameraSettings mUserPreferredSettings;
  CameraSettings mActiveSettings;

  QList<QCameraInfo> mCameras;
  QList<QSize> mPossibleResolutions;
  QList<QCamera::FrameRateRange> mPossibleFrameRates;

  [[nodiscard]] bool camera_loaded() const;

  //void set_valid_active_parameters();
  void reload_possible_resolutions();
  void reload_possible_framerates();

  void select_current_resolution_from_user_preferred(bool select_unique_possibility);
  void select_current_framerate_from_user_preferred(bool select_unique_possibility);

  [[nodiscard]] bool supports(const std::optional<QSize>& resolution, const std::optional<qreal>& framerate) const;


  int mNumVideoCaptures = 0;
  class MyVideoSurface* mVideoSurface = nullptr;
};


#endif //CAPTURE_CAMERACAPTURE_H
