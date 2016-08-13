#include "Parse.hpp"
#include <iostream>
#include <fstream>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using std::ifstream;
using std::istreambuf_iterator;

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;

void Parse::generateBookSet(const string &catergoryDir, BookSet &bs)
{
    char fname[512];
    list<Item> itemList;
    string cmd = "ls " + catergoryDir + "|grep .html";
    FILE* dir = popen(cmd.c_str(), "r");
    while (fgets(fname, 512, dir)) {
        string filepath = catergoryDir + string(fname);
        filepath.back() = 0;
        analysisHtml(filepath, itemList);
    }
    //convert itemlist to BookSet
    bs.generate(itemList);
}

void Parse::analysisHtml(const string &htmlfile, list<Item> &infoList)
{
    //Read html to string
    ifstream htmlFileStream;
    string html;
    htmlFileStream.open(htmlfile, std::ios::in);
    htmlFileStream.seekg(0, std::ios::end);
    html.resize(htmlFileStream.tellg());
    htmlFileStream.seekg(0, std::ios::beg);
    htmlFileStream.read(&html[0], html.size());
    htmlFileStream.close();
    //Parse html
    htmlcxx::HTML::ParserDom parser;
    DomTree dom = parser.parseTree(html);
    DomTreeIter it, end;
    //Find fisrt <li> tag
    for (it = dom.begin(), end = dom.end(); it != end; it++)
        if (it->isTag() && it->tagName() == "li") {
            it->parseAttributes();
            if(it->attribute("class").second == "subject-item")
                break;
        }
    //Traversal
    for ( ; dom.is_valid(it); it = dom.next_sibling(it))
        if (it->isTag()) {
            infoList.push_back(Item());
            extractInfoFromDom(dom, it, infoList.back());
        }
}


int Parse::extractInfoFromDom(DomTree &dom, DomTreeIter &it, Item &Info)
{
    //Declare
    DomTreeIter child;
    int children_num, cnt;
    //Parse attributes and insert (Output)
    it->parseAttributes();
    for (auto attr: it->attributes())
        Info.insert(attr);
    //Recurrent Search
    children_num = it.number_of_children();
    //children_num = dom.number_of_children(it);
    for (cnt = 0; cnt < children_num; cnt++) {
        child = dom.child(it, cnt);
        if (!extractInfoFromDom(dom, child, Info)) {
            string content = child->text();
            boost::trim(content);
            if (content.size()) {
                Info[it->tagName()] = content;
                for (auto attr: it->attributes())
                    Info[attr.second] = content;
            }
        }
    }
    return children_num;
}


int Parse::pandoc_md2html(string input, string style)
{
    std::string cmd = "pandoc " +
                      input + ".md" +
                      " -c " + style +
                      " -o " + input + ".html";
    return popen (cmd.c_str (), "r") == NULL;
}

int Parse::extractInteger(const string& source)
{
    int res = 0;
    for (auto a: source)
        res = a >= '0' && a <= '9'? 10 * res + a - '0': res;
    return res;
}

long long Parse::extractID(const string& source, unsigned pos)
{
    long long res = 0;
    for (; pos < source.size(); pos++)
        res = 10 * res + source[pos] - '0';
    return res;
}

double Parse::extractDouble(const string& source)
{
    string d;
    for_each(  source.begin(), source.end(),
                    [&d](char a){
                    if ((a >= '0' && a <= '9') || a == '.')
                        d.push_back(a);
                });
    return boost::lexical_cast<double>(d);
}

#if 0
void Parse::eraseBlank(string &str)
{
    boost::replace_all(str, "\t", "");
    boost::replace_all(str, "\n", "");
    boost::replace_all(str, "\r", "");
    //boost::replace_all(str, " ", "");
}

#else
void Parse::eraseBlank(string& str)
{
    int count = 0;
    for (auto c: str)
        if (c != '\t' && c != '\n' && c != '\r')
            str[count++] = c;
    str[count] = 0;
    str.resize(count);
}
#endif

#if 0
void Parse::replaceBracket(string &str)
{
    boost::replace_all(str, "[", "<");
    boost::replace_all(str, "(", "<");
    boost::replace_all(str, "]", ">");
    boost::replace_all(str, ")", ">");
    boost::replace_all(str, "、", " ");
}
#else
void Parse::replaceBracket(string &str)
{
    for (auto& c: str)
        if (c == '[' || c== '(')
            c = '<';
        else if (c == ']' || c== ')')
            c = '>';
}
#endif

int Parse::createDir(const string &filepath)
{
    string cmd = "mkdir ";
    if (-1 == access(filepath.c_str(), F_OK)) {
        cmd += filepath;
        return execCommand(cmd);
    }
    return 1;
}

