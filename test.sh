#!/bin/bash
GST_PLUGIN_PATH=. gst-launch-1.0 --gst-debug="" -q filesrc location=./exp/demo_dr_strangelove.mp3 ! decodebin ! audioconvert ! audioresample ! \
  kaldinnet2onlinedecoder \
  use-threaded-decoder=true \
  model=exp/models/final.mdl \
  fst=exp/models/HCLG.fst \
  word-syms=exp/models/words.txt \
  phone-syms=exp/models/phones.txt \
  word-boundary-file=exp/models/word_boundary.int \
  num-nbest=3 \
  num-phone-alignment=3 \
  do-phone-alignment=true \
  feature-type=mfcc \
  mfcc-config=exp/conf/mfcc.conf \
  ivector-extraction-config=exp/conf/ivector_extractor.conf \
  max-active=7000 \
  beam=11.0 \
  lattice-beam=5.0 \
  do-endpointing=true \
  endpoint-silence-phones="1:2:3:4:5:6:7:8:9:10" \
  chunk-length-in-secs=0.2 \
  ! filesink location=/dev/stdout buffer-mode=2
