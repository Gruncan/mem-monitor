


#include "qtreememorywidgetitem.h"



QTreeMemoryWidgetItem::QTreeMemoryWidgetItem(const uint8_t key) : QTreeWidgetItem(), _key(key) {

}

uint8_t QTreeMemoryWidgetItem::get_key() const {
    return _key;
}