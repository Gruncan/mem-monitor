


#include "qtreememorywidgetitem.h"


QTreeMemoryWidgetItem::QTreeMemoryWidgetItem(const mk_size_t key) : QTreeWidgetItem(), _key(key) {
}

mk_size_t QTreeMemoryWidgetItem::get_key() const {
    return _key;
}