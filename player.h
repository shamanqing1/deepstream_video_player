#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QQuickItem>
#include <gst/gst.h>

struct AppCtx {
    GstElement *pipeline;
    GstElement *source;
    GstElement *nvvidconv;
    GstElement *glupload;
    GstElement *glcolorconv;
    GstElement *sink;
};

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uri READ uri WRITE setUri)
    Q_PROPERTY(QVariant output WRITE setOutput)

public:
    explicit Player(QObject *parent = nullptr);
    ~Player();

    const QString &uri() const;
    void setUri(const QString &uri);

    void setOutput(QVariant output);

signals:

private:
    void createPipeline();
    void destroyPipeline();

    QString uri_;
    AppCtx ctx_;
};

#endif // PLAYER_H
