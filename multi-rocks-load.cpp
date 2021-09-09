//
// Created by maratworker on 09.09.2021.
//
#include <cstdio>
#include <string>
#include <iostream>
#include <mutex>
#include <future>

#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"

#include "utils.h"

using namespace ROCKSDB_NAMESPACE;

#if defined(OS_WIN)
std::string kDBPath = "C:\\Windows\\TEMP\\rocksdb_simple_example";
#else
std::string kDBPath = "./rocksdb_simple_example";
#endif

std::mutex mutex_gener_add_block;

int main() {
    DB* db;
    Options options;
    // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
    options.IncreaseParallelism(8);
    options.OptimizeLevelStyleCompaction();
    // create the DB if it's not already present
    options.create_if_missing = true;

    // open DB
    Status s = DB::Open(options, kDBPath, &db);
    assert(s.ok());

    auto count = 100'000'000;
    std::unordered_map<std::string, std::string> v_r_str(count);
    std::cout << "Prepare data " << std::endl;

    auto l_prepare_data = [&]() {
       auto block_count = count;
    std::vector<std::future<void>> v_futures;
    /// create several transactions
    auto conc_threads = std::thread::hardware_concurrency();
    std::cout << "Real thread count: " << conc_threads << std::endl;
    auto interval = block_count / conc_threads;
    for (size_t ith = 0; ith < conc_threads; ++ith)
    {
        v_futures.emplace_back(std::move(std::async(std::launch::async, [=, &v_r_str]()
        {
            size_t i = ith * interval;
            auto max_count = i + interval;
            std::cout << "Iter threads: " << ith << " Interval start: " << i << " Interval end: " << max_count << std::endl;
            for (; i < max_count; i++)
            {
                std::lock_guard<std::mutex> lock(mutex_gener_add_block);
                v_r_str.insert({std::to_string(i),gen_random_str(20)});
            }
        })));
    }
    std::cout << "Wait futures" << std::endl;
    for (auto &it_future : v_futures)
    {
        it_future.get();
    }

    };

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < count; ++i) {
        v_r_str.insert({std::to_string(i),gen_random_str(20)});
    }
//    l_prepare_data();
    std::cout << "prepared " << v_r_str.size() << " count" << std::endl;
    auto end = std::chrono::steady_clock::now();
    std::cout << "Write Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;

//
//    std::cout << "Write to db: " << count << " values." << std::endl;
////    auto hash_val = std::hash<std::string>{}(random_value);
//    start = std::chrono::steady_clock::now();
//    for (auto& it:v_r_str) {
//        // Put key-value
//        s = db->Put(WriteOptions(), it.first, it.second);
//        assert(s.ok());
//    }
//    end = std::chrono::steady_clock::now();
//    std::cout << "Write Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;
//
//
//    std::string value;
//    start = std::chrono::steady_clock::now();
//    for (auto& it:v_r_str) {
//    //     get value
//        s = db->Get(ReadOptions(), it.first, &value);
//        assert(s.ok());
//        assert(value == it.second);
//    }
//    end = std::chrono::steady_clock::now();
//    std::cout << "Read Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;
//
//    start = std::chrono::steady_clock::now();
//    {
//        for (auto& it:v_r_str) {
//            //     get value
//            s = db->Delete(WriteOptions(), it.first);
//            assert(s.ok());
//        }
//    }
//    end = std::chrono::steady_clock::now();
//    std::cout << "Delete Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;
//
//    std::cout << "Write to db over batch: " << count << " values." << std::endl;
//    start = std::chrono::steady_clock::now();
//    WriteBatch batch;
//    auto it_count = 0;
//    auto batch_size = 10000;
//    auto commont_count = 0;
//    for (auto& it:v_r_str) {
//        ++commont_count;
//        if (it_count++ < batch_size)
//        {
//            batch.Put(it.first, it.second);
//        }
//        else
//        {
//            s = db->Write(WriteOptions(), &batch);
//            assert(s.ok());
//            batch.Clear();
//            it_count = 0;
//        }
//    }
//    if (batch.Count())
//    {
//        s = db->Write(WriteOptions(), &batch);
//        assert(s.ok());
//        batch.Clear();
//    }
//    end = std::chrono::steady_clock::now();
//    std::cout << "Write Time over batch: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;
//    std::cout << "common count " << commont_count << std::endl;
//
//    start = std::chrono::steady_clock::now();
//    batch.Clear();
//    it_count = 0;
//    commont_count = 0;
//    for (auto& it:v_r_str) {
//        ++commont_count;
//        if (it_count++ < batch_size)
//        {
//            batch.Delete(it.first);
//        }
//        else
//        {
//            s = db->Write(WriteOptions(), &batch);
//            assert(s.ok());
//            batch.Clear();
//            it_count = 0;
//        }
//    }
//    if (batch.Count())
//    {
//        s = db->Write(WriteOptions(), &batch);
//        assert(s.ok());
//        batch.Clear();
//    }
//    end = std::chrono::steady_clock::now();
//    std::cout << "Delete Time over batch: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;
//    std::cout << "common count " << commont_count << std::endl;

    delete db;

    return 0;
}
