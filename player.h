#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QQuickItem>
#include <gst/gst.h>

constexpr int MAX_NUM_SOURCES = 128;

struct AppCtx {
    GstElement *pipeline;
    GstElement *source[MAX_NUM_SOURCES];
    GstElement *streammux;
    GstElement *pgie;
    GstElement *nvvidconv;
    GstElement *osd;
    GstElement *nvtiler;
    GstElement *nvglconv;
    GstElement *glupload;
    GstElement *glcolorconv;
    GstElement *sink;

    bool source_enable[MAX_NUM_SOURCES];
};

class Player : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QList<QString> playlist READ playlist NOTIFY playlistChanged)
    Q_PROPERTY(QVariant output WRITE setOutput)

public:
    explicit Player(QObject *parent = nullptr);
    ~Player();

    Q_INVOKABLE void addVideo(const QString &uri);
    Q_INVOKABLE void removeVideo(int index);

    QList<QString> playlist() const { return playlist_; }
    void setOutput(QVariant output);

signals:
    void playlistChanged();

private:
    static void cb_newpad(GstElement *decodebin, GstPad *pad, gpointer data);
    void createPipeline();
    void destroyPipeline();
    int getUnusedSourceId();
    int getSourceId(int index);
    int getSourceIndex(int id);


    QList<QString> playlist_;
    AppCtx ctx_;
};

#endif // PLAYER_H
