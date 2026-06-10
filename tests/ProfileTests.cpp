#include <array>
#include <cassert>
#include <string_view>

class IReadinessRule {
 public:
  virtual ~IReadinessRule() = default;
  virtual bool passes(std::string_view evidenceTarget) const = 0;
};

class RequiredEvidenceRule final : public IReadinessRule {
 public:
  bool passes(std::string_view evidenceTarget) const override {
    return !evidenceTarget.empty();
  }
};

struct ProjectProfile {
  std::string_view title;
  std::string_view summary;
  std::string_view evidenceTarget;
  std::array<std::string_view, 8> tags;
};

constexpr ProjectProfile profile{
  "BEMS Edge AI Gateway",
  "C++ edge runtime coordinating BACnet polling, local safety rules, RabbitMQ command transport, and cloud-ready telemetry.",
  "Resilient edge control with simulator-safe fallbacks and observable health checks.",
  {
    "C++17",
    "C++ Design Patterns",
    "SOLID",
    "C++",
    "BACnet/IP",
    "RabbitMQ",
    "Docker",
    "i.MX93"
  }
};

int main() {
  const RequiredEvidenceRule rule;
  assert(!profile.title.empty());
  assert(!profile.summary.empty());
  assert(rule.passes(profile.evidenceTarget));
  assert(profile.tags[0] == "C++17");
  return 0;
}
