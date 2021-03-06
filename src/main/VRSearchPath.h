// ${CMAKE_NO_EDIT_WARNING}
#ifndef VRSEARCHPATH_H
#define VRSEARCHPATH_H

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <fcntl.h>

#include <config/VRDataIndex.h>

namespace MinVR {

/// This is a simple little class to manage the process of searching a path of
/// directories for a particular file.  MinVR uses subclasses of it both for
/// searching for plugin libraries and for searching for configuration files.
/// The class works as is for simple searching for files, but if you want to
/// add semantic restrictions to the file (i.e. "ends in '.xml' or 'in a
/// subdirectory called lib and ends in '.dylib', make a sub-class and
/// redefine the selectFile() method.
class VRSearchPath {
 protected:
  std::list<std::string> _searchPath;

  virtual std::string _selectFile(const std::string &file,
                                  const std::string &directory) {
    std::string validDirectory = directory;
    if (*(validDirectory.rbegin()) != '/') validDirectory += "/";
    return validDirectory + file;
  }

 public:
  VRSearchPath() {};

  /// \brief Add an entry to the search path.
  ///
  /// Adds the given directory name to the list of directories to search.
  /// Entries can use environment variables, bracketed with "${}".
  ///
  /// \param pathEntry The directory name to add.
  /// \param start A boolean value.  If true (default), add the entry at
  /// the beginning of the list.  Otherwise add it to the end.
  void addPathEntry(const std::string &pathEntry, bool start = true);

  /// \brief Create a search path from a single string.
  ///
  /// Adds a whole path from one colon-separated string.
  void digestPathString(const std::string &searchList);

  /// \brief Find a file.
  ///
  /// \return Returns a fully-resolved path name of the file in question,
  /// wherever it lies on the search path.  If the file is not found, the
  /// return value is empty.
  std::string findFile(const std::string &desiredFile);

  /// \brief Return path as a single string.
  ///
  /// Returns the search path as a single colon-separated string.
  std::string getPath() const;

  friend std::ostream &operator<<(std::ostream &os, const VRSearchPath &p) {
    return os << p.getPath();
  };
};

/// Same as VRSearchPath, but built to accommodate the specific semantics of
/// the plugin naming.
class VRSearchPlugin : public VRSearchPath {

 protected:
  std::string _selectFile(const std::string &file,
                          const std::string &directory) {

    // "file" indicates the name of the plugin, which will appear in both the
    // directory name *and* the name of the library containing the plugin
    // code.
    std::string validDirectory = directory;
    if (*(directory.rbegin()) != '/') {
      validDirectory += "/" + file;
    } else {
      validDirectory += file;
    }

    std::string buildType = "";
#ifdef MinVR_DEBUG
    buildType = "d";
#endif

#if defined(WIN32)
    return validDirectory + "/bin/" + file + buildType + ".dll";

#elif defined(__APPLE__)
    return validDirectory + "/lib/lib" + file + buildType + ".dylib";

#else // Linux
    return validDirectory + "/lib/lib" + file + buildType + ".so";

#endif
  }

 public:
  VRSearchPlugin();
};

/// Same as VRSearchPath, but built to accommodate the specific semantics of
/// the configuration file naming.
class VRSearchConfig : public VRSearchPath {

 protected:
  std::string _selectFile(const std::string &file,
                          const std::string &directory) {

    std::string validDirectory = directory;
    if (*(directory.rbegin()) != '/') {
      validDirectory += "/";
    }

    std::string validFile = file;
    // Does the file already have the suffix specified?
    if (validFile.find('.') == std::string::npos) validFile += ".minvr";

    return validDirectory + validFile;
  };

 public:
  // The actual search path is defined in the constructor.
  VRSearchConfig();
};

}

#endif // VRSEARCHPATH_H
