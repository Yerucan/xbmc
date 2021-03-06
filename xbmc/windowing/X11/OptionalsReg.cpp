/*
 *      Copyright (C) 2005-2017 Team XBMC
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "OptionalsReg.h"

//-----------------------------------------------------------------------------
// VAAPI
//-----------------------------------------------------------------------------
#if defined (HAVE_LIBVA)
#include <va/va_x11.h>
#include "cores/VideoPlayer/DVDCodecs/Video/VAAPI.h"
#include "cores/VideoPlayer/VideoRenderers/HwDecRender/RendererVAAPIGL.h"

class CWinSystemX11GLContext;

class CVaapiProxy : public VAAPI::IVaapiWinSystem
{
public:
  CVaapiProxy() = default;
  virtual ~CVaapiProxy() = default;
  VADisplay GetVADisplay() override { return vaGetDisplay(dpy); };
  void *GetEGLDisplay() override { return eglDisplay; };

  Display *dpy;
  void *eglDisplay;
};

CVaapiProxy* X11::VaapiProxyCreate()
{
  return new CVaapiProxy();
}

void X11::VaapiProxyDelete(CVaapiProxy *proxy)
{
  delete proxy;
}

void X11::VaapiProxyConfig(CVaapiProxy *proxy, void *dpy, void *eglDpy)
{
  proxy->dpy = static_cast<Display*>(dpy);
  proxy->eglDisplay = eglDpy;
}

void X11::VAAPIRegister(CVaapiProxy *winSystem, bool hevc)
{
  VAAPI::CDecoder::Register(winSystem, hevc);
}

void X11::VAAPIRegisterRender(CVaapiProxy *winSystem, bool &general, bool &hevc)
{
  EGLDisplay eglDpy = winSystem->eglDisplay;
  VADisplay vaDpy = vaGetDisplay(winSystem->dpy);
  CRendererVAAPI::Register(winSystem, vaDpy, eglDpy, general, hevc);
}

#else

class CVaapiProxy
{
};

CVaapiProxy* X11::VaapiProxyCreate()
{
  return nullptr;
}

void X11::VaapiProxyDelete(CVaapiProxy *proxy)
{
}

void X11::VaapiProxyConfig(CVaapiProxy *proxy, void *dpy, void *eglDpy)
{
}

void X11::VAAPIRegister(CVaapiProxy *winSystem, bool hevc)
{
}

void X11::VAAPIRegisterRender(CVaapiProxy *winSystem, bool &general, bool &hevc)
{
}

#endif

//-----------------------------------------------------------------------------
// GLX
//-----------------------------------------------------------------------------

#ifdef HAS_GLX
#include "GL/glx.h"
#include "VideoSyncGLX.h"
#include "GLContextGLX.h"

XID X11::GLXGetWindow(void* context)
{
  return static_cast<CGLContextGLX*>(context)->m_glxWindow;
}

void* X11::GLXGetContext(void* context)
{
  return static_cast<CGLContextGLX*>(context)->m_glxContext;
}

CGLContext* X11::GLXContextCreate(Display *dpy)
{
  return new CGLContextGLX(dpy);
}

CVideoSync* X11::GLXVideoSyncCreate(void *clock, CWinSystemX11GLContext& winSystem)
{
  return new  CVideoSyncGLX(clock, winSystem);
}

#else

XID X11::GLXGetWindow(void* context)
{
  return 0;
}

void* X11::GLXGetContext(void* context)
{
  return nullptr;
}

CGLContext* X11::GLXContextCreate(Display *dpy)
{
  return nullptr;
}

CVideoSync* X11::GLXVideoSyncCreate(void *clock, CWinSystemX11GLContext& winSystem)
{
  return nullptr;
}

#endif

//-----------------------------------------------------------------------------
// VDPAU
//-----------------------------------------------------------------------------

#if defined (HAVE_LIBVDPAU)
#include "cores/VideoPlayer/DVDCodecs/Video/VDPAU.h"
#include "cores/VideoPlayer/VideoRenderers/HwDecRender/RendererVDPAU.h"

void X11::VDPAURegisterRender()
{
  CRendererVDPAU::Register();
}

void X11::VDPAURegister()
{
  VDPAU::CDecoder::Register();
}

#else

void X11::VDPAURegisterRender()
{

}

void X11::VDPAURegister()
{

}
#endif

//-----------------------------------------------------------------------------
// ALSA
//-----------------------------------------------------------------------------

#ifdef HAS_ALSA
#include "cores/AudioEngine/Sinks/AESinkALSA.h"
bool X11::ALSARegister()
{
  CAESinkALSA::Register();
  return true;
}
#else
bool X11::ALSARegister()
{
  return false;
}
#endif

//-----------------------------------------------------------------------------
// PulseAudio
//-----------------------------------------------------------------------------

#ifdef HAS_PULSEAUDIO
#include "cores/AudioEngine/Sinks/AESinkPULSE.h"
bool X11::PulseAudioRegister()
{
  bool ret = CAESinkPULSE::Register();
  return ret;
}
#else
bool X11::PulseAudioRegister()
{
  return false;
}
#endif

//-----------------------------------------------------------------------------
// sndio
//-----------------------------------------------------------------------------

#ifdef HAS_SNDIO
#include "cores/AudioEngine/Sinks/AESinkSNDIO.h"
bool X11::SndioRegister()
{
  CAESinkSNDIO::Register();
  return true;
}
#else
bool X11::SndioRegister()
{
  return false;
}
#endif
