#include <gst/gst.h>
#include <glib.h>
#include <iostream>
#include "../kaldimarshal.h"

void asr_set(GstElement* asr,
             char* fst,
             char* model,
             char* word_syms,
             char* iv_conf,
             char* mfcc_conf){
    g_object_set(G_OBJECT(asr), "fst", fst, NULL);
    g_object_set(G_OBJECT(asr), "model", model, NULL);
    g_object_set(G_OBJECT(asr), "word-syms", word_syms, NULL);
    g_object_set(G_OBJECT(asr), "feature-type", "mfcc", NULL);
    g_object_set(G_OBJECT(asr), "mfcc-config", mfcc_conf, NULL);
    g_object_set(G_OBJECT(asr), "ivector-extraction-config", iv_conf, NULL);
    g_object_set(G_OBJECT(asr), "max-active", 7000, NULL);
    g_object_set(G_OBJECT(asr), "beam", 10.0, NULL);
    g_object_set(G_OBJECT(asr), "lattice-beam", 6.0, NULL);
    g_object_set(G_OBJECT(asr), "do-endpointing", TRUE, NULL);
    g_object_set(G_OBJECT(asr), "endpoint-silence-phones", "1:2:3:4:5:6:7:8:9:10", NULL);
    g_object_set(G_OBJECT(asr), "use-threaded-decoder", FALSE, NULL);
    g_object_set(G_OBJECT(asr), "chunk-length-in-secs", 0.2, NULL);
}

static gboolean bus_call(GstBus *bus,
                         GstMessage *msg,
                         gpointer data) {
    GMainLoop *loop = (GMainLoop *) data;
    switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            g_main_loop_quit(loop);
            break;

        case GST_MESSAGE_ERROR: {
            gchar *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);

            g_printerr("Error: %s\n", error->message);
            g_error_free(error);

            g_main_loop_quit(loop);
            break;
        }
        default:
            break;
    }
    return TRUE;
}

static void cb_new_pad (GstElement *element, GstPad *pad, gpointer data)
{
    gchar *name;
    auto *other = (GstElement*) data;

    name = gst_pad_get_name (pad);
    g_print ("A new pad %s was created for %s\n", name, gst_element_get_name(element));
    g_free (name);

    g_print ("element %s will be linked to %s\n",
             gst_element_get_name(element),
             gst_element_get_name(other));
    gst_element_link(element, other);
}

static void on_partial_result(GstElement *element, gchar* res, gpointer data ){
    // std::cout << "partial " << res << std::endl;
}

static void on_final_result(GstElement *element, gchar* res, gpointer data ){
    // std::cout << "Final " << res << std::endl;
}

int main(int argc, char* argv[]){
    GMainLoop *loop;
    GstElement *pipeline, *filesrc , *audioconvert , *audioresample, *asr, *fakesink, *decodebin;
    gboolean link_ok = FALSE;
    gst_init(&argc, &argv);
    loop = g_main_loop_new(NULL, FALSE);

    if(argc != 7){
        g_printerr("Usage: %s <audiofile> <fst> <model> <wordsyms>\n"
                   "<iv_conf> <mfcc_conf>", argv[0]);
        return -1;
    }

    /* initialize pipeline and elements */
    pipeline = gst_pipeline_new("asr-pipeline");
    filesrc = gst_element_factory_make("filesrc", "filesrc");
    decodebin = gst_element_factory_make("decodebin", "dec");
    audioconvert = gst_element_factory_make("audioconvert", "converter");
    audioresample = gst_element_factory_make("audioresample", "resample");
    asr = gst_element_factory_make("kaldinnet2onlinedecoder", "asr");
    fakesink = gst_element_factory_make("fakesink", "fakesink");

    if (!pipeline || !filesrc || !decodebin || !audioconvert || !audioresample || !asr || !fakesink) {
        g_printerr ("One element could not be created. Exiting.\n");
        return -1;
    }
    asr_set(asr, argv[2], argv[3], argv[4], argv[5], argv[6]);
    g_object_set(G_OBJECT(filesrc), "location", argv[1]);
    gpointer data;
    g_signal_connect(G_OBJECT(asr), (gchar*)"partial-result",
                     G_CALLBACK(on_partial_result), data);
    g_signal_connect(G_OBJECT(asr), (gchar*)"final-result",
                     G_CALLBACK(on_final_result), data);

    GstBus *bus;
    guint bus_watch_id;
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    // dump asr output
    g_object_set(G_OBJECT(fakesink), "dump", TRUE, NULL);

    GstElement *filesink, *wavenc;
    filesink = gst_element_factory_make("filesink", "fs");
    wavenc = gst_element_factory_make("wavenc", "we");

    //g_object_set(G_OBJECT(audioresample), "rate", (guint64)16000, NULL);
    g_object_set(G_OBJECT(filesink), "location", "test.wav", NULL);

    gst_bin_add_many (GST_BIN (pipeline),
                      filesrc,
                      decodebin,
                      audioconvert,
                      audioresample,
                      asr,
                      fakesink,
                      NULL);

    g_signal_connect(decodebin, (gchar*)"pad-added", G_CALLBACK (cb_new_pad), audioconvert);

    gst_element_link_many (filesrc,
                           decodebin,
                           NULL);

    link_ok = gst_element_link(audioconvert, audioresample);
    if(!link_ok){
        g_print("link error.");
    }

    if(!link_ok){
        g_print("link error.");
    }
    link_ok = gst_element_link_many(audioresample,
                                    asr,
                                    fakesink,
                                    NULL);
    if(!link_ok){
        g_print("link error.");
    }

    g_print ("Now decoding: %s\n", argv[1]);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    /* Iterate */
    g_print ("Running...\n");
    g_main_loop_run (loop);


    /* Out of the main loop, clean up nicely */
    g_print ("Returned, stopping playback\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);

    g_print ("Deleting pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));
    g_source_remove (bus_watch_id);
    g_main_loop_unref (loop);
    return 0;
}