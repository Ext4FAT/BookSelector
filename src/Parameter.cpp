#include "Parameter.hpp"

/**
 * @brief Parameter Impletement
 */
const char* Parameter::parameters_ = "ab:k:o:p:rt:ND";

Parameter::Parameter():
            keyword_("C++"), filename_("Booklist"), browser_("firefox"),
            topk_(100), percent_(100.0),
            reload_(false), savemd_(true), disp_(true)
{

}

Parameter::Parameter(int argc, char** argv):
            keyword_("C++"), filename_("Booklist"), browser_("firefox"),
            topk_(100), percent_(100.0),
            reload_(false), savemd_(true), disp_(true)
{
    analysisParameter(argc, argv);
}

void Parameter::usage()
{
    cout << "[USAGE]\n"
        << "\t[-b browser]  #Browser you want to display result\n"
        << "\t[-k keyword]  #Field you intend to search\n"
        << "\t[-t number]   #show TOP-K Without %, if number = -1, show all\n"
        << "\t              #show TOP k% you With % append to number\n"
        << "\t[-o filename] #File name you are desirous to output\n"
        << "\t[-a]          #Display all book in this field\n"
        << "\t[-r]          #No matter cache exist or not, re-download pages\n"
        << "\t[-N]          #Don't Save markdown file\n"
        << "\t[-D]          #Don't display html with browser\n";
}

void Parameter::configInfo()
{
    cout << "[CONFIG]\n"
         << "\tBrowser: " << browser_ << "\n"
         << "\tKeyword: " << keyword_ << "\n"
         << "\tOutput File: " << filename_ << ".html" << "\n"
         << "\tTop-K: " << (topk_ == -1? string("ALL"): to_string(topk_)) << "\n"
         //<< "\tPercent: " << (percent_ >= 100? 100.0: percent_) << "% \n"
         << "\tReload: " << string(reload_? "YES": "NO")  << "\n"
         << "\tDisplay: " << string(disp_? "YES": "NO") << "\n"
         << "\tSave Markdown: " << string(savemd_? "YES": "NO") << "\n";
}

void Parameter::analysisParameter(int argc, char** argv)
{
    int ch;
    //Analysis parameter
    while ( -1 != (ch = getopt(argc, argv, parameters_)) )
       switch (ch) {
           case 'a':
               topk_ = -1;
               percent_ = 100.0;
               break;
           case 'b':
               browser_ = std::string(optarg);
               break;
           case 'k':
               keyword_ = std::string(optarg);
               break;
           case 'o':
               filename_ = std::string(optarg);
               break;
           /*case 'p':
               percent_ = atof(optarg);
               if(!percent_)	percent_ = 100.0;
               break;*/
           case 'r':
               reload_ = true;
               break;
           case 't':
               topk_ = atoi(optarg);
               if(!topk_)	topk_ = -1;
               break;
           case 'N':
               savemd_ = false;
               break;
           case 'D':
               disp_ = false;
               break;
       }
}

int Parameter::startupBrowser()
{
    static map<string, string> _Browser2Cmdpath_ = {
                                                        {"firefox", "/usr/bin/firefox"},
                                                        {"opera", "/usr/bin/opera"},
                                                        {"midori", "/usr/bin/midori"},
                                                        {"qupzilla", "/usr/bin/qupzilla"},
                                                        {"chrome", "/opt/google/chrome/chrome"}
                                                   };
    string cmd = _Browser2Cmdpath_[browser_] + " ./" + filename_ + ".html 1>/dev/null 2>/dev/null &";
    return system(cmd.c_str());
}
