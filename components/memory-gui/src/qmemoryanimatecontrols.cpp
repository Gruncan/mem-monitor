
#include "qmemoryanimatecontrols.h"

#include <QLabel>
#include <qboxlayout.h>
#include <qspinbox.h>
#include <qstyle.h>


QMemoryAnimateControls::QMemoryAnimateControls(QWidget* parent) :
                    QWidget(parent),
                    isPlaying(true)
{
    playPauseButton = new QPushButton(this);
    rewindButton = new QPushButton(this);
    forwardButton = new QPushButton(this);

    playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
    pauseIcon = style()->standardIcon(QStyle::SP_MediaStop);
    rewindIcon = style()->standardIcon(QStyle::SP_MediaSeekBackward);
    forwardIcon = style()->standardIcon(QStyle::SP_MediaSeekForward);

    playPauseButton->setIcon(playIcon);
    rewindButton->setIcon(rewindIcon);
    forwardButton->setIcon(forwardIcon);

    playPauseButton->setFixedSize(40, 40);
    rewindButton->setFixedSize(40, 40);
    forwardButton->setFixedSize(40, 40);

    buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(rewindButton);
    buttonLayout->addWidget(playPauseButton);
    buttonLayout->addWidget(forwardButton);

    QVBoxLayout* layout = new QVBoxLayout(this);
    spinBox = new QSpinBox(this);
    spinBox->setRange(0, INT_MAX);
    spinBox->setValue(10000);
    spinBox->setSingleStep(10);
    QLabel* label = new QLabel("Time spacing:", this);
    layout->addLayout(buttonLayout);
    // layout->setSpacing(0);
    // layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    layout->addWidget(spinBox);

    connect(playPauseButton, &QPushButton::clicked, this, &QMemoryAnimateControls::onPlayPauseClicked);
    connect(rewindButton, &QPushButton::clicked, this, &QMemoryAnimateControls::onRewindClicked);
    connect(forwardButton, &QPushButton::clicked, this, &QMemoryAnimateControls::onForwardClicked);
    connect(spinBox, &QSpinBox::valueChanged, this, &QMemoryAnimateControls::onTimeSpacingUpdate);
}

QMemoryAnimateControls::~QMemoryAnimateControls() {
    delete playPauseButton;
    delete rewindButton;
    delete forwardButton;
    delete buttonLayout;
}


void QMemoryAnimateControls::onPlayPauseClicked(){
    isPlaying = !isPlaying;
    playPauseButton->setIcon(isPlaying ? playIcon : pauseIcon);
    if(isPlaying) {
        emit playClicked();
    }else {
        emit pauseClicked();
    }
}

void QMemoryAnimateControls::onRewindClicked(){
    emit rewindClicked();
}
void QMemoryAnimateControls::onForwardClicked(){
    emit forwardClicked();
}

void QMemoryAnimateControls::onTimeSpacingUpdate() {
    emit timeSpacingChange(spinBox->value());
}
