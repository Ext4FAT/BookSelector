#include "Parse.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
using std::ifstream;

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
    children_num = dom.number_of_children(it);
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

void Parse::analysisHtml(const string &htmlfile, list<Item> &infoList)
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
    for ( ; dom.is_valid(it); it = dom.next_sibling(it)) {
        if (!it->isTag())   continue;
        infoList.push_back(Item());
        extractInfoFromDom(dom, it, infoList.back());
    }
}

void Parse::convertItem2BookInfo(Item &item, BookInfo &bi)
{
    //Assign bookinfo by swap
    bi.rank_ = atof(item["rating_nums"].c_str());
    bi.name_CN_.swap(item["title"]);
    bi.href_.swap(item["href"]);
    bi.pic_.swap(item["src"]);
    //extract pl num;
    bi.pl_ = extractInteger(item["pl"]);
    //Trim all blank char
    bi.brief_.swap(item["p"]);
    boost::replace_all(bi.brief_, "\t", "");
    boost::replace_all(bi.brief_, "\n", "");
    boost::replace_all(bi.brief_, "\r", "");
    boost::replace_all(bi.brief_, " ", "");
    //Split string
    vector<string> desc;
    string& con = item["pub"];
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
    string& price = desc.back();
    string db_search = "http://book.douban.com/search/";

    if (price.find("-") != std::string::npos) {
        bi.publish_date_ = price;
        bi.publish_company_ = desc[dsize - 2];
        for (dsize -= 2, i = 0; i < dsize; i++)
            bi.author_ += " [" + desc[i] + "](" + db_search + desc[i] +")";
        return;
    }

    if (dsize < 3)	return;

    try{
        //Transfer price to string
        bi.price_type_ = "￥";
        if (price.find("USD")  != std::string::npos)
            bi.price_type_ = "$";
        bi.price_ = extractDouble(price);
        //publication info
        if (desc[dsize - 2].find("$") == std::string::npos)
            bi.publish_date_ = desc[dsize - 2];
        bi.publish_company_= desc[dsize - 3];
    }
    catch(std::exception){
        return;
    }
    for (dsize -= 3, i = 0; i < dsize; i++)
        bi.author_ += " [" + desc[i] + "](" + db_search + desc[i] +")\t";
    return;
}


//Extract digital number
inline int Parse::extractInteger(const string& source)
{
    int res = 0;
    for (auto a: source)
        res = a >= '0' && a <= '9'? 10 * res + a - '0': res;
    return res;
}

inline double Parse::extractDouble(const string& source)
{
    string d;
    for_each(  source.begin(), source.end(),
                    [&d](char a){
                    if ((a >= '0' && a <= '9') || a == '.')
                        d.push_back(a);
                });
    return boost::lexical_cast<double>(d);
}

inline void Parse::eraseBlank(string& str)
{
    int count = 0;
    for (auto c: str)
        if (c != '\t' && c != '\n' && c != '\r' && c != ' ')
            str[count++] = c;
    str[count] = 0;
}
