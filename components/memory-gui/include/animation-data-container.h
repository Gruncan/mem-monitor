
#ifndef ANIMATION_DATA_CONTAINER_H
#define ANIMATION_DATA_CONTAINER_H
#include <QVector>
#include <cstdint>
#include <cstdlib>
#include <mtccdecoder.h>
#include <vector>


template <typename T> class AnimationDataContainer {

public:

    explicit AnimationDataContainer(const uint32_t timeSpacing, const uint32_t stepSize)
      : timeSpacing(timeSpacing), head(stepSize), tail(0), stepSize(stepSize), data_all(nullptr), times_all(nullptr) {
        index = 1;
    }

    ~AnimationDataContainer() {

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

        head++;
        if (head >= timeSpacing) {
            tail++;
        }

        return std::make_pair(v1, v2);
    }



private:

    QVector<double>* getTimeWindow() {
        auto vec = times_all->mid(tail, head);
        return new QVector<double>(vec.begin(), vec.end());
    }

    QVector<double>* getDataWindow() {
        auto vec = data_all->mid(tail, head);
        return new QVector<double>(vec.begin(), vec.end());
    }

    uint32_t timeSpacing;
    uint32_t head;
    uint32_t tail;
    uint32_t stepSize;
    uint32_t index;

    QVector<double>* data_all;
    QVector<double>* times_all;

};


#endif //ANIMATION_DATA_CONTAINER_H
