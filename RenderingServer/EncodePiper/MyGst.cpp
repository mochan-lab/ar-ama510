#include "MyGst.h"

GstBus* gstbus;
GstMessage* gstmsg;
void GstInit()
{
	gst_init(NULL, NULL);
}

void GstFinish()
{
    gst_message_unref(gstmsg);
    gst_object_unref(gstbus);
}

std::vector<std::uint8_t>* MyGst::m_ImageData = nullptr;

MyGst::MyGst(int ImageWidth, int ImageHeight, int OutPort) : m_ImageWidth(ImageWidth), m_ImageHeight(ImageHeight), m_OutPort(OutPort)
{
    printf("Gst start\n");
    m_Pipeline =
        gst_parse_launch
        //("gst-launch-1.0 --gst-debug-level=3 videotestsrc ! capsfilter name=filter ! autovideosink",
        //("gst-launch-1.0 --gst-debug-level=3 appsrc name=appsrc is-live=true stream-type=0 min-latency=0 do-timestamp=true duration=GST_CLOCK_TIME_NONE format=GST_FORMAT_TIME ! capsfilter name=filter ! videoconvert ! autovideosink",
        //("gst-launch-1.0 --gst-debug-level=3 appsrc name=appsrc is-live=true stream-type=0 min-latency=0 do-timestamp=true duration=GST_CLOCK_TIME_NONE format=GST_FORMAT_TIME ! capsfilter name=filter ! videoconvert ! nvh264enc gop-size=0 preset=hp rc-mode=vbr bitrate=500 ! h264parse config-interval=-1 ! rtph264pay ! rtph264depay ! h264parse ! openh264dec ! videoconvert ! autovideosink",
        ("gst-launch-1.0 --gst-debug-level=3 appsrc name=appsrc is-live=true stream-type=0 min-latency=0 do-timestamp=true duration=GST_CLOCK_TIME_NONE format=GST_FORMAT_TIME ! capsfilter name=filter ! videoconvert ! nvh264enc gop-size=0 preset=hp rc-mode=vbr ! h264parse config-interval=-1 ! rtph264pay ! udpsink name=sink",
            NULL);
    
    //set filter
    GstElement* filter;
    gchar* filtercapsstr;
    GstCaps* filtercaps;
    filter = gst_bin_get_by_name(GST_BIN(m_Pipeline), "filter");
    if (filter == NULL)
        g_print("capsfilter is NULL\n");
    filtercapsstr = g_strdup_printf("video/x-raw,width=%d,height=%d", m_ImageWidth, m_ImageHeight);//need videoformatter
    filtercaps = gst_caps_from_string(filtercapsstr);
    g_free(filtercapsstr);
    g_object_set(filter, "caps", filtercaps, NULL);
    gst_caps_unref(filtercaps);
    gst_object_unref(filter);

    /*set udp sink*/
    GstElement* sink;
    gchar* sinkhoststr;
    sink = gst_bin_get_by_name(GST_BIN(m_Pipeline), "sink");
    if (sink == NULL) {
        g_print("udpsink is NULL\n");
    }
    sinkhoststr = g_strdup_printf("127.0.0.1");
    g_object_set(sink, "host", sinkhoststr, NULL);
    //g_object_set(sink, "port", 50002, NULL);
    g_object_set(sink, "port", m_OutPort, NULL);
    g_free(sinkhoststr);
    gst_object_unref(sink);
    
    //set appsink
    GstElement* appsrc;
    gchar* appsrccapsstr;
    GstCaps* appsrccaps;
    appsrc = gst_bin_get_by_name(GST_BIN(m_Pipeline), "appsrc");
    if (appsrc == NULL)
        g_print("appsink is NULL\n");
    appsrccapsstr = g_strdup_printf("video/x-raw,format=RGB,width=%d,height=%d", m_ImageWidth, m_ImageHeight);
    appsrccaps = gst_caps_from_string(appsrccapsstr);
    g_object_set(appsrc, "caps", appsrccaps, NULL);
    g_signal_connect(appsrc, "need-data", G_CALLBACK (appsrcGetFromBytes), NULL);
    g_free(appsrccapsstr);
    gst_caps_unref(appsrccaps);
    gst_object_unref(appsrc);
}

MyGst::~MyGst()
{
    gst_element_set_state(m_Pipeline, GST_STATE_NULL);
    gst_object_unref(m_Pipeline);
}

void MyGst::SetImageData(std::vector<std::uint8_t>& ImageData)
{
    m_ImageData = &ImageData;
}

void MyGst::StartPlay()
{
    gst_element_set_state(m_Pipeline, GST_STATE_PLAYING);
    printf("Play Gst\n");
}

void MyGst::appsrcGetFromBytes (GstElement* appsrc, guint unused_size, gpointer user_data) {
    static int timestamp = 0;
    GstBuffer* buffer;
    guint size;
    GstFlowReturn ret;

    //std::printf("%i im put data......", timestamp);
    //g_print("im put data......");
    size = m_ImageData->size();

    buffer = gst_buffer_new_allocate(NULL, size, NULL);
    gst_buffer_fill(buffer, 0, m_ImageData->data(), size);

    //GST_BUFFER_PTS(buffer) = timestamp;
    //GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, 2);
    //GST_BUFFER_DURATION(buffer) = GST_CLOCK_TIME_NONE;

    timestamp += 1;

    g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);

    gst_buffer_unref(buffer);

    //g_print("finish put\n");

    if (ret != GST_FLOW_OK) {
        g_print("cannot push");
    }
}