#include "app_entry.h"

#include "app.hpp"

namespace {

app::App g_app;

}

extern "C" void App_Init(void)
{
  g_app.Init();
}

extern "C" void App_RunFastTick(void)
{
  g_app.RunFastTick();
}

extern "C" void App_RunSlowTick(void)
{
  g_app.RunSlowTick();
}