int Parse::wget(const string& keyword, int max, bool isReload, int step)
{
    string keywordDir = _PAGE_ + keyword + "/";
    string url= "http://book.douban.com/tag/" + keyword + "?start=";
    string type = "&type=R";
    int ret = 0;
    //Create dir, if exists return
    ret = createDir(_CACHE_);
    ret = createDir(_PAGE_);
    ret = createDir(keywordDir);

    /**TODO, if exist files and noReload return */
    if (ret && !isReload)
        return -1;
    //ret += createDir(_PIC_);

    //Download by wget
    for (int start = 0; start < max; start += step) {
        string cmd = "wget -t 2 -T 10 -q -O 1>/dev/null 2>/dev/null " +
                        keywordDir + to_string(start) + ".html " +
                        url + to_string(start) + type;
        ret += execCommand(cmd);
    }
    return ret;
}

int Parse::execCommand(const string& cmd)
{
    return system(cmd.c_str());
}

int Parse::wget(Parameter &myPara, int max)
{
    return wget(myPara.Keyword(), max, myPara.isReload());
}

inline void outputPixel(uchar value, int index, char x = 'z')
{
    if (value) {
        cout << ("\033["+ to_string(index+30) +";1m") << x;
    }
    else
        cout << " ";
}

typedef vector<vector<Point>> ContourSet;
void outputOneImage(Mat &img, Size sz)
{
    static string _DISPLAY_ = "IDLER";
    Mat contour = Mat::zeros(sz, CV_8U);
    ContourSet contourSet;
    Mat res, bg, fg;
    resize(img, img, sz);
    threshold(img, bg, 250, 255, CV_THRESH_BINARY_INV);
    threshold(img, fg, 150, 128, CV_THRESH_BINARY_INV);
    findContours(bg, contourSet, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    drawContours(contour, contourSet, -1, Scalar(255));
    res = contour + fg;// & binary;
    uchar* ptr = res.data;
    int index = 0;
    for (int i = 0; i < sz.height; i++) {
        for (int j = 0; j < sz.width; j++) {
            uchar value = *ptr++;
            if (value == 255)
                outputPixel(value, 5, _DISPLAY_[index++%_DISPLAY_.size()]);
            else
                outputPixel(value, 4, _DISPLAY_[index++%_DISPLAY_.size()]);
        }
        cout << endl;
    }
    cout << "\033[0m";
}

int Parse::ShowSignature(string imgpath, int width, int height)
{
    Mat src = imread(imgpath, IMREAD_GRAYSCALE);
    //threshold(src, src, 150, 255, 1);
    outputOneImage(src, Size(width, height));
    return 0;
}

int Parse::ShowIcons(string imgpath1, string imgpath2)
{
    Mat src1 = imread(imgpath1, IMREAD_GRAYSCALE);
    Mat src2 = imread(imgpath2, IMREAD_GRAYSCALE);
    threshold(src1, src1, 150, 255, 1);
    threshold(src2, src2, 150, 255, 1);
    //Size showsz(30, 22);
    //outputTwoImage(src1, src2, showsz);
    return 0;
}
    //ContourSet contours;


    /*
    findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    Rect cut = {0, 0, 0, 0};
    for (auto con: contours)
        cut |= minAreaRect(con).boundingRect();
    cout << cut << endl;
    rectangle(src, cut, Scalar(255), 2);
    imshow("src", src);
    waitKey(0);
    Mat ROI = src(cut);
    resize(src, src, showsz);
    uchar* ptr = src.data;
    for (int i = 0; i < src.rows; i++) {
        for (int j = 0; j < src.cols; j++)
            if (*ptr++) {
                //cout << ("\033["+ to_string(index) +";1m") << '.';
                cout << ("\033[31;1m") << 'z';
                usleep(10000);
            }
            else
                cout << " ";
        cout << endl;
    }
    return 0;
}
*/

/*
inline void outputPixel(uchar value, unsigned us = 0)
{
    if (value) {
        //cout << ("\033["+ to_string(index) +";1m") << '.';
        cout << ("\033[34;1m") << 'z';
        usleep(us);
    }
    else
        cout << " ";
}

void outputTwoImage(Mat &img1, Mat &img2, Size sz)
{
    resize(img1, img1, sz);
    resize(img2, img2, sz);
    uchar* ptr1 = img1.data;
    uchar* ptr2 = img2.data;
    for (int i = 0; i < sz.height; i++) {
        for (int j = 0; j < sz.width; j++)
            outputPixel(*ptr1++);
        cout << "   ";
        for (int j = 0; j < sz.width; j++)
            outputPixel(*ptr2++);
        cout << endl;
    }
    cout << "\033[0m";
}
*/
/*
void outputOneImage(Mat &img, Size sz)
{
    resize(img, img, sz);
    uchar* ptr = img.data;
    for (int i = 0; i < sz.height; i++) {
        for (int j = 0; j < sz.width; j++)
            outputPixel(*ptr++);
        cout << endl;
    }
    cout << "\033[0m";
}
*/

