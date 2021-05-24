#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{
    memset(&ctx_, 0, sizeof(ctx_));
    createPipeline();
}

Player::~Player()
{
    destroyPipeline();
}

void Player::addVideo(const QString &uri)
{

    int id = getUnusedSourceId();
    ctx_.source_enable[id] = true;
    int idx = getSourceIndex(id);

    playlist_.insert(idx, uri);
    emit playlistChanged();

    gchar *name = g_strdup_printf("videosrc_%u", id);
    qDebug() << "adding source: " << name;
    ctx_.source[id] = gst_element_factory_make("uridecodebin", name);
    g_object_set(G_OBJECT(ctx_.source[id]), "uri", uri.toStdString().c_str(), NULL);
    g_signal_connect(G_OBJECT(ctx_.source[id]), "pad-added", G_CALLBACK(cb_newpad), &ctx_);

    gst_bin_add(GST_BIN(ctx_.pipeline), ctx_.source[id]);
    gst_element_set_state(ctx_.pipeline, GST_STATE_PLAYING);
}

void Player::removeVideo(int index)
{
    playlist_.removeAt(index);
    emit playlistChanged();
    int id = getSourceId(index);
    ctx_.source_enable[id] = false;
    qDebug() << "remove source: " << gst_element_get_name(ctx_.source[id]);
    gst_element_set_state(ctx_.source[id], GST_STATE_NULL);
    gst_bin_remove(GST_BIN(ctx_.pipeline), ctx_.source[id]);
    ctx_.source[id] = nullptr;
    gst_element_set_state(ctx_.pipeline, GST_STATE_PLAYING);

}

void Player::setOutput(QVariant output)
{
    QQuickItem *item = qvariant_cast<QQuickItem *>(output);
    g_object_set(ctx_.sink, "widget", item, NULL);

}

void Player::cb_newpad(GstElement *decodebin, GstPad *pad, gpointer data)
{
    GstCaps *caps = gst_pad_query_caps(pad, NULL);
    const GstStructure *str = gst_caps_get_structure(caps, 0);
    const gchar *name = gst_structure_get_name(str);

    AppCtx *ctx = (AppCtx *)data;

    if(!strncmp(name, "video", 5)) {
        gchar *elem_name = gst_element_get_name(decodebin);
        gchar *padname = g_strconcat("sink_", g_strsplit(elem_name, "_", 0)[1], NULL);
        qDebug() << "adding pad name: " << padname;
        GstPad *sinkpad = gst_element_get_static_pad(ctx->streammux, padname);
        if(!sinkpad)
            sinkpad = gst_element_get_request_pad(ctx->streammux, padname);

        if(gst_pad_is_linked(pad)) {
            qDebug() << "Already linked decodebin to pipeline, skip";
        } else {
            if(gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
                qDebug() << "Failed to link decodebin to pipeline";
            }
        }
        gst_object_unref(sinkpad);
    }
}

void Player::createPipeline()
{
    ctx_.pipeline = gst_pipeline_new("pipeline");
    // ctx_.source = gst_element_factory_make("uridecodebin", "videosrc");
    ctx_.streammux = gst_element_factory_make("nvstreammux", "streammux");
    ctx_.pgie = gst_element_factory_make ("nvinfer", "primary-nvinference-engine");
    ctx_.nvvidconv = gst_element_factory_make("nvvideoconvert", "nvvidconv");
    ctx_.osd = gst_element_factory_make("nvdsosd", "osd");
    ctx_.nvtiler = gst_element_factory_make("nvmultistreamtiler", "nvtiler");
    ctx_.nvglconv = gst_element_factory_make("nvvideoconvert", "nvglconv");
    ctx_.glupload = gst_element_factory_make("glupload", "glupload");
    ctx_.glcolorconv = gst_element_factory_make("glcolorconvert", "glcolorconv");
    ctx_.sink = gst_element_factory_make("qmlglsink", "sink");

    g_object_set(ctx_.streammux, "batch-size", 1, NULL);
    g_object_set(ctx_.streammux, "width", 1280, NULL);
    g_object_set(ctx_.streammux, "height", 720, NULL);

    g_object_set (G_OBJECT(ctx_.pgie), "config-file-path", "../configures/config_infer_yolov5.txt", NULL);
    g_object_set(ctx_.osd, "process-mode", 1, NULL);


    gst_bin_add_many(GST_BIN(ctx_.pipeline), /*ctx_.source,*/ ctx_.streammux, ctx_.pgie, ctx_.nvvidconv, ctx_.osd, ctx_.nvtiler,
                     ctx_.nvglconv, ctx_.glupload, ctx_.glcolorconv, ctx_.sink, NULL);

    gst_element_link_many(ctx_.streammux, ctx_.pgie, ctx_.nvvidconv, ctx_.osd, ctx_.nvtiler, ctx_.nvglconv, ctx_.glupload, 
                          ctx_.glcolorconv, ctx_.sink, NULL);

}

void Player::destroyPipeline()
{
    gst_element_set_state(ctx_.pipeline, GST_STATE_NULL);
    gst_object_unref(ctx_.pipeline);
}

int Player::getUnusedSourceId()
{
    for(int i = 0; i < MAX_NUM_SOURCES; i++) {
        if(!ctx_.source_enable[i])
            return i;
    }

    return -1;
}

int Player::getSourceId(int index)
{

    int id = 0;
    for(int i = 0; i < MAX_NUM_SOURCES; i++) {
        if(!ctx_.source_enable[i])
            continue;
        if(id == index)
            return i;
        id++;
    }

    return -1;
}

int Player::getSourceIndex(int id)
{
    int idx = -1;
    for(int i = 0; i <= id; i++) {
        if(ctx_.source_enable[i])
            idx++;
    }
    return idx;
}