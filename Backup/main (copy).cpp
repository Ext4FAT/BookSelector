/*************************************************************************
    > File Name: test-htmlcxx.cpp
    > Author: zyy
    > Mail: zyy34472@gmail.com
    > Created Time: Fri 22 Jan 2016 09:29:56 AM CST
 ************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <htmlcxx/html/ParserDom.h>

#include <memory>
#include "Macro.hpp"
#include "Book.hpp"
using std::string;
using std::vector;
using std::map;
using std::shared_ptr;
using std::ifstream;


//Extract digital number
int extractInteger(const string& source)
{
    int res = 0;
    for (auto a: source)
        res = a >= '0' && a <= '9'? 10 * res + a - '0': res;
    return res;
}

double extractDouble(const string& source)
{
    string d;
    for_each(  source.begin(), source.end(),
                    [&d](char a){
                    if ((a >= '0' && a <= '9') || a == '.')
                        d.push_back(a);
                });
    return boost::lexical_cast<double>(d);
}

inline void eraseBlank(string& str)
{
    int count = 0;
    for (auto c: str)
        if (c != '\t' && c != '\n' && c != '\r' && c != ' ')
            str[count++] = c;
    str[count] = 0;
    //str.resize(count + 1);
}

//Create Boook Information from map<string, string>:  <nodename, content>
BookInfo_Ptr createBookInfo(_MSS_ &mss)
{
    //Assign bookinfo by swap
    BookInfo_Ptr bi = BookInfo_Ptr(new BookInfo());
    try
    {
        bi->rank_ = boost::lexical_cast<double>(mss["rating_nums"]);
    }
    catch(boost::bad_lexical_cast&)    // 转换失败会抛出一个异常
    {
        bi->rank_ = 0;
    }
    bi->name_CN_.swap(mss["title"]);
    bi->href_.swap(mss["href"]);
    bi->pic_.swap(mss["src"]);
    //extract pl num;
    bi->pl_ = extractInteger(mss["pl"]);
    //Trim all blank char
    bi->brief_.swap(mss["p"]);
    //eraseBlank(bi->brief_);

    boost::replace_all(bi->brief_, "\t", "");
    boost::replace_all(bi->brief_, "\n", "");
    boost::replace_all(bi->brief_, "\r", "");
    boost::replace_all(bi->brief_, " ", "");

    //Split string
    std::vector<std::string> desc;
    std::string con = mss["pub"];
    boost::trim(con);
    boost::split(desc, con, boost::is_any_of("/"), boost::token_compress_on);
    for (auto& d: desc) {
        boost::replace_all(d, "[", "<");
        boost::replace_all(d, "(", "<");
        boost::replace_all(d, "]", ">");
        boost::replace_all(d, ")", ">");
        boost::replace_all(d, "、", " ");
    }
    //Parse pub_comp, pub_data, author, prcie
    int dsize = desc.size(), i;
    std::string price = desc[dsize - 1];
    std::string db_search = "http://book.douban.com/search/";

    if (price.find("-") != std::string::npos) {
        bi->publish_date_ = price;
        bi->publish_company_ = desc[dsize - 2];
        for (dsize -= 2, i = 0; i < dsize; i++)
            bi->author_ += " [" + desc[i] + "](" + db_search + desc[i] +")";
        return bi;
    }

    if (dsize < 3)	return bi;

    try{
        //Transfer price to string
        bi->price_type_ = "￥";
        if (price.find("USD")  != std::string::npos)
            bi->price_type_ = "$";
        bi->price_ = extractDouble(price);
        //publication info
        if (desc[dsize - 2].find("$") == std::string::npos)
            bi->publish_date_ = desc[dsize - 2];
        bi->publish_company_= desc[dsize - 3];
    }
    catch(std::exception){
        return bi;
    }
    for (dsize -= 3, i = 0; i < dsize; i++)
        bi->author_ += " [" + desc[i] + "](" + db_search + desc[i] +")\t";
    return bi;
}


//Analysis info from Dom-tree
int extractInfoFromDom(_DomTree_ &dom, _DomTreeIter_& it, int level, _MSS_ &Info)
{
    //Declare
    _DomTreeIter_ child;
    int children_num, cnt;
    //Parse attributes and insert (Output)
    it->parseAttributes();
    for (auto attr: it->attributes())
        Info.insert(attr);
    //Recurrent Search
    children_num = dom.number_of_children(it);
    for (cnt = 0, level++; cnt < children_num; cnt++) {
        child = dom.child(it, cnt);
        if (!extractInfoFromDom(dom, child, level, Info)) {
            std::string content = child->text();
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

//Output map<string, string>
void output(_MSS_ &Info)
{
    std::cout << "[MapSize]\t" << Info.size() << std::endl;
    for (auto i: Info)
        std::cout << "[" << i.first <<  "]   " << i.second << std::endl;
}

std::vector<BookInfo_Ptr> analysisHtml(std::string htmlfile)
{
    //Read html to string
    ifstream htmlFileStream;
    htmlFileStream.open(htmlfile, std::ios::in);
    string html;
    htmlFileStream.seekg(0, std::ios::end);
    html.resize(htmlFileStream.tellg());
    htmlFileStream.seekg(0, std::ios::beg);
    htmlFileStream.read(&html[0], html.size());
    htmlFileStream.close();
    //Parse html
    htmlcxx::HTML::ParserDom parser;
    _DomTree_ dom = parser.parseTree(html);
    _DomTreeIter_ it, end;
    //Find fisrt <li> tag
    for (it = dom.begin(), end = dom.end(); it != end; it++)
        if (it->isTag() && it->tagName() == "li") {
            it->parseAttributes();
            if(it->attribute("class").second == "subject-item")
                break;
        }
    //Traversal
    std::vector<BookInfo_Ptr> booklist;
    for ( ; dom.is_valid(it); it = dom.next_sibling(it)) {
        if (!it->isTag())   continue;
        _MSS_ Info;
        extractInfoFromDom(dom, it, 0, Info);
        booklist.push_back(createBookInfo(Info));
    }
    /*
    std::sort(booklist.begin(), booklist.end(), compareP);
    MD_TITLE("####Booklist");
    MD_HEAD;
    int i = 0;
    for (auto b: booklist)
        MD_TABLE(++i, b);
    */
    return booklist;
}

