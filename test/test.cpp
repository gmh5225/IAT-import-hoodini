#include <import-hoodini.hpp>
#include <thread>
#include <iostream>
#include <format>
#include <chrono>

int
main()
{
    ImportHoodini::Setup_AllImports();
    ImportHoodini::ActivateImportCallbacks();

    std::thread t1([]() {
        while (true)
        {
            auto Reports = ImportHoodini::Reports::GetReports();
            for (auto &Rep : Reports)
            {
                std::cout << std::format(
                                 "PatchType:{}, FunctionDump[0]:{:X}",
                                 static_cast<int>(Rep.PatchType),
                                 Rep.FunctionDump[0])
                          << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    t1.detach();

    system("pause");
    return 0;
}