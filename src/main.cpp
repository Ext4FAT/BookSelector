/*************************************************************************
    > File Name: test-htmlcxx.cpp
    > Author: zyy
    > Mail: zyy34472@gmail.com
    > Created Time: Fri 22 Jan 2016 09:29:56 AM CST
 ************************************************************************/
#include "Macro.hpp"
#include "Book.hpp"
#include "Parse.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <algorithm>

using namespace Parse;
using namespace std;

inline double duration(clock_t start, clock_t end)
{
    return (end - start) * 1000.0 / CLOCKS_PER_SEC;
}

inline void outputDuration(string what, clock_t start, clock_t end)
{
    cout << "[" << what << "]   " << duration(start, end) << "ms" << endl;
}


int testParameter()
{
    Parameter::usage();
    Parameter myPara;
    myPara.configInfo();
    return 0;
}

int analysis(Parameter &myPara)
{
    string dir = _PAGE_ + myPara.Keyword() + "/";
    string mdfilename = myPara.Filename() + ".md";
    ofstream fout(mdfilename);
    BookSet bs(myPara.Keyword());
    generateBookSet(dir, bs);
    //out
    bs.outputBook(fout, OUTPUT_TOPK, 100);
    //convert to html
    pandoc_md2html(myPara.Filename());
    ///multi-process
    if (!myPara.isSaveMarkDown()) {
        string cmd = "rm " + mdfilename;
        return system(cmd.c_str());
    }
    return 1;
}

int test(int argc, char** argv)
{
    clock_t init, start, end;
    init = clock();
    _SplitLine_;
    cout << "\033[31;1m [" << basename(argv[0]) << "] Developed By" << endl;
    imageBeepOne("./Configuration/ICON/idler.png", 70, 13);
    //imageBeep("./Configuration/ICON/book2.jpeg", "./Configuration/ICON/book3.png");
    _SplitLine_;
    //Show usage
    Parameter::usage();
    _SplitLine_;
    //Analysis parameter
    start = clock();
    Parameter myPara(argc, argv);
    myPara.configInfo();
    _SplitLine_;
    end = clock();
    outputDuration("Analysis Parameters", start, end);
    _SplitLine_;
    //Download
    start = clock();
    wget(myPara, 1000);
    end = clock();
    outputDuration("Download Htmls", start, end);
    _SplitLine_;
    //Analysis
    start = clock();
    analysis(myPara);
    end = clock();
    outputDuration("Parse Htmls", start, end);
    _SplitLine_;
    //Display
    if (myPara.isDisplay())
        myPara.startupBrowser();
    end = clock();
    outputDuration("Total Time", init, end);
    _SplitLine_;
    return 0;
}


int main(int argc, char** argv)
{
    test(argc, argv);
    //imageBeepOne("./Configuration/ICON/hello.png", 90);
    //imageBeepOne("./Configuration/ICON/world.png", 90);
    //imageBeepOne("./Configuration/ICON/helloword.png", 160, 18);

    //imageBeep("./Configuration/ICON/icon.jpg");

    return 0;
}

