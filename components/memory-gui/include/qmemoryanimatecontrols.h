

#ifndef QMEMORYANIMATECONTROLS_H
#define QMEMORYANIMATECONTROLS_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QSpinBox>

class QMemoryAnimateControls : public QWidget {
    Q_OBJECT

public:
    explicit QMemoryAnimateControls(QWidget* parent = nullptr);
    ~QMemoryAnimateControls();

    QPushButton* exportButton;


public Q_SLOTS:

    void onPlayPauseClicked();
    void onRewindClicked();
    void onForwardClicked();
    void onTimeSpacingUpdate();

Q_SIGNALS:
    void playClicked();
    void pauseClicked();
    void rewindClicked();
    void forwardClicked();
    void timeSpacingChange(int timeSpacing);


private:
    QPushButton* playPauseButton;
    QPushButton* rewindButton;
    QPushButton* forwardButton;

    QIcon playIcon;
    QIcon pauseIcon;
    QIcon rewindIcon;
    QIcon forwardIcon;

    bool isPlaying;

    QHBoxLayout* buttonLayout;

    QSpinBox* spinBox;



};



#endif //QMEMORYANIMATECONTROLS_H
