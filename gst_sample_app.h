// Ref: https://gstreamer.freedesktop.org/documentation/tutorials/playback/playbin-usage.html?gi-language=c
#include <gst/gst.h>

#ifndef PLAY_DURATION_SEC
#define PLAY_DURATION_SEC 0 /* Play complete */
#endif

#ifdef CYCLE_URLS

static char *urlList[] = {
    "https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",
    "http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8",
    "http://184.72.239.149/vod/smil:BigBuckBunny.smil/playlist.m3u8",
    /* Make sure that this table is ALWAYS NULL terminated. */
    NULL,
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
