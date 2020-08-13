// Ref: https://gstreamer.freedesktop.org/documentation/tutorials/playback/playbin-usage.html?gi-language=c
#include <gst/gst.h>

#ifdef CYCLE_URLS

#define URI_COUNT_MAX   5
static char *urlList[URI_COUNT_MAX] = {
    "http://cdn.metrological.com/hls/hawaii720.m3u8",
    "http://cdn.metrological.com/hls/hongkong720.m3u8",
    "http://cdn.metrological.com/hls/dubai720.m3u8",
    "http://cdn.metrological.com/hls/liwa720.m3u8",
    "http://cdn.metrological.com/hls/london720.m3u8",
};

#endif /* CYCLE_URLS */

/* Structure to contain all our information, so we can pass it around */
typedef struct _CustomData {
    GstElement *playbin;  /* Our one and only element */
    gboolean playing;      /* Are we in the PLAYING state? */
    gboolean terminate;    /* Should we terminate execution? */
    gboolean seek_enabled; /* Is seeking enabled for this media? */
    gboolean seek_done;    /* Have we performed the seek already? */
    gint64 duration;       /* How long does this media last, in nanoseconds */
#ifdef CYCLE_URLS
    gint i;
#endif
} CustomData;

/* Forward definition of the message processing function */
static void handle_message (CustomData *data, GstMessage *msg);
int gst_sample_main(int argc, char *argv[]);
