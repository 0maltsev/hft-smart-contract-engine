#include "Contract.h"
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <filesystem>

#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/AsmParser/Parser.h"

namespace ContractLoader {

    using ContractTable = std::unordered_map<std::string, std::unique_ptr<Contract>>;
    ContractTable contracts_;

    std::unique_ptr<llvm::orc::LLJIT> JIT;

    std::string read_file(const std::string& path) {
        std::ifstream file(path);
        if (!file) throw std::runtime_error("Failed to open " + path);
        std::string contents((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        return contents;
    }

    void initLLVM() {
        static bool initialized = false;
        if (initialized) return;

        initialized = true;
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();

        auto jitOrErr = llvm::orc::LLJITBuilder().create();
        if (!jitOrErr) {
            llvm::errs() << "Failed to create LLJIT\n";
            exit(1);
        }
        JIT = std::move(*jitOrErr);
    }

    Contract::FnPtr compileIRToFnPtr(const std::string& llvm_ir, const std::string& func_name) {
        initLLVM();

        llvm::SMDiagnostic err;
        auto ctx = std::make_unique<llvm::LLVMContext>();

        auto module = llvm::parseAssemblyString(llvm_ir, err, *ctx);
        if (!module) {
            err.print("ContractLoader", llvm::errs());
            throw std::runtime_error("Failed to parse LLVM IR");
        }

        llvm::orc::ThreadSafeModule tsm(std::move(module), std::move(ctx));

        if (auto err2 = JIT->addIRModule(std::move(tsm))) {
            llvm::errs() << "Failed to add module to JIT\n";
            exit(1);
        }

        auto sym = JIT->lookup(func_name);
        if (!sym) {
            llvm::errs() << "Failed to lookup symbol: " << func_name << "\n";
            exit(1);
        }

        llvm::orc::ExecutorAddr executorAddr = *sym;

        uintptr_t fn_address = executorAddr.getValue();
        return reinterpret_cast<Contract::FnPtr>(fn_address);
    }

    std::unique_ptr<Contract> load_contract_from_ll(const std::string& ll_path) {
        std::string llvm_ir = read_file(ll_path);
        auto fn_ptr = compileIRToFnPtr(llvm_ir, "on_event");
        return std::make_unique<Contract>(fn_ptr);
    }

    void load_contracts_from_folder(const std::string& folder_path) {
        namespace fs = std::filesystem;
        for (auto& entry : fs::directory_iterator(folder_path)) {
            if (entry.path().extension() == ".ll") {  // LLVM IR файлы
                std::string symbol = entry.path().stem().string();
                contracts_[symbol] = load_contract_from_ll(entry.path().string());
                std::cout << "Loaded contract for symbol: " << symbol << std::endl;
            }
        }
    }

    Contract* get_contract(const std::string& symbol) {
        auto it = contracts_.find(symbol);
        return (it != contracts_.end()) ? it->second.get() : nullptr;
    }

}
