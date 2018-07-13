// Copyright (c) 2018 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/browser/api/gpuinfo_manager.h"
#include "atom/browser/api/gpu_info_enumerator.h"
#include "content/public/browser/browser_thread.h"
#include "gpu/config/gpu_info_collector.h"

namespace atom {

GPUInfoManager::GPUInfoManager(v8::Isolate* isolate)
    : gpuinfo_promise_(new util::Promise(isolate)),
      gpu_data_manager_(content::GpuDataManagerImpl::GetInstance()) {
  gpu_data_manager_->AddObserver(this);
}

GPUInfoManager::~GPUInfoManager() {
  content::GpuDataManagerImpl::GetInstance()->RemoveObserver(this);
}

bool GPUInfoManager::NeedsCompleteGpuInfoCollection() {
  const auto& gpu_info = gpu_data_manager_->GetGPUInfo();
#if defined(OS_MACOSX)
  return gpu_info.gl_vendor.empty();
#elif defined(OS_WIN)
  return (gpu_info.dx_diagnostics.values.empty() &&
          gpu_info.dx_diagnostics.children.empty());
#else
  return false;
#endif
}

void GPUInfoManager::OnGpuInfoUpdate() {
  // Ignore if called when not asked for complete GPUInfo
  if (NeedsCompleteGpuInfoCollection())
    return;
  EnumerateGPUInfo(gpu_data_manager_->GetGPUInfo());
  delete this;
}

void GPUInfoManager::FetchCompleteInfo() {
  if (NeedsCompleteGpuInfoCollection()) {
    gpu_data_manager_->RequestCompleteGpuInfoIfNeeded();
  } else {
    content::BrowserThread::PostTask(
        content::BrowserThread::UI, FROM_HERE,
        base::BindOnce(&GPUInfoManager::FetchAvailableInfo,
                       base::Unretained(this)));
  }
}

void GPUInfoManager::FetchAvailableInfo() {
  gpu::GPUInfo gpu_info;
  CollectBasicGraphicsInfo(&gpu_info);
  EnumerateGPUInfo(gpu_info);
}

void GPUInfoManager::EnumerateGPUInfo(gpu::GPUInfo gpu_info) const {
  GPUInfoEnumerator enumerator;
  gpu_info.EnumerateFields(&enumerator);
  gpuinfo_promise_->Resolve(*enumerator.GetDictionary());
}

}  // namespace atom
