#include <string>
#include <vector>

namespace ASI
{

  enum ProgramTarget
    {
     Info,
     Live,
     Pictures,
     Replay
    };

  struct ProgramOptions
  {
    ProgramTarget target = Info;
    std::vector<size_t> channels;
    size_t loglevel;
    std::string logfile;

    // for live
    size_t duration;

    // for replay
    std::string start, end;
    std::string folder;
  };

  bool getProgramOptions(int argc, char ** argv, ProgramOptions & options);

}
