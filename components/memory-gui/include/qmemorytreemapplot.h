

#ifndef QMEMORYTREEMAPPLOT_H
#define QMEMORYTREEMAPPLOT_H
#include <QtGui/QColor>
#include "qcustomplot.h"


struct MemoryMapping {
    quint64 startAddr;
    quint64 endAddr;
    QString permissions;
    quint64 offset;
    QString device;
    QString inode;
    QString pathname;
    QColor color;

    MemoryMapping(quint64 start, quint64 end, const QString& perms,
                 quint64 off, const QString& dev, const QString& i,
                 const QString& path)
        : startAddr(start), endAddr(end), permissions(perms),
          offset(off), device(dev), inode(i), pathname(path) {
        if (pathname.contains(".so"))
            color = QColor(100, 149, 237);
        else if (pathname.contains("[heap]"))
            color = QColor(50, 205, 50);
        else if (pathname.contains("[stack]"))
            color = QColor(255, 99, 71);
        else if (pathname.isEmpty())
            color = QColor(211, 211, 211);
        else if (permissions.contains("x"))
            color = QColor(147, 112, 219);
        else
            color = QColor(255, 215, 0);
    }
};

class QMemoryTreeMapPlot : public QCustomPlot {
    Q_OBJECT

public:
    explicit QMemoryTreeMapPlot(QWidget* parent = nullptr);
    ~QMemoryTreeMapPlot();

    void loadFromMapsFile(const QString& filename);

    void visualizeMappings();

    void parseMapsLine(const QString& line);

    void showTooltip(QMouseEvent* event);

private:
    QVector<MemoryMapping> mappings;
    QVector<QCPItemRect*> visualBlocks;
};


#endif //QMEMORYTREEMAPPLOT_H
