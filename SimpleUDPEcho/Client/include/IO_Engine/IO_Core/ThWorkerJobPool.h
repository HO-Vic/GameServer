#include "Utility/Pool/ObjectPool.h"
#include "Utility/SingletonBase/Singleton.h"
#include "Utility/Thread/ThWorkerJob.h"
#include "../IO_Metric/IO_Metric.h"

namespace sh::IO_Engine {
class ThWorkerJobPool
    : public Utility::SingletonBase<ThWorkerJobPool>,
      public Utility::RawObjectPool<sh::Utility::ThWorkerJob> {
 public:
  void Init(const uint32_t initSize, const bool useMetric = false) {
    InitSize(initSize, useMetric);
    m_useMetric = useMetric;
  }

  void RecordMetric(IO_MetricSlot& metricSlot) {
    if (!m_useMetric) {
      return;
    }
    metricSlot.thWorkerJobTotal.store(GetTotalCnt());
    metricSlot.thWorkerJobUsing.store(GetUsingCnt());
    metricSlot.thWorkerJobAdd.store(GetAddedCnt());
  }

 private:
  bool m_useMetric = false;
};
}  // namespace sh::IO_Engine
