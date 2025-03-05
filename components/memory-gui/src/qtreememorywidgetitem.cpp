


#include "qtreememorywidgetitem.h"


QTreeMemoryWidgetItem::QTreeMemoryWidgetItem(const mk_size_t key, const std::string& name) : QTreeWidgetItem(), _key(key),
            _name(name){
}

mk_size_t QTreeMemoryWidgetItem::get_key() const {
    return _key;
}

std::string QTreeMemoryWidgetItem::get_name() const {
    return _name;
}