//
// Created by fhq on 2021/5/12.
//

#ifndef KALDI_GST_AUDIO_SOURCE_H
#define KALDI_GST_AUDIO_SOURCE_H

#include <matrix/kaldi-vector.h>
#include <gst/gst.h>

namespace kaldi {


// OnlineAudioSourceItf implementation using a queue of Gst Buffers
    class GstBufferSource {
    public:
        typedef int16 SampleType;  // hardcoded 16-bit audio

        GstBufferSource();

        // Implementation of the OnlineAudioSourceItf
        bool Read(Vector<BaseFloat> *data);

        void PushBuffer(GstBuffer *buf);

        void SetEnded(bool ended);

        ~GstBufferSource();

    private:

        GAsyncQueue* buf_queue_;
        gint pos_in_current_buf_;
        GstBuffer *current_buffer_;
        bool ended_;
        GMutex lock_;
        GCond data_cond_;
        KALDI_DISALLOW_COPY_AND_ASSIGN(GstBufferSource);
    };

}  // namespace kaldi



#endif //KALDI_GST_AUDIO_SOURCE_H
