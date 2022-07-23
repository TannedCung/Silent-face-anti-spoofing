#include "detect_meta.h"

gpointer detect_meta_copy_func (gpointer data, gpointer user_data) {
  NvDsUserMeta *user_meta = (NvDsUserMeta *) data;
  DetectMeta *srcMeta = (DetectMeta *) user_meta->user_meta_data;
  DetectMeta *dstMeta = NULL;

  dstMeta = (DetectMeta *)g_memdup (srcMeta, sizeof(DetectMeta));

  // dstMeta->is_sideface = srcMeta->is_sideface;

  return dstMeta;
}


void detect_meta_free_func (gpointer data, gpointer user_data) {
  NvDsUserMeta *user_meta = (NvDsUserMeta *) data;
  DetectMeta *srcMeta = (DetectMeta *) user_meta->user_meta_data;

  g_free (user_meta->user_meta_data);
  user_meta->user_meta_data = NULL;
}