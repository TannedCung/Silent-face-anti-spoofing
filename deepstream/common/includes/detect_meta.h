#ifndef _detect_meta_H_
#define _detect_meta_H_

#include "nvdsmeta.h"
#include "nvdsinfer.h"

typedef struct _DetectMeta{
  gboolean is_sideface;
  gboolean is_maskon;
  NvDsInferObjectDetectionInfo aligned_face;
} DetectMeta;

gpointer detect_meta_copy_func (gpointer data, gpointer user_data);
void detect_meta_free_func (gpointer data, gpointer user_data);

#endif