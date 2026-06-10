#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace bems_gateway {

struct ZoneSample {
  int bacnetInstance;
  std::string zone;
  double temperatureC;
  double humidityPct;
  bool occupied;
  bool equipmentAvailable;
};

struct AiRecommendation {
  std::string zone;
  double requestedSetpointC;
  double confidence;
  std::string reason;
};

struct GatewayCommand {
  std::string zone;
  double setpointC;
  std::string routeKey;
};

struct SafetyDecision {
  bool accepted;
  double boundedSetpointC;
  std::string reason;
};

struct TelemetryRecord {
  std::string zone;
  double temperatureC;
  double humidityPct;
  double requestedSetpointC;
  bool commandPublished;
  std::string status;
};

struct CycleReport {
  std::vector<GatewayCommand> commands;
  std::vector<TelemetryRecord> telemetry;
  bool fallbackUsed;
};

class IBacnetClient {
 public:
  virtual ~IBacnetClient() = default;
  virtual std::vector<ZoneSample> readPresentValues() = 0;
};

class IAiAdvisor {
 public:
  virtual ~IAiAdvisor() = default;
  virtual AiRecommendation recommend(const ZoneSample& sample) const = 0;
};

class ICommandPublisher {
 public:
  virtual ~ICommandPublisher() = default;
  virtual void publish(const GatewayCommand& command) = 0;
};

class ITelemetrySink {
 public:
  virtual ~ITelemetrySink() = default;
  virtual void record(const TelemetryRecord& telemetry) = 0;
};

class SimulatedBacnetClient final : public IBacnetClient {
 public:
  explicit SimulatedBacnetClient(std::vector<ZoneSample> samples);

  std::vector<ZoneSample> readPresentValues() override;

 private:
  std::vector<ZoneSample> samples_;
};

class RuleBasedAiAdvisor final : public IAiAdvisor {
 public:
  AiRecommendation recommend(const ZoneSample& sample) const override;
};

class SafetyPolicy {
 public:
  SafetyPolicy(double minSetpointC, double maxSetpointC, double minConfidence);

  SafetyDecision evaluate(const ZoneSample& sample, const AiRecommendation& recommendation) const;

 private:
  double minSetpointC_;
  double maxSetpointC_;
  double minConfidence_;
};

class InMemoryCommandPublisher final : public ICommandPublisher {
 public:
  void publish(const GatewayCommand& command) override;

  const std::vector<GatewayCommand>& published() const;

 private:
  std::vector<GatewayCommand> published_;
};

class InMemoryTelemetrySink final : public ITelemetrySink {
 public:
  void record(const TelemetryRecord& telemetry) override;

  const std::vector<TelemetryRecord>& records() const;

 private:
  std::vector<TelemetryRecord> records_;
};

class EdgeGateway {
 public:
  EdgeGateway(IBacnetClient& bacnet,
              const IAiAdvisor& advisor,
              const SafetyPolicy& safetyPolicy,
              ICommandPublisher& commandPublisher,
              ITelemetrySink& telemetrySink);

  CycleReport runOnce();

 private:
  IBacnetClient& bacnet_;
  const IAiAdvisor& advisor_;
  const SafetyPolicy& safetyPolicy_;
  ICommandPublisher& commandPublisher_;
  ITelemetrySink& telemetrySink_;
};

std::string toTelemetryPayload(const TelemetryRecord& telemetry);
std::string toCommandPayload(const GatewayCommand& command);
std::vector<ZoneSample> defaultCommissioningSamples();
std::vector<ZoneSample> fallbackSamples();

}  // namespace bems_gateway
