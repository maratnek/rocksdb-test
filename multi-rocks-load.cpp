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
std::atomic<uint32_t> atom_count = 0;
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

    auto count = 1000'000;
//    std::unordered_map<std::string, std::string> v_r_str;
    std::vector<std::string> v_r_str;
    v_r_str.reserve(count);
    v_r_str.resize(count);
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
//                std::lock_guard<std::mutex> lock(mutex_gener_add_block);
//                v_r_str.insert({std::to_string(i),gen_random_str(20)});
                v_r_str[i] = gen_random_str(20);
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
    l_prepare_data();
    std::cout << "prepared " << v_r_str.size() << " count" << std::endl;
    auto end = std::chrono::steady_clock::now();
    std::cout << "Write Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;

//    auto l_write_data = [&]() {
//        auto block_count = count;
//        std::vector<std::future<void>> v_futures;
//        /// create several transactions
//        auto conc_threads = std::thread::hardware_concurrency();
//        std::cout << "Real thread count: " << conc_threads << std::endl;
//        auto interval = block_count / conc_threads;
//        for (size_t ith = 0; ith < conc_threads; ++ith)
//        {
//            v_futures.emplace_back(std::move(std::async(std::launch::async, [=, &v_r_str]()
//            {
//                size_t i = ith * interval;
//                auto max_count = i + interval;
//                std::cout << "Iter threads: " << ith << " Interval start: " << i << " Interval end: " << max_count << std::endl;
//                for (; i < max_count; i++)
//                {
//                    Status s = db->Put(WriteOptions(), std::to_string(i), v_r_str[i]);
//                    assert(s.ok());
//                }
//            })));
//        }
//        std::cout << "Wait futures" << std::endl;
//        for (auto &it_future : v_futures)
//        {
//            it_future.get();
//        }
//
//    };
//    std::cout << "Write to db: " << count << " values." << std::endl;
//
//    start = std::chrono::steady_clock::now();
//    l_write_data();
//    end = std::chrono::steady_clock::now();
//    std::cout << "Write Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;

//    using TLoadFunc = std::function<void(size_t, size_t)>;
//    auto l_thread_data = [&](TLoadFunc func_load, size_t add_threads = 0) {
//        auto block_count = count;
//        std::vector<std::future<void>> v_futures;
//        /// create several transactions
//        auto conc_threads = std::thread::hardware_concurrency();
//        conc_threads += add_threads;
//        std::cout << "Real thread count: " << conc_threads << std::endl;
//        auto interval = block_count / conc_threads;
//        for (size_t ith = 0; ith < conc_threads; ++ith)
//        {
//            v_futures.emplace_back(std::move(std::async(std::launch::async, [=, &func_load]()
//            {
//                size_t i = ith * interval;
//                auto max_count = i + interval;
//                std::cout << "Iter threads: " << ith << " Interval start: " << i << " Interval end: " << max_count << std::endl;
//                func_load(i, max_count);
//            })));
//        }
//        std::cout << "Wait futures" << std::endl;
//        for (auto &it_future : v_futures)
//        {
//            it_future.get();
//        }
//
//    };
//
//    std::cout << "Write to db: " << count << " values." << std::endl;
//
//    start = std::chrono::steady_clock::now();
//    l_thread_data([&v_r_str, &db](size_t i, size_t max_count){
//        WriteBatch batch;
//        Status s;
//        auto it_count = 0;
//        auto batch_size = 10000;
//        auto commont_count = 0;
//
//        for (; i < max_count; i++)
//        {
//            ++commont_count;
//            if (it_count++ < batch_size)
//            {
//                batch.Put(std::to_string(i), v_r_str[i]);
//            }
//            else
//            {
//                s = db->Write(WriteOptions(), &batch);
//                assert(s.ok());
//                batch.Clear();
//                it_count = 0;
//            }
//        }
//        if (batch.Count())
//        {
//            s = db->Write(WriteOptions(), &batch);
//            assert(s.ok());
//            batch.Clear();
//        }
//        atom_count += commont_count;
//        std::cout << "Common count " << commont_count << std::endl;
//    }, 0);
//    end = std::chrono::steady_clock::now();
//
//    std::cout << "Common count " << atom_count << std::endl;
//    std::cout << "Write Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;


