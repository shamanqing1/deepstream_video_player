#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{
    createPipeline();
}

Player::~Player()
{
    destroyPipeline();
}

const QString &Player::uri() const
{
    return uri_;
}

void Player::setUri(const QString &uri)
{
    uri_ = uri;
    qDebug() << "open uri: " << uri;
    gst_element_set_state(ctx_.pipeline, GST_STATE_READY);
    g_object_set(G_OBJECT(ctx_.source), "uri", uri.toStdString().c_str(), NULL);
    gst_element_set_state(ctx_.pipeline, GST_STATE_PLAYING);
}

void Player::setOutput(QVariant output)
{
    QQuickItem *item = qvariant_cast<QQuickItem *>(output);
    g_object_set(ctx_.sink, "widget", item, NULL);

}

static void cb_newpad(GstElement *decodebin, GstPad *pad, gpointer data)
{
    GstCaps *caps = gst_pad_query_caps(pad, NULL);
    const GstStructure *str = gst_caps_get_structure(caps, 0);
    const gchar *name = gst_structure_get_name(str);

    AppCtx *ctx = (AppCtx *)data;

    if(!strncmp(name, "video", 5)) {
        GstPad *sinkpad = gst_element_get_static_pad(ctx->nvvidconv, "sink");

        if(gst_pad_is_linked(pad)) {
            qDebug() << "Already linked decodebin to pipeline, skip";
        } else {
            if(gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
                qDebug() << "Failed to link decodebin to pipeline";
            }
        }
    }
}

void Player::createPipeline()
{
    ctx_.pipeline = gst_pipeline_new("pipeline");
    ctx_.source = gst_element_factory_make("uridecodebin", "videosrc");
    ctx_.nvvidconv = gst_element_factory_make("nvvideoconvert", "nvvidconv");
    ctx_.glupload = gst_element_factory_make("glupload", "glupload");
    ctx_.glcolorconv = gst_element_factory_make("glcolorconvert", "glcolorconv");
    ctx_.sink = gst_element_factory_make("qmlglsink", "sink");


    g_signal_connect(G_OBJECT(ctx_.source), "pad-added", G_CALLBACK(cb_newpad), &ctx_);

    gst_bin_add_many(GST_BIN(ctx_.pipeline), ctx_.source, ctx_.nvvidconv,
                     ctx_.glupload, ctx_.glcolorconv, ctx_.sink, NULL);

    gst_element_link_many(ctx_.nvvidconv, ctx_.glupload, ctx_.glcolorconv, ctx_.sink, NULL);

}

void Player::destroyPipeline()
{
    gst_element_set_state(ctx_.pipeline, GST_STATE_NULL);
    gst_object_unref(ctx_.pipeline);
}