#ifndef ALIFE_H
#define ALIFE_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

#include "NGLWindow.h"

class Alife : public QObject
{
    Q_OBJECT
public:
    explicit Alife(QObject *_parent = 0);
    ~Alife();

    void initialize();
    void go();
    
signals:
    
public slots:
    void update();
    void refresh();

private:
    Scene* m_scene;
    NGLWindow* m_window;

    QTimer m_updateTimer;
    QTimer m_refreshTimer;
    QElapsedTimer m_chronometer;
};

#endif // ALIFE_H
