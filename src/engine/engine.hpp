#ifndef ASRYX_ENGINE_ENGINE_HPP
#define ASRYX_ENGINE_ENGINE_HPP

#include <stdexcept>
#include <string>
#include <sys/types.h>

namespace engine {

class TranscriptionCancelled final : public std::runtime_error
{
public:
  TranscriptionCancelled();
};

struct TranscriptionRequest
{
  std::string model_path;
  std::string vad_model_path;
  std::string wav_path;
  std::string language;
  std::string cancel_marker_path;
};

pid_t start_recording(const std::string& wav_path, const std::string& err_path);
bool stop_recording(pid_t pid);
std::string transcribe(const TranscriptionRequest& request);
bool copy_to_clipboard(const std::string& text);
bool send_notification(const std::string& message);

} // namespace engine

#endif // ASRYX_ENGINE_ENGINE_HPP
