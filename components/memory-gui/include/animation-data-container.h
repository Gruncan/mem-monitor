
#ifndef ANIMATION_DATA_CONTAINER_H
#define ANIMATION_DATA_CONTAINER_H
#include <QVector>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mtccdecoder.h>
#include <vector>


template <typename T> class AnimationDataContainer {

public:

    explicit AnimationDataContainer(const uint32_t timeSpacing, const uint32_t stepSize)
      : timeSpacing(timeSpacing), head(timeSpacing), tail(0), stepSize(stepSize), data_all(nullptr), times_all(nullptr) {
        index = timeSpacing;
        data_slice = new QVector<double>();
        time_slice = new QVector<double>();
    }

    ~AnimationDataContainer() {
        delete data_slice;
        delete time_slice;
    }

    uint32_t getTimeSpacing() const {
        return this->timeSpacing;
    }

    void setTimes(QVector<double>* times) {
        this->times_all = times;
    }

    void setData(QVector<double>* data) {
        this->data_all = data;
    }


    std::pair<QVector<double>*, QVector<double>*> getWindow() {
        auto v1 = getTimeWindow();
        auto v2 = getDataWindow();

        tail += stepSize;

        return std::make_pair(v1, v2);
    }

    std::pair<double, double> getTimeRange() {
        return std::make_pair(times_all->at(tail), times_all->at(tail + timeSpacing));
    }

    void rewind() {
        tail -= (2 * stepSize);
    }

    void forward() {
        tail += stepSize;
    }

    void restart(int timeSpacing) {
        tail = 0;
        this->timeSpacing = timeSpacing;
        this->timeSpacing /= 10;
    }

private:

    QVector<double>* getTimeWindow() {
        auto vec = times_all->mid(0, timeSpacing);
        time_slice->swap(vec);
        return time_slice;
    }

    QVector<double>* getDataWindow() {
        auto vec = data_all->mid(tail, timeSpacing);
        data_slice->swap(vec);
        return data_slice;
    }

    uint32_t timeSpacing;
    uint32_t head;
    uint32_t tail;
    uint32_t stepSize;
    uint32_t index;

    QVector<double>* data_all;
    QVector<double>* times_all;
    QVector<double>* data_slice;
    QVector<double>* time_slice;

};


#endif //ANIMATION_DATA_CONTAINER_H
