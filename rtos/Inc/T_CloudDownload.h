// wifi_task.h
#ifndef T_CloudDownload_H
#define T_CloudDownload_H

void F_CloudDownload(void *argument);
void Cb_WifiDown(void *argument);

#define CLOUD_DOWNLOAD_EVENT      (1U << 2)

#endif
