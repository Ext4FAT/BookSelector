#include "Macro.hpp"
#include "Book.hpp"
#include "Parameter.hpp"
#include <htmlcxx/html/ParserDom.h>

//Typedef
typedef tree<htmlcxx::HTML::Node> DomTree;
typedef DomTree::iterator DomTreeIter;

namespace Parse
{
    /**
     * @brief generateBookSet: based on html files, generate BookSet
     * @param catergoryDir  html files Dir
     * @param keyword       category name
     * @param bs            return BookSet
     */
    void generateBookSet(const string& catergoryDir, BookSet& bs);

    /**
     * @brief extractInfoFromDom: transfer DOM tree to Item (map<string, string>)
     * @param dom       DOM tree root node
     * @param it        DOM tree Iterator
     * @param Info      Item info will be returned
     * @return return   children number
     */
    int extractInfoFromDom(DomTree &dom, DomTreeIter &it, Item &Info);

    /**
     * @brief analysisHtml: analysis htmlfile and generate item list
     * @param htmlfile  html file path
     * @param infoList  return info item list which html contains
     */
    void analysisHtml(const string &htmlfile, list<Item> &infoList);

    /**
     * @brief pandoc_md2html    convert markdown to html by pandoc
     * @param input             input filename
     * @param style             CSS style file path
     * @return                  success / fail
     */
    int pandoc_md2html(string input = "BookList.md", string style = _CSSSTYLE_);

    /** @brief contained int */
    int extractInteger(const string &source);

    /** @brief url_id + "/" convert to number as book id*/
    long long extractID(const string &source, unsigned pos);

    /** @brief contained double */
    double extractDouble(const string &source);

    /** @brief erase '\t' '\n' '\r' ' '*/
    void eraseBlank(string &str);

    /** @brief replace () [] {} to <> */
    void replaceBracket(string &str);

    /** @brief create directory, if exist return*/
    int createDir(const string &filepath);

    /** @brief download Html file and saved to cache */
    int wget(const string& keyword, int max, bool isReload, int step = 20);
    int wget(Parameter &myPara, int max = 1000);

    /** @brief ImageBeep: read image and beep output on the screen */
    int ShowSignature(string imgpath, int width = 70, int height = 16);
    int ShowIcons(string imgpath1, string imgpath2);

    inline int execCommand(const string& cmd);
}
