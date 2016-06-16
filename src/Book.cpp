
/*************************************************************************
    > File Name: Book.cpp
    > Author: zyy
    > Mail: zyy34472@gmail.com
    > Created Time: Fri 22 Jan 2016 09:29:56 AM CST
 ************************************************************************/

#include "Book.hpp"
#include "Parse.hpp"
#include <exception>
#include <boost/algorithm/string.hpp>
using namespace Parse;


/**
 * @brief BookInfo Impletement
 */

const string BookInfo::_DB_SEARCH_ = "https://book.douban.com/search/";
const unsigned BookInfo::_START_ = string("https://book.douban.com/subject/").size();

BookInfo::BookInfo()
{
    rank_ = 0;
}

BookInfo::BookInfo(Item &item)
{
    convert(item);
    /*
    int x = convert(item);
    if (0 > x) {
        cout << x << endl;
        cout << *this <<endl;
    }
    */
}

BookInfo::~BookInfo()
{

}

int BookInfo::convert(Item &item)
{
    //Assign bookinfo by swap
    rank_ = atof(item["rating_nums"].c_str());
    name_CN_.swap(item["title"]);
    href_.swap(item["href"]);
    pic_.swap(item["src"]);
    //extract url id
    id_ = extractID(href_, _START_);
    //extract pl num;
    pl_ = extractInteger(item["pl"]);
    //Trim all blank char
    brief_.swap(item["p"]);
    //if (!(brief_[0] & 0x80))
    eraseBlank(brief_);

    //Split string
    vector<string> desc;
    string& con = item["pub"];
    boost::trim(con);
    replaceBracket(con);
    boost::split(desc, con, boost::is_any_of("/"), boost::token_compress_on);
    //Parse pub_comp, pub_data, author, prcie
    int dsize = desc.size(), i;
    string& price = desc.back();
    if (price.find("-") != std::string::npos) {
        publish_date_ = price;
        publish_company_ = desc[dsize - 2];
        for (dsize -= 2, i = 0; i < dsize; i++)
            author_ += " [" + desc[i] + "](" + _DB_SEARCH_ + desc[i] +")";
        return -1;
    }

    if (dsize < 3)	return -2;

    try{
        //Transfer price to string
        price_type_ = "￥";
        if (price.find("USD")  != std::string::npos ||
            price.find("$") != std::string::npos)
            price_type_ = "$";
        else if (price.find("EUR")  != std::string::npos ||
            price.find("€") != std::string::npos)
            price_type_ = "€";
        price_ = extractDouble(price);
        //publication info
        if (desc[dsize - 2].find("$") == std::string::npos)
            publish_date_ = desc[dsize - 2];
        publish_company_= desc[dsize - 3];
    }
    catch(std::exception& e){
        cout << e.what() << endl;
        return -3;
    }
    for (dsize -= 3, i = 0; i < dsize; i++)
        author_ += " [" + desc[i] + "](" + _DB_SEARCH_ + desc[i] +")\t";
    return 0;
}

ostream& operator <<(ostream &out, BookInfo &_BOOK_)
{
        out <<
        "|![pic](" << _BOOK_.pic_ << ")|[" <<
        _BOOK_.name_CN_ << "](" << _BOOK_.href_ << ")|" <<
        setiosflags(std::ios::fixed) << std::setprecision(1) << _BOOK_.rank_ << "|" <<
        _BOOK_.pl_ << " 人评价|" <<
        _BOOK_.author_ << "|" <<
        _BOOK_.brief_ << "|" <<
        _BOOK_.publish_company_ << "|" <<
        _BOOK_.publish_date_ << "|" <<
        _BOOK_.price_type_ << setiosflags(std::ios::fixed) << std::setprecision(2) << _BOOK_.price_ << "|" <<
        _BOOK_.id_ << "|";
    return out;
}


/**
 * @brief BookSet Impletement
 */
BookSet::BookSet()
{
    count_ = 0;
}

BookSet::BookSet(const string& keyword)
{
    count_ = 0;
    category_= keyword;
}

BookSet::BookSet(const string& keyword, list<Item> &lbi)
{
    count_ = 0;
    category_ = keyword;
    generate(lbi);
}

void BookSet::generate(list<Item> &lbi)
{
    /**TODO
     * remove  rank 0 books
    */
    for (auto item: lbi) {
        int rank = 10 * atof(item["rating_nums"].c_str());
        if (!rank)
            continue;
        books_.addBucketItem(BookInfo(item), rank);
        //books_.getBucket(rank).insert(BookInfo(item));
        count_++;

    }
}

int BookSet::addBook(BookInfo &bi)
{
   // if (id_index_.find(bi.id_) != id_index_.end())
    books_.addBucketItem(bi, bi.rank());
    return 1;
    //books_.push_back(x);
}

void BookSet::outputTableHead(ostream &out)
{
    out << "|INDEX|COVER|BOOK|RANK|EVALUATION|AUTHOR|BRIEF|PUBLISH-COMPANY|PUBLISH-DATE|PRICE|ID|" << endl;
    out << "|------|:------:|:------:|:------:|:------:|:------:|:------:|:------:|:------:|:------:|------:|" << endl;
}

void BookSet::outputBook(ostream &out, OUPUT_TPYE OT, double rank, double rank2)
{
    /** TODO */
    switch (OT) {
        case OUTPUT_ALL:
            out << *this;
            break;
        case OUTPUT_OVER:
            out << "# BookList: [" << category_ << "]\tOver: " << rank << endl;
            outputTableHead(out);
            books_.outputOver(out, 10*rank);
            break;
        case OUTPUT_BETWEEN:
            out << "# BookList: [" << category_ << "]\tBetween: " << rank <<" and " << rank2 << endl;
            outputTableHead(out);
            break;
        case OUTPUT_EQUAL:
            //out << books_.getPosBuck(rank);
            break;
        case OUTPUT_TOPK:
            rank = !rank || count_ < rank? count_: rank;
            out << "# BookList: [" << category_ << "]\tTOP: " << (int)rank << endl;
            outputTableHead(out);
            books_.outputTOP(out, rank);
            break;
    };

}

ostream& operator <<(ostream &out, BookSet &bs)
{
    out << "# BookList: [" << bs.category() << "]" << endl;
    bs.outputTableHead(out);
    //BookBucket& theBucket = bs.bookBucket();
    //topk = !topk || bs.size() < topk? bs.size(): topk;

    //bs.bookBucket() << out << endl;
    out << bs.bookBucket() << endl;
    return out;
}


