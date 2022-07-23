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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_ANTISPOOF_H_
#define _GST_ANTISPOOF_H_

#include <gst/base/gstbasetransform.h>
#include "nvdsinfer_custom_impl.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "nvdsinfer_custom_impl.h"
#include "gstnvdsinfer.h"
#include "utils.h"
#include "detect_meta.h"
#include "nvbufsurface.h"
// #include "cuda_runtime_api.h"
#include "gstnvdsmeta.h"
#include "nvdsmeta_schema.h"
#include "nvds_version.h"

#define DEFAULT_MUXER_WIDTH 1920
#define DEFAULT_MUXER_HEIGHT 1080
#define DEFAULT_SCALE 1
#define DEFAULT_DO_CAL false
#define DEFAULT_NEXT_EMB false


G_BEGIN_DECLS

#define GST_TYPE_ANTISPOOF   (gst_antispoof_get_type())
#define GST_ANTISPOOF(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ANTISPOOF,GstAntispoof))
#define GST_ANTISPOOF_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_ANTISPOOF,GstAntispoofClass))
#define GST_IS_ANTISPOOF(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ANTISPOOF))
#define GST_IS_ANTISPOOF_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_ANTISPOOF))

typedef struct _GstAntispoof GstAntispoof;
typedef struct _GstAntispoofClass GstAntispoofClass;

struct _GstAntispoof
{
  GstBaseTransform base_antispoof;
  guint muxer_width;
  guint muxer_height;
  gfloat scale;
  gboolean do_spoof_calculation;
  gboolean is_emb_next;
};

struct _GstAntispoofClass
{
  GstBaseTransformClass base_antispoof_class;
};

GType gst_antispoof_get_type (void);

G_END_DECLS

#endif
