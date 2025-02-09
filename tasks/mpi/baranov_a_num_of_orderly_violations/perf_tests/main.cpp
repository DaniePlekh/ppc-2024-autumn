#include <gtest/gtest.h>

#include <boost/mpi/timer.hpp>
#include <random>

#include "core/perf/include/perf.hpp"
#include "mpi/baranov_a_num_of_orderly_violations/src/source.cpp"
TEST(mpi_baranov_a_num_of_orderly_violations_perf_test, test_pipeline_run) {
  const int count_size_vector = 10000000;
  boost::mpi::communicator world;
  std::vector<int> global_vec(count_size_vector);
  std::vector<int32_t> out(1, 0);
  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  if (world.rank() == 0) {
    std::random_device rd;
    std::default_random_engine reng(rd());
    std::uniform_int_distribution<int> dist(0, global_vec.size());
    std::generate(global_vec.begin(), global_vec.end(), [&dist, &reng] { return dist(reng); });
    taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    taskDataPar->inputs_count.emplace_back(global_vec.size());
    taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskDataPar->outputs_count.emplace_back(out.size());
  }
  auto testMpiTaskParallel =
      std::make_shared<baranov_a_num_of_orderly_violations_mpi::num_of_orderly_violations<int, int>>(taskDataPar);
  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const boost::mpi::timer current_timer;
  perfAttr->current_timer = [&] { return current_timer.elapsed(); };
  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();
  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testMpiTaskParallel);
  perfAnalyzer->pipeline_run(perfAttr, perfResults);
  if (world.rank() == 0) {
    ppc::core::Perf::print_perf_statistic(perfResults);
    auto temp = testMpiTaskParallel->seq_proc(global_vec);
    ASSERT_EQ(temp, out[0]);
  }
}
TEST(mpi_baranov_a_num_of_orderly_violations_perf_test, test_task_run) {
  const int count_size_vector = 10000000;
  boost::mpi::communicator world;
  std::vector<int> global_vec(count_size_vector);
  std::vector<int32_t> out(1, 0);
  // Create TaskData
  std::shared_ptr<ppc::core::TaskData> taskDataPar = std::make_shared<ppc::core::TaskData>();
  if (world.rank() == 0) {
    std::random_device rd;
    std::default_random_engine reng(rd());
    std::uniform_int_distribution<int> dist(0, global_vec.size());
    std::generate(global_vec.begin(), global_vec.end(), [&dist, &reng] { return dist(reng); });
    taskDataPar->inputs.emplace_back(reinterpret_cast<uint8_t*>(global_vec.data()));
    taskDataPar->inputs_count.emplace_back(global_vec.size());
    taskDataPar->outputs.emplace_back(reinterpret_cast<uint8_t*>(out.data()));
    taskDataPar->outputs_count.emplace_back(out.size());
  }
  auto testMpiTaskParallel =
      std::make_shared<baranov_a_num_of_orderly_violations_mpi::num_of_orderly_violations<int, int>>(taskDataPar);
  // Create Perf attributes
  auto perfAttr = std::make_shared<ppc::core::PerfAttr>();
  perfAttr->num_running = 10;
  const boost::mpi::timer current_timer;
  perfAttr->current_timer = [&] { return current_timer.elapsed(); };
  // Create and init perf results
  auto perfResults = std::make_shared<ppc::core::PerfResults>();
  // Create Perf analyzer
  auto perfAnalyzer = std::make_shared<ppc::core::Perf>(testMpiTaskParallel);
  perfAnalyzer->task_run(perfAttr, perfResults);
  if (world.rank() == 0) {
    ppc::core::Perf::print_perf_statistic(perfResults);
    auto temp = testMpiTaskParallel->seq_proc(global_vec);
    ASSERT_EQ(out[0], temp);
  }
}