std::vector<BookInfo_Ptr> readFromDir(std::string dirpath)
{
    std::vector<BookInfo_Ptr> booklist;
    std::string cmd = "ls " + dirpath + "|grep .html";
    //std::cout << cmd << std::endl;
    FILE* dir = popen(cmd.c_str(), "r");
    char fname[512];
    while (fgets(fname, 512, dir)) {
        string filepath = dirpath + std::string(fname);
        filepath.back() = 0;
        std::vector<BookInfo_Ptr> sublist = analysisHtml(filepath);
        booklist.insert(booklist.end(), sublist.begin(), sublist.end());
    }
    return booklist;
}

int pandoc_md2html(std::string input = "BookList", std::string style = _CSSSTYLE_)
{
    std::string cmd = "pandoc " +
                      input + ".md" +
                      " -c " + style +
                      " -o " + input + ".html";
    return popen (cmd.c_str (), "r") == NULL;
}

void outputBookInfo(std::vector<BookInfo_Ptr> booklist, std::string &keyword, int topk = -1)
{
    MD_TITLE("#BookList: [" << keyword << "]");
    MD_HEAD;
    int index = 1;
    if(topk == -1)
        for (auto book: booklist)
            MD_TABLE_PTR(index++, book);
    else
        for (auto book: booklist) {
            MD_TABLE_PTR(index++, book);
            if (index > topk)  break;
        }
}

bool compareP(BookInfo_Ptr b1, BookInfo_Ptr b2)
{
    return *b1 > *b2;
}

