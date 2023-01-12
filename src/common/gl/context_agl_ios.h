#pragma once

#include <TargetConditionals.h>
#if TARGET_OS_TV || TARGET_OS_IOS

#include "context.h"
#include <glad.h>

// GLAD has to come first so that Qt doesn't pull in the system GL headers,
// which are incompatible with glad.
#include <glad.h>

// Hack to prevent Apple's glext.h headers from getting included via qopengl.h, since we still want to use glad.
#if defined(__APPLE__)
//&& defined(__OBJC__)
#define __glext_h_
#endif

#include "common/gl/context.h"
#include "common/gl/program.h"
#include "common/gl/stream_buffer.h"
#include "common/gl/texture.h"
#include "common/window_info.h"
#include "core/host_display.h"
#include <memory>

#if defined(__APPLE__) && defined(__OBJC__)
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#else
struct UIView;
struct EAGLContext;
typedef enum EAGLRenderingAPI
{
    kEAGLRenderingAPIOpenGLES1 = 1,
    kEAGLRenderingAPIOpenGLES2 = 2,
    kEAGLRenderingAPIOpenGLES3 = 3,
} EAGLRenderingAPI;
#define __bridge
#endif

namespace GL {

class ContextEAGL final : public Context
{
public:
    ContextEAGL(const WindowInfo& wi);
  ~ContextEAGL() override;

  static std::unique_ptr<Context> Create(const WindowInfo& wi,
                                         const Version* versions_to_try,
                                         size_t num_versions_to_try);

  void* GetProcAddress(const char* name) override;
  bool ChangeSurface(const WindowInfo& new_wi) override;
  void ResizeSurface(u32 new_surface_width = 0, u32 new_surface_height = 0) override;
  bool SwapBuffers() override;
  bool MakeCurrent() override;
  bool DoneCurrent() override;
  bool SetSwapInterval(s32 interval) override;
  std::unique_ptr<Context> CreateSharedContext(const WindowInfo& wi) override;

private:
    ALWAYS_INLINE UIView* GetView() const { return static_cast<UIView*>((__bridge UIView*)m_wi.window_handle); }

  bool Initialize(const Version* versions_to_try, size_t num_versions_to_try);
  bool CreateContext(EAGLContext* share_context, int profile, bool make_current);
  void BindContextToView();

  // returns true if dimensions have changed
  bool UpdateDimensions();

    EAGLContext* m_context = nullptr;
    EAGLRenderingAPI m_pixel_format = kEAGLRenderingAPIOpenGLES3;
  void* m_opengl_module_handle = nullptr;
};

} // namespace GL
#endif
