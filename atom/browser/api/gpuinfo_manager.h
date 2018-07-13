// Copyright (c) 2018 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_GPUINFO_MANAGER_H_
#define ATOM_BROWSER_API_GPUINFO_MANAGER_H_

#include "atom/common/native_mate_converters/value_converter.h"
#include "atom/common/promise_util.h"
#include "content/browser/gpu/gpu_data_manager_impl.h"
#include "content/public/browser/gpu_data_manager.h"
#include "content/public/browser/gpu_data_manager_observer.h"

namespace atom {

class GPUInfoManager : public content::GpuDataManagerObserver {
 public:
  explicit GPUInfoManager(v8::Isolate* isolate);
  ~GPUInfoManager();
  bool NeedsCompleteGpuInfoCollection();
  void FetchCompleteInfo();
  void FetchAvailableInfo();
  void OnGpuInfoUpdate() override;
  util::Promise* Promise() const { return gpuinfo_promise_; }

 private:
  void EnumerateGPUInfo(gpu::GPUInfo gpu_info) const;

  util::Promise* gpuinfo_promise_;
  content::GpuDataManager* gpu_data_manager_;
};

}  // namespace atom
#endif  // ATOM_BROWSER_API_GPUINFO_MANAGER_H_
