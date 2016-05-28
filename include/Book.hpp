#pragma once

#include "Common.hpp"
#include "Bucket.hpp"

typedef map<string, string> Item;   // eg. [href] = "http://item.xxxx"

class BookInfo {
private:
    static const string _DB_SEARCH_;
    static const unsigned _START_;
public:
    /** @brief default constructor */
    BookInfo();
    /** @brief BookInfo: extract information from Item */
    explicit BookInfo(Item& item);
    /** @brief ~BookInfo: deconstructor */
    ~BookInfo();
    /** @brief convert: extract Bookinfo from Item, swap PTR!!! , after this operator Item disabled */
    int convert(Item &item);
    /** @brief rank: return book's rank, because client cann't modify rank */
    double rank() const { return rank_; }
    /** @brief operator <<: output bookinfo*/
    friend ostream & operator <<(ostream &out, BookInfo &b);
    /** @brief operator: use to order books by url */
    bool operator >(const BookInfo &b) const { return pl_ < b.pl_ || (pl_ == b.pl_ && id_ > b.id_) ; }
    bool operator <(const BookInfo &b) const { return pl_ > b.pl_ || (pl_ == b.pl_ && id_ < b.id_) ; }
    bool operator >=(const BookInfo &b) const { return !(*this < b); }
    bool operator <=(const BookInfo &b) const { return !(*this > b); }

private:
    /** @brief Direct Read from Item */
    string name_CN_;    // Chinese book name
    string href_;       // url
    string pic_;        // cover pic url
    string brief_;      // brief introduction
    int pl_;            // evluation people number
    /** @brief Get by Parse String */
    long long id_;      // convert with url
    double rank_;       // score by people on the Internet
    double price_;
    string price_type_; // RMB, Dollar, Eur, Pound ...
    string publish_company_;
    string publish_date_;
    string author_;
};

//typedef shared_ptr<BookInfo> BookInfo_Ptr;
//typedef vector<BookInfo> BookList;
typedef Bucket<BookInfo> BookBucket;
typedef Buck<BookInfo> BookBuck;


//Include boundary
enum OUPUT_TPYE {
    OUTPUT_ALL,
    OUTPUT_UNDER,
    OUTPUT_OVER,
    OUTPUT_EQUAL,
    OUTPUT_BETWEEN,
    OUTPUT_TOPK
};

class BookSet {
public:
    /** @brief default constructor */
    BookSet();
    /** @brief assign the category */
    explicit BookSet(const string& keyword);
    /** @brief extract information from Item List */
    explicit BookSet(const string& keyword, list<Item>& lbi);
    /** @brief add items into booklist */
    void generate(list<Item>& lbi);
    /**
     * @brief addBook: add BookInfo to bookset
     * @param bi
     * @return  if repeated, return 0, otherwise return 1;
     */
    int addBook(BookInfo &bi);
    /**
     * @brief outputBook: output BookSet in certain rank
     * @param rank: if rank == -1, output all books
     */
    void outputBook(ostream &out, OUPUT_TPYE OT, double rank, double rank2 = -1);
    /**
     * @brief operator << : output all books in this bookset
     */
    friend ostream& operator <<(ostream& out, BookSet& bs);
    /**
     * @brief outputTableHead: output markdown table head
     */
    inline void outputTableHead(ostream &out);
    /*
    void test()
    {
       BookBucket::Iterator it = books_.begin();
       BookBucket::Iterator end = books_.end();
    }
    */



    /*
    BookSet operator &(BookSet& bs);
    BookSet operator |(BookSet& bs);
    BookSet operator ^(BookSet& bs);
    */

    /** @brief get member varible */
    BookBucket& bookBucket() {return books_;}
    string& category() {return category_;}
    size_t size() const {return count_;}

private:
    BookBucket books_;
    string category_;
    size_t count_;
    //map<long long, int> id_index_; //id-index map
};
