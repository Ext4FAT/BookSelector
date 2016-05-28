#include <iostream>
#include <string>
#include <iomanip>

//Debugs
#define _Output_Space_(_LEVEL_) \
    for (int j = 0; j < _LEVEL_; j++) std::cout << "   "

//Typedef
#define _DomTree_ tree<htmlcxx::HTML::Node>
#define _DomTreeIter_ _DomTree_::iterator
#define _MSS_ std::map<std::string, std::string>

//Dir
#define _CACHE_ "../Cache/"
#define _PAGE_ "../Cache/Page/"
#define _PIC_ "../Cache/Pic/"
#define _ICON_ "../Configuration/ICON/idler.png"
#define _CSSSTYLE_ "../Configuration/CSS/github2.css"

//Convert
#define NAME2STR(_VAR_) (#_VAR_)

//COUT
#define _MYOUT_ std::cout
#define _SplitLine_ std::cout << "----------------------------------------------------------------------\n"
/********************Markdown**********************/
#define MD_HEAD	\
        _MYOUT_ << "|INDEX|COVER|BOOK|RANK|EVALUATION|AUTHOR|BRIEF|PUBLISH-COMPANY|PUBLISH-DATE|PRICE|ID|" << std::endl;	\
        _MYOUT_ << "|------|:------:|:------:|:------:|:------:|:------:|:------:|:------:|:------:|:------:|------:|------:|" << std::endl

#define MD_TABLE(_INDEX_, _BOOK_) \
        _MYOUT_ << "|" << _INDEX_ << \
        "|![pic](" << _BOOK_.pic_ << ")|[" << \
        _BOOK_.name_CN_ << "](" << _BOOK_.href_ << ")|" << \
        setiosflags(std::ios::fixed) << std::setprecision(1) << _BOOK_.rank_ << "|" << \
        _BOOK_.pl_ << " 人评价|" << \
        _BOOK_.author_ << "|" << \
        _BOOK_.brief_ << "|" << \
        _BOOK_.publish_company_ << "|" << \
        _BOOK_.publish_date_ << "|" << \
        _BOOK_.price_type_ << setiosflags(std::ios::fixed) << std::setprecision(2) << _BOOK_.price_ << "|" << \
        _BOOK_.id_ << "|" << \
        std::endl

#define MD_TABLE_PTR(_INDEX_, _BOOK_) \
        _MYOUT_ << "|" << _INDEX_ << \
        "|![pic](" << _BOOK_->pic_ << ")|[" << \
        _BOOK_->name_CN_ << "](" << _BOOK_->href_ << ")|" << \
        setiosflags(std::ios::fixed) << std::setprecision(1) << _BOOK_->rank_ << "|" << \
        _BOOK_->pl_ << " 人评价|" << \
        _BOOK_->author_ << "|" << \
        _BOOK_->brief_ << "|" << \
        _BOOK_->publish_company_ << "|" << \
        _BOOK_->publish_date_ << "|" << \
        _BOOK_->price_type_ << setiosflags(std::ios::fixed) << std::setprecision(2) << _BOOK_->price_ << "|" << \
        std::endl

#define MD_TITLE(_LABEL_) \
        _MYOUT_ << _LABEL_  << std::endl
/***************************************************/
