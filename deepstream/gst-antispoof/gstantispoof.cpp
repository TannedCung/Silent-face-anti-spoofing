/* GStreamer
 * Copyright (C) 2022 FIXME <fixme@example.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstantispoof
 *
 * The self element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! self ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include "gstantispoof.h"

GST_DEBUG_CATEGORY_STATIC (gst_antispoof_debug_category);
#define GST_CAT_DEFAULT gst_antispoof_debug_category

/* prototypes */


static void gst_antispoof_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_antispoof_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_antispoof_dispose (GObject * object);
static void gst_antispoof_finalize (GObject * object);

// static GstCaps *gst_antispoof_transform_caps (GstBaseTransform * trans,
//     GstPadDirection direction, GstCaps * caps, GstCaps * filter);
// static GstCaps *gst_antispoof_fixate_caps (GstBaseTransform * trans,
//     GstPadDirection direction, GstCaps * caps, GstCaps * othercaps);
// static gboolean gst_antispoof_accept_caps (GstBaseTransform * trans,
//     GstPadDirection direction, GstCaps * caps);
// static gboolean gst_antispoof_set_caps (GstBaseTransform * trans,
//     GstCaps * incaps, GstCaps * outcaps);
// static gboolean gst_antispoof_query (GstBaseTransform * trans,
//     GstPadDirection direction, GstQuery * query);
// static gboolean gst_antispoof_decide_allocation (GstBaseTransform * trans,
//     GstQuery * query);
// static gboolean gst_antispoof_filter_meta (GstBaseTransform * trans,
//     GstQuery * query, GType api, const GstStructure * params);
// static gboolean gst_antispoof_propose_allocation (GstBaseTransform * trans,
//     GstQuery * decide_query, GstQuery * query);
// static gboolean gst_antispoof_transform_size (GstBaseTransform * trans,
//     GstPadDirection direction, GstCaps * caps, gsize size, GstCaps * othercaps,
//     gsize * othersize);
// static gboolean gst_antispoof_get_unit_size (GstBaseTransform * trans,
//     GstCaps * caps, gsize * size);
static gboolean gst_antispoof_start (GstBaseTransform * trans);
static gboolean gst_antispoof_stop (GstBaseTransform * trans);
// static gboolean gst_antispoof_sink_event (GstBaseTransform * trans,
//     GstEvent * event);
// static gboolean gst_antispoof_src_event (GstBaseTransform * trans,
//     GstEvent * event);
// static GstFlowReturn gst_antispoof_prepare_output_buffer (GstBaseTransform *
//     trans, GstBuffer * input, GstBuffer ** outbuf);
// static gboolean gst_antispoof_copy_metadata (GstBaseTransform * trans,
//     GstBuffer * input, GstBuffer * outbuf);
// static gboolean gst_antispoof_transform_meta (GstBaseTransform * trans,
//     GstBuffer * outbuf, GstMeta * meta, GstBuffer * inbuf);
// static void gst_antispoof_before_transform (GstBaseTransform * trans,
//     GstBuffer * buffer);
// static GstFlowReturn gst_antispoof_transform (GstBaseTransform * trans,
//     GstBuffer * inbuf, GstBuffer * outbuf);
static GstFlowReturn gst_antispoof_transform_ip (GstBaseTransform * trans,
    GstBuffer * buf);

enum
{
  PROP_0,
  PROP_MUXER_OUPUT_WIDTH,
  PROP_MUXER_OUPUT_HEIGHT,
  PROP_SCALE,
  PROP_DO_CAL,
  PROP_NEXT_EMB
};

static GQuark dsMetaQuark;

/* pad templates */

static GstStaticPadTemplate gst_antispoof_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    // GST_STATIC_CAPS ("application/unknown")
    GST_STATIC_CAPS_ANY
    );

static GstStaticPadTemplate gst_antispoof_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    // GST_STATIC_CAPS ("application/unknown")
    GST_STATIC_CAPS_ANY
    );


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstAntispoof, gst_antispoof, GST_TYPE_BASE_TRANSFORM,
    GST_DEBUG_CATEGORY_INIT (gst_antispoof_debug_category, "self", 0,
        "debug category for self element"));

