
#ifndef QTREEMEMORYWIDGETITEM_H
#define QTREEMEMORYWIDGETITEM_H
#include <qtreewidget.h>

class QTreeMemoryWidgetItem : public QTreeWidgetItem {
public:
    explicit QTreeMemoryWidgetItem(uint8_t key);

    uint8_t get_key() const;

private:
    uint8_t _key;



};


#endif //QTREEMEMORYWIDGETITEM_H
