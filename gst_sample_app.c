#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "gst_sample_app.h"

static void handle_message (CustomData *data, GstMessage *msg)
{
    GError *err;
    gchar *debug_info;

    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error (msg, &err, &debug_info);
            g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
            g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error (&err);
            g_free (debug_info);
            data->terminate = TRUE;
            break;
        case GST_MESSAGE_EOS:
            g_print ("End-Of-Stream reached.\n");
            data->terminate = TRUE;
            break;
        case GST_MESSAGE_DURATION:
            /* The duration has changed, mark the current one as invalid */
            data->duration = GST_CLOCK_TIME_NONE;
            break;
        case GST_MESSAGE_STATE_CHANGED:
            {
                GstState old_state, new_state, pending_state;
                gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
                if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data->playbin)) {
                    g_print ("Pipeline state changed: '%s' -> '%s':\n",
                            gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));

                    /* Remember whether we are in the PLAYING state or not */
                    data->playing = (new_state == GST_STATE_PLAYING);

                    if (data->playing) {
                        /* We just moved to PLAYING. Check if seeking is possible */
                        GstQuery *query;
                        gint64 start, end;
                        query = gst_query_new_seeking (GST_FORMAT_TIME);
                        if (gst_element_query (data->playbin, query)) {
                            gst_query_parse_seeking (query, NULL, &data->seek_enabled, &start, &end);
                            if (data->seek_enabled) {
                                g_print ("Seeking is ENABLED from %" GST_TIME_FORMAT " to %" GST_TIME_FORMAT "\n",
                                        GST_TIME_ARGS (start), GST_TIME_ARGS (end));
                            } else {
                                g_print ("Seeking is DISABLED for this stream.\n");
                            }
                        }
                        else {
                            g_printerr ("Seeking query failed.");
                        }
                        gst_query_unref (query);
                    }
                }
            } break;
        default:
            /* We should not reach here */
            g_printerr ("Unexpected message received.\n");
            break;
    }
    gst_message_unref (msg);
}

int main(int argc, char *argv[])
{
    CustomData data;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    data.playing = FALSE;
    data.terminate = FALSE;
    data.seek_enabled = FALSE;
    data.seek_done = FALSE;
    data.duration = GST_CLOCK_TIME_NONE;

    /* Initialize GStreamer */
    gst_init (&argc, &argv);

    /* Create the elements */
    data.playbin = gst_element_factory_make ("playbin", "playbin");

    if (!data.playbin) {
        g_printerr ("Not all elements could be created.\n");
        return -1;
    }

#ifdef CYCLE_URLS
    data.i = 0;
    g_print("\nLoading initial uri: '%s'...\n", urlList[data.i]);
    /* Set the URI to play next index and increment the index to next stream url. */
    g_object_set (data.playbin, "uri", urlList[data.i++], NULL);
#else /* !CYCLE_URLS */
    /* Set the URI to play */
    g_object_set (data.playbin, "uri", url, NULL);
#endif /* !CYCLE_URLS */

    /* Start playing */
    ret = gst_element_set_state (data.playbin, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("Unable to set the pipeline to the playing state.\n");
        gst_object_unref (data.playbin);
        return -1;
    }

    /* Listen to the bus */
    bus = gst_element_get_bus (data.playbin);

    do {
        msg = gst_bus_timed_pop_filtered (bus, 100 * GST_MSECOND,
                GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_DURATION);

        /* Parse message */
        if (msg != NULL) {
            handle_message (&data, msg);
        } else {
            /* We got no message, this means the timeout expired */
            if (data.playing) {
                gint64 current = -1;

                /* Query the current position of the stream */
                if (!gst_element_query_position (data.playbin, GST_FORMAT_TIME, &current)) {
                    //g_printerr("Could not query current position.\n");
                }

                /* If we didn't know it yet, query the stream duration */
                if (!GST_CLOCK_TIME_IS_VALID (data.duration)) {
                    if (!gst_element_query_duration (data.playbin, GST_FORMAT_TIME, &data.duration)) {
                        //g_printerr("Could not query current duration.\n");
                    }
                }

                /* Print current position and total duration */
                /* g_print ("Position %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",
                        GST_TIME_ARGS (current), GST_TIME_ARGS (data.duration));
                 */
                /* If seeking is enabled, we have not done it yet, and the time is right, seek */
                if (data.seek_enabled && !data.seek_done && current > (PLAY_DURATION_SEC * GST_SECOND)) {
#ifdef CYCLE_URLS
                    /* Pause pipeline... */
                    ret = gst_element_set_state (data.playbin, GST_STATE_NULL);
                    if (ret == GST_STATE_CHANGE_FAILURE) {
                        g_printerr ("Unable to set the pipeline to NULL state.\n");
                        data.terminate = TRUE;
                    } else {
                        if (!urlList[data.i]) {
                            /* Array is NULL terminated; reset index. */
                            data.i = 0;
                        }
                        g_print ("\nReached %ds, loading next uri: '%s'...\n", PLAY_DURATION_SEC, urlList[data.i]);
                        /* Set the URI to play next index and increment the index to next stream url. */
                        g_object_set (data.playbin, "uri", urlList[data.i++], NULL);
                        ret = gst_element_set_state (data.playbin, GST_STATE_PLAYING);
                        if (ret == GST_STATE_CHANGE_FAILURE) {
                            g_printerr ("Unable to set the pipeline to the playing state.\n");
                            data.terminate = TRUE;
                        } else {
                            data.playing = FALSE;
                            data.terminate = FALSE;
                            data.seek_enabled = FALSE;
                            data.seek_done = FALSE;
                            data.duration = GST_CLOCK_TIME_NONE;
                        }
                    }
#else /* !CYCLE_URLS */
                    ret = gst_element_set_state (data.playbin, GST_STATE_NULL);
                    if (ret == GST_STATE_CHANGE_FAILURE) {
                        g_printerr ("Unable to set the pipeline to NULL state.\n");
                    }
                    data.terminate = TRUE;
#endif /* !CYCLE_URLS */
                }
            }
        }
    } while (!data.terminate);

    /* Free resources */
    gst_object_unref (bus);
    gst_element_set_state (data.playbin, GST_STATE_NULL);
    gst_object_unref (data.playbin);
    return 0;
}
