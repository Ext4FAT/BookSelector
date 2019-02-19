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
#include <htmlcxx/html/ParserDom.h>
#include "Macro.hpp"
#include "BookInfo.hpp"
using std::string;
using std::vector;
using std::map;

//Create Boook Information from map<string, string>:  <nodename, content>
BookInfo* createBookInfo(_MSS_ &mss)
{
    //Assign bookinfo by swap
    BookInfo* bi = new BookInfo(); 
    bi->rank_ = atof(mss["rating_nums"].c_str());
    bi->name_CN_.swap(mss["title"]); 
    bi->href_.swap(mss["href"]); 
    bi->pic_.swap(mss["src"]); 
    bi->pl_.swap(mss["pl"]); 
    //Trim all blank char
    bi->brief_.swap(mss["p"]); 
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
	std::stringstream transfer;
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
    std::string type = "￥"; 
    if (price.find("USD")  != std::string::npos) type = "$"; 
    //Transfer price to string
    for_each(  price.begin(), price.end(), 
                    [&transfer](char a){
                    if (a == '.' ||  (a >= '0' && a <= '9'))
                        transfer << a; 
                }); 	
    try{
		transfer >> bi->price_;
        bi->price_ = type + bi->price_;
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
int extractInfoFromDom(_DomTree_ &dom, _DomTreeIter_ it, int level, _MSS_ &Info)
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

std::vector<BookInfo*> analysisHtml(std::string htmlfile)
{
    //Read html to string
    std::ifstream htmlFileStream; 
    htmlFileStream.open(htmlfile, std::ios::in);
    std::string html; 
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
    std::vector<BookInfo*> booklist; 
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

std::vector<BookInfo*> readFromDir(std::string dirpath)
{
    std::vector<BookInfo*> booklist; 
    std::string cmd = "ls " + dirpath;
	FILE* dir = popen(cmd.c_str(), "r");
	char fname[512];
	while (fgets(fname, 512, dir)) {
        std::string filepath = dirpath + std::string(fname);
        boost::trim(filepath); 
        std::vector<BookInfo*> sublist = analysisHtml(filepath); 
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

void outputBookInfo(std::vector<BookInfo*> booklist, std::string &keyword, int topk = -1)
{
	MD_TITLE("#BookList: [" << keyword << "]");
	MD_HEAD;
	int index = 1;
    if(topk == -1)
 	    for (auto book: booklist)
		    MD_TABLE(index++, book);
    else 
        for (auto book: booklist) {
            MD_TABLE(index++, book); 
            if (index > topk)  break; 
        }
}

bool compareP(BookInfo* b1, BookInfo* b2)
{
    return *b1 > *b2; 
}

int main()
{
    std::vector<BookInfo*> booklist = readFromDir(std::string(_PAGE_));
    std::sort(booklist.begin(), booklist.end(), compareP); 
    std::string keyword = "COMPUTER SCIENCE"; 
    outputBookInfo(booklist, keyword, 100); 
    return 0; 
}