int main()
{
    std::vector<BookInfo_Ptr> booklist = readFromDir(std::string(_PAGE_));
    std::sort(booklist.begin(), booklist.end(), compareP);

    std::string keyword = "COMPUTER SCIENCE";
    outputBookInfo(booklist, keyword, 100);


    return 0;
}









    /*
    int i = 0;

    for (it = dom.begin(), end = dom.end(); it != end; it++) {
        if (it->isTag() && it->tagName() == "li") {
            it->parseAttributes();
            if (it->attribute("class").second == "subject-item") {
                unsigned int count = dom.number_of_siblings(it);
                do {
                    if (it->isTag()) {
                        std::cout << "[" << count << "]" <<  it->text() << std::endl;
                        next = it;
                        while(!(++next)->isTag())   ;
                        unsigned int subcount = dom.number_of_siblings(next);
                        do {
                            if(next->isTag()){
                                next->parseAttributes();
                                std::cout << "\t[" << subcount <<  "]" << next->text() << std::endl;
                                for (auto attr: next->attributes()) {
                                    std::cout << "\t" << attr.second << std::endl;
                                }
                            }
                            next = dom.next_sibling(next);
                        }while(subcount--);

                    }
                    it = dom.next_sibling(it);
                }while(count--);
                return  -1;
            }
        }
    }

    */

    /*
     for (it = dom.begin(), end = dom.end(); it != end; it++) {
        it->parseAttributes();
        if (it->isTag() && 0 == it->tagName().compare("li")) {
            if (it->attribute("class").second == "subject-item") {
                it++;
                while(!(it->isTag()))  {
                    it++;
                }
                it->parseAttributes();
                //next = dom.begin(it);
                //next->parseAttributes();
                std::cout << "\t" << it->text() << std::endl;
                for (auto attr: it->attributes()) {
                    cout << "\t\t" << attr.first << "\t" << attr.second << std::endl;
                }
                next = dom.begin(it);
                next->parseAttributes();
                std::cout << it->attribute("href").second << std::endl;
                std::cout << "\t" << it->text() << std::endl;
                */
    /*
    for (auto d: dom) {
        if (!d.isTag()) continue;
        if (d.tagName().compare("li")) continue;
        d.parseAttributes();
        if (d.attribute("class").second == "subject-item") {
            std::cout << "[" << i++ << "]" << std::endl;
            std::cout << d.attribute("class").second  << std::endl;
        }
        for (auto attr: d.attributes()) {
            std::cout << "\t" << attr.first << "\t" << attr.second << std::endl;
        }*/
        //std::cout << std::endl;


    /*
    it  = dom.begin();
    end = dom.end();
    for (; it != end; it++) {
        //it->parseAttributes();
        if (it->isTag() && 0 == it->tagName().compare("div")){
            //std::cout << it->text() << std::endl;
            it->parseAttributes();
            next = dom.begin(it);
            //next->parseAttributes();
            std::cout << next->text() << std::endl;
            std::cout << "\t" << next->text() << std::endl;
            for (auto attr: next->attributes()) {
                cout << "\t\t" << attr.first << "\t" << attr.second << std::endl;
            }
            //next = dom.begin(it);
            //next->parseAttributes();
            //std::cout << it->attribute("href").second << std::endl;
            //std::cout << "\t" << it->text() << std::endl;
        }
    }
    */



        //if(d.tagName().compare("a") == 0)
        /*
        if (d.tagName().compare("div") == 0) {
            d.parseAttributes();
            if (d.attribute("class").first) {
                std::cout << "\t" << d.attribute("class").second << std::endl;
            }
            for (auto attr: d.attributes()) {
                //std::cout << "\t" << attr.second << std::endl;
                if (attr.second == "pic" || attr.second == "info"|| attr.second == "pub" || attr.second == "star clearfix")
                std::cout << "\t" << attr.first << "\t" << attr.second << endl;
            }
        }
        else if (d.tagName().compare("a") == 0) {
            d.parseAttributes();
            //std::pair<string>
            if (d.attribute("title").first){
                std::cout << "a" << std::endl;
                std::cout << "\t" << d.attribute("title").second << std::endl;
                std::cout << "\t" << d.attribute("href").second << std::endl;
            }
            */
            /*
            for (auto attr: d.attributes()) {
                std::cout << "\t" << attr.first << "\t" << attr.second << endl;
            }
            */


        /*
        if (d.attribute("href").first) {
            std::cout << "\t" << d.attribute("href").second << std::endl;
        }

    }*/
