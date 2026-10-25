#include "AnkerBase.hpp"
#include <wx/stdpaths.h>
#include <wx/filename.h>
#ifdef __linux__
#include "libslic3r/Utils.hpp"
#endif


#ifdef _WIN32
wxString AnkerBase::AnkerResourceIconPath = "/resources/icons/";
#elif __APPLE__
wxString AnkerBase::AnkerResourceIconPath = "/../Resources/icons/";
#else
wxString AnkerBase::AnkerResourceIconPath = "/resources/icons/";
#endif

AnkerBase::AnkerBase()
{
    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    wxString applicationPath = wxFileName(exePath).GetPath();
#ifdef _WIN32
    applicationPath += "/resources/icons/";
#elif __APPLE__
    applicationPath += "/../Resources/icons/";
#else
    applicationPath = wxString::FromUTF8(Slic3r::resources_dir()) + "/icons/";
#endif
    AnkerResourceIconPath = applicationPath;
}

AnkerBase::~AnkerBase()
{
}
