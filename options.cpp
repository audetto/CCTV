#include "options.h"

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <iostream>

namespace po = boost::program_options;

namespace
{
  template<class T>
  const T & get(const po::variables_map & map, const char * name)
  {
    const po::variable_value & value = map[name];
    if (value.empty())
    {
      throw std::runtime_error("Missing options: " + std::string(name));
    }

    return value.as<T>();
  }

  std::vector<size_t> parseChannels(const std::string & value)
  {
    boost::char_separator<char> sep(",");
    boost::tokenizer<boost::char_separator<char>> tok(value, sep);

    std::vector<size_t> result;
    for(const auto & token : tok)
    {
      const size_t channel = std::stoul(token);
      result.push_back(channel);
    }

    return result;
  }

}

namespace ASI
{

  bool getProgramOptions(int argc, char ** argv, ProgramOptions & options)
  {
    po::options_description desc("ASI CCTV");
    desc.add_options()
      ("help,h", "Print this help message")
      ("logfile,lf", po::value<std::string>()->default_value(""), "Log file")
      ("loglevel,ll", po::value<size_t>()->default_value(0), "Log level")
      ("channels,c", po::value<std::string>()->default_value(""), "Digital channels");

    po::options_description infoDesc("Info");
    infoDesc.add_options()
      ("info,i", "CCTV Info");
    desc.add(infoDesc);

    po::options_description liveDesc("Live");
    liveDesc.add_options()
      ("live,l", "Live play")
      ("live:duration", po::value<std::size_t>(), "Duration in seconds")
      ("live:out", po::value<std::string>(), "Output folder");
    desc.add(liveDesc);

    po::options_description replayDesc("Replay");
    replayDesc.add_options()
      ("replay,r", "Replay")
      ("replay:start", po::value<std::string>(), "Start replay time (%Y-%m-%dT%H:%M:%S)")
      ("replay:end", po::value<std::string>(), "End replay time (%Y-%m-%dT%H:%M:%S)")
      ("replay:out", po::value<std::string>(), "Output folder");
    desc.add(replayDesc);

    po::options_description picDesc("Pictures");
    picDesc.add_options()
      ("pictures,p", "Capture pictures")
      ("pictures:out", po::value<std::string>(), "Output folder");
    desc.add(picDesc);

    po::variables_map vm;
    try
    {
      po::store(po::parse_command_line(argc, argv, desc), vm);

      if (vm.count("help"))
      {
        std::cout << "ASI CCTV" << std::endl << std::endl << desc << std::endl;
        return false;
      }

      options.channels = parseChannels(get<std::string>(vm, "channels"));
      options.logfile = get<std::string>(vm, "logfile");
      options.loglevel = get<size_t>(vm, "loglevel");

      if (vm.count("info"))
      {
        options.target = Info;
      }
      else if (vm.count("live"))
      {
        options.target = Live;
        options.folder = get<std::string>(vm, "live:out");
        options.duration = get<size_t>(vm, "live:duration");
      }
      else if (vm.count("replay"))
      {
        options.target = Replay;
        options.start = get<std::string>(vm, "replay:start");
        options.end = get<std::string>(vm, "replay:end");
        options.folder = get<std::string>(vm, "replay:out");
      }
      else if (vm.count("pictures"))
      {
        options.target = Pictures;
        options.folder = get<std::string>(vm, "pictures:out");
      }

      return true;
    }
    catch (const po::error& e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl << desc << std::endl;
      return false;
    }
    catch (const std::exception & e)
    {
      std::cerr << "ERROR: " << e.what() << std::endl;
      return false;
    }
  }

}
