
#ifndef QTREEMEMORYWIDGETITEM_H
#define QTREEMEMORYWIDGETITEM_H

#include "mtc-config.h"

#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QTreeView>


class QTreeMemoryWidgetItem : public QTreeWidgetItem {
public:
    explicit QTreeMemoryWidgetItem(mk_size_t key);

    mk_size_t get_key() const;

private:
    mk_size_t _key;



};


#endif //QTREEMEMORYWIDGETITEM_H
