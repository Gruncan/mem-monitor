
#ifndef QTREEMEMORYWIDGETITEM_H
#define QTREEMEMORYWIDGETITEM_H
#include <qtreewidget.h>

#include "mtc-config.h"

class QTreeMemoryWidgetItem : public QTreeWidgetItem {
  public:
    explicit QTreeMemoryWidgetItem(mk_size_t key, const std::string& name);

    mk_size_t get_key() const;
    std::string get_name() const;

  private:
    mk_size_t _key;
    std::string _name;
};


#endif // QTREEMEMORYWIDGETITEM_H
