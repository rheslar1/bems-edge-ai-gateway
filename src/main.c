#include <stdio.h>
#include <stddef.h>

typedef struct {
  const char *title;
  const char *summary;
  const char *evidence_target;
  const char *tags[8];
  size_t tag_count;
} project_profile_t;

static const project_profile_t profile = {
  "BEMS Edge AI Gateway",
  "C++ edge runtime coordinating BACnet polling, local safety rules, RabbitMQ command transport, and cloud-ready telemetry.",
  "Resilient edge control with simulator-safe fallbacks and observable health checks.",
  {
  "C++",
  "BACnet/IP",
  "RabbitMQ",
  "Docker",
  "i.MX93"
  },
  5u
};

int main(void) {
  printf("%s\n", profile.title);
  printf("Summary: %s\n", profile.summary);
  printf("Evidence target: %s\n", profile.evidence_target);
  printf("Stack:");

  for (size_t index = 0; index < profile.tag_count; ++index) {
    printf(" %s%s", profile.tags[index], index + 1u == profile.tag_count ? "" : ",");
  }

  printf("\n");
  return 0;
}
