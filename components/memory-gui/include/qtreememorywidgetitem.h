/***********************************
 *
 * @brief Tree Memory widget
 * @details A wrapping class from the QTreeWidgetItem used in sidebar control
 *
 * @file qtreememorywidgetitem.h
 *
************************************/
#ifndef QTREEMEMORYWIDGETITEM_H
#define QTREEMEMORYWIDGETITEM_H
#include <qtreewidget.h>

#include "mtc-config.h"

class QTreeMemoryWidgetItem : public QTreeWidgetItem {
  public:
    explicit QTreeMemoryWidgetItem(mk_size_t key);

    mk_size_t get_key() const;

  private:
    mk_size_t _key;
};


#endif // QTREEMEMORYWIDGETITEM_H