//    auto l_write_batch_data = [&]() {
//        auto block_count = count;
//        std::vector<std::future<void>> v_futures;
//        /// create several transactions
//        auto conc_threads = std::thread::hardware_concurrency();
////        conc_threads += 8;
//        std::cout << "Real thread count: " << conc_threads << std::endl;
//        auto interval = block_count / conc_threads;
//        for (size_t ith = 0; ith < conc_threads; ++ith)
//        {
//            v_futures.emplace_back(std::move(std::async(std::launch::async, [=, &v_r_str]()
//            {
//                size_t i = ith * interval;
//                auto max_count = i + interval;
//                std::cout << "Iter threads: " << ith << " Interval start: " << i << " Interval end: " << max_count << std::endl;
//
//                WriteBatch batch;
//                Status s;
//                auto it_count = 0;
//                auto batch_size = 10000;
//                auto commont_count = 0;
//
//                for (; i < max_count; i++)
//                {
//                    ++commont_count;
//                    if (it_count++ < batch_size)
//                    {
//                        batch.Put(std::to_string(i), v_r_str[i]);
//                    }
//                    else
//                    {
//                        {
//                            std::lock_guard<std::mutex> lock(mutex_gener_add_block);
//                            s = db->Write(WriteOptions(), &batch);
//                        }
//                        assert(s.ok());
//                        batch.Clear();
//                        it_count = 0;
//                    }
//                }
//                if (batch.Count())
//                {
//                    {
//                        std::lock_guard<std::mutex> lock(mutex_gener_add_block);
//                        s = db->Write(WriteOptions(), &batch);
//                    }
//                    assert(s.ok());
//                    batch.Clear();
//                }
//
//                atom_count += commont_count;
//                std::cout << "Common count " << commont_count << std::endl;
//            })));
//        }
//        std::cout << "Wait futures" << std::endl;
//        for (auto &it_future : v_futures)
//        {
//            it_future.get();
//        }
//
//    };
//    std::cout << "Write to db: " << count << " values." << std::endl;
//
//    start = std::chrono::steady_clock::now();
//    l_write_batch_data();
//    end = std::chrono::steady_clock::now();
//
//    std::cout << "Common count " << atom_count << std::endl;
//    std::cout << "Write Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;


//    atom_count = 0;
//    start = std::chrono::steady_clock::now();
//    l_thread_data([&v_r_str, &db](size_t i, size_t max_count){
//        std::string value;
//    Status s;
//        auto comm_thread_count = 0;
//        for (; i < max_count; i++)
//        {
//            s = db->Get(ReadOptions(), std::to_string(i), &value);
//            if (!s.ok())
//                std::cout << "Not OK " << std::to_string(i) << " " << value << " - " << v_r_str[i] << std::endl;
//            assert(s.ok());
//            assert(value == v_r_str[i]);
//            ++comm_thread_count;
//        }
//        atom_count += comm_thread_count;
//    }, 0);
//    end = std::chrono::steady_clock::now();
//    std::cout << "Common count " << atom_count << std::endl;
//    std::cout << "Read Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;

    auto l_read_batch_data = [&]() {
        auto block_count = count;
        std::vector<std::future<void>> v_futures;
        /// create several transactions
        auto conc_threads = std::thread::hardware_concurrency();
//        conc_threads += 8;
        std::cout << "Real thread count: " << conc_threads << std::endl;
        auto interval = block_count / conc_threads;
        for (size_t ith = 0; ith < conc_threads; ++ith)
        {
            v_futures.emplace_back(std::move(std::async(std::launch::async, [=, &v_r_str]()
            {
                size_t i = ith * interval;
                auto max_count = i + interval;
                std::cout << "Iter threads: " << ith << " Interval start: " << i << " Interval end: " << max_count << std::endl;

                Status s;
                auto commont_count = 0;

                for (; i < max_count; i++)
                {
                    ++commont_count;
                    std::string value;
                    s = db->Get(ReadOptions(), std::to_string(i), &value);
                    if (!s.ok())
                        std::cout << "Not OK " << std::to_string(i) << " " << value << " - " << v_r_str[i] << std::endl;
                    assert(s.ok());
                    assert(value == v_r_str[i]);
                }
                atom_count += commont_count;
                std::cout << "Common count " << commont_count << std::endl;
            })));
        }
        std::cout << "Wait futures" << std::endl;
        for (auto &it_future : v_futures)
        {
            it_future.get();
        }

    };
    std::cout << "Read to db: " << count << " values." << std::endl;

    atom_count = 0;
    start = std::chrono::steady_clock::now();
    l_read_batch_data();
    end = std::chrono::steady_clock::now();

    std::cout << "Common count " << atom_count << std::endl;
    std::cout << "Read Time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000'000.0 << " sec" << std::endl;

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
