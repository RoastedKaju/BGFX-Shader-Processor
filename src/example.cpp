#include <Windows.h>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bx/platform.h"

#include "shader_processor.h"

int main() {

  std::cout << "=============== SHADER PROCESSOR ===============" << std::endl;

  const auto& files = shader::processor::findShaderFiles(
      std::filesystem::path(SHADER_ROOT_PATH));

  shader::processor::processShaders(files, SHADER_BIN_PATH, SHADER_TOOL_PATH,
                                    L"windows", L"120");

  std::cout << "=============== BGFX APPLICATION ===============" << std::endl;

  bgfx::Init init;
  init.type = bgfx::RendererType::Count;
  init.resolution.width = 1;
  init.resolution.height = 1;
  init.resolution.reset = BGFX_RESET_NONE;

  // Provide window handle
  HWND hwnd = CreateWindowEx(0, "STATIC", "HiddenWindow", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, nullptr,
                             nullptr, GetModuleHandle(nullptr), nullptr);

  init.platformData.nwh = hwnd;
  init.platformData.ndt = nullptr;

  if (!bgfx::init(init)) {
    std::cout << "BGFX failed to init." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "BGFX init successful" << std::endl;

  bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f,
                     0);
  bgfx::setViewRect(0, 0, 0, init.resolution.width, init.resolution.height);

  // Render one frame
  bgfx::touch(0);
  bgfx::frame();

  bgfx::shutdown();

  return EXIT_SUCCESS;
}