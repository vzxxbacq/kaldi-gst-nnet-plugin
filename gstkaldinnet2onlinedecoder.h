//
// Created by fhq on 2021/5/12.
//

#ifndef KALDI_GSTKALDINNET2ONLINEDECODER_H
#define KALDI_GSTKALDINNET2ONLINEDECODER_H


#include <gst/gst.h>

#include "simple_option.h"
#include "gst_audio_source.h"

#include "online2/online-nnet2-decoding-threaded.h"
#include "online2/online-nnet2-decoding.h"

// support for nnet3
#include "online2/online-nnet3-decoding.h"

#include "online2/onlinebin-util.h"
#include "online2/online-timing.h"
#include "online2/online-endpoint.h"
#include "fstext/fstext-lib.h"
#include "lat/lattice-functions.h"
#include "lm/const-arpa-lm.h"
#include "lat/word-align-lattice.h"
#include "lat/determinize-lattice-pruned.h"

namespace kaldi {

    G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_KALDINNET2ONLINEDECODER \
  (gst_kaldinnet2onlinedecoder_get_type())
#define GST_KALDINNET2ONLINEDECODER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_KALDINNET2ONLINEDECODER,Gstkaldinnet2onlinedecoder))
#define GST_KALDINNET2ONLINEDECODER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_KALDINNET2ONLINEDECODER,Gstkaldinnet2onlinedecoderClass))
#define GST_IS_KALDINNET2ONLINEDECODER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_KALDINNET2ONLINEDECODER))
#define GST_IS_KALDINNET2ONLINEDECODER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_KALDINNET2ONLINEDECODER))

    typedef struct _Gstkaldinnet2onlinedecoder Gstkaldinnet2onlinedecoder;
    typedef struct _Gstkaldinnet2onlinedecoderClass Gstkaldinnet2onlinedecoderClass;

#define NNET2  2
#define NNET3  3

    struct _Gstkaldinnet2onlinedecoder {
        GstElement element;

        GstPad *sinkpad, *srcpad;

        GstCaps *sink_caps;

        guint nnet_mode;
        gboolean silent;
        gboolean do_endpointing;
        gboolean inverse_scale;
        float lmwt_scale;
        GstBufferSource *audio_source;
        gboolean do_phone_alignment;

        gchar* model_rspecifier;
        gchar* fst_rspecifier;
        gchar* word_syms_filename;
        gchar* phone_syms_filename;
        gchar* word_boundary_info_filename;

        SimpleOptionsGst *simple_options;
        OnlineEndpointConfig *endpoint_config;
        OnlineNnet2FeaturePipelineConfig *feature_config;
        OnlineNnet2DecodingThreadedConfig *nnet2_decoding_threaded_config;
        OnlineNnet2DecodingConfig *nnet2_decoding_config;
        // support for nnet3
        nnet3::NnetSimpleLoopedComputationOptions *nnet3_decodable_opts;
        LatticeFasterDecoderConfig *decoder_opts;
        fst::DeterminizeLatticePrunedOptions *det_opts;

        OnlineSilenceWeightingConfig *silence_weighting_config;

        OnlineNnet2FeaturePipelineInfo *feature_info;
        TransitionModel *trans_model;
        nnet2::AmNnet *am_nnet2;
        nnet3::AmNnetSimple *am_nnet3;
        nnet3::DecodableNnetSimpleLoopedInfo *decodable_info_nnet3;
        fst::Fst<fst::StdArc> *decode_fst;
        fst::SymbolTable *word_syms;
        fst::SymbolTable *phone_syms;
        WordBoundaryInfo *word_boundary_info;
        int sample_rate;
        gboolean decoding;
        float chunk_length_in_secs;
        float traceback_period_in_secs;
        bool use_threaded_decoder;
        guint num_nbest;
        guint num_phone_alignment;
        guint min_words_for_ivector;
        OnlineIvectorExtractorAdaptationState *adaptation_state;
        OnlineCmvnState *cmvn_state;
        float segment_start_time;
        float total_time_decoded;

        // The following are needed for optional LM rescoring with a "big" LM
        gchar* lm_fst_name;
        gchar* big_lm_const_arpa_name;
        fst::MapFst<fst::StdArc, LatticeArc, fst::StdToLatticeMapper<BaseFloat> > *lm_fst;
        fst::TableComposeCache<fst::Fst<LatticeArc> > *lm_compose_cache;
        ConstArpaLm *big_lm_const_arpa;
    };

    struct _Gstkaldinnet2onlinedecoderClass {
        GstElementClass parent_class;
        void (*partial_result)(GstElement *element, const gchar *result_str);
        void (*final_result)(GstElement *element, const gchar *result_str);
        void (*full_final_result)(GstElement *element, const gchar *result_str);
    };

    GType gst_kaldinnet2onlinedecoder_get_type(void);

    G_END_DECLS
}


#endif //KALDI_GSTKALDINNET2ONLINEDECODER_H
