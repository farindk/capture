
#ifndef CAPTURE_SETTINGS_H
#define CAPTURE_SETTINGS_H

class CameraCapture;
class Recorder;


class Settings
{
public:
  static void load_camera_settings(CameraCapture& capture, int cameraId);

  static void save_camera_settings(const CameraCapture& capture, int cameraId);

  static void load_recording_settings(Recorder& recorder, int recorderId);

  static void save_recording_settings(const Recorder& recorder, int recorderId);
};


#endif //CAPTURE_SETTINGS_H