static void
gst_antispoof_class_init (GstAntispoofClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *base_transform_class =
      GST_BASE_TRANSFORM_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass),
      &gst_antispoof_src_template);
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass),
      &gst_antispoof_sink_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
      "TannedCung", "antispoofing parser", "Parser for anti spoofing",
      "<tan.dx@cxview.ai>");

  gobject_class->set_property = gst_antispoof_set_property;
  gobject_class->get_property = gst_antispoof_get_property;
  gobject_class->dispose = gst_antispoof_dispose;
  gobject_class->finalize = gst_antispoof_finalize;
  // base_transform_class->transform_caps =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_transform_caps);
  // base_transform_class->fixate_caps =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_fixate_caps);
  // base_transform_class->accept_caps =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_accept_caps);
  // base_transform_class->set_caps = GST_DEBUG_FUNCPTR (gst_antispoof_set_caps);
  // base_transform_class->query = GST_DEBUG_FUNCPTR (gst_antispoof_query);
  // base_transform_class->decide_allocation =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_decide_allocation);
  // base_transform_class->filter_meta =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_filter_meta);
  // base_transform_class->propose_allocation =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_propose_allocation);
  // base_transform_class->transform_size =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_transform_size);
  // base_transform_class->get_unit_size =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_get_unit_size);
  base_transform_class->start = GST_DEBUG_FUNCPTR (gst_antispoof_start);
  base_transform_class->stop = GST_DEBUG_FUNCPTR (gst_antispoof_stop);
  // base_transform_class->sink_event =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_sink_event);
  // base_transform_class->src_event = GST_DEBUG_FUNCPTR (gst_antispoof_src_event);
  // base_transform_class->prepare_output_buffer =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_prepare_output_buffer);
  // base_transform_class->copy_metadata =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_copy_metadata);
  // base_transform_class->transform_meta =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_transform_meta);
  // base_transform_class->before_transform =
  //     GST_DEBUG_FUNCPTR (gst_antispoof_before_transform);
  // base_transform_class->transform = GST_DEBUG_FUNCPTR (gst_antispoof_transform);
  base_transform_class->transform_ip =
      GST_DEBUG_FUNCPTR (gst_antispoof_transform_ip);
  g_object_class_install_property (gobject_class, PROP_MUXER_OUPUT_WIDTH,
    g_param_spec_uint ("muxerOutputWidth", "muxerOutputWidth",
    "Width of the image to perform on, default = 1920",
    0, G_MAXUINT, 1920,
    (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));
  g_object_class_install_property (gobject_class, PROP_MUXER_OUPUT_HEIGHT,
    g_param_spec_uint ("muxerOutputHeight", "muxerOutputHeight",
    "Height of the image to perform on, default = 1080",
    0, G_MAXUINT, 1080,
    (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));
  g_object_class_install_property (gobject_class, PROP_SCALE,
    g_param_spec_float ("scale", "scale",
    "scale for next nvinfer, default = 1",
    0, G_MAXFLOAT, 1.0,
    (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)));
  g_object_class_install_property (gobject_class, PROP_DO_CAL,
    g_param_spec_boolean ("doSpoofCalculation", "doSpoofCalculation",
    "do spoof calculation at this plugin, default = false",
    FALSE,
    (GParamFlags) (G_PARAM_READWRITE | G_PARAM_CONSTRUCT)));
  g_object_class_install_property (gobject_class, PROP_DO_CAL,
    g_param_spec_boolean ("nextEmb", "nextEmb",
    "swap box for embedding, default = false",
    FALSE,
    (GParamFlags) (G_PARAM_READWRITE | G_PARAM_CONSTRUCT)));
  dsMetaQuark = g_quark_from_static_string (NVDS_META_STRING);
}

static void
gst_antispoof_init (GstAntispoof * self)
{
  self->muxer_width = DEFAULT_MUXER_WIDTH;
  self->muxer_height = DEFAULT_MUXER_HEIGHT;
  self->scale = DEFAULT_SCALE;
  self->do_spoof_calculation = DEFAULT_DO_CAL;
  self->is_emb_next = DEFAULT_NEXT_EMB;
  gst_base_transform_set_passthrough (GST_BASE_TRANSFORM (self), TRUE);
}

void
gst_antispoof_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstAntispoof *self = GST_ANTISPOOF (object);

  GST_DEBUG_OBJECT (self, "set_property");

  switch (property_id) {
    case PROP_MUXER_OUPUT_WIDTH:
      self->muxer_width = g_value_get_uint (value);
      GST_INFO_OBJECT(self, "muxer_width: %d", self->muxer_width);
      break;
    case PROP_MUXER_OUPUT_HEIGHT:
      self->muxer_height = g_value_get_uint (value);
      GST_INFO_OBJECT(self, "muxer_height: %d", self->muxer_height);
      break;
    case PROP_SCALE:
      self->scale = g_value_get_float(value);
      GST_INFO_OBJECT(self, "scale: %f", self->scale);
      break;
    case PROP_DO_CAL:
      self->do_spoof_calculation = g_value_get_boolean(value);
      GST_INFO_OBJECT(self, "do_spoof_calculation: %d", self->do_spoof_calculation);
      break;
    case PROP_NEXT_EMB:
      self->is_emb_next = g_value_get_boolean(value);
      GST_INFO_OBJECT(self, "is_emb_next: %d", self->is_emb_next);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_antispoof_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstAntispoof *self = GST_ANTISPOOF (object);

  GST_DEBUG_OBJECT (self, "get_property");

  switch (property_id) {
    case PROP_MUXER_OUPUT_WIDTH:
      g_value_set_uint (value, self->muxer_width);
      break;
    case PROP_MUXER_OUPUT_HEIGHT:
      g_value_set_uint (value, self->muxer_height);
      break;
    case PROP_SCALE:
      g_value_set_float (value, self->scale);
      break;
    case PROP_DO_CAL:
      g_value_set_boolean (value, self->do_spoof_calculation);
      break;
    case PROP_NEXT_EMB:
      g_value_set_boolean (value, self->is_emb_next);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_antispoof_dispose (GObject * object)
{
  GstAntispoof *self = GST_ANTISPOOF (object);

  GST_DEBUG_OBJECT (self, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_antispoof_parent_class)->dispose (object);
}

void
gst_antispoof_finalize (GObject * object)
{
  GstAntispoof *self = GST_ANTISPOOF (object);

  GST_DEBUG_OBJECT (self, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_antispoof_parent_class)->finalize (object);
}

/* states */
static gboolean
gst_antispoof_start (GstBaseTransform * trans)
{
  GstAntispoof *self = GST_ANTISPOOF (trans);

  GST_DEBUG_OBJECT (self, "start");

  return TRUE;
}

static gboolean
gst_antispoof_stop (GstBaseTransform * trans)
{
  GstAntispoof *self = GST_ANTISPOOF (trans);

  GST_DEBUG_OBJECT (self, "stop");

  return TRUE;
}

static GstFlowReturn
gst_antispoof_transform_ip (GstBaseTransform * trans, GstBuffer * buf)
{
  GstAntispoof *self = GST_ANTISPOOF (trans);

  GST_DEBUG_OBJECT (self, "transform_ip");
  GstMeta *gstMeta = NULL;
  gpointer state = NULL;
  NvDsMeta *meta = NULL;
  NvDsBatchMeta *batch_meta = NULL;
  GstMapInfo in_map_info;
  
  while ((gstMeta = gst_buffer_iterate_meta (buf, &state))) {
    if (gst_meta_api_type_has_tag (gstMeta->info->api, dsMetaQuark)) {
      // g_print("%d \n", gstMeta->info->api);
      meta = (NvDsMeta *) gstMeta;
      // g_print("%d  - %d \n", meta->meta_type, NVDS_BATCH_GST_META);
      if (meta->meta_type == NVDS_BATCH_GST_META) {
        batch_meta = (NvDsBatchMeta *) meta->meta_data;
        break;
      }
    }
  }
  if (batch_meta) {
    if (!gst_buffer_map (buf, &in_map_info, GST_MAP_READ)) {
      g_error ("Error: Failed to map gst buffer\n");
    }
    for (NvDsMetaList *l_frame = batch_meta->frame_meta_list; l_frame != NULL; l_frame=l_frame->next) {
      NvDsFrameMeta *frame_meta = (NvDsFrameMeta *) l_frame->data;
      for (NvDsMetaList * l_obj = frame_meta->obj_meta_list; l_obj != NULL; l_obj=l_obj->next) {
        NvDsObjectMeta *obj_meta = (NvDsObjectMeta *) l_obj->data;
        NvOSD_RectParams & sgie_rect_params = obj_meta->sgie_rect_params; // for next sgie infer
        NvOSD_RectParams & rect_params = obj_meta->rect_params; // for tracking

        NvOSD_TextParams & text_params = obj_meta->text_params;

        if (sgie_rect_params.left <= 0 || sgie_rect_params.height <= 0 ) continue; // a trigger set earlier that this box should be skipped
        NvDsInferObjectDetectionInfo to_scale;
        NvDsInferObjectDetectionInfo scaled;
        to_scale.top =  rect_params.top;
        to_scale.left =  rect_params.left;
        to_scale.width =  rect_params.width;
        to_scale.height =  rect_params.height;

        scaled = crop_box_by_scale(self->muxer_width, self->muxer_height, self->scale, to_scale);

        sgie_rect_params.top = scaled.top;
        sgie_rect_params.left = scaled.left;
        sgie_rect_params.width = scaled.width;
        sgie_rect_params.height = scaled.height;

        std::vector<float> spoof_res;
        for (NvDsMetaList * l_user = obj_meta->obj_user_meta_list; l_user != NULL; l_user = l_user->next) {
          NvDsUserMeta *user_meta = (NvDsUserMeta *) l_user->data;
          if (user_meta->base_meta.meta_type == NVDS_GST_CUSTOM_META) {
            // do box scaling
            if (self->is_emb_next){
              // swap box from detect meta for embedding
              DetectMeta *detect_meta = (DetectMeta *) user_meta->user_meta_data;
              sgie_rect_params.left = detect_meta->aligned_face.left;
              sgie_rect_params.top = detect_meta->aligned_face.top;
              sgie_rect_params.width = detect_meta->aligned_face.width;
              sgie_rect_params.height = detect_meta->aligned_face.height;
            }
          }
          else if (user_meta->base_meta.meta_type == NVDSINFER_TENSOR_OUTPUT_META) {
            NvDsInferTensorMeta *meta = (NvDsInferTensorMeta *) user_meta->user_meta_data;
            std::cout << "[DEBUG]: meta->unique_id: " << meta->unique_id << std::endl;
            if (meta->unique_id != 2 && meta->unique_id != 3) continue;
            for (unsigned int i = 0; i < meta->num_output_layers; i++) {
              NvDsInferLayerInfo *info = &meta->output_layers_info[i];
              info->buffer = meta->out_buf_ptrs_host[i];
            }
            NvDsInferDimsCHW dims;
            getDimsCHWFromDims (dims, meta->output_layers_info[0].inferDims);
            unsigned int output_dim = dims.c;
            float *outputCoverageBuffer = (float *) meta->output_layers_info[0].buffer;
            std::vector<float> res(outputCoverageBuffer, outputCoverageBuffer + output_dim);
            res.push_back(1.0);
            // aggregate spoof res from last infers
            if (spoof_res.size()<=0) spoof_res = res;
            else spoof_res = vector_add<float>(res, spoof_res);
          }
        }
        // do the spoof calculation
        if (spoof_res.size()<=0) {
          std::cout << "[WARM] spoof_res is empty, this is not normal \n";
          continue;
        }
        if (self->do_spoof_calculation){
          std::vector<float>::iterator max_value = std::max_element(spoof_res.begin(), spoof_res.end()-1);

          std::cout << "[INFO]: spoof_res: [";
          for (auto &i: spoof_res) std::cout << i << ", ";
        
          int class_max = std::distance(spoof_res.begin(), max_value);
          
          std::cout << "] - class: " << class_max << std::endl;
          std::string str = "Real face " +  std::to_string(*max_value);

          text_params.display_text = g_strdup(str.c_str());
          rect_params.border_color = (NvOSD_ColorParams) {0, 1, 0, 1};
          if (class_max!=1){
            std::cout << "[INFO]: Spoof face, score = " << *max_value << std::endl;
            // skip emb for spoof faces
            sgie_rect_params.width = 0;
            sgie_rect_params.height = 0;
            std::string str = "Spoof face " +  std::to_string(*max_value);
            text_params.display_text = g_strdup(str.c_str());
            rect_params.border_color = (NvOSD_ColorParams) {1, 0, 0, 1};
          }
          else std::cout << "[INFO]: Real face, score = " << *max_value << std::endl;
        }
      }
    }
  }
  return GST_FLOW_OK;
}

static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "antispoof", GST_RANK_NONE,
      GST_TYPE_ANTISPOOF);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.1"
#endif
#ifndef PACKAGE
#define PACKAGE "basetransform"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "anti spoof"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "not yet available"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    antispoof,
    "FIXME plugin description",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)
