#ifndef PIFO_PIPELINE_STAGE_H_
#define PIFO_PIPELINE_STAGE_H_

#include <cstdint>

enum class QueueType {
  PRIORITY_QUEUE,
  CALENDAR_QUEUE
};

/// PIFOPipelineStage models a stage of PIFOs
/// ---each of which can be a priority queue or a
/// calendar queue. The constraint is that on any
/// tick you can do at most one enqueue or one dequeue
/// to the PIFOPipelineStage using the enq and deq methods.
/// These enq and deq methods can be external or from
/// previous or succeeding PIFOPipelineStages.
/// This mechanism of muxing the various sources into a port for each PIFOPipelineStage is the interconnect.
/// The configuration of these muxes is handled by the compiler based on
/// the graphviz dot file input describing the scheduling hierarchy.
template <typename ElementType, typename PriorityType>
class PIFOPipelineStage {
 public:
  /// Constructor for PIFOPipelineStage
  PIFOPipelineStage(const uint32_t & num_prio_queues, const uint32_t & num_cal_queues) : priority_queue_bank_(num_prio_queues), calendar_queue_bank_(num_cal_queues) {}

  /// Enqueue
  /// These happen externally from the ingress pipeline
  /// or from a push from a calendar queue/
  void enq(const QueueType & q_type, const uint32_t & queue_id, const ElementType & element, const PriorityType & prio, const uint32_t & tick)
  { if (q_type == QueueType::PRIORITY_QUEUE) priority_queue_bank_.at(queue_id).enq(element, prio, tick);
    else                                     calendar_queue_bank_.at(queue_id).enq(element, prio, tick); }

  /// Dequeues
  /// Happen implicitly starting from the root PIFO
  Optional<ElementType> deq(const QueueType & q_type, const uint32_t & queue_id, const uint32_t & tick)
  { if (q_type == QueueType::PRIORITY_QUEUE) return priority_queue_bank_.at(queue_id).deq(tick);
    else                                     return calendar_queue_bank_.at(queue_id).deq(tick); }

  /// Overload stream insertion operator
  friend std::ostream & operator<<(std::ostream & out, const PIFOPipelineStage & pipe_stage) {
    out << "Priority Queues: " << std::endl;
    for (uint32_t i = 0; i < pipe_stage.priority_queue_bank_.size(); i++) {
      out << "Index " << i << pipe_stage.priority_queue_bank_.at(i) << std::endl;
    }

    out << "Calendar Queues: " << std::endl;
    for (uint32_t i = 0; i < pipe_stage.calendar_queue_bank_.size(); i++) {
      out << "Index " << i << pipe_stage.calendar_queue_bank_.at(i) << std::endl;
    }

    return out;
  }

 private:
  /// Bank of priority queues
  std::vector<PriorityQueue<ElementType, PriorityType>> priority_queue_bank_ = {};

  /// Bank of calendar queues
  std::vector<CalendarQueue<ElementType, PriorityType>> calendar_queue_bank_ = {};
};

#endif  // PIFO_PIPELINE_STAGE_H_
