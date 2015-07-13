#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/limits.h>

#include "http_server.h"

char *DOCROOT = "./public";

// @see http://www.ipa.go.jp/security/awareness/vendor/programmingv2/contents/c803.html
static FILE *content_open(char *pathname) {
  struct stat lstat_result, fstat_result;

  if (lstat(pathname, &lstat_result) != 0) {
    return NULL;
  }

  if (S_ISDIR(lstat_result.st_mode)) {
    return content_open(strcat(pathname, "/index.html"));
  } else if (!S_ISREG(lstat_result.st_mode)) {
    return NULL;
  }

  int fd = open(pathname, O_RDONLY, 0);
  if (fd < 0) {
    return NULL;
  }

  if (fstat(fd, &fstat_result) != 0) {
    close(fd);

    return NULL;
  }

  if (lstat_result.st_ino != fstat_result.st_ino ||
      lstat_result.st_dev != fstat_result.st_dev) {
    close(fd);

    return NULL;
  }

  return fdopen(fd, "r");
}

HttpResponse *handler(const HttpRequest *request) {
  char pathname[PATH_MAX];
  char *absolute_path = NULL;
  snprintf(pathname, sizeof(pathname), "%s/%s", DOCROOT, realpath(request->path, absolute_path));

  FILE *content_stream = content_open(pathname);
  if (content_stream == NULL) {
    return responder(&(HttpResponse) {
      .status = NOT_FOUND
    });
  }

  fseek(content_stream, 0, SEEK_END);
  int64_t size = ftell(content_stream);

  char *whole_contents = (char *)malloc(size);

  rewind(content_stream);
  int64_t length = fread(whole_contents, 1, size, content_stream);
  if (length != size) {
    return responder(&(HttpResponse) {
      .status = INTERNAL_SERVER_ERROR
    });
  }
  fclose(content_stream);

  return responder(&(HttpResponse) {
    .status = OK,
    .body = whole_contents
  });
}

int main() {
  http_server(&(HttpServer) {
    .tcp.port_no = 8000,
    .handle = handler
  });

  return 0;
}
