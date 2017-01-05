#define LAMEVST_MAJOR_VERSION_INT 1
#define LAMEVST_MAJOR_VERSION_STR "1"

#define LAMEVST_MINOR_VERSION_STR "0"
#define LAMEVST_MINOR_VERSION_INT 0

#define LAMEVST_MAINT_VERSION_STR "3"
#define LAMEVST_MAINT_VERSION_INT 3

#define LAMEVST_FULL_VERSION_STR                                               \
  LAMEVST_MAJOR_VERSION_STR "." LAMEVST_MINOR_VERSION_STR                      \
                            "." LAMEVST_MAINT_VERSION_STR
#define LAMEVST_VERSION_INT                                                    \
  ((LAMEVST_MAJOR_VERSION_INT * 1000) + (LAMEVST_MINOR_VERSION_INT * 100) +    \
   LAMEVST_MAINT_VERSION_INT)
