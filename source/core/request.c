#include <curl/curl.h>
#include <jansson.h>
#include <string.h>

#include "./store.c"

void request(const char *path, const char *method, const char *data) {
  char url[128], authorization[256];
  sprintf(url, "https://discord.com/api/v9/%s", path);
  sprintf(authorization, "Authorization: Bot %s", store.token);

  struct curl_slist *headers = curl_slist_append(NULL, "Content-Type: application/json");
  headers = curl_slist_append(headers, authorization);

  CURL *curl = curl_easy_init();
  if (strcmp(method, "POST") == 0) {
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(data));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
  } else if (strcmp(method, "GET") == 0) curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

  // TODO: block output the result, add CURLOPT_WRITEFUNCTION
  // curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
}
