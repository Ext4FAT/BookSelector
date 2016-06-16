#include "Common.hpp"

class Parameter {
public:
    /** @brief usage: Output Parameter Usage */
    static void usage();
    /** @brief Parameter: Construction */
    Parameter();
    Parameter(int argc, char** argv);
    /** @brief analysisParameter: getopt with argc and argv */
    void analysisParameter(int argc, char** argv);
    /** @brief configInfo: output configuration setting */
    void configInfo();
    /** @brief startupBrowser: show result in which Browser you config */
    int startupBrowser();

    /** @brief Get Attribute */
    inline const string& Keyword() { return keyword_; }
    inline const string& Filename() { return filename_; }
    inline const string& Browser() { return browser_; }
    inline bool isReload() const { return reload_; }
    inline bool isSaveMarkDown() const { return savemd_; }
    inline bool isDisplay() const { return disp_; }

private:
    string keyword_;
    string filename_;
    string browser_;
    int topk_;
    double percent_;
    bool reload_;
    bool savemd_;
    bool disp_;
    /*
    string keyword_ = "C++";
    string filename_ = "Booklist";
    string browser_ = "firefox";
    int topk_ = -1;
    double percent_ = 100.0;
    bool reload_ = false;
    bool genmd_ = true;
    bool disp_ = true;
    */
private:
    static const char* parameters_;
};



