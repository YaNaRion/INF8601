#include <stdio.h>

#include "filter.h"
#include "pipeline.h"
#include <unistd.h>

int getSysInfo() {
  long num_processors = sysconf(_SC_NPROCESSORS_ONLN);
  if (num_processors != -1) {
    printf("Number of available logical processors/threads: %ld\n",
           num_processors);
  } else {
    perror("sysconf");
  }
}

int thread(image_t *image1, image_dir_t *image_dir) {
  image_t *image2 = filter_scale_up(image1, 2);
  image_destroy(image1);
  if (image2 == NULL) {
    goto fail_exit;
  }

  image_t *image3 = filter_sharpen(image2);
  image_destroy(image2);
  if (image3 == NULL) {
    goto fail_exit;
  }

  image_t *image4 = filter_sobel(image3);
  image_destroy(image3);
  if (image4 == NULL) {
    goto fail_exit;
  }

  image_dir_save(image_dir, image4);
  printf(".");
  fflush(stdout);
  image_destroy(image4);

fail_exit: // retour neg diff
  return -1;
}

int pipeline_pthread(image_dir_t *image_dir) {
  // boucle sur toutes les images
  while (1) {
    image_t *image1 = image_dir_load_next(image_dir);
    if (image1 == NULL) {
      break;
    }
    thread(image1, image_dir);
  }
  printf("\n");
  return 0;
}
