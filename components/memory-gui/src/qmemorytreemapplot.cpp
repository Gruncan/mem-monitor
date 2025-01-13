
#include "qmemorytreemapplot.h"
#include "QTextStream"


QMemoryTreeMapPlot::QMemoryTreeMapPlot(QWidget* parent) : QCustomPlot(parent)  {
    this->xAxis->setVisible(false);
    this->yAxis->setVisible(false);
    this->xAxis->grid()->setVisible(false);
    this->yAxis->grid()->setVisible(false);

    this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    this->setMouseTracking(true);
    connect(this, &QCustomPlot::mouseMove, this, &QMemoryTreeMapPlot::showTooltip);
}

QMemoryTreeMapPlot::~QMemoryTreeMapPlot() {

}

void QMemoryTreeMapPlot::parseMapsLine(const QString& line) {
    QStringList parts = line.split(" ", Qt::SkipEmptyParts);
    if (parts.size() < 6) return;

    // Parse address range
    QStringList addrRange = parts[0].split("-");
    if (addrRange.size() != 2) return;

    bool ok1, ok2;
    quint64 startAddr = addrRange[0].toULongLong(&ok1, 16);
    quint64 endAddr = addrRange[1].toULongLong(&ok2, 16);
    if (!ok1 || !ok2) return;

    // Get other fields
    QString perms = parts[1];
    quint64 offset = parts[2].toULongLong(nullptr, 16);
    QString device = parts[3];
    QString inode = parts[4];
    QString pathname = parts.size() > 5 ? parts[5] : "";

    mappings.append(MemoryMapping(startAddr, endAddr, perms, offset,
                                device, inode, pathname));
}

void QMemoryTreeMapPlot::visualizeMappings() {
    clearPlottables();
    // removeItems();
    visualBlocks.clear();

    if (mappings.isEmpty()) return;

    quint64 minAddr = mappings[0].startAddr;
    quint64 maxAddr = mappings[0].endAddr;
    for (const auto& map : mappings) {
        minAddr = qMin(minAddr, map.startAddr);
        maxAddr = qMax(maxAddr, map.endAddr);
    }

    // Calculate grid layout
    int cols = 4;  // Number of columns in grid
    double totalSize = maxAddr - minAddr;
    double blockWidth = 100.0 / cols;
    double blockHeight = blockWidth / 2;

    // Place blocks in grid layout
    int col = 0;
    int row = 0;

    for (const auto& map : mappings) {
        double size = (map.endAddr - map.startAddr) / static_cast<double>(totalSize);

        // Create block
        QCPItemRect* rect = new QCPItemRect(this);
        double x1 = col * blockWidth;
        double y1 = row * blockHeight;

        // Size block based on mapping size (with minimum size)
        double width = qMax(blockWidth * size * cols, blockWidth * 0.5);

        rect->topLeft->setCoords(x1, y1);
        rect->bottomRight->setCoords(x1 + width, y1 + blockHeight);

        rect->setBrush(QBrush(map.color));
        rect->setPen(QPen(Qt::black));

        // Add basic label
        // QCPItemText* label = new QCPItemText(this);
        // label->position->setCoords(x1 + width/2, y1 + blockHeight/2);
        // QString shortName = QFileInfo(map.pathname).fileName();
        // if (shortName.isEmpty())
        //     shortName = QString("anon_%1").arg(map.permissions);
        // label->setText(shortName);
        // label->setPositionAlignment(Qt::AlignCenter);
        // label->setTextAlignment(Qt::AlignCenter);
        // label->setColor(Qt::black);

        visualBlocks.append(rect);

        // Move to next position
        col++;
        if (col >= cols) {
            col = 0;
            row++;
        }
    }

    // Set plot ranges
    this->xAxis->setRange(-5, 105);
    this->yAxis->setRange(-5, (row + 1) * blockHeight + 5);

    replot();
}

void QMemoryTreeMapPlot::loadFromMapsFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    mappings.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        parseMapsLine(line);
    }

    visualizeMappings();
}

void QMemoryTreeMapPlot::showTooltip(QMouseEvent* event) {
    double x = xAxis->pixelToCoord(event->pos().x());
    double y = yAxis->pixelToCoord(event->pos().y());
    for (QCPItemRect* block : visualBlocks) {

        double x1 = block->topLeft->coords().x();
        double y1 = block->topLeft->coords().y();
        double x2 = block->bottomRight->coords().x();
        double y2 = block->bottomRight->coords().y();
         if (x >= x1 && x <= x2 && y >= y1 && y <= y2) {

            int index = visualBlocks.indexOf(block);
            if (index >= 0 && index < mappings.size()) {
                const auto& map = mappings[index];
                QString tooltip = QString(
                    "Address: 0x%1 - 0x%2\n"
                    "Size: %3 KB\n"
                    "Permissions: %4\n"
                    "Path: %5")
                    .arg(map.startAddr, 16, 16, QChar('0'))
                    .arg(map.endAddr, 16, 16, QChar('0'))
                    .arg((map.endAddr - map.startAddr) / 1024)
                    .arg(map.permissions)
                    .arg(map.pathname.isEmpty() ? "Anonymous" : map.pathname);

                QToolTip::showText(event->globalPos(), tooltip);
                return;
            }
            }
    }
    QToolTip::hideText();
}
